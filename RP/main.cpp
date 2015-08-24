/*
 * File: main.cpp
 *
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
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


#include <iostream>
#include <fstream>

#include "types.hpp"
#include "util.hpp"
#include "app.hpp"
#include "generator.hpp"

#include "point.hpp"
#include "compact_maker.hpp"
#include "compact_maker_utils.hpp"
#include "ps.hpp"

#include "checks.hpp"
#include "rna_tree_matcher.hpp"


#include "functs.hpp"

using namespace std;

int main(int argc, char** argv)
{
    cout << boolalpha;
    srand(1);

    functs();

    if (argc == 2)
    {
        generator::generate_files();
        exit(0);
    }

    string from, to;

    from = "human";
    to = "frog";

    //from = "mouse";
    //to = "human";
    //to = "rabbit";

    //from = "rabbit";
    //to = "human";
    from = "frog";
    to = "rabbit";

    swap(from, to);

    from = "rabbit";
    to = "frog";

    app a;
    a.run_between(from, to);
    //a.run_app();

    return 0;
}

void def_ps_init()
{
    APP_DEBUG_FNAME;

    string s = document::default_prologue();
    psout = ps::init("build/files/ps.ps");
    psout.print_to_ps(s);
}


