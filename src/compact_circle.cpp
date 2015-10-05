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

#define BASES_RATIO                 1.4
#define BASES_DISTANCE_PRECISION    2

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
                fmod(segment_angle() + 360, 360)) ||
            double_equals(
                fmod(angle(p2, centre, p) + angle(p, centre, p1) + 360, 360),
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

    DEBUG("segment_angle: %f, n %lu", segment_angle(), n);

    for (size_t i = 1; i <= n; ++i)
        vec.push_back(rotate(delta * i));

    return vec;
}

/* static */ double compact::circle::min_circle_length(size_t nodes_count)
{
    double out;
    out = nodes_count * BASES_DISTANCE * BASES_RATIO;

    if (nodes_count == 0)
        out = BASES_DISTANCE;
    else if (nodes_count < 3)
        out += 3;

    DEBUG("min_circle_length(%lu) = %f", nodes_count, out);

    return out;
}

/* static */ double compact::circle::max_circle_length(size_t nodes_count)
{
    double out;
    out = nodes_count * BASES_DISTANCE * BASES_RATIO;

    if (nodes_count == 0)
        out = BASES_DISTANCE;
    else if (nodes_count < 3)
        out += 3;

    out *= 2;
    DEBUG("max_circle_length(%lu) = %f", nodes_count, out);

    return out;
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

void compact::circle::init(
                size_t n)
{
    CIRCLE_POINTS_INITED();
    CIRCLE_SGN_INITED();

    circle c = *this;
    if (n == 0)
        return;

    DEBUG("init(%lu, %s)", n, to_cstr(*this));

    size_t max_iterations = 100;
    double needed_length, actual_length, shift_size;
    point shift;
    bool lt;

    needed_length = min_circle_length(n) + BASES_DISTANCE_PRECISION;
    actual_length = segment_length();
    shift_size = 15;
    shift = -normalize(orthogonal(p2 - p1, direction - p1));

    while (--max_iterations > 0)
    {
        if (double_equals_precision(actual_length, needed_length,
                    BASES_DISTANCE_PRECISION))
        {
            INFO("circle::init OK");
            break;
        }
        shift = normalize(shift) * shift_size;
        
        if (actual_length < needed_length)
        {
            centre = centre + shift;
            lt = true;
        }
        else
        {
            centre = centre - shift;
            lt = false;
        }
        actual_length = segment_length();
        
        if ((lt && actual_length > needed_length) ||
                (!lt && actual_length < needed_length))
            shift_size /= 2.0;
    }
    if (max_iterations == 0)
        WARN("max_iterations reached");
    DEBUG("from circle \n%s -> to \n%s", to_cstr(c), to_cstr(*this));
}


void compact::circle::compute_sgn()
{
    CIRCLE_POINTS_INITED();

    DEBUG("%s", to_cstr(print_points()));
    assert(centre != direction);
    assert(!lies_on_line(p1, p2, direction));
    assert(!double_equals(angle(p1, centre, direction), 0)
            && !double_equals(angle(p1, centre, direction), 180));

    if (angle(p1, centre, direction) < angle(p1, centre, p2))
        sgn = -1;
    else
        sgn = 1;

    assert(!lies_in_segment(direction));
}

std::string compact::circle::print_points()
{
    stringstream out;

    out
        << "p1: "
        << p1
        << "| p2: "
        << p2
        << "| centre: "
        << centre
        << "| direction: "
        << direction;
    return out.str();
}

