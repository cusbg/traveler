/*
 * File: compact_circle.test.cpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard@ba30.eu>
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

#ifndef TEST
#define TEST
#endif

#include "compact_circle.test.hpp"
#include "compact_circle.hpp"

using namespace std;

compact_circle_test::compact_circle_test()
    : test("compact_circle_test")
{ }

static compact::circle create_valid_circle()
{
    APP_DEBUG_FNAME;

    compact::circle c;
    c.p1 = {0, 10};
    c.p2 = {0, -10};
    c.centre = {0, 0};
    c.direction = {10, 0};
    // radius = 10
    // segment_angle = 90
    // segment_lenght = 10 * PI / 4
    c.compute_sgn();

    return c;
}

static compact::circle create_invalid_circle()
{
    APP_DEBUG_FNAME;

    compact::circle c;
    c.p1 = {0, 10};
    c.p2 = {0, -10};
    c.centre = {0, 0};
    c.direction = {10, 0};
    c.compute_sgn();
    c.p1 = {100, 100};

    return c;
}

void compact_circle_test::run()
{
    APP_DEBUG_FNAME;

    test_operations_success();
    test_operations_fail();
}


void compact_circle_test::test_operations_fail()
{
    APP_DEBUG_FNAME;

#define circle create_invalid_circle()

    assert_fail(circle.compute_sgn());
    assert_fail(circle.radius());
    assert_fail(circle.segment_angle());
    assert_fail(circle.segment_length());
    assert_fail(circle.init(2));
    assert_fail(circle.split(3));
    assert_fail(circle.rotate(2));
    assert_fail(circle.lies_in_segment(point::bad_point()));

    assert_fail(create_valid_circle().lies_in_segment(point::bad_point()));

#undef circle
}


void compact_circle_test::test_operations_success()
{
    APP_DEBUG_FNAME;

#define circle create_valid_circle()

    assert_equals((int)circle.radius(), 10);
    assert_equals((int)circle.segment_angle(), 180);
    assert_equals((int)circle.segment_length(), (int)(10 * M_PI));
    assert_true(circle.lies_in_segment({-1, 0}));
    assert_equals(circle.split(10).size(), 10);

    assert_equals(circle.rotate(90), point({-10, 0}));
    assert_equals(circle.rotate(180), point({0, -10}));
    assert_equals(circle.rotate(-90), point({10, 0}));

#undef circle
}

