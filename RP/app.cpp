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
#include "macros.hpp"
#include "ps.hpp"
#include "compact_maker.hpp"


using namespace std;


#define is(_iter, _status) \
    (_iter->get_label().status == rna_pair_label::_status)


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

    string labels1      = read_file(SEQ(first));
    string brackets1    = read_file(FOLD(first));

    string labels2      = read_file(SEQ(second));
    string brackets2    = read_file(FOLD(second));

    string fileIn       = PS_IN(first);
    string fileOut      = PS_OUT(first, second);

    psout = ps::init(fileOut);

    auto map = mapping::read_mapping_file(MAP(first, second));

    rna_tree rna1(brackets1, labels1, first);
    rna_tree rna2(brackets2, labels2, second);

    document doc = read_ps(fileIn);
    doc.rna = rna1;
    doc.update_rna_points();

    psout.print_to_ps(doc.prolog);
    doc.rna.merge(rna2, map);
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










