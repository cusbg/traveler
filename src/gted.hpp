/*
 * File: gted.hpp
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

#ifndef GTED_HPP
#define GTED_HPP

#include <unordered_map>

#include "types.hpp"
#include "rna_tree.hpp"
#include "strategy.hpp"
#include "gted_tree.hpp"


#define GTED_COST_MODIFY    0
#define GTED_COST_DELETE    1

class mapping;

class gted
{
public:
    typedef gted_tree                                   tree_type;
    typedef typename tree_type::iterator                iterator;
    typedef typename tree_type::post_order_iterator     post_order_iterator;
    typedef typename tree_type::reverse_post_order_iterator
                                                        rev_post_order_iterator;
    typedef typename tree_type::sibling_iterator        sibling_iterator;
    typedef std::vector<std::vector<size_t>>            tree_distance_table_type;
    typedef std::vector<std::vector<size_t>>            forest_distance_table_type;

public:
    gted(
                const rna_tree& _t1,
                const rna_tree& _t2,
                const strategy_table_type& _str);
    void run();

    mapping compute_mapping();

private:
    void checks();

private:
    void compute_distance_recursive(
                iterator root1,
                iterator root2);
    void single_path_function(
                iterator root1,
                iterator root2);
    void compute_distance(
                iterator root1,
                iterator root2);
    template <typename iterator_type, typename funct_get_begin>
        std::vector<std::vector<size_t>> compute_distance_LR(
                iterator root1,
                iterator root2,
                tree_type& t1,
                tree_type& t2,
                funct_get_begin get_begin);

private:
    size_t get_tdist(
                iterator it1,
                iterator it2);
    void set_tdist(
                iterator it1,
                iterator it2,
                size_t value);

    size_t get_fdist(
                const forest_distance_table_type& fdist,
                const iterator& it1,
                const iterator& it2,
                size_t id1,
                size_t id2);

    void set_fdist(
                forest_distance_table_type& fdist,
                const iterator& it1,
                const iterator& it2,
                size_t value,
                size_t id1,
                size_t id2);

private:
    tree_type t1, t2;
    strategy_table_type STR;
    strategy actual_str;
    tree_distance_table_type tdist;
};


#endif /* !GTED_HPP */

