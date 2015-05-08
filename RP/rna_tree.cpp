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

using namespace std;

rna_tree::rna_tree(const std::string& brackets, const std::string& labels, const std::string& _name)
    : tree_base<rna_node_type>(brackets, convert(labels)), name(_name)
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    set_ids_postorder();
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


rna_tree::iterator rna_tree::modify(iterator it, rna_node_type node)
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    APP_DEBUG_FNAME;

    DEBUG("modify node %s to %s", label(it), node.get_label().to_string().c_str());

    auto l1 = it->get_label();
    auto l2 = node.get_label();
    
    typedef vector<rna_label> labelvec;
    auto set_label_str = [](labelvec& l1, const labelvec l2, size_t i)
    {
        if (l1.size() <= i || l2.size() <= i)
        {
            DEBUG("size < %lu, returning", i);
            return;
        }
        DEBUG("modify %s -> %s", l1.at(i).label.c_str(), l2.at(i).label.c_str());
        l1.at(i).label = l2.at(i).label;
    };

    //l1.labels.at(0).point = Point({0, 1});
    //cout << l1.get_points() << endl;
    set_label_str(l1.labels, l2.labels, 0);
    set_label_str(l1.labels, l2.labels, 1);
    //cout << l1.get_points() << endl;

    //cout << *it << endl;
    it->set_label(l1);
    //cout << *it << endl;

    return it;
}

rna_tree::iterator rna_tree::remove(iterator it)
{
    APP_DEBUG_FNAME;

    DEBUG("removing node %s", label(it));
    // vsetky child-y da za dany uzol
    it = _tree.flatten(it);
    assert(is_leaf(it));
    it = _tree.erase(it);

    return it;
}

rna_tree::iterator rna_tree::insert(iterator it, rna_node_type node)
{
    APP_DEBUG_FNAME;

    throw "ex";
}









std::ostream& operator<<(std::ostream& out, const rna_pair_label& l)
{
    for (auto val : l.labels)
        out << val.label;

    return out;
}

rna_pair_label rna_pair_label::operator+(const rna_pair_label& other) const
{
    assert(labels.size() == 1 && other.labels.size() == 1);

    rna_pair_label out;
    out.labels.push_back(labels.back());
    out.labels.push_back(other.labels.back());

    assert(out.labels.size() == 2);

    return out;
}

rna_pair_label::rna_pair_label(const std::string& s)
{
    rna_label l;
    l.label = s;
    labels.push_back(l);
}

