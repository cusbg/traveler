/*
 * File: rna_tree.cpp
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

#include "rna_tree.hpp"

using namespace std;

inline std::vector<rna_pair_label> convert(
                const std::string& labels);

inline std::string trim(
                std::string& s);


rna_tree::rna_tree(
                std::string _brackets,
                std::string _labels,
                std::string _name)
    : tree_base<rna_pair_label>(
            trim(_brackets), convert(trim(_labels))), _name(_name)
{
    set_postorder_ids();

    logger.debug_stream()
        << "TREE '"
        << id()
        << ":"
        << _name
        << "' WAS CONSTRUCTED, size = "
        << size()
        << "\n"
        << print_tree(false);
}

rna_tree::rna_tree(
                const std::string& _brackets,
                const std::string& _labels,
                const std::vector<point>& _points,
                const std::string& _name)
    : rna_tree(_brackets, _labels, _name)
{
    update_points(_points);
}



std::vector<rna_pair_label> convert(
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
        it->set_points_exact(points[i], it.label_index());

    assert(i == points.size() && ++pre_post_order_iterator(it) == end_pre_post());
}


rna_tree::sibling_iterator rna_tree::erase(
                sibling_iterator sib)
{
    DEBUG("erase(%s:%lu)", clabel(sib), ::id(sib));

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
    // TODO: skontrolovat
    DEBUG("insert(%s, %lu) <- %s",
            clabel(sib), steal_children, to_cstr(lbl));

    sibling_iterator pos, beg, end;
    rna_pair_label node(lbl);

    pos = _tree.insert(sib, node);
    beg = end = ++sibling_iterator(pos);

    while (steal_children-- != 0)
        ++end;

    _tree.reparent(pos, beg, end);

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

    pre_post_order_iterator begin(root, true);
    pre_post_order_iterator end(root, false);
    ++end;

    for_each(begin, end, f);

    return out.str();
}

std::string rna_tree::get_labels() const
{
    ostringstream out;
    iterator root = _tree.begin();
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

    pre_post_order_iterator begin(root, true);
    pre_post_order_iterator end(root, false);
    ++end;

    for_each(begin, end, f);

    return out.str();
}

std::string rna_tree::get_brackets() const
{
    ostringstream out;
    iterator root = _tree.begin();
    for (sibling_iterator ch = root.begin(); ch != root.end(); ++ch)
        out << get_brackets(ch);
    return out.str();
}

/* inline */ std::string trim(
                std::string& s)
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
