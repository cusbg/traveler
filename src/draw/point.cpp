/*
 * File: point.cpp
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */


#include <iomanip>
#include <cmath>

#include "point.hpp"
#include "types.hpp"

using namespace std;


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
: point(bad_point())
{ }

point::point(double _x, double _y)
: x(_x), y(_y)
{ }

/* static */ const point& point::bad_point()
{
    static point bad = point({0xBADF00D, 0xBADF00D});
    return bad;
}

bool point::operator==(const point& other) const
{
    //BINARY(*this, other); // !!!
    
    return
    double_equals(x, other.x) &&
    double_equals(y, other.y);
}

std::ostream& operator<<(std::ostream& out, const point& p)
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


point point::operator+(const point& other) const
{
    assert(!(this->bad() || other.bad()))
    BINARY(*this, other);
    
    return {x + other.x, y + other.y};
}

point point::operator-(const point& other) const
{
    BINARY(*this, other);
    
    return {x - other.x, y - other.y};
}

point point::operator*(const point& other) const
{
    BINARY(*this, other);

    return {x * other.x, y * other.y};
}

point point::operator-() const
{
    UNARY(*this);
    
    return {-x, -y};
}

point point::operator/(const point& other) const
{
    BINARY(*this, other);
    for (double value : {other.x, other.y})
        assert(!iszero(value) && !std::isnan(x / value) && !std::isnan(y / value));
    
    return {x / other.x, y / other.y};
}

point point::operator/(double value) const
{
    UNARY(*this);
    assert(!iszero(value) && !std::isnan(x / value) && !std::isnan(y / value));
    
    return {x / value, y / value};
}

point point::operator*(double value) const
{
    UNARY(*this);
//    assert(!iszero(value) && !std::isnan(x * value) && !std::isnan(y * value));
    assert(!std::isnan(x * value) && !std::isnan(y * value));
    
    return {x * value, y * value};
}

bool point::bad() const
{
    //UNARY(*this); // !!!
    
    return *this == bad_point() ||
    std::isnan(x) || std::isnan(y);
}

point operator*(double value, const point& p)
{
    return p * value;
}



point center(const point &p1, const point &p2)
{
    BINARY(p1, p2);
    
    return (p1 + p2) / 2;
}

point center(const std::vector<point> points) {
    point c = point(0, 0);
    for(point p: points) {
        c += p;                
    }

    return  c / points.size();
}



double distance(const point& p1, const point& p2)
{
    BINARY(p1, p2);


    return size(p2 - p1);
}

double size(const point& vector)
{
    UNARY(vector);
    
    return sqrt(squared(vector.x) + squared(vector.y));
}

point normalize(const point& p)
{
    UNARY(p);

    if (size(p) == 0) {
        int a = 1;
    }
    assert(size(p) != 0);
    return p / size(p);
}

double angle(const point& p)
{
    UNARY(p);
    
    double out = fmod(radians_to_degrees(atan2(p.y, p.x)) + 360, 360);
    
    assert(isnormal(out) || out == 0);
    
    return out;
}

double angle(const point& p1, const point& centre, const point& p2)
{
    BINARY(p1, p2);
    UNARY(centre);
    
    double out = fmod(angle(p2 - centre) - angle(p1 - centre) + 360, 360);
    
    assert(isnormal(out) || out == 0);
    
    return out;
}

point rotate(const point& centre, double alpha, double radius)
{
    UNARY(centre);
    assert(!double_equals(radius, 0));
    
    point out({
        centre.x + radius * cos(degrees_to_radians(alpha)),
        centre.y + radius * sin(degrees_to_radians(alpha))
    });
    
    return out;
}

point rotate_point_around_pivot(const point& pivot, const point &p, double angle)
{
    double rad = M_PI / 180 * angle;
    float s = sin(rad);
    float c = cos(rad);

    point r = point(p);

    // translate point back to origin:
    r.x -= pivot.x;
    r.y -= pivot.y;

    // rotate point
    float xnew = r.x * c - r.y * s;
    float ynew = r.x * s + r.y * c;

    // translate point back:
    r.x = xnew + pivot.x;
    r.y = ynew + pivot.y;
    return r;
}

point orthogonal(const point& p)
{
    UNARY(p);
    
    return normalize({p.y, -p.x});
}

point orthogonal(const point& p, const point& direction)
{
    BINARY(p, direction);
    
    point o = orthogonal(p);
    
    assert(!double_equals(distance(o, direction), distance(-o, direction)));
    
    if (distance(o, direction) < distance(-o, direction))
        return o;
    else
        return -o;
}

point move_point(const point& p, const point& move_to, double length)
{
    BINARY(p, move_to);
    
    point vec = normalize(move_to - p);
    
    return p + length * vec;
}

bool lies_on_line(const point& p1, const point& p2, const point& p3)
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
    
    if (iszero(p.x) &&
        iszero(p.y) &&
        iszero(to.x) &&
        iszero(to.y))
        return true;
    
    if (iszero(p.x) && iszero(to.x))
    {
        assert(!iszero(to.y));
        return p.y / to.y <= 1;
    }
    
    if (iszero(p.y) && iszero(to.y))
    {
        assert(!iszero(to.x));
        return p.x / to.x <= 1;
    }
    
    if (iszero(to.x) || iszero(to.y))
        return false;
    
    assert(!iszero(to.x) && !iszero(to.y));
    
    return p.x / to.x <= 1 &&
    p.x / to.x >= 0 &&
    double_equals(p.x / to.x, p.y / to.y);
}

point abs(const point& p)
{
    UNARY(p);
    
    return {fabs(p.x), fabs(p.y)};
}



bool double_equals_precision(
                             double val1,
                             double val2,
                             double precision)
{
    return fabs(val1 - val2) < fabs(precision);
}

bool double_equals(
                   double val1,
                   double val2)
{
    return double_equals_precision(val1, val2, 0.0001);
}

double radians_to_degrees(
                          double val)
{
    return val * 180. / M_PI;
}

double degrees_to_radians(
                          double val)
{
    return val * M_PI / 180.;
}

bool iszero(
            double val,
            bool exact_test)
{
    return exact_test ? val == 0 : double_equals(val, 0);
}
