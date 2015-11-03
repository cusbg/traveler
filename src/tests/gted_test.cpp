/*
 * File: gted_test.cpp
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
#include "../gted.hpp"
#include "../mapping.hpp"


// == figure 4, str. 337
#define LABELS1      "1234565731"
#define BRACKETS1    "(.(.(.).))"

// == figure 6, str. 341
#define LABELS21     "3123"
#define BRACKETS21   "(..)"
#define LABELS22     "212"
#define BRACKETS22   "(.)"

using namespace std;

static bool operator==(
                const mapping::mapping_pair p1,
                const mapping::mapping_pair& p2)
{
    return p1.from == p2.from &&
        p1.to == p2.to;
}

/* static */ void gted::test()
{
    LOGGER_PRIORITY_ON_FUNCTION(WARN);

    auto test_funct =
        [](rna_tree rna1, rna_tree rna2, size_t distance) {
            strategy_table_type STR(rna1.size(), strategy_table_type::value_type(rna2.size(), RTED_T1_LEFT));

            gted g(rna1, rna2);
            g.run(STR);
            auto m1 = g.get_mapping();
            assert(g.get_mapping().distance == distance);

            STR = strategy_table_type(rna1.size(), strategy_table_type::value_type(rna2.size(), RTED_T2_RIGHT));
            g.run(STR);
            auto m2 = g.get_mapping();

            assert(m1.distance == m2.distance); // no mapping::operator==
            for (auto val : m1.map)
                assert(contains(m2.map, val));
            for (auto val : m2.map)
                assert(contains(m1.map, val));
        };

    test_funct(rna_tree(BRACKETS21, LABELS21, "21"), rna_tree(BRACKETS22, LABELS22, "22"), 1);
    test_funct(rna_tree(BRACKETS1, LABELS1, "1"), rna_tree(BRACKETS21, LABELS21, "21"), 4);
    test_funct(rna_tree(BRACKETS1, LABELS1, "1"), rna_tree(BRACKETS22, LABELS22, "22"), 5);

    INFO("GTED: tests OK");
}


