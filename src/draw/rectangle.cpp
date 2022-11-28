/*
 * File: point.cpp
 *
 * Copyright (C) 2019 David Hoksza <david.hoksza@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */


#include "rectangle.hpp"
#include "geometry.hpp"

#include <cmath>
#include <cassert>

using namespace std;


rectangle::rectangle()
: top_left(point::bad_point()), bottom_right(point::bad_point())
{
}

rectangle::rectangle(point p1, point p2)
{
    top_left = point(min(p1.x, p2.x), max(p1.y, p2.y));
    bottom_right = point(max(p1.x, p2.x), min(p1.y, p2.y));
}

rectangle rectangle::operator+(const rectangle& other) const
{
    if (this->initiated()) {
        return rectangle(
                point(min(top_left.x, other.top_left.x), max(top_left.y, other.top_left.y)),
                point(max(bottom_right.x, other.bottom_right.x), min(bottom_right.y, other.bottom_right.y)));
    } else {
        return rectangle(other.top_left, other.bottom_right);
    }
}

rectangle rectangle::operator+(const point& other) const
{
    if (this->initiated()) {
        return rectangle(
                point(min(top_left.x, other.x), max(top_left.y, other.y)),
                point(max(bottom_right.x, other.x), min(bottom_right.y, other.y)));
    } else {
        return rectangle(
                point(other.x, other.y),
                point(other.x, other.y));
    }
}

rectangle& rectangle::operator+=(const point& other)
{
    if (this->initiated()) {
        top_left = point(min(top_left.x, other.x), max(top_left.y, other.y));
        bottom_right = point(max(bottom_right.x, other.x), min(bottom_right.y, other.y));
    } else {
        top_left = point(other.x, other.y);
        bottom_right = point(other.x, other.y);
    }

    return *this;
}

rectangle& rectangle::operator+=(const rectangle& other)
{
    if (this->initiated()) {
        top_left = point(min(top_left.x, other.top_left.x), max(top_left.y, other.top_left.y));
        bottom_right =  point(max(bottom_right.x, other.bottom_right.x), min(bottom_right.y, other.bottom_right.y));
    } else {
        top_left = other.top_left;
        bottom_right = other.bottom_right;
    }

    return *this;
}



rectangle& rectangle::operator=(const rectangle& other){
    top_left = other.top_left;
    bottom_right = other.bottom_right;
    return *this;

}

bool rectangle::intersects(const rectangle& rect) const{
    // If one rectangle is on left side of other

//    if ((rect.has(top_left) && rect.has(bottom_right)) || (has(rect.top_left) && has(rect.bottom_right)))
//        return true;

    if (bottom_right.x < rect.top_left.x || top_left.x > rect.bottom_right.x)
        return false;

    // If one rectangle is above other
    if (top_left.y < rect.bottom_right.y || bottom_right.y > rect.top_left.y)
        return false;

    return true;
}

// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(point p, point q, point r)
{
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
        return true;

    return false;
}

bool rectangle::intersects(const point& p1, const point& p2) const {
    return lines_intersect(p1, p2, get_top_left(), get_top_right() ) ||
            lines_intersect(p1, p2, get_top_right(), get_bottom_right() ) ||
            lines_intersect(p1, p2, get_bottom_right(), get_bottom_left() ) ||
            lines_intersect(p1, p2, get_top_left(), get_bottom_left() );
}

point lines_intersection_point(point ps1, point pe1, point ps2, point pe2)
{
// Get A,B of first line - points : ps1 to pe1
    double A1 = pe1.y-ps1.y;
    double B1 = ps1.x-pe1.x;
    // Get A,B of second line - points : ps2 to pe2
    double A2 = pe2.y-ps2.y;
    double B2 = ps2.x-pe2.x;

    // Get delta and check if the lines are parallel
    double delta = A1*B2 - A2*B1;
    if(delta == 0) return point();

    // Get C of first and second lines
    double C2 = A2*ps2.x+B2*ps2.y;
    double C1 = A1*ps1.x+B1*ps1.y;
    //invert delta to make division cheaper
    double invdelta = 1/delta;
    // now return the Vector2 intersection point
    return point( (B2*C1 - B1*C2)*invdelta, (A1*C2 - A2*C1)*invdelta );
}

point rectangle::intersection(const point& p1, const point& p2) const{

    if (lines_intersect(p1, p2, get_top_left(), get_top_right() ) ) return lines_intersection_point (p1, p2, get_top_left(), get_top_right() );
    if (lines_intersect(p1, p2, get_top_right(), get_bottom_right() ) ) return lines_intersection_point (p1, p2, get_top_right(), get_bottom_right() );
    if (lines_intersect(p1, p2, get_bottom_right(), get_bottom_left() ) ) return lines_intersection_point (p1, p2, get_bottom_right(), get_bottom_left() );
    //if (lines_intersect(p1, p2, get_top_left(), get_bottom_left() ) ) return lines_intersection_point (p1, p2, get_top_left(), get_bottom_left() );
    return lines_intersection_point (p1, p2, get_top_left(), get_bottom_left() );

//    assert(false);

}


bool rectangle::has(const point& point) const{
    return top_left.x <= point.x && point.x  <= bottom_right.x
           && top_left.y >= point.y && point.y >= bottom_right.y;
}


bool rectangle::initiated() const {
    return !top_left.bad() && !bottom_right.bad();
}
