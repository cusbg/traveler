/*
 * File: rna_tree.test.cpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard@ba30.eu>
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

#include "../rna_tree.hpp"

using namespace std;

#define LABELS          "1234565731"
#define BRACKETS        "(.(.(.).))"

#define LABELS_DEL      "134565731"
#define BRACKETS_DEL    "((.(.).))"

#define INDEX       2

/* static */
void rna_tree::test()
{
    APP_DEBUG_FNAME;

    rna_tree rna(BRACKETS, LABELS);
    iterator it;

    assert(rna.get_labels() == LABELS);
    assert(rna.get_brackets() == BRACKETS);

    it = plusplus(rna.begin(), INDEX);

    assert(get_labels(it) == "2");
    assert(get_brackets(it) == ".");

    rna.erase(it);

    assert(rna.get_labels() == LABELS_DEL);
    assert(rna.get_brackets() == BRACKETS_DEL);

    it = plusplus(rna.begin(), INDEX);
    rna.insert(it, rna_pair_label("2"), 0);

    assert(rna.get_labels() == LABELS);
    assert(rna.get_brackets() == BRACKETS);

    INFO("RNA_TREE: tests OK");
}


