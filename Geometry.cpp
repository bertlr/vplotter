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
 * File:   Geometry.cpp
 * Author: Herbert Roider <herbert@roider.at>
 * 
 * Created on 16. Februar 2016, 18:15
 */

#include "Geometry.h"
#include <iostream>
#include <math.h>

/**
 * return a point in polar coordinates from cartesian.
 * the x of the new point is the radius, and y is the angle in radian.
 * @param p
 * @return   the x is the radius and the y is the angle [rad]
 */
Point toPolar(Point p) {
    Point polar;
    polar.y = atan2(p.y, p.x);
    polar.x = sqrt(p.x * p.x + p.y * p.y);
    return polar;
}

/**
 * return a new point with cartesian coordinates from polar
 * @param polar  the x is the radius and the y is the angle [rad]
 * @return 
 */
Point toCartesian(Point polar) {
    Point cart;
    cart.x = polar.x * cos(polar.y);
    cart.y = polar.x * sin(polar.y);
    return cart;

}
/**
 * add the values for x and y to the point and return the new point
 * @param p
 * @param x
 * @param y
 * @return 
 */
Point movePoint(Point p, double x, double y){
    p.x += x;
    p.y += y;
    return p;
    
    
    
}

Geometry::Geometry() {
}

Geometry::Geometry(const Geometry& orig) {
}

Geometry::~Geometry() {
}

/**
 * calculates the center point of the arc
 * @param begin  first point of the arc
 * @param end    last point
 * @param r      radius
 * @param ccw    counter clockwise: true or false
 * @return      the center point
 */
Point Geometry::getArcCenterPoint(Point begin, Point end, double r, bool ccw) {

    double distance;
    double dist_center;

    // distance between begin and end:
    distance = sqrt(pow((end.x - begin.x), 2.0) + pow((end.y - begin.y), 2.0));

    // center point from the line from begin to end:
    Point middle((begin.x + end.x) / 2.0, (begin.y + end.y) / 2.0);
    //middle.y() = ;

    // center point of the circle:
    double x;
    double y;
    if (ccw) {  // counter clockwise
        x = middle.x + sqrt(pow(r, 2.0) - pow((distance / 2.0), 2.0)) * (begin.y - end.y) / distance;
        y = middle.y + sqrt(pow(r, 2.0) - pow((distance / 2.0), 2.0)) * (end.x - begin.x) / distance;
    } else {
        x = middle.x - sqrt(pow(r, 2.0) - pow((distance / 2.0), 2.0)) * (begin.y - end.y) / distance;
        y = middle.y - sqrt(pow(r, 2.0) - pow((distance / 2.0), 2.0)) * (end.x - begin.x) / distance;

    }
    Point center(x, y);

    return center;
}

//int Geometry::ccw(Point p0, Point p1, Point p2) {
//    double dx1, dx2, dy1, dy2;
//    dx1 = p1.x - p0.x;
//    dy1 = p1.y - p0.y;
//    dx2 = p2.x - p0.x;
//    dy2 = p2.y - p0.y;
//    if (dx1 * dy2 > dy1 * dx2) {
//        return 1;
//    }
//    if (dx1 * dy2 < dy1 * dx2) {
//        return -1;
//    }
//    if ((dx1 * dx2 < 0) || (dy1 * dy2 < 0)) {
//        return -1;
//    }
//    if ((dx1 * dx1 + dy1 * dy1) < (dx2 * dx2 + dy2 * dy2)) {
//        return 1;
//    }
//
//    return 0;
//
//}
/** calculate multiple points on the arc without the first point.
 * 
 * @param begin first point of the arc
 * @param end    last point of the arc
 * @param r     radius
 * @param ccw   counter clockwise: true or false
 * @return      std::vector<Point> with the points on the arc without the first point.
 */
std::vector<Point> Geometry::getArcPolygon(Point begin, Point end, double r, bool ccw) {
    //std::cout << " begin=" << begin.x << ", " << begin.y << std::endl;
    Point center = this->getArcCenterPoint(begin, end, r, ccw);
//    std::cout << " center=" << center.x << ", " << center.y << ", r=" << r << std::endl;
//    std::cout << " end=" << end.x << ", " << end.y << std::endl;
    std::vector<Point> points;
    
    Point begin_polar = toPolar(movePoint(begin, -center.x, -center.y));
    //std::cout << " begin trans polar=" << begin_polar.x << ", " << begin_polar.y << std::endl;    
    
    Point end_polar = toPolar(movePoint(end, -center.x, -center.y));
    //std::cout << " end trans polar=" << end_polar.x << ", " << end_polar.y << std::endl;
   
    
    
    double delta_angle = 0.1; // rad
    //points.push_back(begin);        
    if (ccw) {
        if (end_polar.y < begin_polar.y) {
            end_polar.y += 2.0 * PI;
        }
        begin_polar.y += delta_angle;
        while (begin_polar.y < end_polar.y) {
            Point p = toCartesian(begin_polar);
            p = movePoint(p, center.x, center.y);
            points.push_back(p);
            begin_polar.y += delta_angle;
        }



    } else {
        if (end_polar.y > begin_polar.y) {
            end_polar.y -= 2.0 * PI;
        }
        begin_polar.y -= delta_angle;
        while (begin_polar.y > end_polar.y) {
            Point p = toCartesian(begin_polar);
            p = movePoint(p, center.x, center.y);
            points.push_back(p);
            begin_polar.y -= delta_angle;
        }


    }
    //std::cout << " end=" << end.x << ", " << end.y << std::endl;
    points.push_back(end);

    return points;
}