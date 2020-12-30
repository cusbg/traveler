/*
 * File: compact_utils.cpp
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

#include "compact_utils.hpp"


using namespace std;

void compact::intervals::init(
                              iterator parent)
{
    DEBUG("init interval for parent %s", label(parent));

    if (rna_tree::is_leaf(parent)) return;
//    assert(!rna_tree::is_leaf(parent));
    
    sibling_iterator ch;
    size_t i = 0;
    
    vec.assign(1, interval());
    vec.back().beg = {parent, 0};
    
    for (ch = parent.begin(); ch != parent.end(); ++ch, ++i)
    {
        /*
         * We iterate through each child of the parent node and store in the vector elements
         * continuous regions between pairs of basepairs. Each such region (interval) then stores in its
         * vec vector a list of the non-paired bases between the edge basepairs defining the region.
         * In the end, the number of items in the vec defines the number of branches in the loop starting in the
         * parent. In the tree representation, this corresponds to the number of non-leaf descendants of the parent.
         */
        bool rmk = remake_child(parent, i);
        
        if (rmk)
            vec.back().remake = true;
        
        if (ch->paired())
        {
            vec.back().end = {ch, 0};
            vec.push_back(interval());
            vec.back().beg = {ch, 1};
        }
        else
            vec.back().vec.push_back(ch);
        
        if (rmk)
            vec.back().remake = true;
    }
    vec.back().end = {parent, 1};
    
    if (!parent->remake_ids.empty())
        if (*std::max_element(parent->remake_ids.begin(),
                              parent->remake_ids.end()) >= i)
            vec.back().remake = true;
    
    if (is(parent, rna_pair_label::inserted))
        for (auto & i : vec)
            i.remake = true;
    
    switch (vec.size())
    {
#define typeswitch(t) type = compact::intervals::t; break;
        case 1:
            typeswitch(hairpin);
        case 2:
            typeswitch(interior_loop);
        default:
            typeswitch(multibranch_loop);
#undef typeswitch
    }
}

point compact::intervals::get_circle_direction() const
{
    assert(!vec.empty());
    
    if (type == hairpin)
        return rna_tree::parent(vec.back().beg.it)->center();
    
    size_t n = 0;
    point psum = {0, 0};
    
    for (const auto& i : vec)
    {
        psum += i.beg.it->center();
        ++n;
    }
    psum = psum / n;
    return psum;
}
