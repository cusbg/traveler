/*
 * File: rna_tree.cpp
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


#include <cfloat>

#include "rna_tree.hpp"

using namespace std;

inline static std::vector<rna_pair_label> convert(
                const std::string& labels);

inline static std::string trim(
                std::string s);


rna_tree::rna_tree(
                const std::string& _brackets,
                const std::string& _labels,
                const std::string& _name)
    : tree_base<rna_pair_label>(
            trim(_brackets), convert(trim(_labels))), _name(_name)
{
    set_postorder_ids();
    distances = {0xBADF00D, 0xBADF00D, 0xBADF00D};

    DEBUG("Tree '%s:%s' was constructed, size=%s;\n%s",
            id(), name(), size(), print_tree(false));
}

rna_tree::rna_tree(
                const std::string& _brackets,
                const std::string& _labels,
                const std::vector<point>& _points,
                const std::string& _name)
    : rna_tree(_brackets, _labels, _name)
{
    update_points(_points);
    compute_distances();
}

void rna_tree::set_name(
                const std::string& name)
{
    _name = name;
}



/* inline, local */ std::vector<rna_pair_label> convert(
                const std::string& labels)
{
    vector<rna_pair_label> vec;

    vec.reserve(labels.size());

    for (size_t i = 0; i < labels.size(); ++i)
        vec.emplace_back(labels.substr(i, 1));

    return vec;
}



void rna_tree::update_points(
                const vector<point>& points)
{
    APP_DEBUG_FNAME;

    pre_post_order_iterator it;
    size_t i = 0;

    for (it = ++begin_pre_post();
            it != end_pre_post() && i < points.size();
            ++it, ++i)
        it->at(it.label_index()).p = points[i];

    assert(i == points.size() && ++pre_post_order_iterator(it) == end_pre_post());

    update_ends_in_rna(*this);
}

//highlights 5' and 3' end
void update_ends_in_rna(
                rna_tree& rna)
{
    APP_DEBUG_FNAME;

    typedef rna_tree::iterator iterator;

    iterator root = rna.begin();
    point pf, pl;
    iterator f, l;

    f = rna_tree::first_child(root);
    l = rna_tree::last_child(root);

    if (f == l && !f->paired())
    {
        WARN("Cannot initialize rna ends (3', 5'), returning");
        return;
    }

    pf = f->at(0).p;
    pl = l->paired() ? l->at(1).p : l->at(0).p;

    if (pf.bad() || pl.bad() || pf == pl)
    {
        WARN("Cannot initialize rna ends (3', 5'), returning");
        return;
    }

    if (f == l) {
        //first base is paired
        point dir = normalize(pl - pf) * 7;
        root->at(0).p = pf - dir;
        root->at(1).p = pl + dir;
    } else{
        //using first and first but last siblings to get the position for the 5' and 3' labels
        //when merging, the template can incorporate some uninitiated nodes, so next or previous siblings do not
        //necessarilly be initiated
        iterator f2 = rna.next_sibling(f);
        while (f2.node->next_sibling && !f2->initiated_points()) f2++;
        iterator l2 = rna.previous_sibling(l);
        while (l2.node->prev_sibling && !l2->initiated_points()) l2--;

        point pf2, pl2;
        pf2 = f2->at(0).p;
        pl2 = l2->paired() ? l2->at(1).p : l2->at(0).p;

        pf2.bad() ? root->at(0).p = pf : root->at(0).p = pf - (pf2 - pf);
        pl2.bad() ? root->at(1).p = pl : root->at(1).p = pl - (pl2 - pl);
    }



    root->at(0).label = "5'";

    root->at(1).label = "3'";

    INFO("RNA ends (3', 5') are updated");
}



rna_tree::sibling_iterator rna_tree::erase(
                sibling_iterator sib)
{
    DEBUG("Erasing node %s:%s", label(sib), ::id(sib));

    sibling_iterator del;

    sib = _tree.flatten(sib);
    del = sib++;

    assert(is_leaf(del));
    _tree.erase(del);
    --_size;

    return sib;
}

rna_tree::sibling_iterator rna_tree::insert(
                sibling_iterator sib,
                rna_pair_label lbl,
                size_t steal_children)
{
    DEBUG("Inserting node %s to %s with %s children",
            lbl, label(sib), steal_children);

    sibling_iterator pos, beg, end;
    rna_pair_label node(lbl);

    pos = _tree.insert(sib, node);
    beg = end = ++sibling_iterator(pos);

    while (steal_children-- != 0)
        ++end;

    _tree.reparent(pos, beg, end);
    ++_size;

    return pos;
}

std::string rna_tree::name() const
{
    return _name;
}


/* static */ std::string rna_tree::get_labels(
                const iterator& root)
{
    std::ostringstream out;

    auto f =
        [&out](const pre_post_order_iterator& iter) {
            out << iter->at(iter.label_index()).label;
        };

    for_each_in_subtree(root, f);

    return out.str();
}

std::string rna_tree::get_labels() const
{
    ostringstream out;
    iterator root = begin();
    for (sibling_iterator ch = root.begin(); ch != root.end(); ++ch)
        out << get_labels(ch);

    return out.str();
}

/* static */ std::string rna_tree::get_brackets(
                const iterator& root)
{
    std::ostringstream out;

    auto f =
        [&out](const pre_post_order_iterator& iter) {
            if (is_leaf(iter))
                out << ".";
            else if (iter.preorder())
                out << "(";
            else
                out << ")";
        };

    for_each_in_subtree(root, f);

    return out.str();
}

