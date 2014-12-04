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
#include "types.hpp"
#include "tree_hh/tree.hh"
#include "tree_hh/tree_util.hh"
#include "node_base.hpp"
#include "tree_base.hpp"
#include "rna_tree.hpp"
#include <fstream>
#include "rted.hpp"
#include "tests.hpp"
#include "gted.hpp"




using namespace std;




string readseq()
{
    ifstream input("../InFiles/seq");
    string s;
    input >> s;
    return s;
}
string readbrackets()
{
    ifstream input("../InFiles/zatvorky");
    string s;
    input >> s;
    return s;
}


int main(int argc, char** argv)
{
    rted_tests();
    return 0;


    rna_tree rr(".", "a");
    rna_tree::last_child(rr.tree_ptr->begin());

    string b1, l1, b2, l2;
    l1 = LABELS1;
    b1 = BRACKETS1;
    l2 = LABELS22;
    b2 = BRACKETS22;

    rna_tree rna1(b1, l1);
    rna_tree rna2(b2, l2);

    rted r(rna1, rna2);

    gted g(rna1, rna2, r.run_rted());
    g.run_gted();



	return 0;
}





