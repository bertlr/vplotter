/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Geometry.h
 * Author: Herbert Roider <herbert@roider.at>
 *
 * Created on 16. Februar 2016, 18:15
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Geometry.h"
#include <math.h>
#include <vector>

class Point {
public:
    Point(const Point & orig){x=orig.x; y=orig.y;};
    Point(double _x = 0.0, double _y = 0.0){ x=_x;y=_y;};
    double x;
    double y;
};
Point toPolar(Point p);

Point toCartesian(Point polar);

Point movePoint(Point p, double x, double y);

const double PI  =3.141592653589793238463;
class Geometry {
public:
    Geometry();
    Geometry(const Geometry& orig);
    virtual ~Geometry();
    
    Point getArcCenterPoint(Point begin, Point end, double r, bool ccw); 
    std::vector<Point> getArcPolygon(Point begin, Point end, double r, bool ccw);
    
    int ccw(Point p0, Point p1, Point p2);
    
private:

};
#endif /* GEOMETRY_H */

