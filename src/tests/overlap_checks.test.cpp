/*
 * File: overlap_checks_test.cpp
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

#define TESTS
#include "../overlap_checks.hpp"

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

/* static */ void overlap_checks::test()
{
    APP_DEBUG_FNAME;

    vector<point> v1, v2;
    v1 = {{5, 0}, {0, 5}, {-5, 0}, {0, -5}};
    v2 = {{2, 2}, {-2, 2}, {-2, -2}, {-2, 2}};

    bool b = false;
    edge e1, e2;

    e1.p1 = {0, 0};
    e2.p1 = {10, 10};

    for (point p1 : v1)
    {
        e1.p2 = p1;
        for (point p2 : v2)
        {
            e2.p2 = p2;
            point i = intersection(e1, e2);
            if (!i.bad())
            {
                DEBUG("edges (%s) (%s) intersect in (%s)",
                        to_cstr(e1), to_cstr(e2), to_cstr(i));
                b = true;
            }
        }
    }

    e1.p2 = {100, 0};
    e2.p2 = {10, -10};

    assert(!intersection(e1, e2).bad());
    assert(b);
}

