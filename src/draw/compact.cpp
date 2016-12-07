/*
 * File: compact.cpp
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

#include "compact.hpp"
#include "compact_circle.hpp"
#include "compact_utils.hpp"

using namespace std;

#define MULTIBRANCH_MINIMUM_SPLIT   10

#define PAIRS_DISTANCE rna.get_pair_base_distance()
#define BASES_DISTANCE rna.get_pairs_distance()

compact::compact(
                rna_tree& _rna)
    : rna(_rna)
{ }


void compact::run()
{
    APP_DEBUG_FNAME;

    INFO("BEG: Computing RNA layout for:\n%s", rna.print_tree(false));

    init();
    make();
    update_ends_in_rna(rna);
    checks();

    INFO("END: Computing RNA layout");
}



/* static */ void compact::shift_branch(
                iterator parent,
                point vector)
{
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

    recursion(parent);
}

/* static */ void compact::set_distance(
                iterator parent,
                iterator child,
                double dist)
{
    assert(rna_tree::parent(child) == parent);

    set_distance(child, parent->centre(), dist);
}

/* static */ void compact::set_distance(
                iterator it,
                point from,
                double dist)
{
    point p = it->centre();
    point vec = normalize(p - from);
    double actual = distance(p, from);
    vec = vec * (dist - actual);

    shift_branch(it, vec);
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
    assert(!rna_tree::is_leaf(it));

    for (pre_post_order_iterator ch = pre_post_order_iterator(it, true); id(ch) <= id(it); ++ch)
    {
        if (!ch->inited_points())
            continue;

        double dist = distance(c.centre, ch->at(ch.label_index()).p);
        c.p1 = ch->at(ch.label_index()).p;
        c.p2 = move_point(c.centre, c.p2, dist);

        ch->at(ch.label_index()).p = c.rotate(alpha);
    }
}


void compact::init()
{
    APP_DEBUG_FNAME;

    assert(rna.is_ordered_postorder());

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
            // init OK
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

    auto log = logger.debug_stream();
    log << "Points initialization:\n";
    auto f = [&](pre_post_order_iterator it)
    {
        if (it->paired())
        {
            if (it.preorder())
            {
                mprintf("it[%s][%s = %s][%s = %s]\n", log,
                        it->status,
                        it->at(0).label, it->at(0).p,
                        it->at(1).label, it->at(1).p);
            }
        }
        else
        {
            mprintf("it[%s][%s = %s]\n", log,
                    it->status,
                    it->at(0).label, it->at(0).p);
        }
    };
    rna_tree::for_each_in_subtree(rna.begin_pre_post(), f);

    // if first node was inserted and it is only one branch - do not remake it
    // because it shares parents (3'5' node) position: 3'-NODE1 <-> NODE2-5'
    sibling_iterator ch = get_onlyone_branch(rna.begin());
    if (rna_tree::is_valid(ch)
            && !rna.begin()->remake_ids.empty()
            && ch->inited_points())
    {
        sibling_iterator parent = rna_tree::parent(ch);
        parent->remake_ids.clear();
    }

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
    assert(rna_tree::is_valid(get_onlyone_branch(rna_tree::parent(it))));   // => 1 branch

    point p1, p2, vec;
    iterator par = rna_tree::parent(it);
    assert(!rna_tree::is_root(par));
    vec = normalize(par->centre() - rna_tree::parent(par)->centre());
        // ^^ direction (parent(par)->par)
    p1 = par->at(0).p + vec;
    p2 = par->at(1).p + vec;

    it->at(0).p = p1;
    it->at(1).p = p2;
    // ^^ initialize points of `it` to lie next to parent
    // .. that means only initialization, to get direction where child should be
}

