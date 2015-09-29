/*
 * File: point.cpp
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


#include <iomanip>

#include "point.hpp"

using namespace std;

#define BAD_POINT       (point({0xBADF00D, 0xBADF00D}))
#define squared(val)    ((val) * (val))


//#define PRINT_FUNCTIONS

#ifdef PRINT_FUNCTIONS
#define BINARY_OUTPUT(P1, P2) \
            DEBUG("%s: %s; %s", __PRETTY_FUNCTION__, to_cstr(P1), to_cstr(P2));
#define UNARY_OUTPUT(P) \
            DEBUG("%s: %s", __PRETTY_FUNCTION__, to_cstr(P));
#else
#define BINARY_OUTPUT(P1, P2)
#define UNARY_OUTPUT(P)
#endif // PRINT_FUNCTIONS ^^

#define BINARY(P1, P2) \
    BINARY_OUTPUT(P1, P2); \
    assert(!(P1).bad() && !(P2).bad());

#define UNARY(P) \
    UNARY_OUTPUT(P); \
    assert(!(P).bad());

point::point()
    : point(BAD_POINT)
{ }

point::point(double _x, double _y)
    : x(_x), y(_y)
{ }

bool point::operator==(point other) const
{
    //BINARY(*this, other); // !!!

    return x == other.x && y == other.y;
}

std::ostream& operator<<(std::ostream& out, point p)
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


point point::operator+(point other) const
{
    BINARY(*this, other);

    return {x + other.x, y + other.y};
}

point point::operator-(point other) const
{
    BINARY(*this, other);

    return {x - other.x, y - other.y};
}

point point::operator-() const
{
    UNARY(*this);

    return {-x, -y};
}

point point::operator/(double value) const
{
    UNARY(*this);
    assert(value != 0 && !::isnan(x / value) && !::isnan(y / value));

    return {x / value, y / value};
}

point point::operator*(double value) const
{
    UNARY(*this);
    assert(value != 0 && !::isnan(x * value) && !::isnan(y * value));

    return {x * value, y * value};
}

point& point::operator+=(point other)
{
    BINARY(*this, other);

    *this = *this + other;
    return *this;
}

point& point::operator-=(point other)
{
    BINARY(*this, other);

    *this = *this - other;
    return *this;
}

bool point::bad() const
{
    //UNARY(*this); // !!!

    return *this == bad_point() ||
        ::isnan(x) || ::isnan(y);
}

/* static */ point point::bad_point()
{
    return BAD_POINT;
}

point operator*(double value, point p)
{
    return p * value;
}


// TODO: pridat do funkcii kontrolu na badpoint

point centre(point p1, point p2)
{
    BINARY(p1, p2);

    return (p1 + p2) / 2;
}

double distance(point p1, point p2)
{
    BINARY(p1, p2);

    return size(p2 - p1);
}

double size(point vector)
{
    UNARY(vector);

    return sqrt(squared(vector.x) + squared(vector.y));
}

point normalize(point p)
{
    UNARY(p);

    assert(size(p) != 0);
    return p / size(p);
}

double angle(point p)
{
    UNARY(p);

    double out = fmod(radians_to_degrees(atan2(p.y, p.x)) + 360, 360);

    assert(isnormal(out) || out == 0);

    return out;
}

double angle(point p1, point centre, point p2)
{
    BINARY(p1, p2);
    UNARY(centre);

    double out = fmod(angle(p2 - centre) - angle(p1 - centre) + 360, 360);

    assert(isnormal(out) || out == 0);

    return out;
}

point orthogonal(point p)
{
    UNARY(p);

    return normalize({p.y, -p.x});
}

point orthogonal(point p, point direction)
{
    BINARY(p, direction);

    point o = orthogonal(p);

    assert(!double_equals(distance(o, direction), distance(-o, direction)));

    if (distance(o, direction) < distance(-o, direction))
        return o;
    else
        return -o;
}

point move_point(point p, point move_to, double length)
{
    BINARY(p, move_to);

    point vec = normalize(move_to - p);

    return p + length * vec;
}

point base_pair_edge_point(point from, point to)
{
    BINARY(from, to);

    point vec = {3, 3};
    vec = vec + normalize(to - from) * 4;
    return from + vec;
}

bool lies_on_line(point p1, point p2, point p3)
{
    BINARY(p1, p2);
    UNARY(p3);

    return lies_between(p1, p2, p3) ||
        lies_between(p2, p3, p1) ||
        lies_between(p3, p1, p2);
}

bool lies_between(point p, point from, point to)
{
    BINARY(from, to);
    UNARY(p);

	p = p - from;
	to = to - from;

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


