/*
 * File: point.cpp
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


#include <iomanip>
#include <cmath>

#include "point.hpp"
#include "types.hpp"

using namespace std;

#define BAD_POINT     Point({0xBADF00D, 0xBADF00D})


Point::Point()
{
    *this = BAD_POINT;
}

Point::Point(std::initializer_list<double> l)
{
    x = *l.begin();
    y = *(l.begin() + 1);
}


Point Point::operator+(Point other) const
{
    return Point({x + other.x, y + other.y});
}

Point Point::operator-(Point other) const
{
    return Point({x - other.x, y - other.y});
}

Point Point::operator-() const
{
    return Point({-x, -y});
}

Point Point::operator*(double val) const
{
    return Point({ x * val, y * val});
}

Point Point::operator/(double val) const
{
    assert (val != 0);
    return Point({x / val, y / val});
}

bool Point::operator==(Point other) const
{
    return x == other.x && y == other.y;
}

std::ostream& operator<< (std::ostream& out, Point p)
{
    if (p.bad())
        out << "0xBADF00D 0xBADF00D";
    else
        out 
            << std::fixed
            << std::setprecision(2)
            << p.x
            << " "
            << std::setprecision(2)
            << p.y;

    return out;
}

std::string Point::to_string() const
{
    stringstream str;
    str << *this;
    return str.str();
}

Point Point::swap_xy() const
{
    return Point({y, x});
}

bool Point::bad() const
{
    return *this == bad_point();
}

/* static */ Point Point::bad_point()
{
    return BAD_POINT;
}


// GLOBAL FUNCTIONS:

Point centre(Point p1, Point p2)
{
    return (p1 + p2) / 2;
}

double distance(Point p1, Point p2)
{
    return size(p2 - p1);
}

double size(Point vector)
{
    return sqrt(vector.x * vector.x + vector.y * vector.y);
}

Point normalize(Point p)
{
    assert(size(p) != 0);
    return p / size(p);
}

double angle(Point p)
{
    // pre 3., 4. kvadrant vracia zaporne cisla.. takto sa to vyriesi..
    double out = fmod(radians_to_degrees(atan2(p.y, p.x)) + 360, 360);

    DEBUG("angle(%s)=\t%f", p.to_string().c_str(), out);
    assert(isnormal(out) || out == 0);

    return out;
}

double angle(Point p1, Point centre, Point p2)
{
    return fmod(angle(p2 - centre) - angle(p1 - centre) + 360, 360);
}

Point orthogonal(Point p)
{
    return normalize(Point({p.y, -p.x}));
}

Point orthogonal(Point p, Point direction)
{
    Point o = orthogonal(p);
    if (distance(o, direction) < distance(-o, direction))
        return o;
    else
        return -o;
}

