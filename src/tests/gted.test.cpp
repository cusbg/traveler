/*
 * File: gted.test.cpp
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


#include "gted.test.hpp"
#include "gted.hpp"
#include "mapping.hpp"


// == figure 4, str. 337
#define LABELS1      "1234565731"
#define BRACKETS1    "(.(.(.).))"

// == figure 6, str. 341
#define LABELS21     "3123"
#define BRACKETS21   "(..)"
#define LABELS22     "212"
#define BRACKETS22   "(.)"

using namespace std;

static ostream& operator<<(
                ostream& out,
                const mapping& map)
{
    out << "Mapping<>";
    return out;
}

static bool operator==(
                const mapping::mapping_pair& m1,
                const mapping::mapping_pair& m2)
{
    return m1.from == m2.from &&
        m1.to == m2.to;
}

static bool operator==(
                const mapping& m1,
                const mapping& m2)
{
    bool value = m1.distance == m2.distance;
    for (const auto& val : m1.map)
    {
        if (!contains(m2.map, val))
            value = false;
    }
    for (const auto& val : m2.map)
    {
        if (!contains(m1.map, val))
            value = false;
    }
    return value;
}

gted_test::gted_test()
    : test("gted")
{ }

void gted_test::run()
{
    test_gted(rna_tree(BRACKETS21, LABELS21, "21"), rna_tree(BRACKETS22, LABELS22, "22"), 1);
    test_gted(rna_tree(BRACKETS1, LABELS1, "1"), rna_tree(BRACKETS21, LABELS21, "21"), 4);
    test_gted(rna_tree(BRACKETS1, LABELS1, "1"), rna_tree(BRACKETS22, LABELS22, "22"), 5);
}

void gted_test::test_gted(
                rna_tree rna1,
                rna_tree rna2,
                size_t distance)
{
    strategy_table_type STR(rna1.size(), strategy_table_type::value_type(rna2.size(), RTED_T1_LEFT));

    gted g(rna1, rna2);
    g.run(STR);
    auto m1 = g.get_mapping();

    assert_equals(g.get_mapping().distance, distance);

    STR = strategy_table_type(rna1.size(), strategy_table_type::value_type(rna2.size(), RTED_T2_RIGHT));
    g.run(STR);
    auto m2 = g.get_mapping();

    assert_equals(m1, m2);
}

