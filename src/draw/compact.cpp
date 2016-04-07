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
#include "compact_circle.hpp"
#include "compact_utils.hpp"


using namespace std;

#define MULTIBRANCH_MINIMUM_SPLIT   10

compact::compact(
                rna_tree& _rna)
    : rna(_rna)
{ }

void compact::run()
{
    APP_DEBUG_FNAME;

    INFO("BEG: COMPACT");

    init();
    make();
    checks();

    INFO("END: COMPACT");
}



/* static */ void compact::shift_branch(
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

/* static */ void compact::set_distance(
                iterator parent,
                iterator child,
                double dist)
{
    APP_DEBUG_FNAME;

    assert(rna_tree::parent(child) == parent);

    set_distance(child, parent->centre(), dist);
}

/* static */ void compact::set_distance(
                iterator it,
                point from,
                double dist)
{
    APP_DEBUG_FNAME;

    point p = it->centre();
    point vec = normalize(p - from);
    double actual = distance(p, from);
    vec = vec * (dist - actual);

    shift_branch(it, vec);
}

/* static */ compact::sibling_iterator compact::get_onlyone_branch(
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

/* static */ bool compact::remake_child(
                iterator parent,
                size_t n)
{
    return is(parent, rna_pair_label::inserted) ||
        contains(parent->remake_ids, n);
}

/* static */ bool compact::to_remake_children(
                iterator parent)
{
    return parent->paired() &&
        (!parent->remake_ids.empty() ||
         is(parent, rna_pair_label::inserted));
}

/* static */ void compact::rotate_branch(
                iterator it,
                circle c,
                double alpha)
{
    APP_DEBUG_FNAME;

    assert(!rna_tree::is_leaf(it));

    DEBUG("rotate %s by %f", clabel(it), alpha);

    for (pre_post_order_iterator ch = pre_post_order_iterator(it, true); id(ch) <= id(it); ++ch)
    {
        if (!ch->inited_points())
            continue;

        double dist = distance(c.centre, ch->at(ch.label_index()).p);
        c.p1 = ch->at(ch.label_index()).p;
        c.p2 = move_point(c.centre, c.p2, dist);

        ch->set_points_exact(c.rotate(alpha), ch.label_index());
    }
}


void compact::init()
{
    APP_DEBUG_FNAME;

    //LOGGER_PRIORITY_ON_FUNCTION(DEBUG);

    for (iterator it = ++rna.begin(); it != rna.end(); ++it)
    {
        if (it->inited_points() || !it->paired())
            continue;

        iterator par = rna_tree::parent(it);
        point p = par->centre();

        assert(!p.bad());

        if (rna_tree::is_root(par))
        {
            init_branch_recursive(it);
        }
        else if (!init_branch_recursive(it, p).bad())    // => is good
        {
            DEBUG("INIT_rec OK");
        }
        else
        {
            if (rna_tree::is_valid(get_onlyone_branch(par)))   // 1 branch
            {
                init_by_ancestor(it);
            }
            else
            {
                init_multibranch(par);
            }
        }
    }

    init_even_branches();

    DEBUG("compact::init() OK");
}

void compact::init_even_branches()
{
    // for nodes in one branch, set them to lie on straight line
    for (iterator it = rna.begin(); it != rna.end(); ++it)
    {
        if (rna_tree::is_leaf(it))
            continue;
        // run only when branching
        if (!rna_tree::is_valid(get_onlyone_branch(it)))
        {
            for (sibling_iterator ch = it.begin(); ch != it.end(); ++ch)
                if (!rna_tree::is_leaf(ch))
                    make_branch_even(ch);
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
        return p;
    }

    ch = get_onlyone_branch(it);
    if (!rna_tree::is_valid(ch))
        return point::bad_point();

    p = init_branch_recursive(ch, from);
    if (!p.bad())
    {
        assert(ch->paired());

        shift_branch(it, p);
        it->at(0).p = ch->at(0).p - p;
        it->at(1).p = ch->at(1).p - p;

        return p;
    }

    return point::bad_point();
}

point compact::init_branch_recursive(
                sibling_iterator it)
{
    // init for root children
    APP_DEBUG_FNAME;

    point p;
    sibling_iterator ch;

    if (it->inited_points())
    {
        for (ch = it.begin(); ch != it.end(); ++ch)
        {
            if (ch->inited_points())
            {
                p = it->centre() - ch->centre();
                return p;
            }
        }
        abort();
    }
    ch = get_onlyone_branch(it);
    assert(rna_tree::is_valid(ch));

    p = init_branch_recursive(ch);

    it->at(0).p = ch->at(0).p;
    it->at(1).p = ch->at(1).p;
    shift_branch(ch, -p);

    return p;
}

void compact::init_by_ancestor(
                sibling_iterator it)
{
    APP_DEBUG_FNAME;

    assert(rna_tree::is_valid(get_onlyone_branch(rna_tree::parent(it))));   // => 1 branch

    point p1, p2, vec;
    iterator par = rna_tree::parent(it);
    assert(!rna_tree::is_root(par));
    vec = normalize(par->centre() - rna_tree::parent(par)->centre());
        // ^^ direction (parent(par)->par)
    p1 = par->at(0).p + vec;
    p2 = par->at(1).p + vec;

    it->set_points_exact(p1, 0);
    it->set_points_exact(p2, 1);
    // ^^ initialize points of `it` to lie next to parent
    // .. that means only initialization, to get direction where child should be

    DEBUG("INIT OK");
}

void compact::init_multibranch(
                sibling_iterator it)
{
    APP_DEBUG_FNAME;

#define LEAF_POINTS 1
#define PAIRED_POINTS 5 /* we use only # 1 and 3; # 0,2,4 will be free space */

    auto create_circle =
        [](iterator iter)
        {
            circle c;
            c.p1 = iter->at(0).p;
            c.p2 = iter->at(1).p;
            c.direction = rna_tree::parent(iter)->at(0).p;
            c.centre = centre(c.p1, c.p2);
            c.compute_sgn();
            return c;
        };
    auto get_number_of_places_for_bases =
        [](sibling_iterator root)
        {
            int n = 0;
            for (sibling_iterator ch = root.begin(); ch != root.end(); ++ch)
            {
                if (ch->paired())
                    n += PAIRED_POINTS;
                else
                    n += LEAF_POINTS;
            }
            return n;
        };
    auto rotate_subtree =
        [this](sibling_iterator ch, circle c, point newpos)
        {
            c.p1 = ch->centre();
            c.p2 = newpos;
            rotate_branch(ch, c, c.segment_angle());
        };
    auto rotate_out_of_circle =
        [this](iterator it, point p1, point p2, point direction)
        {
            assert(it->paired());

            circle c;
            c.p1 = p1;
            c.p2 = p2;
            c.centre = centre(p1, p2);
            c.direction = direction;
            c.compute_sgn();

            double alpha = angle(direction, it->centre(), get_onlyone_branch(it)->centre());
            alpha = alpha - angle(direction, it->centre(), move_point(direction, it->centre(), 10000));

            rotate_branch(it, c, alpha);

            it->at(0).p = p1;
            it->at(1).p = p2;
            int i = 0;
            for (sibling_iterator ch = it.begin(); ch != it.end(); ++ch)
                it->remake_ids.push_back(i++);
        };

    int bases = get_number_of_places_for_bases(it);
    circle c = create_circle(it);
    c.init(bases);
    auto points = c.split(bases);

    int i = 0;
    for (sibling_iterator ch = it.begin(); ch != it.end(); ++ch)
    {
        if (rna_tree::is_leaf(ch))
        {
            ch->set_points_exact(points[i], 0);

            i += LEAF_POINTS;
        }
        else
        {
            if (ch->inited_points())
            {
                // we will use only points # 1 and 3;
                // # 0, 2, 4 will be free space

                set_distance(ch, c.centre, c.radius());
                rotate_subtree(ch, c, points[i + 2]);
                rotate_out_of_circle(ch, points[i + 1], points[i + 2], c.centre);
            }
            else
            {
                ch->set_points_exact(points[i + 1], 0);
                ch->set_points_exact(points[i + 3], 1);
            }

            i += PAIRED_POINTS;
        }
    }
}

void compact::make_branch_even(
                sibling_iterator it)
{
    APP_DEBUG_FNAME;
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
        // distances between nodes will be same..
        shift = normalize(shift) * distance(vec[0]->centre(), vec[i]->centre());

        newpos = p1 + shift;
        p = newpos - vec[i]->at(0).p;

        for (it = vec[i - 1].begin(); it != vec[i]; ++it)
        {
            // shift leaf children
            assert(rna_tree::is_leaf(it));
            shift_branch(it, p);
        }

        newpos = p2 + shift;
        p = newpos - vec[i]->at(1).p;

        for (it = ++sibling_iterator(vec[i]); it != vec[i-1].end(); ++it)
        {
            // shift leaf children
            assert(rna_tree::is_leaf(it));
            shift_branch(it, p);
        }

        vec[i]->at(0).p = p1 + shift;
        vec[i]->at(1).p = p2 + shift;
    }

    // shift rest of tree
    // (subtree of this branch)
    for (it = vec.back().begin(); it != vec.back().end(); ++it)
        shift_branch(it, p);
}




void compact::make()
{
    APP_DEBUG_FNAME;
    
    iterator it;
    intervals in;

    for (it = rna.begin(); it != rna.end(); ++it)
    {
        if (!to_remake_children(it))
            continue;

        in.init(it);
        set_distances(in);
        for (auto& i : in.vec)
            if (i.remake)
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
    assert(in.type == intervals::interior_loop &&
            in.vec.size() == 2);

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

void compact::set_distance_multibranch_loop(
                intervals& in)
{
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

        assert(length > 0);

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

void compact::reinsert(
                const circle& c,
                const nodes_vec& nodes)
{
    APP_DEBUG_FNAME;

    if (nodes.empty())
        return;

    assert(!nodes.empty());

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

void compact::remake(
                const interval& i,
                point direction)
{
    APP_DEBUG_FNAME;

    assert(i.remake);

    circle c;
    c.p1 = i.beg.it->at(i.beg.index).p;
    c.p2 = i.end.it->at(i.end.index).p;
    c.direction = direction;
    c.centre = centre(c.p1, c.p2);
    c.compute_sgn();
    c.init(i.vec.size());

    reinsert(c, i.vec);
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


/* inline */ void compact::checks()
{
    APP_DEBUG_FNAME;

    // all but root should be inited
    for (iterator it = ++rna.begin(); it != rna.end(); ++it)
    {
        if (!it->inited_points())
        {
            ERR("!inited(%s) after compact",
                    clabel(it));
            abort();
        }
    }
}


