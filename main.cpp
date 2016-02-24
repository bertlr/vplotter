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

    double x_prev = x;
    double y_prev = y;
    double z_prev = z;

    int M_command = -1;
    int G_command = -1;

    double base_length = 650.0;
    double x0 = 100.0;
    double y0 = -700.0;
    double stepsPermm = 40.0;
    int servo_up = 13;
    int servo_down = 8;

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

            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long(argc, argv, "b:y:x:s:u:p:",
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

            default:
                abort();
        }
    }
    if (count_options < 6) {
        std::cout << "not all options are set" << std::endl;
        exit(1);
    }

    // end read options
    std::cout << "--------------------------------------------------------------" << std::endl
            << "                  VPLOTTER                                    " << std::endl
            << "--------------------------------------------------------------" << std::endl;


    Machine m(base_length, x0, y0, stepsPermm, servo_down, servo_up);
    std::cout << "started" << std::endl;
    
    std::string line;
    int linenumber = 0;
    while (std::getline(std::cin, line)) {
        std::stringstream is;
        std::stringstream os;
        linenumber++;
        std::cout << linenumber << ": ";

        is << line;

        std::cout << line;
        // parse one line:
        xxFlexLexer lexer(&is, &os); // das prefix "xx" siehe flexerfile
        int ret = 0;
        do {
            ret = lexer.yylex();
            std::string s = lexer.YYText();
            s.erase(0, 1);
            if (ret == 1) {
                G_command = ::atoi(s.c_str());
            } else if (ret == 2) {
                x = ::atof(s.c_str());
            } else if (ret == 3) {
                y = ::atof(s.c_str());
            } else if (ret == 4) {
                z = ::atof(s.c_str());
            } else if (ret == 5) {
                M_command = ::atoi(s.c_str());
            } else if (ret == 6) {
                r = ::atof(s.c_str());
            } else if (ret == 7) {
                i = ::atof(s.c_str());
            } else if (ret == 8) {
                j = ::atof(s.c_str());
            } else if (ret == 9) {
                f = ::atof(s.c_str());
            }
        } while (ret != 0);


        if (G_command == 0 || G_command == 1 || G_command == 2 || G_command == 3) {
            if (z != z_prev) {
                if (z > 0) {
                    std::cout << "pen up" << std::endl;
                    m.penDown(false);
                } else {
                    std::cout << "pen down" << std::endl;
                    m.penDown(true);
                }
            }
            if (x != x_prev || y != y_prev) {
                if (G_command == 0) {
                    std::cout << "   -> rapid  to: " << x << ", " << y << std::endl;
                    m.MoveToPoint(x, y, 0.0);
                } else if (G_command == 1) {
                    std::cout << "   -> line  to: " << x << ", " << y << ", f=" << f << std::endl;
                    m.MoveToPoint(x, y, f);
                } else if (G_command == 2 || G_command == 3) {

                    bool ccw;
                    // because the y-axis is from top (-) to bottom (+)
                    if (G_command == 2) {
                        ccw = false;
                    } else {
                        ccw = true;
                    }
                    if (r == DBL_MAX & i < DBL_MAX && j < DBL_MAX) {
                        r = sqrt(i * i + j * j);
                    } else if (r != DBL_MAX) {

                    } else {
                        std::cout << "Error: either R or I and J" << std::endl;
                        exit(1);
                    }
                    std::cout << "   -> arc to: " << x << ", " << y << " r=" << r << std::endl;
                    Geometry g;
                    std::vector<Point> points = g.getArcPolygon(Point(x_prev, y_prev), Point(x, y), r, ccw);
                    for (int i = 0; i < points.size(); i++) {
                        //std::cout << points[i].x << ", " << points[i].y << std::endl;
                        x = points[i].x;
                        y = points[i].y;
                        //m.CalculateLine(x, y);
                        m.MoveToPoint(x, y, f);
                    }
                }



            }

        }

        if (M_command == 30) {
            m.penDown(false);
            m.MoveToPoint(0.0, 0.0, 0.0);
            return 0;

        }
        //std::cout << "x =" << x << " y=" << y << std::endl;

        //G_command = -1;
        M_command = -1;
        x_prev = x;
        y_prev = y;
        z_prev = z;
        r = DBL_MAX; // radius
        i = DBL_MAX;
        j = DBL_MAX;



    }
    std::cout << "Finish" << std::endl;
    return 0;
}
