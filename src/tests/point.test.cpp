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

#define point_0_1 point(0, 1)
#define point_0_2 point(0, 2)
#define point_0_3 point(0, 3)
#define point_0_m1 point(0, -1)

#define point_90deg point(1, 0)
#define point_bad point::bad_point()

test_point::test_point()
    : test("point")
{ }

void test_point::run()
{
    //LOGGER_PRIORITY_ON_FUNCTION(INFO);
    APP_DEBUG_FNAME;

    test_operations_success();
    test_operations_fail();
}

void test_point::test_operations_success()
{
    APP_DEBUG_FNAME;
    
    assert_equals(point::bad_point(), point::bad_point());
    assert_true((point_bad).bad());
    
    assert_equals(point_0_1 + point_0_2, point_0_3);
    assert_equals(point_0_3 - point_0_1, point_0_2);
    assert_equals(point_0_2 / 2, point_0_1);
    assert_equals(point_0_1 * 2, point_0_2);
    assert_equals(2 * point_0_1, point_0_2);

    assert_equals(-point_0_m1, point_0_1);

    assert_equals(to_string(point_bad), "0xBADF00D 0xBADF00D");
    assert_equals(to_string(point_0_1), "0.00 1.00");
}

void test_point::test_operations_fail()
{
    APP_DEBUG_FNAME;

    assert_fail(point_0_1 / 0);
    assert_fail(point_0_1 * 0);
    assert_fail(point_0_1 + (point_bad));


}

