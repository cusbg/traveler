/*
 * File: overlap_checks.test.cpp
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


#ifndef TESTS
#define TESTS
#endif

#include "overlap_checks.hpp"
#include "overlap_checks.test.hpp"

using namespace std;

static ostream& operator<<(
            ostream& out, overlap_checks::edge e)
{
    out
        << "E: ("
        << e.p1
        << ") ~> ("
        << e.p2
        << ")";

    return out;
}

overlap_checks_test::overlap_checks_test()
    : test("overlaps")
{ }

void overlap_checks_test::run()
{
    APP_DEBUG_FNAME;

    vector<point> v1, v2;
    v1 = {{5, 0}, {0, 5}, {-5, 0}, {0, -5}};
    v2 = {{2, 2}, {-2, 2}, {-2, -2}, {-2, 2}};

    vector<bool> intersects(v1.size() * v2.size(), true);
    for (int i : {0, 1, 3, 4, 8, 9, 11, 12, 13, 15})
        intersects[i] = false;

    int i = 0;
    for (point p1 : v1)
        for (point p2 : v2)
            test_intersection(p1, p2, intersects[i++]);

    test_intersection({100, 0}, {10, -10}, true);
}

void overlap_checks_test::test_intersection(
                point p1,
                point p2,
                bool intersects)
{
    overlap_checks::edge e1, e2;
    static int i = 0;
    e1 = {{0, 0}, p1};
    e2 = {{10, 10}, p2};

    point intersection = overlap_checks::intersection(e1, e2);

    DEBUG("#i %i, e1 %s, e2 %s, intersection %s", i, to_cstr(e1), to_cstr(e2), to_cstr(intersection));
    ++i;
    assert_equals(!intersection.bad(), intersects);
}

