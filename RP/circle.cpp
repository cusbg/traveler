/*
 * File: circle.cpp
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

#include "compact_maker.hpp"
#include "ps.hpp"

using namespace std;

std::ostream& operator<<(std::ostream& out, const compact::circle& c)
{
    out
        << "circle: p1="
        << c.p1
        << "; p2="
        << c.p2
        << "; centre="
        << c.centre
        << "; direction="
        << c.direction;

    return out;
}

Point compact::circle::rotate(double delta) const
{
    APP_DEBUG_FNAME;

    inited();
    assert(double_equals(size(p1 - centre), size(p2 - centre)));

    if (sgn != 1 && sgn != -1)
    {
        ERR("circle::sgn not/bad initialized");
        abort();
    }

    double r = radius();
    double a = sgn * delta + angle(p1 - centre);

    Point p({
            centre.x + r * cos(degrees_to_radians(a)),
            centre.y + r * sin(degrees_to_radians(a))
            });

    DEBUG("centre %s: point %s rotated by %.3g -> %s",
            centre.to_string().c_str(), p1.to_string().c_str(),
            delta, p.to_string().c_str());

    if (!lies_in_segment(p))
        WARN("output node does not lie in circle segment");

    return p;
}

std::vector<Point> compact::circle::split(size_t n) const
{
    vector<Point> vec;
    inited();

    double delta = segment_angle() / (double)(n + 1);
    for (size_t i = 1; i <= n; ++i)
        vec.push_back(rotate(delta * i));

    return vec;
}

void compact::circle::inited() const
{
    assert(!centre.bad() || !p1.bad() || !p2.bad() || !direction.bad());
}

bool compact::circle::lies_in_segment(Point p) const
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    inited();
    bool b1, b2;
    b1 = double_equals(
            fmod(angle(p1, centre, p) + angle(p, centre, p2) + 360, 360),
            segment_angle());
    b2 = double_equals(
            fmod(angle(p2, centre, p) + angle(p, centre, p1) + 360, 360),
            segment_angle());

    //INFO("p1cp=%f, pcp2=%f, pcp1=%f, p2cp=%f, sa=%f",
            //angle(p1, centre, p),
            //angle(p, centre, p2),
            //angle(p, centre, p1),
            //angle(p2, centre, p),
            //segment_angle());

    assert(b1 || b2);

    if (sgn == 1)
        return angle(p1, centre, p) < segment_angle();
    else
        return angle(p2, centre, p) < segment_angle();
}

void compact::circle::compute_sgn()
{
    APP_DEBUG_FNAME;

    inited();

    if (angle(p1, centre, direction) < angle(p1, centre, p2))
        sgn = -1;
    else
        sgn = 1;

    DEBUG("sgn=%i", (int)sgn);
#define cstr(x) (x).to_string().c_str()
    logger.debugStream() << *this;
    //DEBUG("circle: p1=%s, p2=%s, centre=%s, direction=%s",
            //cstr(p1), cstr(p2), cstr(centre), cstr(direction));

    assert(!lies_in_segment(direction));
}

double compact::circle::radius() const
{
    inited();
    assert(double_equals(distance(p1, centre), distance(p2, centre)));

    return distance(p1, centre);
}

double compact::circle::segment_angle() const
{
    assert (sgn == 1 || sgn == -1);
    inited();

    if (sgn == 1)
        return angle(p1, centre, p2);
    else
        return angle(p2, centre, p1);
}

double compact::circle::segment_length() const
{
    double out = 2 * M_PI * radius() * segment_angle() / 360;
    DEBUG("length=%f", out);
    return out;
}

void compact::circle::init(size_t n)
{
    assert(!p1.bad() || !p2.bad() || !direction.bad());

    centre = ::centre(p1, p2);
    compute_sgn();
    // ^^ zakladna verzia so stredom medzi bazami...

    size_t max_iterations = 50;
    double needed_length = min_circle_length(n);
    double actual_length = segment_length();
    double shift_size = needed_length / actual_length;
    Point shift_vector = normalize(orthogonal(p2 - p1));
    double difference_precision = 1;

    if (distance(centre + shift_vector, direction) < distance(centre - shift_vector, direction))
    {
        DEBUG(".");
        shift_vector = -shift_vector;
    }

    while (!double_equals_precision(actual_length, needed_length, difference_precision))
    {
        if (actual_length < needed_length)
        {
            DEBUG("A");
            centre = centre + shift_vector * shift_size;
        }
        else
        {
            centre = centre - shift_vector * shift_size;
        }
        shift_size = needed_length / actual_length;
        actual_length = segment_length();

        if (--max_iterations == 0)
        {
            INFO("max_iterations reached when finding circle, returning");
            break;
        }
    }

    INFO("inited for %lu nodes, radius=%f, length=%f", n, radius(), actual_length);
    INFO("min_radius=%f, max_radius=%f", min_circle_radius(n), max_circle_radius(n));

    //print(*this);
    //wait_for_input();
}




void print(compact::circle c)
{
    APP_DEBUG_FNAME;

    psout.print_to_ps(ps::print(red));
    psout.print_to_ps(ps::print_line(c.centre, c.direction));
    psout.print_to_ps(ps::print(c.direction, "d"));
    psout.print_to_ps(ps::print(c.p1, "p1"));
    psout.print_to_ps(ps::print(c.p2, "p2"));

    psout.print_to_ps(ps::print(blue));
    psout.print_to_ps(ps::print_line(c.centre, c.p1));
    psout.print_to_ps(ps::print_line(c.centre, c.p2));
    psout.print_to_ps(ps::print(black));

    int angle;
    angle = 10;

    for (int i = angle; i <= c.segment_angle(); i+=angle)
    {
        //wait_for_input();

        Point p = c.rotate(i);
        psout.print_to_ps(ps::print_line(c.centre, p));

    }
}

void f()
{
    APP_DEBUG_FNAME;

    compact::circle c;
    c.centre    = {238, -497};
    c.p1        = {228, -497};
    c.p2        = {248, -497};
    c.direction = {238, -489};

    c.compute_sgn();

    print(c);

    DEBUG("r=%f, seg_angle=%f, lies=%s",
            c.radius(),
            c.segment_angle(),
            to_string(c.lies_in_segment(c.direction)).c_str());

    abort();
}

