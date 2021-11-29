/*
Added PWM-control for a BLDC-motor.
By Robert Friberg

This version:
Sanity check for bldc_dc values is working.

Previous version:
Experimenting with sanity check for bldc_dutycycle.

Previous version:
Experimenting with stopping the bldc motor on ctrl+c
Program catches ctrl+c and stops bldc motor before exit.

Previous version:
Experimenting with generating pwm output for bldc.
Catching command line argument for bldc_dutycycle. Calling function for starting and stopping the bldc motor.

Previous version:
Experimenting with bldc-ds argument at startup.
Argument for bldc_dutycycle working. Not doing anything with it yet.

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
 * File:   Machine.cpp
 * Author: Herbert Roider <herbert@roider.at>
 * 
 * Created on 13. Februar 2016, 08:42
 */

#include "Machine.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "Geometry.h"

// step wiringPi pin 0 (GPIO 17)
#define LEFT_STEPPER01 0
// direction wiringPi pin 7 (GPIO 4)
#define LEFT_STEPPER02 7


// step wiringPi pin 3 (GPIO 22)
#define RIGHT_STEPPER01  3
// direction wiringPi pin 4 (GPIO 23)
#define RIGHT_STEPPER02  4

// servo wiringPi pin 2 (GPIO 27)
#define Z_SERVO 2

// BLDC wiringPi pin 1 (GPIO 18)    //RF: Added define.
#define BLDC 1

//RF: Function for starting and stopping the bldc motor.
void startstopBLDC(int dc)
{
    #define dcStart 10   //RF: The ECS I am using needs a low value > 0 to register "trottle down".

    if (wiringPiSetup () == -1) //using wiringPi pin numbering
        exit (1) ;

    pinMode (BLDC, PWM_OUTPUT) ;
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(384); //clock at 50kHz (20us tick)
    pwmSetRange(1000); //range at 1000 ticks (20ms)
    pwmWrite(BLDC, dcStart); //RF: Trottle down.
    sleep(2);//RF: wait 2 sec.
    pwmWrite(BLDC, dc);  //theretically 50 (1ms) to 100 (2ms)
}

//RF: Signal handler for SIGINT
void sigintHandler(int signum)
{
    startstopBLDC(0);
    exit(signum);
}


/**
 * 
 * @param _BaseLength sets the distance between the stepper motors in mm
 * @param _X0  horizontal distance to the zero point from the left stepper
 * @param _Y0  vertical distance to the zero point from the left stepper
 * @param _StepsPermm  Steps per mm, for microsteping: 1600 / circumference of the pulley
 * @param _z_down  value for the servo to turn the pen down to the paper
 * @param _z_up   value for the servo to lift the pen from the paper
 */
Machine::Machine(double _BaseLength, double _X0, double _Y0, double _StepsPermm, int _z_down, int _z_up) {
    this->BaseLength = _BaseLength;
    this->X0 = _X0;
    this->Y0 = _Y0;

    this->StepsPermm = _StepsPermm; // 51   //2000.0 / 98.0;
    this->z_down = _z_down;
    this->z_up = _z_up;

    this->currentX = 0.0;
    this->currentY = 0.0;
    
    this->currentZ = -1;

    if (wiringPiSetup() == -1) {
        std::cout << "Could not run wiringPiSetup, require root privileges!\n";
        exit(1);
    }

    softPwmCreate(Z_SERVO, this->z_up, 200);

    this->penDown(false);


    pinMode(LEFT_STEPPER01, OUTPUT);
    pinMode(LEFT_STEPPER02, OUTPUT);

    digitalWrite(LEFT_STEPPER01, 0);
    digitalWrite(LEFT_STEPPER02, 0);

    pinMode(RIGHT_STEPPER01, OUTPUT);
    pinMode(RIGHT_STEPPER02, OUTPUT);

    digitalWrite(RIGHT_STEPPER01, 0);
    digitalWrite(RIGHT_STEPPER02, 0);



    this->CordLengthLeft = sqrt(this->X0 * this->X0 + this->Y0 * this->Y0) * this->StepsPermm;
    this->CordLengthRight = sqrt((this->BaseLength - this->X0) * (this->BaseLength - this->X0) + this->Y0 * this->Y0) * this->StepsPermm;



    std::cout << "X0=" << this->X0 << ", Y0=" << this->Y0 << ", Steps per mm=" << this->StepsPermm << std::endl;

}

Machine::Machine(const Machine& orig) {
}

Machine::~Machine() {
}

/**
 * moves the left motor 1 step
 *
 * @param direction -1 or +1
 * @param count    count of steps
 * @param time     duration per step [microsecond], for G0 is 200 a good value
 */
void Machine::MakeStepLeft(int direction, int count, unsigned int time) {

    if (direction > 0) {
        digitalWrite(LEFT_STEPPER02, 1);
    } else {
        digitalWrite(LEFT_STEPPER02, 0);
    }
    time /= 2;
    for (int i = 0; i < count; i++) {
        digitalWrite(LEFT_STEPPER01, 1);
        ::usleep(time);
        digitalWrite(LEFT_STEPPER01, 0);
        ::usleep(time);
        this->CordLengthLeft += direction;
    }

}

/**
 *  move the right motor 1 step 
 * 
 * @param direction -1 or +1
 * @param count    count of steps
 * @param time     duration per step [microsecond], for G0 is 200 a good value
 */
