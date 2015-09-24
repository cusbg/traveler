/*
 * File: tree_matcher.hpp
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

#ifndef TREE_MATCHER_HPP
#define TREE_MATCHER_HPP


#include "rna_tree.hpp"

class mapping;

class matcher
{
private:
    typedef rna_tree::iterator iterator;
    typedef rna_tree::post_order_iterator post_order_iterator;
    typedef rna_tree::sibling_iterator sibling_iterator;
    typedef std::vector<size_t> indexes_type;

public:
    matcher(
                const rna_tree& templated,
                const rna_tree& other);
    rna_tree& run(
                const mapping& m);
    inline size_t child_index(
                sibling_iterator sib);

private:
    void mark(
                rna_tree& rna,
                const indexes_type& postorder_indexes,
                rna_pair_label::status_type status);
    void merge();
    void erase();
    void compute_sizes();
    inline void make_unique(
                iterator it);
private:
    rna_tree t1, t2;
    std::vector<size_t> s1, s2;
};


#endif /* !TREE_MATCHER_HPP */

