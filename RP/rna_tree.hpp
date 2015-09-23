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


class rna_tree
    : public tree_base<rna_pair_label>
{
public:
    virtual ~rna_tree() = default;
    rna_tree() = default;
    rna_tree(
                const std::string& _brackets,
                const std::string& _labels,
                const std::string& _name = "");

    void update_points(
                const std::vector<point>& points);

    sibling_iterator insert(
                sibling_iterator sib,
                rna_pair_label label,
                size_t steal_children = 0);
    sibling_iterator erase(
                sibling_iterator sib);

    std::string name() const;

private:
    std::string _name;
};



#endif /* !RNA_TREE_HPP */