void Machine::MakeStepRight(int direction, int count, unsigned int time) {

    if (direction > 0) {
        digitalWrite(RIGHT_STEPPER02, 0);
    } else {
        digitalWrite(RIGHT_STEPPER02, 1);
    }
    time /= 2;
    for (int i = 0; i < count; i++) {
        digitalWrite(RIGHT_STEPPER01, 1);
        ::usleep(time);
        digitalWrite(RIGHT_STEPPER01, 0);
        ::usleep(time);
        this->CordLengthRight += direction;
    }

}

/**
 * Moves to the given point. The path is not a straight line, therefore it should not be too long!
 * @param X
 * @param Y
 * @param time   time per step  [microsecond]
 * @return true if position is reached 
 */
bool Machine::moveShortDist(double X, double Y, unsigned int time) {
    double newCordLengthLeft;
    double newCordLengthRight;
    int dirRight;
    int dirLeft;
    bool ready = true;

    X += this->X0;
    Y += this->Y0;


    newCordLengthLeft = this->StepsPermm * sqrt((X * X) + (Y * Y));
    newCordLengthRight = this->StepsPermm * sqrt(((BaseLength - X) * (BaseLength - X)) + (Y * Y));

    double old_CordLengthLeft = (double) this->CordLengthLeft;
    double old_CordLengthRight = (double) this->CordLengthRight;

    double stepsLeft = newCordLengthLeft - (double) CordLengthLeft;
    double stepsRight = newCordLengthRight - (double) CordLengthRight;


    // set the directions for the steppers
    if (stepsLeft > 0) {
        dirLeft = 1;
    } else {
        dirLeft = -1;

    }
    if (stepsRight > 0) {
        dirRight = 1;
    } else {
        dirRight = -1;

    }

    stepsLeft = fabs(stepsLeft);
    stepsRight = fabs(stepsRight);



    //std::cout << "steps left=" << stepsLeft << ", right=" << stepsRight << std::endl;

    double fact;

    if (stepsLeft < 0.5 && stepsRight < 0.5) {
        return ready;
    } else if (stepsRight < 0.5) {
        this->MakeStepLeft(dirLeft, stepsLeft, time);
    } else if (stepsLeft < 0.5) {
        this->MakeStepRight(dirRight, stepsRight, time);
    } else if (stepsLeft >= stepsRight) {
        fact = stepsLeft / stepsRight;
        for (double r = 0.0; r < stepsRight; r++) {
            this->MakeStepRight(dirRight, 1, time);
            this->MakeStepLeft(dirLeft, (int) (r * fact - fabs(old_CordLengthLeft - (double) this->CordLengthLeft)), time);
        }
    } else if (stepsRight > stepsLeft) {
        fact = stepsRight / stepsLeft;
        for (double l = 0.0; l < stepsLeft; l++) {
            this->MakeStepLeft(dirLeft, 1, time);
            this->MakeStepRight(dirRight, (int) (l * fact - fabs(old_CordLengthRight - (double) this->CordLengthRight)), time);
        }
    } else {
        std::cout << "Error  4567" << std::endl;
        exit(1);

    }




    /*
    std::cout << "soll pos: l=" << (int)newCordLengthLeft << ", r=" << (int)newCordLengthRight << " ist pos: l=" << this->CordLengthLeft << ", r=" << this->CordLengthRight 
            << " delta l=" << (int)stepsLeft << " delta r=" << (int)stepsRight << std::endl;
     
     
     */

    return ready;


}

/**
 * moves the pen to a point in a straight way
 * @param X
 * @param Y
 * @param F Feed in mm/min
 * @return 
 */
int Machine::MoveToPoint(double X, double Y, double F) {
    unsigned int time = 0;
    if(X == this->currentX && Y == this->currentY){
        return 0;
    }
    
    if (F <= 0.0) {
        F = 100000; // big number for rapid speed
    }
    F *= 1.2; // correction factor, the value fits better
    // calculate the time of a step from feed F:
    time = 1000000.0 * 60.0 / (this->StepsPermm * F);
    // set the time per step not too small:
    if (time < 200) {
        time = 200;
    }
    //std::cout << "time=" << time << std::endl;

    Point p2(X, Y);
    p2 = movePoint(p2, -this->currentX, -this->currentY);
    p2 = toPolar(p2);
    Point pi(p2); // interpolatios point
    pi.x = 3.0; // steps of one millimeter
    Point pi_cart;

    while (pi.x < p2.x) {
        pi_cart = movePoint(toCartesian(pi), this->currentX, this->currentY);
        this->moveShortDist(pi_cart.x, pi_cart.y, time);
        pi.x += 3.0; // steps of one millimeter

    }
    this->moveShortDist(X, Y, time);

    this->currentX = X;
    this->currentY = Y;
    return 0;
}

/**
 * raise and down the pen
 * @param down
 */
void Machine::penDown(bool down) {
    if (down) {
        if(this->currentZ == 0){
            return;
        }
        softPwmWrite(Z_SERVO, this->z_down);
        usleep(500000);
        softPwmWrite(Z_SERVO, 0);
        this->currentZ = 0;

    } else {
        if(this->currentZ == 1){
            return;
        }
        softPwmWrite(Z_SERVO, this->z_up);
        usleep(500000);
        softPwmWrite(Z_SERVO, 0);
        this->currentZ = 1;
    }

}