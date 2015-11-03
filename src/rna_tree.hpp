/*
 * File: rna_tree.hpp
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

#ifndef RNA_TREE_HPP
#define RNA_TREE_HPP

#include "tree_base.hpp"
#include "rna_tree_label.hpp"

struct point;

class rna_tree
    : public tree_base<rna_pair_label>
{
public:
    virtual ~rna_tree() = default;
    rna_tree() = default;
    rna_tree(
                std::string _brackets,
                std::string _labels,
                std::string _name = "");
    rna_tree(
                const std::string& _brackets,
                const std::string& _labels,
                const std::vector<point>& _points,
                const std::string& _name = "");

    /**
     * update postorder points
     */
    void update_points(
                const std::vector<point>& points);

    /**
     * insert `label` to tree before `it` and set
     * `steal` next siblings to be children of inserted node
     */
    sibling_iterator insert(
                sibling_iterator it,
                rna_pair_label label,
                size_t steal = 0);
    /**
     * reparent `sib`-s children to be its next siblings,
     * remove `sib` from tree and return its next sibling
     */
    sibling_iterator erase(
                sibling_iterator sib);

    std::string name() const;

private:
    std::string _name;
};

inline bool is(
                const rna_tree::base_iterator& iter,
                rna_pair_label::status_type s)
{
    return iter->status == s;
}


#endif /* !RNA_TREE_HPP */

