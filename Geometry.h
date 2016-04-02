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
 * File:   Geometry.h
 * Author: Herbert Roider <herbert@roider.at>
 *
 * Created on 16. Februar 2016, 18:15
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <math.h>
#include <vector>

class Point {
public:

    Point(const Point & orig) {
        x = orig.x;
        y = orig.y;
    };

    Point(double _x = 0.0, double _y = 0.0) {
        x = _x;
        y = _y;
    };
    double x;
    double y;
};
Point toPolar(Point p);

Point toCartesian(Point polar);

Point movePoint(Point p, double x, double y);

const double PI = 3.141592653589793238463;

class Geometry {
public:
    Geometry();
    Geometry(const Geometry& orig);
    virtual ~Geometry();

    Point getArcCenterPoint(Point begin, Point end, double r, bool ccw);
    std::vector<Point> getArcPolygon(Point begin, Point end, double r, bool ccw);

private:

};
#endif /* GEOMETRY_H */
