/*
 * File: compact_utils.cpp
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

#include "compact_utils.hpp"
#include "write_ps_document.hpp"

#define remake_child(parent, id) \
    (is(parent, rna_pair_label::inserted) || \
    contains(parent->remake_ids, id))

using namespace std;

void compact::intervals::init(
                iterator parent)
{
    APP_DEBUG_FNAME;

    assert(!rna_tree::is_leaf(parent));

    sibling_iterator ch;
    size_t i = 0;

    vec.assign(1, interval());
    vec.back().beg = {parent, 0};

    for (ch = parent.begin(); ch != parent.end(); ++ch, ++i)
    {
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
    //if (is(parent, rna_pair_label::inserted))
        //vec.back().remake = true;
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
#undef set_type
    }

    rna_tree::print_subtree(parent);
    DEBUG("created %s: %s", to_cstr(type), to_cstr(vec.back()));
}

point compact::intervals::get_circle_direction() const
{
    assert(!vec.empty());

    if (type == hairpin)
        return rna_tree::parent(vec.back().beg.it)->centre();

    size_t n = 0;
    point psum = {0, 0};

    for (const auto& i : vec)
    {
        psum += i.beg.it->centre();
        ++n;
    }
    psum = psum / n;
    DEBUG("direction = %s", to_cstr(psum));
    return psum;
}

std::string to_string(
                const compact::interval& i)
{ 
    stringstream str;
    str
        << "INTERVAL: "
        << label(i.beg.it)
        << ":"
        << (int)i.beg.index
        << "| "
        << label(i.end.it)
        << ":"
        << (int)i.end.index;
    str << endl;
    for (auto val : i.vec)
        str << clabel(val) << " ";

    return str.str();
}

std::string to_string(compact::intervals::rna_structure_type t)
{
#undef typeswitch
#define typeswitch(t) case compact::intervals::t : return #t;
    switch (t)
    {
        typeswitch(hairpin);
        typeswitch(multibranch_loop);
        typeswitch(interior_loop);
    }
    abort();
}

