/*
 * File: tests.cpp
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

#include "../types.hpp"
#include "rted.test.cpp"
#include "gted.test.cpp"
#include "overlap_checks.test.cpp"
#include "rna_tree.test.cpp"


using namespace std;

void tests()
{
    APP_DEBUG_FNAME;

    rna_tree::test();
    rted::test();
    gted::test();
    overlap_checks::test();    

    INFO("TESTS OK");
}

int main()
{
    tests();

    return 0;
}

