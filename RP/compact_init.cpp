/*
 * File: compact_init.cpp
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

#include "compact_init.hpp"

using namespace std;


compact::init::init(
                rna_tree& _rna)
    : rna(_rna)
{
    run();
}

void compact::init::run()
{
    APP_DEBUG_FNAME;

    iterator it;
    point p;

    for (it = rna.begin(); it != rna.end(); ++it)
    {
        if (it->inited_points() || !it->paired())
            continue;

        assert(!it->inited_points() && it->paired());

        p = rna_tree::parent(it)->centre();
        if (!init_branch_recursive(it, p).bad())
        {
            rna.print_subtree(it);
            DEBUG("INIT OK");
            continue;
        }

        DEBUG("ELSE");
    }
}

/* static */ void compact::init::shift_branch(
                iterator parent,
                point vector)
{
    APP_DEBUG_FNAME;

    function<size_t(iterator)> recursion =
        [&recursion, &vector](iterator it) {

            sibling_iterator ch;
            size_t out = 1;

            for (ch = it.begin(); ch != it.end(); ++ch)
                out += recursion(ch);

            if (it->inited_points())
                for (size_t i = 0; i < it->size(); ++i)
                    it->at(i).p += vector;

            return out;
        };

    size_t n;
    n = recursion(parent);
    DEBUG("shift #%lu '%s'", n, to_cstr(vector));
}

/* static */ void compact::init::rotate_branch(
                iterator parent,
                double alpha)
{
    APP_DEBUG_FNAME;

    function<size_t(iterator)> recursion =
        [&recursion, &alpha](iterator it) {

            sibling_iterator ch;
            size_t out = 1;


            return out;
        };
}

compact::sibling_iterator compact::init::get_prev(
                sibling_iterator it)
{
    while (!rna_tree::is_first_child(it))
    {
        --it;

        if (it->inited_points())
            break;
    }
    if (it->inited_points())
        return it;
    else
        abort();
}

compact::sibling_iterator compact::init::get_next(
                sibling_iterator it)
{
    while (!rna_tree::is_last_child(it))
    {
        ++it;

        if (it->inited_points())
            break;
    }
    if (it->inited_points())
        return it;
    else
        abort();
}

point compact::init::init_branch_recursive(
                sibling_iterator it,
                point from)
{
    point p;

    if (it->inited_points())
    {
        p = normalize(it->centre() - from) * BASES_DISTANCE;
        shift_branch(it, p);

        return p;
    }
    if (rna_tree::is_leaf(it))
        return point::bad_point();
    if (rna_tree::first_child(it) == rna_tree::last_child(it))  // has 1 child
    {
        iterator ch = rna_tree::first_child(it);
        p = init_branch_recursive(ch, from);
        if (!p.bad())
        {
            if (ch->paired())
            {
                it->at(0).p = ch->at(0).p - p;
                it->at(1).p = ch->at(1).p - p;
            }
            else
            {
                it->at(0).p = ch->at(0).p - p;
                it->at(1).p = orthogonal(ch->at(0).p) * PAIRS_DISTANCE - p;
            }

            return p;
        }
        return point::bad_point();
    }

    return point::bad_point();
}







