/*
 * File: point.hpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard.elias@matfyz.cz>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */


#ifndef POINT_HPP
#define POINT_HPP

#include <ios>


struct point
{
    double x;
    double y;

public:
    point();
    point(double _x, double _y);
    bool operator==(const point& other) const;

    point operator+(const point& other) const;
    point operator-(const point& other) const;
    point operator-() const;
    point operator/(const point& other) const;
    point operator/(double value) const;
    point operator*(double value) const;

#define OPERATION_FUNCTION(operation) \
    point& operator operation ## = (const point& other) \
    { \
        *this = *this operation other; \
        return *this; \
    }
    // operations +=, -=, /=
    OPERATION_FUNCTION(+);
    OPERATION_FUNCTION(-);
    OPERATION_FUNCTION(/);
#undef OPERATION_FUNCTION


    bool bad() const;
    static const point& bad_point();

    friend std::ostream& operator<<(std::ostream& out, const point& p);
};



// other useful functions for points

point operator*(double value, const point& p);

point centre(const point& p1, const point& p2);

double distance(const point& p1, const point& p2);

double size(const point& vector);

point normalize(const point& p);

double angle(const point& p);

double angle(const point& p1, const point& centre, const point& p2);

point rotate(const point& centre, double alpha, double radius);

point orthogonal(const point& p);

point orthogonal(const point& p, const point& direction);

point move_point(const point& p, const point& move_to, double length);

bool lies_on_line(const point& p1, const point& p2, const point& p3);

bool lies_between(point p, point from, point to);

point abs(const point& p);


// functions for double comparing

bool double_equals_precision(
                double val1,
                double val2,
                double precision);

bool double_equals(
                double val1,
                double val2);

double radians_to_degrees(
                double val);

double degrees_to_radians(
                double val);

bool iszero(
                double val);

#endif /* !POINT_HPP */

