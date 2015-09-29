/*
 * File: compact.cpp
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

#include "compact.hpp"
#include "write_ps_document.hpp"
#include "compact_circle.hpp"
#include "compact_utils.hpp"


using namespace std;

#define UPDATE_PS psout.seek(psout.print(psout.sprint(rna))); WAIT;

inline string to_string(const rna_tree::sibling_iterator& it)
{
    return label(it);
}

compact::compact(
                const rna_tree& _rna)
    : rna(_rna)
{ }

void compact::run()
{
    APP_DEBUG_FNAME;

    init();
    make();
}


/* static */ void compact::shift_branch(
                iterator parent,
                point vector)
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);
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

/* static */ void compact::rotate_branch(
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
    // TODO
    abort();
}

/* static */ void compact::set_distance(
                iterator parent,
                iterator child,
                double dist)
{
    APP_DEBUG_FNAME;

    assert(rna_tree::parent(child) == parent);

    point p1, p2, vec, shift;
    double actual;

    p1 = parent->centre();
    p2 = child->centre();
    vec = normalize(p2 - p1);
    actual = distance(p1, p2);
    shift = vec * (dist - actual);

    shift_branch(child, shift);
}



compact::sibling_iterator compact::get_onlyone_branch(
                sibling_iterator it)
{
    sibling_iterator ch, out;
    sibling_iterator bad;

    if (rna_tree::is_leaf(it))
        return bad;

    for (ch = it.begin(); ch != it.end(); ++ch)
    {
        if (ch->paired() && rna_tree::is_valid(out))
            return bad;
        else if (ch->paired())
            out = ch;
    }
    return out;
}



void compact::init()
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);
    APP_DEBUG_FNAME;

    iterator it;
    point p;

    for (it = rna.begin(); it != rna.end(); ++it)
    {
        if (it->inited_points() || !it->paired())
            continue;

        p = rna_tree::parent(it)->centre();

        assert(!it->inited_points() && it->paired());
        assert(!p.bad());

        if (!init_branch_recursive(it, p).bad())
        {
            DEBUG("INIT OK");
            continue;
        }
        else
            abort();
    }

    for (it = rna.begin(); it != rna.end(); ++it)
    {
        if (rna_tree::is_leaf(it))
            continue;
        if (!rna_tree::is_valid(get_onlyone_branch(it)))
        {
            for (sibling_iterator ch = it.begin(); ch != it.end(); ++ch)
                if (!rna_tree::is_leaf(ch))
                    even_branch(ch);
        }
    }
}

