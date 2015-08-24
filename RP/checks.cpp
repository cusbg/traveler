/*
 * File: checks.cpp
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

#include "checks.hpp"
#include "rna_tree.hpp"

using namespace std;


struct crossing_check::edge
{
    Point p1, p2;
};


crossing_check::edges crossing_check::split_to_edges(rna_tree rna)
{
    APP_DEBUG_FNAME;

    edges vec;
    edge e;
    rna_tree::pre_post_order_iterator it;

    it = ++rna.begin_pre_post();

    e.p1 = it->get_label().lbl(get_label_index(it)).point;
    //++it;

    for (; !it->is_root(); ++it)
    {
        e.p2 = it->get_label().lbl(get_label_index(it)).point;

        DEBUG("%s\t->\t%s", e.p1.to_string().c_str(), e.p2.to_string().c_str());
        vec.push_back(e);
        ++it;
        e.p1 = e.p2;
    }

    for (auto val : vec)
        DEBUG("%s\t->\t%s", val.p1.to_string().c_str(), val.p2.to_string().c_str());

    return vec;
}

bool crossing_check::intersect(rna_tree rna)
{
    APP_DEBUG_FNAME;

    auto vec = split_to_edges(rna);
    edge e1, e2;

    for (size_t i = 0; i < vec.size(); ++i)
    {
        for (size_t j = 0; j < vec.size(); ++i)
        {
            if (i == j)
                continue;

            e1 = vec[i];
            e2 = vec[j];
        }
    }

    return false;
}

bool crossing_check::intersect(edge e1, edge e2)
{

}




