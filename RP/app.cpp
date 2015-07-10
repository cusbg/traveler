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

    labels1      = read_file(SEQ(first));
    brackets1    = read_file(FOLD(first));

    labels2      = read_file(SEQ(second));
    brackets2    = read_file(FOLD(second));

    fileIn       = PS_IN(first);
    fileOut      = PS_OUT(first, second);

    rna1 = rna_tree(brackets1, labels1, first);
    rna2 = rna_tree(brackets2, labels2, second);

    psout = ps::init(fileOut);

    auto map = mapping::read_mapping_file(MAP(first, second));

    doc = read_ps(fileIn);
    doc.rna = rna1;
    doc.update_rna_points();

    psout.print_to_ps(doc.prolog);
    print_color_help();
    rna_tree::merge(doc.rna, rna2, map);

    compact c(doc);
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










