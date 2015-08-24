/*
 * File: compact_circle.cpp
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

#include "compact_circle.hpp"
#include "ps.hpp"

using namespace std;


#define CIRCLE_INITED()             assert(!centre.bad() || !p1.bad() || !p2.bad() || !direction.bad())
#define CIRCLE_SGN_INITED()         assert(sgn == 1 || sgn == -1)

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

Point compact::circle::rotate(
                double delta) const
{
    CIRCLE_INITED();
    CIRCLE_SGN_INITED();
    assert(double_equals(size(p1 - centre), size(p2 - centre)));

    double r = radius();
    double alpha = sgn * delta + angle(p1 - centre);

    Point p({
            centre.x + r * cos(degrees_to_radians(alpha)),
            centre.y + r * sin(degrees_to_radians(alpha))
            });

    if (!lies_in_segment(p))
        WARN("output node does not lie in circle segment");

    return p;
}

std::vector<Point> compact::circle::split(
                size_t n) const
{
    CIRCLE_INITED();

    vector<Point> vec;
    double delta = segment_angle() / (double)(n + 1);

    for (size_t i = 1; i <= n; ++i)
        vec.push_back(rotate(delta * i));

    return vec;
}

bool compact::circle::lies_in_segment(
                Point p) const
{
    CIRCLE_INITED();

    bool b1, b2;

    b1 = double_equals(
            fmod(angle(p1, centre, p) + angle(p, centre, p2) + 360, 360),
            segment_angle());
    b2 = double_equals(
            fmod(angle(p2, centre, p) + angle(p, centre, p1) + 360, 360),
            segment_angle());

    assert(b1 || b2);

    if (sgn == 1)
        return angle(p1, centre, p) < segment_angle();
    else
        return angle(p2, centre, p) < segment_angle();
}

void compact::circle::compute_sgn()
{
    APP_DEBUG_FNAME;

    CIRCLE_INITED();
    assert(centre != direction);
    assert(!lies_on_line(p1, p2, direction));
    assert(!double_equals(angle(p1, centre, direction), 0)
            && !double_equals(angle(p1, centre, direction), 180));

    if (angle(p1, centre, direction) < angle(p1, centre, p2))
        sgn = -1;
    else
        sgn = 1;

    logger.debugStream()
        << "sgn="
        << (int)sgn
        << " "
        << *this;

    assert(!lies_in_segment(direction));
}

double compact::circle::radius() const
{
    CIRCLE_INITED();
    assert(double_equals(distance(p1, centre), distance(p2, centre)));

    return distance(p1, centre);
}

double compact::circle::segment_angle() const
{
    CIRCLE_INITED();
    CIRCLE_SGN_INITED();

    if (sgn == 1)
        return angle(p1, centre, p2);
    else
        return angle(p2, centre, p1);
}

double compact::circle::segment_length() const
{
    double out = 2 * M_PI * radius() * segment_angle() / 360;
    return out;
}

// TODO:
void compact::circle::init(
                size_t n)
{
    CIRCLE_INITED();
    CIRCLE_SGN_INITED();

    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    if (n == 0)
        return;

    size_t max_iterations = 100;
    double needed_length, actual_length, shift_size, difference_precision;
    Point shift_vector;

    difference_precision = 2;
    needed_length = min_circle_length(n) + difference_precision;
    actual_length = segment_length();
    shift_size = 15;
    shift_vector = -normalize(orthogonal(p2 - p1, direction - p1));

    INFO("START circle::init(%lu), need %f, has %f, diff %f, n %lu",
            n, needed_length, actual_length, difference_precision, n);

    while(true)
    {
        if (double_equals_precision(actual_length, needed_length, difference_precision))
        {
            INFO("init OK, length %f, iterations to end %lu", actual_length, max_iterations);
            break;
        }
        bool lt = true;
        if (actual_length < needed_length)
        {
            DEBUG("<");
            centre = centre + shift_vector * shift_size;
        }
        else
        {
            DEBUG(">");
            centre = centre - shift_vector * shift_size;
            lt = false;
        }
        actual_length = segment_length();

        if ((lt && actual_length > needed_length) ||
                (!lt && actual_length < needed_length))
        {
            DEBUG("<>");
            shift_size = shift_size / 2;
        }

        if (--max_iterations == 0)
        {
            INFO("init X, max_iterations reached when finding circle");
            break;
        }
    }
    INFO("END init, need %f, has %f, diff %f, n %lu", needed_length, actual_length, difference_precision, n);
}

/* static */ double compact::circle::min_circle_length(size_t nodes_count)
{
    double out = nodes_count * BASES_DISTANCE * BASES_RATIO;
    if (nodes_count == 0)
        out = BASES_DISTANCE;
    else if (nodes_count < 3)
        out += 3;
    return out;
}

/*
// TODO: vv
[> static <] double compact::circle::min_circle_radius(size_t nodes_count)
{
#define MIN_RAD_DIFF    0.6
    double out;
    out = sqrt(nodes_count) * nodes_count * MIN_RAD_DIFF * BASES_DISTANCE;
    //if (nodes_count == 1)
        //out = 2 * out;
    return out;
}

[> static <] double compact::circle::max_circle_radius(size_t nodes_count)
{
#define MAX_RAD_DIFF    0.8
    double out = nodes_count * MAX_RAD_DIFF * BASES_DISTANCE;
    //if (nodes_count == 1)
        //out = 2 * out;
    return out;
}
// TODO: ^^
*/

void compact::circle::draw()
{
    APP_DEBUG_FNAME;

    psout.print_to_ps(ps::print(red));
    psout.print_to_ps(ps::print_line(centre, direction));
    psout.print_to_ps(ps::print(direction, "d"));
    psout.print_to_ps(ps::print(p1, "p1"));
    psout.print_to_ps(ps::print(p2, "p2"));

    psout.print_to_ps(ps::print(blue));
    psout.print_to_ps(ps::print_line(centre, p1));
    psout.print_to_ps(ps::print_line(centre, p2));
    psout.print_to_ps(ps::print(black));

    int angle;
    angle = 10;

    for (int i = angle; i <= segment_angle(); i+=angle)
    {
        Point p = rotate(i);
        psout.print_to_ps(ps::print_line(centre, p));
    }
}



void compact::circle::tests() const
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    vector<Point> vecp = {
        {0,     100},
        {100,   100},
        {100,   0},
        {-100,  100},
        {-100,  0},
        {-100,  -100},
        {0,     -100},
    };
    vector<double> vecv = {
        0,
        45,
        90,
        135,
        180,
        225,
        270,
        315,
    };
    assert(vecp.size() == vecv.size());

    for (size_t i = 0; i < vecp.size(); ++i)
    {
        double x = angle(vecp[i]);
        double y = vecv[i];
        if (!double_equals(x, y))
        {
            ERR("not equal in %s: %f != %f",
                    vecp[i].to_string().c_str(), x, y);
        }
    }
}


