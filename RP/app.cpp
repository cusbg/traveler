/*
 * File: app.cpp
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

#include <fstream>

#include "util.hpp"
#include "app.hpp"
#include "rna_tree.hpp"
#include "types.hpp"
#include "ps.hpp"
#include "compact_maker.hpp"
#include "rna_tree_matcher.hpp"


using namespace std;



#define is(_iter, _status) \
    (_iter->get_label().status == rna_pair_label::_status)
#define has(_iter, _status) \
    (count_children_if(_iter, [](rna_tree::iterator __iter) {return is(__iter, rna_pair_label::_status);}) != 0)

void app::run_app()
{
    APP_DEBUG_FNAME;

    vector<string> vec = FILES;

    for (auto val1 : vec)
    {
        for (auto val2 : vec)
        {
            if (val1 == val2)
                continue;

            run_between(val1, val2);
        }
    }
}

void app::run_between(
                const std::string& first,
                const std::string& second)
{
    APP_DEBUG_FNAME;

    string labels1, labels2, brackets1, brackets2, fileIn, fileOut;
    rna_tree rna1, rna2;
    document doc;

    fileIn       = PS_IN(first);
    fileOut      = PS_OUT(first, second);

    rna1 = get_rna(first);
    rna2 = get_rna(second);

    psout = ps::init(fileOut);

    auto map = get_map(first, second);

    doc = read_ps(fileIn);
    doc.rna = rna1;
    doc.update_rna_points();

    psout.print_to_ps(doc.prolog);
    print_color_help();

    print_default(doc.rna);

    matcher m;
    m.run(doc.rna, rna2, map);

    //print_default(doc.rna);
    compact c(doc.rna);
    c.make_compact();
}

void app::print_default(const rna_tree& rna)
{
    APP_DEBUG_FNAME;

    psout.print_to_ps(ps::print(gray));
    for (auto it = rna.begin_pre_post(); it != rna.end_pre_post(); ++it)
    {
        //psout.print_pair(it);
        psout.print_to_ps(ps::print_normal(it));
        //psout.print_to_ps(ps::print_edge(it));
    }
    psout.print_to_ps(ps::print(black));
}

/* static */ rna_tree app::get_rna(const std::string& filename)
{
    APP_DEBUG_FNAME;

    string labels, brackets;
    rna_tree rna;

    labels = read_file(SEQ(filename));
    brackets = read_file(FOLD(filename));

    rna = rna_tree(brackets, labels, filename);

    return rna;
}

/* static */ mapping app::get_map(const std::string& first, const std::string& second)
{
    APP_DEBUG_FNAME;

    auto map = mapping::read_mapping_file(MAP(first, second));

    return map;
}