void compact::init_multibranch(
                sibling_iterator it)
{
    APP_DEBUG_FNAME;

#define LEAF_POINTS 1
#define PAIRED_POINTS 5 /* we use only # 1 and 3; # 0,2,4 will be free space */

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
        [this](sibling_iterator root, point centre, point p1, point p2)
        {
            if (init_branch_recursive(root, centre).bad())
            {
                rna_tree::for_each_in_subtree(root,
                        [](iterator iter)
                        {
                            rna_tree::parent(iter)->remake_ids.push_back(child_index(iter));
                        });

                root->at(0).p = p1;
                root->at(1).p = p2;
                return;
            }

            point rp1 = root->at(0).p;
            point rp2 = root->at(1).p;

            root->at(0).p = p1;
            root->at(1).p = p2;

            double beta = angle(rp1 - rp2) - angle(p1 - p2);

            for (pre_post_order_iterator it = ++pre_post_order_iterator(root, true); id(it) < id(root); ++it)
            {
                point from = it->at(it.label_index()).p;
                if (from.bad())
                    continue;

                double alpha = angle(from - rp1);
                double radius = distance(from, rp1);

                point to = rotate(p1, alpha - beta, radius);
                it->at(it.label_index()).p = to;
            }
        };

    circle c;
    c.p1 = it->at(0).p;
    c.p2 = it->at(1).p;
    c.direction = rna_tree::parent(it)->at(0).p;
    c.centre = centre(c.p1, c.p2);
    c.compute_sgn();
    auto points = c.init(get_number_of_places_for_bases(it), BASES_DISTANCE);

    int i = 0;
    for (sibling_iterator ch = it.begin(); ch != it.end(); ++ch)
    {
        if (rna_tree::is_leaf(ch))
        {
            ch->at(0).p = points[i];

            i += LEAF_POINTS;
        }
        else
        {
            rotate_subtree(ch, c.centre, points[i + 1], points[i + 3]);
            i += PAIRED_POINTS;
        }
    }
    auto set_label_status =
        [](iterator iter)
        {
#define lbl(type) rna_pair_label::type
            if (!contains({lbl(edited), lbl(deleted), lbl(inserted), lbl(reinserted)}, iter->status))
            {
                iter->status = lbl(rotated);
            }
#undef lbl
        };
    rna_tree::for_each_in_subtree(it, set_label_status);
}

void compact::make_branch_even(
                sibling_iterator it)
{
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
    assert(in.type == intervals::interior_loop &&
            in.vec.size() == 2);

    iterator parent, child;
    double actual, avg;
    vector<double> l(2);

    parent = in.vec[0].beg.it;
    child = in.vec[0].end.it;

    actual = distance(parent->centre(), child->centre());
    l[0] = circle::min_circle_length(in.vec[0].vec.size(), BASES_DISTANCE);
    l[1] = circle::min_circle_length(in.vec[1].vec.size(), BASES_DISTANCE);
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
    for (size_t i = 0; i < in.vec.size(); ++i)
    {
        try {
            if (in.vec[i].vec.size() > MULTIBRANCH_MINIMUM_SPLIT)
            {
                split(in.vec[i]);
                in.vec[i].remake = false;
            }
        } catch (...)
        {
            ERR("Error occured in drawing multibranch loop. Some bases could not be drawn");
            // TODO handle error
            return;
        }
    }
}

void compact::split(
                const interval& in)
{
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

    assert(j == p2.size());

    j = 0;
    for (auto val : in.vec)
        val->at(0).p = p2[j++];
}

void compact::reinsert(
                const points_vec& points,
                const nodes_vec& nodes)
{
    if (nodes.empty())
        return;

    assert(!nodes.empty());

    assert(points.size() == nodes.size());
    for (size_t i = 0; i < points.size(); ++i)
    {
        assert(rna_tree::is_leaf(nodes[i]));

        nodes[i]->at(0).p = points[i];
        if (is(nodes[i], rna_pair_label::touched))
            nodes[i]->status = rna_pair_label::reinserted;
    }
}

void compact::remake(
                const interval& i,
                point direction)
{
    assert(i.remake);

    circle c;
    c.p1 = i.beg.it->at(i.beg.index).p;
    c.p2 = i.end.it->at(i.end.index).p;
    c.direction = direction;
    c.centre = centre(c.p1, c.p2);
    c.compute_sgn();

    reinsert(c.init(i.vec.size(), BASES_DISTANCE), i.vec);
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
    return len;
}


/* inline */ void compact::checks()
{
    // all but root should be inited
    for (iterator it = ++rna.begin(); it != rna.end(); ++it)
    {
        if (!it->inited_points())
        {
            ERR("All bases should be visualized, but they are not.");
        }
    }
}


