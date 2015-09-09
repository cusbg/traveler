/*
 * File: Point.hpp
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


#include <ostream>

struct Point
{
    double x, y;

    Point();
    Point(std::initializer_list<double>);

    friend std::ostream& operator<< (std::ostream& out, Point p);
    Point operator+(Point other) const;
    Point operator-(Point other) const;
    Point operator-() const;
    Point operator/(double value) const;
    Point operator*(double value) const;
    Point& operator+=(Point other);
    Point& operator-=(Point other);
    bool operator==(Point other) const;

    Point swap_xy() const;
    std::string to_string() const;
    bool bad() const;
    static Point bad_point();
};

Point operator*(double value, Point p);

Point centre(Point p1, Point p2);

double distance(Point p1, Point p2);

double size(Point vector);

Point normalize(Point p);

double angle(Point p);

double angle(Point p1, Point centre, Point p2);

Point orthogonal(Point p);

Point orthogonal(Point p, Point direction);

Point move_point(Point p, Point move_to, double length);

Point base_pair_edge_point(Point from, Point to);

bool lies_on_line(Point p1, Point p2, Point p3);

bool lies_between(Point p, Point from, Point to);


template <typename T>
inline T squared(const T& t)
{
    return t * t;
}


namespace std
{
inline std::string to_string(Point p)
{
    return p.to_string();
}
}

#endif /* !POINT_HPP */

