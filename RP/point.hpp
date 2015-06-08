/*
 * File: point.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard@ba30.eu>
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


#include <iostream>

struct Point
{
    double x, y;

    friend std::ostream& operator<< (std::ostream& out, Point p);
    Point operator+(Point other) const;
    Point operator-(Point other) const;
    Point operator/(double value) const;
    Point operator*(double value) const;
    bool operator==(Point other) const;

    Point swap_xy() const;
    std::string to_string() const;
    bool bad() const;
    static Point bad_point();
};

Point centre(Point p1, Point p2);

//double distance(Point p1, Point p2);

double size(Point vector);

Point normalize(Point p);

double angle(Point p);



#endif /* !POINT_HPP */