point compact::init_branch_recursive(
                sibling_iterator it,
                point from)
{
    APP_DEBUG_FNAME;

    point p;
    sibling_iterator ch;

    if (it->inited_points())
    {
        p = normalize(it->centre() - from) * BASES_DISTANCE;
        shift_branch(it, p);

        return p;
    }

    ch = get_onlyone_branch(it);
    if (sibling_iterator() == ch)
        return point::bad_point();

    assert(sibling_iterator() != ch);

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

void compact::even_branch(
                sibling_iterator it)
{
    //APP_DEBUG_FNAME;
    assert(!rna_tree::is_leaf(it));

    vector<sibling_iterator> vec;

    vec.push_back(it);

    while(!rna_tree::is_leaf(it))
    {
        it = get_onlyone_branch(it);
        if (!rna_tree::is_valid(it))
            break;
        vec.push_back(it);
    }

    point p1, p2, p, shift, newpos;

    if (vec.size() < 2)
        return;

    p1 = vec[0]->at(0).p;
    p2 = vec[0]->at(1).p;
    if (!double_equals(distance(p1, p2), BASES_DISTANCE))
    {
        p = centre(p1, p2);
        p1 = move_point(p, p1, PAIRS_DISTANCE / 2);
        p2 = move_point(p, p2, PAIRS_DISTANCE / 2);

        vec[0]->at(0).p = p1;
        vec[0]->at(1).p = p2;
    }

    shift = orthogonal(p1 - p2, vec[1]->centre() - p2);

    for (size_t i = 1; i < vec.size(); ++i)
    {
        shift = normalize(shift) * distance(vec[0]->centre(), vec[i]->centre());
        newpos = p1 + shift;

        p = newpos - vec[i]->at(0).p;
        for (it = vec[i-1].begin(); it != vec[i]; ++it)
        {
            assert(rna_tree::is_leaf(it));
            shift_branch(it, p);
        }
        newpos = p2 + shift;
        p = newpos - vec[i]->at(1).p;
        for (it = ++sibling_iterator(vec[i]); it != vec[i-1].end(); ++it)
        {
            assert(rna_tree::is_leaf(it));
            shift_branch(it, p);
        }


        vec[i]->at(0).p = p1 + shift;
        vec[i]->at(1).p = p2 + shift;
    }
    for (it = vec.back().begin(); it != vec.back().end(); ++it)
        shift_branch(it, p);
}


void compact::reinsert(
                const circle& c,
                const nodes_vec& nodes)
{
    APP_DEBUG_FNAME;

    if (nodes.empty())
        return;

    assert(!nodes.empty());
    LOGGER_PRINT_CONTAINER(nodes, "nodes");

    points_vec points = c.split(nodes.size());

    assert(points.size() == nodes.size());
    for (size_t i = 0; i < points.size(); ++i)
    {
        assert(rna_tree::is_leaf(nodes[i]));

        nodes[i]->set_points_exact(points[i], 0);
        if (is(nodes[i], rna_pair_label::touched))
            nodes[i]->status = rna_pair_label::reinserted;
    }
}

compact::circles_vec compact::get_circles(
                const intervals& in)
{
    APP_DEBUG_FNAME;

    circles_vec cvec;
    circle c;
    point dir = in.get_circle_direction();

    assert(!in.vec.empty());

    for (const auto& i : in.vec)
    {
        c.p1 = i.beg.it->at(i.beg.index).p;
        c.p2 = i.end.it->at(i.end.index).p;
        c.direction = dir;
        c.centre = centre(c.p1, c.p2);
        c.compute_sgn();
        c.init(i.vec.size());
        cvec.push_back(c);
    }

    return cvec;
}


void compact::make()
{
    APP_DEBUG_FNAME;
    
    iterator it;
    circles_vec cvec;
    intervals in;

    for (it = rna.begin(); it != rna.end(); ++it)
    {
        if (it->remake_ids.empty())
            continue;

        in.init(it);
        set_distances(in);
        cvec = get_circles(in);

        assert(in.vec.size() == cvec.size());
        for (size_t i = 0; i < in.vec.size(); ++i)
            if (in.vec[i].remake)
                reinsert(cvec[i], in.vec[i].vec);
    }
}

void compact::set_distances(
                intervals& in)
{
    APP_DEBUG_FNAME;

    switch (in.type)
    {
        case intervals::hairpin:
            break;
        case intervals::interior_loop:
            set_distance_interior_loop(in);
            break;
        case intervals::multibranch_loop:
            set_distance_multibranch_loop(in);
            break;
    }
}

void compact::set_distance_interior_loop(
                intervals& in)
{
    APP_DEBUG_FNAME;
    assert(in.vec.size() == 2);

    iterator parent, child;
    double actual, avg;
    vector<double> l(2);

    parent = in.vec[0].beg.it;
    child = in.vec[0].end.it;

    actual = distance(parent->centre(), child->centre());
    l[0] = circle::min_circle_length(in.vec[0].vec.size());
    l[1] = circle::min_circle_length(in.vec[1].vec.size());
    sort(l.begin(), l.end());
    avg = l[0] + (l[1] - l[0])/4;

    if (avg < BASES_DISTANCE)
        avg = BASES_DISTANCE;

    if (!double_equals_precision(actual, avg, 1))
    {
        set_distance(parent, child, avg);
        in.vec[0].remake = true;
        in.vec[1].remake = true;
    }
    UPDATE_PS;
}

void compact::set_distance_multibranch_loop(
                intervals& in)
{
    APP_DEBUG_FNAME;

}

