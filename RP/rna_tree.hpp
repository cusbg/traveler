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

class mapping;

class rna_tree : public tree_base<rna_node_type>
{
public:
    virtual ~rna_tree() = default;
    rna_tree() = default;

    bool operator==(
                rna_tree& other);
    rna_tree(
                const std::string& brackets,
                const std::string& labels,
                const std::string& _name = "");



    iterator insert(sibling_iterator it, rna_node_type node, size_t steal_children = 0);
    template <typename iter>
        iter remove(iter it);
    void merge(rna_tree other, const mapping& m);

private:
    void mark(std::vector<size_t> node_ids, rna_pair_label::label_status_type status);
    void modify(const rna_tree& other);

public:
    std::string name;
};

template <typename iter>
    iter rna_tree::remove(iter it)
{
    DEBUG("erasing node %s", label_str(*it));

    it = _tree.flatten(it);
    assert(is_leaf(it));
    iter del = it++;
    _tree.erase(del);
    --_size;
    return --it;
}


size_t get_label_index(
                rna_tree::pre_post_order_iterator iter);

size_t get_label_index(
                rna_tree::base_iterator iter);

bool is(
                rna_tree::iterator it,
                rna_pair_label::label_status_type status);
bool has_child(
                rna_tree::iterator it,
                rna_pair_label::label_status_type status);

#endif /* !RNA_TREE_HPP */

