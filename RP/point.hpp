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

#include "types.hpp"

#define double_equals_precision(val1, val2, precision) \
    (abs(val1 - val2) < abs(precision))
#define double_equals(val1, val2) \
    double_equals_precision(val1, val2, 0.0001)

#define radians_to_degrees(x)   (x * 180 / M_PI)
#define degrees_to_radians(x)   (x * M_PI / 180)


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
    point operator/(double value) const;
    point operator*(double value) const;
    point& operator+=(point other);
    point& operator-=(point other);

    bool bad() const;
    static point bad_point();

    friend std::ostream& operator<<(std::ostream& out, point p);
};


point centre(point p1, point p2);

double distance(point p1, point p2);

double size(point vector);

point normalize(point p);

double angle(point p);

double angle(point p1, point centre, point p2);

point orthogonal(point p);

point orthogonal(point p, point direction);

point move_point(point p, point move_to, double length);

point base_pair_edge_point(point from, point to);

bool lies_on_line(point p1, point p2, point p3);

bool lies_between(point p, point from, point to);


#endif /* !POINT_HPP */

