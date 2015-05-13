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

using namespace std;

#define label_str(node) (node).get_label().to_string().c_str()

rna_tree::rna_tree(const std::string& brackets, const std::string& labels, const std::string& _name)
    : tree_base<rna_node_type>(brackets, convert(labels)), name(_name)
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    std::stringstream stream;
    stream
        << "TREE '"
        << name
        << "' WAS CONSTRUCTED, size = "
        << size()
        << std::endl;
    kptree::print_tree_bracketed(_tree, stream);
    logger.debugStream() << stream.str();
}

std::vector<rna_node_type> rna_tree::convert(const std::string& labels)
{
    std::vector<rna_node_type> vec;
    vec.reserve(labels.size());
    for (size_t i = 0; i < labels.size(); ++i)
        vec.emplace_back(labels.substr(i, 1));
    return vec;
}




// MODIFY/REMOVE/INSERT operations:

rna_tree::iterator rna_tree::modify(iterator it, rna_node_type node)
{
    //LOGGER_PRIORITY_ON_FUNCTION(INFO);
    //APP_DEBUG_FNAME;

    DEBUG("modify node %s to %s",
            label_str(*it), label_str(node));

    assert(it->get_label().status == rna_pair_label::untouched);

    auto l1 = it->get_label();
    auto l2 = node.get_label();
    
    l1.set_label_strings(l2);
    it->set_label(l1);

    assert(it->get_label().status != rna_pair_label::untouched);

    return it;
}

rna_tree::iterator rna_tree::remove(iterator it)
{
    // iba zapise do vrhcolu status deleted
    //
    APP_DEBUG_FNAME;
    DEBUG("removing node %s", it->get_label().to_string().c_str());

    assert(it->get_label().status == rna_pair_label::untouched);

    auto label = it->get_label();
    label.status = rna_pair_label::deleted;
    it->set_label(label);

    assert(it->get_label().status != rna_pair_label::untouched);
    
    return it;



}

rna_tree::iterator rna_tree::insert(iterator it, rna_node_type node)
{
    return it;

    APP_DEBUG_FNAME;

    DEBUG("inserting node %s to %s",
            label_str(node), label_str(*it));
    cout << it->get_label().to_string() << endl;
    print_subtree(it);

    wait_for_input();
    return it;

    auto compute_points = [&](iterator iter)
    {
        auto label = iter->get_label();
        auto par = parent(iter);
        label.status = rna_pair_label::inserted;

        label.set_points(par->get_label());
        iter->set_label(label);
    };

    if (node.get_label().is_paired())
    {
        WARN("paired");
        wait_for_input();
        abort();
    }

    iterator iter;

    if (rna_tree::is_leaf(it))
    {
        DEBUG("is_leaf");
        iter = it = _tree.insert_after(it, node);
    }
    else
    {
        DEBUG("else");
        iter = _tree.append_child(it, node);
    }

    compute_points(iter);
    wait_for_input();

    return it;
}

rna_tree::iterator rna_tree::erase(iterator it)
{
    APP_DEBUG_FNAME;

    rna_tree::post_order_iterator it2 = it;
    ++it2;

    _tree.flatten(it);
    assert(rna_tree::is_leaf(it));
    _tree.erase(it);
    --it2;
    return it2;
}









