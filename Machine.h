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
 * File:   Machine.h
 * Author: Herbert Roider <herbert@roider.at>
 *
 * Created on 13. Februar 2016, 08:42
 */

#ifndef MACHINE_H
#define MACHINE_H

class Machine {
public:
    //Machine();
    Machine(double _BaseLength, double _X0, double _Y0, double _StepsPermm, int _z_down, int _z_up);
    Machine(const Machine& orig);
    virtual ~Machine();
    //int CalculateLine(double moveToX, double moveToY);
    int MoveToPoint(double X, double Y, double F);
    void penDown(bool down);
    double getX(){return currentX;};
    double getY(){return currentY;};
private:
    double currentX;
    double currentY;
    long CordLengthLeft;
    long CordLengthRight;
    double BaseLength;
    int z_down;
    int z_up;
    /** -1  don't know
     * 0 down
     * 1 up
     */
    int currentZ;
    /**
    
     horizontal direction from the left stepper
    */
    double X0;
    /**
    
      Vertikal direction from the left stepper
     
     */
    double Y0;
    int currentPlotDown;
    double StepsPermm; // 51   //2000.0 / 98.0;

    void MakeStepLeft(int direction, int count, unsigned int time);
    void MakeStepRight(int direction, int count, unsigned int time);
    //void moveXY(double X, double Y);
     
    bool moveShortDist(double X, double Y, unsigned int time);

};

#endif /* MACHINE_H */
