/*
 * File: rna_tree.test.cpp
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


#include "rna_tree.test.hpp"
#include "rna_tree.hpp"


#define LABELS          "1234565731"
#define BRACKETS        "(.(.(.).))"
#define CONSTRAINTS     ""

#define LABELS_DEL      "134565731"
#define BRACKETS_DEL    "((.(.).))"

#define INDEX       2


using namespace std;

rna_tree_test::rna_tree_test()
    : test("rna_tree")
{ }

void rna_tree_test::run()
{
    APP_DEBUG_FNAME;

    typedef rna_tree::iterator iterator;

    rna_tree rna(BRACKETS, CONSTRAINTS, LABELS);
    iterator it;

    assert_equals(rna.get_labels(), LABELS);
    assert_equals(rna.get_brackets(), BRACKETS);

    it = plusplus(rna.begin(), INDEX);

    assert_equals(rna.get_labels(it), "2");
    assert_equals(rna.get_brackets(it), ".");

    rna.erase(it);

    assert_equals(rna.get_labels(), LABELS_DEL);
    assert_equals(rna.get_brackets(), BRACKETS_DEL);

    it = plusplus(rna.begin(), INDEX);
    rna.insert(it, rna_pair_label("2"), 0);

    assert_equals(rna.get_labels(), LABELS);
    assert_equals(rna.get_brackets(), BRACKETS);
}


