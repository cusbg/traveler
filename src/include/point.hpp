/*
 * File: point.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard.elias@matfyz.cz>
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

#include <sstream>
#include <cmath>

#define PI              M_PI


struct point
{
    double x;
    double y;

    point();
    point(double _x, double _y);
    bool operator==(point other) const;

    point operator+(point other) const;
    point operator-(point other) const;
    point operator-() const;
    point operator/(point other) const;
    point operator/(double value) const;
    point operator*(double value) const;

#define OPERATION_FUNCTION(operation) \
    point& operator operation ## = (const point& other) \
    { \
        *this = *this operation other; \
        return *this; \
    }
    OPERATION_FUNCTION(+);
    OPERATION_FUNCTION(-);
    OPERATION_FUNCTION(/);
#undef OPERATION_FUNCTION


    bool bad() const;
    static point bad_point();

    friend std::ostream& operator<<(std::ostream& out, point p);

};



inline bool double_equals_precision(
                double val1,
                double val2,
                double precision)
{
    return fabs(val1 - val2) < fabs(precision);
}

inline bool double_equals(
                double val1,
                double val2)
{
    return double_equals_precision(val1, val2, 0.0001);
}

inline double radians_to_degrees(
                double val)
{
    return val * 180. / PI;
}

inline double degrees_to_radians(
                double val)
{
    return val * PI / 180.;
}

inline bool iszero(
                double val)
{
    return double_equals(val, 0);
}

point operator*(double value, point p);

point centre(point p1, point p2);

double distance(point p1, point p2);

double size(point vector);

point normalize(point p);

double angle(point p);

double angle(point p1, point centre, point p2);

point rotate(point p, point centre, double alpha);

point orthogonal(point p);

point orthogonal(point p, point direction);

point move_point(point p, point move_to, double length);

bool lies_on_line(point p1, point p2, point p3);

bool lies_between(point p, point from, point to);


#endif /* !POINT_HPP */

