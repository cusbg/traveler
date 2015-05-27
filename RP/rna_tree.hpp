/*
 * File: rna_tree.hpp
 *
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
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

#ifndef RNA_TREE_HPP
#define RNA_TREE_HPP

#include "tree/tree_base.hpp"
#include "tree/node_base.hpp"
#include "rna_tree_labels.hpp"

typedef node_base<rna_pair_label> rna_node_type;


class rna_tree : public tree_base<rna_node_type>
{
public:
    virtual ~rna_tree() = default;
    rna_tree() = default;
    bool operator==(rna_tree& other)
    {
        return _tree.equal_subtree(begin(), other.begin());
    }
    rna_tree(const std::string& brackets, const std::string& labels, const std::string& _name = "");

    iterator modify(iterator it, rna_node_type node);
    iterator insert_pre(iterator it, rna_node_type node);
    iterator insert_post(iterator it, rna_node_type node);
    iterator remove(iterator it);
    template <typename iter>
    iter erase(iter it);
private:
    inline std::vector<rna_node_type> convert(const std::string& labels);

public:
    std::string name;
};


template <typename iter>
iter rna_tree::erase(iter it)
{
    // zmaze vrchol zo stromu
    //APP_DEBUG_FNAME;
    DEBUG("erasing node %s", label_str(*it));

    it = _tree.flatten(it);
    assert(rna_tree::is_leaf(it));
    iter del = it++;
    _tree.erase(del);

    --_size;
    return --it;
}

template <typename iter>
iter move_it_plus(iter it, size_t count)
{
    while(count--)
        ++it;
    return it;
}
template <typename iter>
iter move_it_minus(iter it, size_t count)
{
    while(count--)
        --it;
    return it;
}

size_t get_label_index(rna_tree::pre_post_order_iterator iter);



#endif /* !RNA_TREE_HPP */