std::string rna_tree::get_brackets() const
{
    ostringstream out;
    iterator root = begin();
    for (sibling_iterator ch = root.begin(); ch != root.end(); ++ch)
        out << get_brackets(ch);

    return out.str();
}


bool rna_tree::correct_pairing() const
{
    APP_DEBUG_FNAME;

    for (iterator it = begin(); it != end(); ++it)
    {
        // if is leaf and is paired..
        if (is_leaf(it) == it->paired())
            return false;
    }

    return true;
}

/* static */
rna_tree::iterator rna_tree::get_leftest_initiated_descendant(const rna_tree::iterator& node)
{
    APP_DEBUG_FNAME;

    if (node->initiated_points()) return node;
    else if (is_leaf(node))
    {
        if (node.node->next_sibling->next_sibling == NULL) return node;
        else return get_leftest_initiated_descendant(iterator(node.node->next_sibling));
    }
    else return get_leftest_initiated_descendant(node.begin());
}

/* static */
rna_tree::iterator rna_tree::get_rightest_initiated_descendant(const rna_tree::iterator& node)
{
    APP_DEBUG_FNAME;

    if (node->initiated_points()) return node;
    else if (is_leaf(node))
    {
        if (node.node->prev_sibling->prev_sibling == NULL) return node;
        else return get_rightest_initiated_descendant(iterator(node.node->prev_sibling));
    }
    else return get_rightest_initiated_descendant(rna_tree::iterator(node.node->last_child));
}


/* static */ rna_tree::sibling_iterator get_onlyone_branch(
                rna_tree::sibling_iterator it)
{
    /*
     * Goes through all children of one parent and searches for base pairs. If more are found, returns
     * invalid value.
     */

    rna_tree::sibling_iterator ch, out;
    rna_tree::sibling_iterator bad;

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

void rna_tree::compute_distances()
{
    int elements;
    double dist;

    // compute distances between 2 pairs (CG <-> CG)
    elements = 0;
    dist = 0;
    for (iterator it = begin(); it != end(); ++it)
    {
        if (!it->initiated_points()
                || is_root(it)
                || is_leaf(it)
                || !is_valid(get_onlyone_branch(parent(it)))
                || !rna_tree::is_only_child(it))
            continue;

        dist += distance(parent(it)->center(), it->center());
        DEBUG("dist=%s", dist);
        ++elements;
    }
    distances.pairs_distance = dist / (double)elements;

    // distance between bases in pair (C <-> G)
    elements = 0;
    dist = 0;
    for (iterator it = begin(); it != end(); ++it)
    {
        if (!it->initiated_points()
                || is_leaf(it))
            continue;

        dist += distance(it->at(0).p, it->at(1).p);
        ++elements;
    }
    distances.pair_base_distance = dist / (double)elements;

    // distance between unpaired bases in loops
    elements = 0;
    dist = 0;
    for (iterator it = begin(); it != end(); ++it)
    {
        if (!it->initiated_points())
            continue;

        sibling_iterator end = it.end();
        for (sibling_iterator ch = it.begin(); ch != end; ++ch)
        {
            sibling_iterator prev;
            while (is_leaf(prev = ch++)
                    && ch != end
                    && is_leaf(ch))
            {
                dist += distance(prev->center(), ch->center());
                ++elements;
            }
        }
    }
    distances.loops_bases_distance = dist / (double)elements;

    INFO("Distances: pairs %s, pairbase %s, loops %s",
            distances.pairs_distance,
            distances.pair_base_distance,
            distances.loops_bases_distance);
}



point rna_tree::top_right_corner(
                rna_tree::iterator root)
{
    // x, y should be maximal in subtree
    point p = { -DBL_MAX, -DBL_MAX };

    auto f = [&p] (rna_tree::pre_post_order_iterator it) {
        if (rna_tree::is_root(it) || !it->initiated_points())
            return;
        point o = it->at(it.label_index()).p;
        if (o.x > p.x)
            p.x = o.x;
        if (o.y > p.y)
            p.y = o.y;
    };

    rna_tree::for_each_in_subtree(root, f);

    assert(p.x != -DBL_MAX && p.y != -DBL_MAX);

    return p;
}

point rna_tree::bottom_left_corner(
                rna_tree::iterator root)
{
    // x, y should be minimal in subtree
    point p = { DBL_MAX, DBL_MAX };

    auto f = [&p] (rna_tree::pre_post_order_iterator it) {
        if (rna_tree::is_root(it) || !it->initiated_points())
            return;
        point o = it->at(it.label_index()).p;
        if (o.x < p.x)
            p.x = o.x;
        if (o.y < p.y)
            p.y = o.y;
    };

    rna_tree::for_each_in_subtree(root, f);

    assert(p.x != DBL_MAX && p.y != DBL_MAX);

    return p;
}



/* global */ size_t child_index(
                rna_tree::sibling_iterator sib)
{
    size_t n = 0;
    while (!rna_tree::is_first_child(sib))
    {
        ++n;
        --sib;
    }
    return n;
}


/* inline, local */ std::string trim(
                std::string s)
{
#define WHITESPACES " \t\n\r\f\v"
    size_t pos;

    pos = s.find_first_not_of(WHITESPACES);
    if (pos != s.npos)
        s.erase(0, pos);
    pos = s.find_last_not_of(WHITESPACES);
    if (pos != s.npos)
        s.erase(pos + 1);

    return s;
}


/* static */ point rna_tree::base_pair_edge_point(
                point from,
                point to)
{
    assert(!from.bad() && !to.bad());

    point vec = {3, 3};
    vec = vec + normalize(to - from) * 4;
    return from + vec;
}

