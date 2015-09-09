/*
 * File: Point.cpp
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
#include <functional>

#include "point.hpp"
#include "types.hpp"

using namespace std;

#define BAD_POINT     Point({0xBADF00D, 0xBADF00D})

//#define OUTPUT_FUNCTIONS
#ifdef OUTPUT_FUNCTIONS

#define BINARY_OUTPUT(P1, P2) \
            { \
                APP_DEBUG_FNAME; \
                DEBUG("%s; %s", ::to_string(P1).c_str(), ::to_string(P2).c_str()); \
            }
#define UNARY_OUTPUT(P) \
            { \
                APP_DEBUG_FNAME; \
                DEBUG("%s", ::to_string(P).c_str()); \
            }

#else
#define BINARY_OUTPUT(P1, P2)
#define UNARY_OUTPUT(P)
#endif

Point::Point()
    : Point(BAD_POINT)
{ }

Point::Point(std::initializer_list<double> l)
{
    x = *l.begin();
    y = *(l.begin() + 1);
}


Point Point::operator+(Point other) const
{
    BINARY_OUTPUT(*this, other);

    return Point({x + other.x, y + other.y});
}

Point Point::operator-(Point other) const
{
    BINARY_OUTPUT(*this, other);

    return Point({x - other.x, y - other.y});
}

Point Point::operator-() const
{
    UNARY_OUTPUT(*this);

    return Point({-x, -y});
}

Point Point::operator*(double val) const
{
    BINARY_OUTPUT(*this, val);

    return Point({ x * val, y * val});
}

Point Point::operator/(double val) const
{
    BINARY_OUTPUT(*this, val);

    assert (val != 0);
    return Point({x / val, y / val});
}

bool Point::operator==(Point other) const
{
    BINARY_OUTPUT(*this, other);

    return double_equals(x, other.x) &&
        double_equals(y, other.y);
    //return x == other.x && y == other.y;
}

Point& Point::operator+=(Point other)
{
    *this = *this + other;
    return *this;
}

Point& Point::operator-=(Point other)
{
    *this = *this - other;
    return *this;
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

Point operator*(double value, Point p)
{
    return p * value;
}

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
    return sqrt(squared(vector.x) + squared(vector.y));
}

Point normalize(Point p)
{
    assert(size(p) != 0);
    return p / size(p);
}

double angle(Point p)
{
    double out = fmod(radians_to_degrees(atan2(p.y, p.x)) + 360, 360);

    assert(isnormal(out) || out == 0);

    return out;
}

double angle(Point p1, Point centre, Point p2)
{
    double out = fmod(angle(p2 - centre) - angle(p1 - centre) + 360, 360);

    assert(isnormal(out) || out == 0);

    return out;
}

Point orthogonal(Point p)
{
    return normalize(Point({p.y, -p.x}));
}

Point orthogonal(Point p, Point direction)
{
    Point o = orthogonal(p);

    assert(!double_equals(distance(o, direction), distance(-o, direction)));

    if (distance(o, direction) < distance(-o, direction))
        return o;
    else
        return -o;
}

Point move_point(Point p, Point move_to, double length)
{
    APP_DEBUG_FNAME;

    Point vec = normalize(move_to - p);

    return p + length * vec;
}

Point base_pair_edge_point(Point from, Point to)
{
    Point vec = {3, 3};
    vec = vec + normalize(to - from) * 4;
    return from + vec;
}

bool lies_on_line(Point p1, Point p2, Point p3)
{
    APP_DEBUG_FNAME;

    return lies_between(p1, p2, p3) ||
        lies_between(p2, p3, p1) ||
        lies_between(p3, p1, p2);
}

bool lies_between(Point p, Point from, Point to)
{
	p = p - from;
	to = to - from;

    cout << p << endl << to << endl;

    if (p.x == 0 &&
            p.y == 0 &&
            to.x == 0 &&
            to.y == 0)
        return true;

    if (p.x == 0 && to.x == 0)
    {
        assert(to.y != 0);
        return p.y / to.y <= 1;
    }
    if (p.y == 0 && to.y == 0)
    {
        assert(to.y != 0);
        return p.x / to.x <= 1;
    }
    if (to.x == 0 || to.y == 0)
        return false;
    assert(to.x != 0 && to.y != 0);
    return p.x / to.x <= 1 &&
        p.x / to.x >= 0 &&
        p.x / to.x == p.y / to.y;
}




