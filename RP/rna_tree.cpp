/*
 * File: rna_tree.cpp
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

#include "rna_tree.hpp"
#include "rna_tree_labels.hpp"
#include "util.hpp"

using namespace std;

/*
 * make vector of rna_nodes from string
 */
inline std::vector<rna_node_type> convert(
                const std::string& labels);


rna_tree::rna_tree(
                const std::string& brackets,
                const std::string& labels,
                const std::string& _name)
    : tree_base<rna_node_type>(brackets, convert(labels)), name(_name)
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    std::stringstream stream;
    stream
        << "TREE '"
        << name
        << "' WAS CONSTRUCTED, size = "
        << size()
        << std::endl
        << print_tree(false);
    logger.debugStream() << stream.str();
}

std::vector<rna_node_type> convert(
                const std::string& labels)
{
    std::vector<rna_node_type> vec;
    vec.reserve(labels.size());
    for (size_t i = 0; i < labels.size(); ++i)
        vec.emplace_back(labels.substr(i, 1));
    return vec;
}

bool rna_tree::operator==(
                const rna_tree& other) const
{
    return _tree.equal_subtree(begin(), other.begin());
}

void rna_tree::test_branches() const
{
    APP_DEBUG_FNAME;

    iterator it = ++begin();

    while(it != end())
    {
        assert(is_leaf(it) == !it->get_label().is_paired());
        ++it;
    }
}






rna_tree::iterator rna_tree::insert(sibling_iterator it, rna_pair_label lbl, size_t steal)
{
    DEBUG("insert(%s, %lu) <- %s", clabel(it), steal, lbl.to_string().c_str());
    //print_subtree(parent(it));

    sibling_iterator in, next;
    rna_node_type node(lbl);
    
    in = _tree.insert(it, node);
    ++_size;

    while (steal-- != 0)
    {
        next = in;
        ++next;
        assert(next != it.end());

        _tree.reparent(in, next, ++sibling_iterator(next));
    }
    //print_subtree(parent(in));

    return in;
}


/* global */ size_t get_label_index(
                rna_tree::pre_post_order_iterator iter)
{
    if (iter.is_preorder() || !iter->get_label().is_paired())
        return 0;
    else
        return 1;
}


/* global */ bool is(
                rna_tree::iterator it,
                rna_pair_label::label_status_type status)
{
    return it->get_label().status == status;
}

/* global */ bool has_child(
                rna_tree::iterator it,
                rna_pair_label::label_status_type status)
{
    return count_children_if(it,
            [status](rna_tree::iterator _iter) {return is(_iter, status);}) != 0;
}

