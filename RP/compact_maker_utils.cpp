/*
 * File: compact_maker_utils.cpp
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

#include "compact_maker_utils.hpp"
#include "compact_circle.hpp"
#include "ps.hpp"

using namespace std;

void compact::intervals::interval::print()
{
    std::stringstream str;
    str
        << "INTERVAL: "
        << " indexes: "
        << (int)b_index << " " << (int)e_index
        << " it: beg: " << *begin << "; end: " << *end
        << " count: " << vec.size()
        << " remake: " << to_string(remake)
        << " VEC: ";
    for (auto val : vec)
        str << *val << ";";

    logger.infoStream() << str.str();
}

Point compact::intervals::interval::get_circle_direction()
{
    APP_DEBUG_FNAME;

    assert(begin->get_label().inited_points());

    if (begin != end)   // != hairpin
        return begin->get_label().get_centre();
    else
        return rna_tree::parent(begin)->get_label().get_centre();
}

void compact::intervals::create(iterator it)
{
    APP_DEBUG_FNAME;

    sibling_iterator sib;
    interval i;
    size_t index = 0;
    const auto& remake = it->get_label().remake;

    assert(vec.empty());
    i.begin = it;
    i.b_index = 0;
    sib = it.begin();

    while(sib != it.end())
    {
        if (contains(remake, index))
            i.remake = true;

        if (sib->get_label().is_paired())
        {
            i.end = sib;
            i.e_index = 0;

            i.print();
            vec.push_back(i);

            i = interval();
            i.remake = false;
            i.begin = sib;
            i.b_index = 1;
        }
        else
            i.vec.push_back(sib);

        ++sib;
        ++index;
    }

    //assert(!vec.empty() || !i.vec.empty());

    i.end = it;
    i.e_index = 1;
    i.print();
    vec.push_back(i);

    rna_tree::print_subtree(it);
    assert(branches_count(it) + 1 == vec.size());
    switch (vec.size())
    {
#define set_type(t) type = t; DEBUG("type = %s", #t); break;
        case 1:
            set_type(hairpin);
        case 2:
            set_type(interior_loop);
        default:
            set_type(multibranch_loop);
#undef set_type
    }
}

Point compact::intervals::get_circle_direction() const
{
    APP_DEBUG_FNAME;

    Point p, p_sum;
    size_t n;

    n = 0;
    p_sum = {0, 0};

    for (auto i : vec)
    {
        p = i.begin->get_label().get_centre();
        assert(!p.bad());

        p_sum += p;
        ++n;
    }

    return p_sum / n;
}

Point get_direction(rna_tree::iterator it)
{
    APP_DEBUG_FNAME;

    Point p;
    rna_tree::iterator par;

    par = rna_tree::parent(it);

    assert(it->get_label().inited_points());
    assert(par->get_label().inited_points());

    p = normalize (
            it->get_label().get_centre() -
            rna_tree::parent(it)->get_label().get_centre()
            );

    return p;
}

