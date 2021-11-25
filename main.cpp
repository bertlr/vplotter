/*
Added PWM-control for a BLDC-motor.
By Robert Friberg

This version:
Experimenting with bldc-ds argument at startup.

Previous version:
In Machine.cpp - Moving Z_SERVO from wiringPi pin 1 to pin 2, freeing pin 1 for the bldc motor.
wiringPi only supports hardware pwm on pin 1.
No other changes in original code.
*∕

/***************************************************************************
 *   Copyright (C) 2016 by Herbert Roider                                  *
 *   herbert.roider@utanet.at                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *

 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* 
 * File:   main.cpp
 * Author: Herbert Roider <herbert@roider.at>
 *
 * Created on 12. Februar 2016, 22:15
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <limits>
#include <cfloat>
#include "Machine.h"
#include "Geometry.h"
#include <getopt.h>

// siehe manpages flex :
#undef yyFlexLexer
#define yyFlexLexer xxFlexLexer
#include <FlexLexer.h>

/*
 * 
 */
int main(int argc, char** argv) {
    double x = 0.0;
    double y = 0.0;
    double z = 1.0;
    double r = DBL_MAX; // radius
    double i = DBL_MAX;
    double j = DBL_MAX;
    double f = 0.0; // feed in mm/min, 0 is rapid speed

    int M_command = -1;
    int G_command = -1;
    // Fact to convert the values, used when inches:
    double to_metric = 1.0;

    double base_length = 650.0;
    double x0 = 100.0;
    double y0 = -700.0;
    double stepsPermm = 40.0;
    int servo_up = 13;
    int servo_down = 8;
    int bldc_dc = 0; //RF: Dutycycle for the bldc motor. Added line. Changed fron double to int.

    int c;
    int count_options = 0;
    // Read the options
    while (1) {
        static struct option long_options[] = {

            /* These options don’t set a flag.
               We distinguish them by their indices. */
            {"baselength", required_argument, 0, 'b'},
            {"y0", required_argument, 0, 'y'},
            {"x0", required_argument, 0, 'x'},
            {"steps", required_argument, 0, 's'},
            {"z_up", required_argument, 0, 'u'},
            {"z_down", required_argument, 0, 'd'},
            {"bldc_dutycycle", required_argument, 0, 'm'}, //RF: Dutycycle for the bldc motor. Added line. Changed BLDC-Dutycycle to BLDC_Dutycycle to bldc_dutycycle.

            {0, 0, 0, 0},
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long(argc, argv, "b:y:x:s:u:d:m:",           //RF: Dutycycle for the bldc motor. Added m: Changed p to d. Added : before b. Removed :.
                long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c) {


            case 'b':
                base_length = atof(optarg);
                std::cout << "baselength = " << base_length << std::endl;
                count_options++;
                break;

            case 'y':
                y0 = atof(optarg);
                std::cout << "y0 = " << y0 << std::endl;
                count_options++;
                break;

            case 'x':
                x0 = atof(optarg);
                std::cout << "x0 = " << x0 << std::endl;
                count_options++;
                break;

            case 's':
                stepsPermm = atof(optarg);
                std::cout << "steps = " << stepsPermm << std::endl;
                count_options++;
                break;

            case 'u':
                servo_up = atoi(optarg);
                std::cout << "z_up = " << servo_up << std::endl;
                count_options++;
                break;

            case 'd':
                servo_down = atof(optarg);
                std::cout << "z_down = " << servo_down << std::endl;
                count_options++;
                break;

            case 'm':                                                    //RF: Dutycycle for the bldc motor. Added case m. Changed BLDC-Dutycycle to BLDC_Dutycycle to bldc_dutycycle. Changet atof to atoi.
                bldc_dc = atoi(optarg);
                std::cout << "bldc_dutycycle = " << bldc_dc << std::endl;
                count_options++;
                break;

            default:
                abort();
        }
    }
    if (count_options < 7) {                                            //RF: Dutycycle for the bldc motor. Changed from 6 to 7
        std::cout << "not all options are set" << std::endl;
        exit(1);
    }

    // end read options
    std::cout << "--------------------------------------------------------------" << std::endl
            << "                  VPLOTTER                                    " << std::endl
            << "--------------------------------------------------------------" << std::endl;


    Machine m(base_length, x0, y0, stepsPermm, servo_down, servo_up, bldc_dc);
    std::cout << "started" << std::endl;

    std::string line;
    int linenumber = 0;
    while (std::getline(std::cin, line)) {
        std::stringstream is;
        std::stringstream os;
        linenumber++;

        is << line;

        std::cout << line;
        // parse one line:
        xxFlexLexer lexer(&is, &os); // das prefix "xx" siehe flexerfile
        int ret = 0;
        do {
            int G;
            ret = lexer.yylex();
            std::string s = lexer.YYText();
            s.erase(0, 1);
            if (ret == 1) {
                G = ::atoi(s.c_str());
                switch (G) {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                        G_command = G;
                        break;
                    case 20:
                        to_metric = 25.4;
                        break;
                    case 21:
                        to_metric = 1.0;
                        break;
                }

            } else if (ret == 2) {
                x = ::atof(s.c_str()) * to_metric;
            } else if (ret == 3) {
                y = ::atof(s.c_str()) * to_metric;
            } else if (ret == 4) {
                z = ::atof(s.c_str()) * to_metric;
            } else if (ret == 5) {
                M_command = ::atoi(s.c_str());
            } else if (ret == 6) {
                r = ::atof(s.c_str()) * to_metric;
            } else if (ret == 7) {
                i = ::atof(s.c_str()) * to_metric;
            } else if (ret == 8) {
                j = ::atof(s.c_str()) * to_metric;
            } else if (ret == 9) {
                f = ::atof(s.c_str()) * to_metric;
            }
        } while (ret != 0);

        // Execute on line:

        if (M_command == 30) {
            m.penDown(false);
            m.MoveToPoint(0.0, 0.0, 0.0);
            return 0;

        }

        // movement
        if (G_command == 0 || G_command == 1 || G_command == 2 || G_command == 3) {
            if (z > 0) {
                m.penDown(false);
            } else {
                m.penDown(true);
            }
            if (G_command == 1 || G_command == 2 || G_command == 3) {
                if (f <= 0.0) {
                    std::cout << "Error: no feed rate (f.e.: F400)" << std::endl;
                    exit(1);
                }
            }

            std::cout << " --> " << linenumber << ": ";
            if (G_command == 0) {
                std::cout << "   -> rapid  to: " << x << ", " << y << std::endl;
                m.MoveToPoint(x, y, 0.0);
            } else if (G_command == 1) {
                std::cout << "   -> line  to: " << x << ", " << y << ", f=" << f << std::endl;
                m.MoveToPoint(x, y, f);
            } else if (G_command == 2 || G_command == 3) {

                bool ccw;
                // G2 clockwise, G3 counter clockwise
                if (G_command == 2) {
                    ccw = false;
                } else {
                    ccw = true;
                }
                // either r (radius) or i and k (relative center point coordinates)
                if (r == DBL_MAX & i < DBL_MAX && j < DBL_MAX) {
                    r = sqrt(i * i + j * j);
                }
                if (r == DBL_MAX) {
                    std::cout << "Error: either R or I and J" << std::endl;
                    exit(1);
                } 
                std::cout << "   -> arc to: " << x << ", " << y << " r=" << r << std::endl;
                Geometry g;
                std::vector<Point> points = g.getArcPolygon(Point(m.getX(), m.getY()), Point(x, y), r, ccw);
                for (int i = 0; i < points.size(); i++) {
                    //std::cout << points[i].x << ", " << points[i].y << std::endl;
                    m.MoveToPoint(points[i].x, points[i].y, f);
                }
            }

        }

        //std::cout << "x =" << x << " y=" << y << std::endl;

        M_command = -1;

        r = DBL_MAX; // radius
        i = DBL_MAX;
        j = DBL_MAX;



    }
    std::cout << "Finish" << std::endl;
    return 0;
}
