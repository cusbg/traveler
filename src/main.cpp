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

//#define TESTS

#ifdef TESTS

#include "tests/tests.cpp"

#else

#include <iostream>
#include <fstream>

#include "types.hpp"
#include "utils.hpp"
#include "app.hpp"
#include "generator.hpp"
#include "rna_tree.hpp"

#include "point.hpp"
#include "rted.hpp"
#include "gted.hpp"

using namespace std;

void f()
{
    generator::generate_gted();
    exit(0);

    rna_tree rna1, rna2;

    rna1 = get_rna("rabbit");
    rna2 = get_rna("frog");

    //rna1 = rna_tree(".((...(.))).",
                    //"123456787329");
    //rna2 = rna_tree("..(((..(.(.)).).))",
                    //"efghijklmnonlpiqhg");

    rted r(rna1, rna2);
    r.run();
    gted g(rna1, rna2, r.get_strategies());
    g.run();
    exit(0);
}


int main(int argc, char** argv)
{
    cout << boolalpha;
    srand(1);

    app app;
    app.run(vector<string>(argv, argv + argc));

    return 0;
}

#endif
