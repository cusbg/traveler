/*
 * File: rted_test.cpp
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
#include "../rted.hpp"


// == figure 4, str. 337
#define LABELS1      "1234565731"
#define BRACKETS1    "(.(.(.).))"

// == figure 6, str. 341
#define LABELS21     "3123"
#define BRACKETS21   "(..)"
#define LABELS22     "212"
#define BRACKETS22   "(.)"

using namespace std;

/* static */
void rted::test()
{
    APP_DEBUG_FNAME;
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    auto test_funct = [](string b1, string l1, string b2, string l2)
    {
        rna_tree rna1 = rna_tree(b1, l1);
        rna_tree rna2 = rna_tree(b2, l2);
        rted::tree_type::iterator it1, it2;
        it1 = rted::tree_type::first_child(rna1.begin());
        it2 = rted::tree_type::first_child(rna2.begin());

        DEBUG("%s <-> %s", clabel(it1), clabel(it2));

        rted r(rna1, rna2);

        r.run_rted();
        auto val = r.get_strategies();
        auto t1_id = id(it1);
        auto t2_id = id(it2);
        return val.at(t1_id).at(t2_id);
    };

    auto out1 = test_funct(BRACKETS1, LABELS1, BRACKETS1, LABELS1);
    //cout << out1 << endl;
    assert(out1.is_right());
    auto out2 = test_funct(BRACKETS21, LABELS21, BRACKETS22, LABELS22);
    //cout << out2 << endl;
    assert(out2.is_left());

    INFO("tests OK");
}


