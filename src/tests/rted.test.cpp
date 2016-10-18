/*
 * File: rted.test.cpp
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */


#include "rted.test.hpp"
#include "rted.hpp"

// == figure 4, str. 337
#define LABELS1      "1234565731"
#define BRACKETS1    "(.(.(.).))"

// == figure 6, str. 341
#define LABELS21     "3123"
#define BRACKETS21   "(..)"
#define LABELS22     "212"
#define BRACKETS22   "(.)"

using namespace std;

rted_test::rted_test()
    : test("rted")
{ }

void rted_test::run()
{
    APP_DEBUG_FNAME;

    test_rted(BRACKETS1, LABELS1, BRACKETS1, LABELS1,
            [this](strategy str)
            {
                assert_true(str.is_right());
            });
    test_rted(BRACKETS21, LABELS21, BRACKETS22, LABELS22,
            [this](strategy str)
            {
                assert_true(str.is_left());
            });
}

template<typename funct>
void rted_test::test_rted(
                const std::string& b1,
                const std::string& l1,
                const std::string& b2,
                const std::string& l2,
                funct test_funct)
{
    rna_tree rna1(b1, l1, "rna1");
    rna_tree rna2(b2, l2, "rna2");
    rna_tree::iterator it1, it2;
    it1 = rna_tree::first_child(rna1.begin());
    it2 = rna_tree::first_child(rna2.begin());

    DEBUG("%s <-> %s", clabel(it1), clabel(it2));

    rted r(rna1, rna2);

    r.run();
    strategy_table_type val = r.get_strategies();

    test_funct(val.at(id(it1)).at(id(it2)));
}

