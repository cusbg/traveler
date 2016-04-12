/*
 * File: point.test.cpp
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

#include "point.test.hpp"

using namespace std;

#define point_0_0 point(0, 0)

#define point_0_1 point(0, 1)
#define point_0_2 point(0, 2)
#define point_0_3 point(0, 3)

#define point_1_1 point(1, 1)
#define point_1_0 point(1, 0)
#define point_0_m1 point(0, -1)
#define point_m1_0 point(-1, 0)
#define point_90deg point(1, 0)

#define point_bad point({0xBADF00D, 0xBADF00D})

test_point::test_point()
    : test("point")
{ }

void test_point::run()
{
    APP_DEBUG_FNAME;

    test_basics();
    test_operations();
    test_functions();
}

void test_point::test_basics()
{
    APP_DEBUG_FNAME;
    
    assert_equals(point::bad_point(), point::bad_point());
    assert_true((point_bad).bad());
    
    assert_equals(to_string(point_bad), "0xBADF00D 0xBADF00D");
    assert_equals(to_string(point_0_1), "0.00 1.00");

    assert_true(double_equals(1, 1.00009));
    assert_true(double_equals_precision(1, 10, 10));
    assert_true(double_equals(0.0, -0.0));
}

void test_point::test_functions()
{
    assert_equals(centre(point_0_1, point_0_3), point_0_2);
    assert_equals(centre(point_0_m1, point_0_3), point_0_1);
    assert_equals(size(point_0_3), 3);
    assert_equals(normalize(point_0_3), point_0_1);

    assert_equals(angle(point_90deg), 0);
    assert_equals(angle(point_0_1), 90);

    assert_equals(rotate(point_0_0, 0, 1), point_1_0);
    assert_equals(rotate(point_0_0, 45, sqrt(2.)), point_1_1);
    assert_equals(rotate(point_0_0, 90, 1), point_0_1);
    assert_equals(rotate(point_0_0, 180, 1), point_m1_0);
    assert_equals(rotate(point_0_0, 270, 1), point_0_m1);
    assert_equals(rotate(point_0_0, 360, 1), point_1_0);

    assert_equals(orthogonal(point_90deg), point_0_m1);
    assert_equals(orthogonal(point_90deg, point_0_1), point_0_1);

    assert_equals(move_point(point_0_1, point_0_2, 2), point_0_3);

    assert_true(lies_on_line(point_0_1, point_0_2, point_0_3));
    assert_false(lies_on_line(point_0_1, point_0_2, point_90deg));

    assert_true(lies_between(point_0_2, point_0_1, point_0_3));
    assert_false(lies_between(point_0_1, point_0_2, point_90deg));
}

void test_point::test_operations()
{
    APP_DEBUG_FNAME;

    assert_equals(point_0_1 + point_0_2, point_0_3);
    assert_equals(point_0_3 - point_0_1, point_0_2);
    assert_equals(point_0_2 / 2, point_0_1);
    assert_equals(point_0_1 * 2, point_0_2);
    assert_equals(2 * point_0_1, point_0_2);
    assert_equals(point_0_1 / point_1_1, point_0_1);

    assert_equals(-point_0_m1, point_0_1);

    assert_fail(point_0_1 / 0);
    assert_fail(point_0_1 * 0);
    assert_fail(point_0_1 + (point_bad));
    assert_fail(point_0_2 / point_0_1);
}

