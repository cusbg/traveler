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

#define UPDATE_PS
//#define UPDATE_PS psout.seek(psout.print(psout.sprint(rna))); WAIT;
#define toremake(iter) \
    (iter->paired() && \
        (!iter->remake_ids.empty() || \
        is(iter, rna_pair_label::inserted)))


void print_line(point from, point to)
{
    psout.print(psout.sprint(green));
    psout.print(psout.sprint_edge(from, to, false));
    psout.print(psout.sprint(from, "1"));
    psout.print(psout.sprint(to, "2"));
    psout.print(psout.sprint(black));
}


static inline string to_string(const rna_tree::sibling_iterator& it)
{
    return label(it);
}

compact::compact(
                rna_tree& _rna)
    : rna(_rna)
{ }

void compact::run()
{
    APP_DEBUG_FNAME;

    init();
    UPDATE_PS;
    make();
    UPDATE_PS;
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
    APP_DEBUG_FNAME;

    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    iterator it;
    point p;

    for (it = rna.begin(); it != rna.end(); ++it)
    {
        if (it->inited_points() || !it->paired())
            continue;

        p = rna_tree::parent(it)->centre();

        assert(!it->inited_points() && it->paired());
        assert(!p.bad());

        rna.print_subtree(it);

        if (!init_branch_recursive(it, p).bad())
        {
            DEBUG("INIT_rec OK");
        }
        else
        {
            rna.print_subtree(it);
            assert(rna_tree::is_valid(get_onlyone_branch(rna_tree::parent(it))));   // => 1 branch

            point p1, p2, vec;
            iterator par = rna_tree::parent(it);
            vec = normalize(par->centre() - rna_tree::parent(par)->centre());
            p1 = par->at(0).p + vec;
            p2 = par->at(1).p + vec;

            it->set_points_exact(p1, 0);
            it->set_points_exact(p2, 1);

            DEBUG("INIT OK");
        }
    }

    for (it = rna.begin(); it != rna.end(); ++it)
    {
        if (rna_tree::is_leaf(it))
            continue;
        if (!rna_tree::is_valid(get_onlyone_branch(it)))
        {
            for (sibling_iterator ch = it.begin(); ch != it.end(); ++ch)
                if (!rna_tree::is_leaf(ch))
                    make_branch_even(ch);
        }
    }
    INFO("compact::init() OK");
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
        return p;
    }

    ch = get_onlyone_branch(it);
    if (!rna_tree::is_valid(ch))
        return point::bad_point();

    assert(rna_tree::is_valid(ch));
    p = init_branch_recursive(ch, from);
    if (!p.bad())
    {
        shift_branch(it, p);
        if (ch->paired())
        {
            it->at(0).p = ch->at(0).p - p;
            it->at(1).p = ch->at(1).p - p;
        }
        else
        {
            abort();
            it->at(0).p = ch->at(0).p - p;
            it->at(1).p = orthogonal(ch->at(0).p) * PAIRS_DISTANCE - p;
        }

        return p;
    }

    return point::bad_point();
}

void compact::make_branch_even(
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
        if (!toremake(it))
            continue;

        in.init(it);
        set_distances(in);
        for (auto& i : in.vec)
            remake(i, in.get_circle_direction());
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

    rna.print_subtree(in.vec[0].beg.it);

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
}


double compact::get_length(
                const interval& in)
{
    if (in.vec.empty())
        return 0;

    double len = 0;
    point p1 = in.beg.it->at(in.beg.index).p;
    point p2;

    for (const auto& val : in.vec)
    {
        assert(!val->paired());
        if (val->inited_points())
        {
            p2 = val->at(0).p;
            len += distance(p1, p2);
            p1 = p2;
        }
    }
    p2 = in.end.it->at(in.end.index).p;
    len += distance(p1, p2);
    len = len / (in.vec.size() + 1);
    DEBUG("len = %f", len);
    return len;
}

void compact::split(
                const interval& in)
{
    APP_DEBUG_FNAME;

    vector<point> p1, p2;
    point p;
    double length;
    size_t i, j;

    length = get_length(in);

    p1.push_back(in.beg.it->at(in.beg.index).p);
    p2.resize(in.vec.size());
    for (const auto& i : in.vec)
        if (i->inited_points())
            p1.push_back(i->at(0).p);
    p1.push_back(in.end.it->at(in.end.index).p);
    p = p1[0];

    i = 1;
    j = 0;

    for (; j < p2.size(); ++j)
    {
        double l = length;
        while (distance(p, p1.at(i)) < l)
        {
            l -= distance(p, p1.at(i));
            p = p1[i];
            ++i;
        }
        p = move_point(p, p1.at(i), l);
        p2[j] = p;
    }
    DEBUG("i %lu, j %lu, p1 %lu, p2 %lu",
            i, j, p1.size(), p2.size());
    assert(j == p2.size());

    j = 0;
    for (auto val : in.vec)
        val->at(0).p = p2[j++];
}

void compact::remake(
                const interval& i,
                point direction)
{
    APP_DEBUG_FNAME;
    if (!i.remake)
        return;
    circle c;
    c.p1 = i.beg.it->at(i.beg.index).p;
    c.p2 = i.end.it->at(i.end.index).p;
    c.direction = direction;
    c.centre = centre(c.p1, c.p2);
    c.compute_sgn();
    c.init(i.vec.size());

    reinsert(c, i.vec);
}


void compact::set_distance_multibranch_loop(
                intervals& in)
{
#define MULTIBRANCH_MINIMUM_SPLIT   10

    APP_DEBUG_FNAME;

    for (size_t i = 0; i < in.vec.size(); ++i)
    {
        if (in.vec[i].vec.size() > MULTIBRANCH_MINIMUM_SPLIT)
        {
            split(in.vec[i]);
            in.vec[i].remake = false;
        }
    }
}


