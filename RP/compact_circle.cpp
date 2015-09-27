/*
 * File: compact_circle.cpp
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

#include "compact_circle.hpp"

using namespace std;

#define CIRCLE_POINTS_INITED() \
    assert(!(centre.bad() || p1.bad() || \
                p2.bad() || direction.bad()))
#define CIRCLE_SGN_INITED() \
    assert(sgn == 1 || sgn == -1)

#define PI      M_PI

double compact::circle::radius() const
{
    CIRCLE_POINTS_INITED();
    assert(double_equals(distance(p1, centre),
                distance(p2, centre)));

    return distance(p1, centre);
}

double compact::circle::segment_angle() const
{
    CIRCLE_POINTS_INITED();
    CIRCLE_SGN_INITED();

    if (sgn == 1)
        return angle(p1, centre, p2);
    else
        return angle(p2, centre, p1);
}

double compact::circle::segment_length() const
{
    return 2. * PI * radius() * segment_angle() / 360.;
}

bool compact::circle::lies_in_segment(const point& p) const
{
    CIRCLE_POINTS_INITED();

    // angle(p1, centre, p) + angle(p, centre, p2) == segment_angle()
    assert(
            double_equals(
                fmod(angle(p1, centre, p) + angle(p, centre, p2) + 360, 360),
                fmod(segment_angle() + 360, 360)));

    if (sgn == 1)
        return angle(p1, centre, p) < segment_angle();
    else
        return angle(p2, centre, p) < segment_angle();
}

point compact::circle::rotate(double delta) const
{
    CIRCLE_POINTS_INITED();
    CIRCLE_SGN_INITED();
    assert(double_equals(distance(p1, centre), distance(p2, centre)));

    double r = radius();
    double alpha = sgn * delta + angle(p1 - centre);

    point p({
            centre.x + r * cos(degrees_to_radians(alpha)),
            centre.y + r * sin(degrees_to_radians(alpha))
            });

    if (!lies_in_segment(p))
        WARN("output node does not lie in circle segment");

    return p;
}

std::vector<point> compact::circle::split(size_t n) const
{
    vector<point> vec;
    double delta = segment_angle() / (double)(n + 1);

    for (size_t i = 1; i <= n; ++i)
        vec.push_back(rotate(delta * i));

    return vec;
}


std::ostream& operator<<(
                std::ostream& out,
                const compact::circle& c)
{
    out
        << "circle: p1="
        << c.p1
        << "; p2="
        << c.p2
        << "; centre="
        << c.centre
        << "; direction="
        << c.direction
        << "; radius="
        << c.radius()
        << "; seg_length="
        << c.segment_length()
        << "; seg_angle="
        << c.segment_angle();

    return out;
}


