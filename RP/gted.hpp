/*
 * File: gted.hpp
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

#ifndef GTED_HPP
#define GTED_HPP

#include "types.hpp"
#include "rna_tree.hpp"
#include "rted.hpp"
#include <unordered_map>


class gted
{
private:
    typedef rted::tree_type tree_type;
    struct subforest
    {
        tree_type::reverse_post_order_iterator  left;
        tree_type::post_order_iterator          right;
        tree_type::iterator                     root;
        tree_type::iterator                     path_node;

        enum last_added {Lnode, Rnode, undef}   last = undef;

        bool operator==(const subforest& other) const;
        // hashovacia funkcia do mapy..
        struct hash
        {
            size_t operator()(const subforest& s) const;
        };
    };
    typedef std::unordered_map<subforest,
            size_t, subforest::hash> _subforest_map_type;
    typedef std::unordered_map<subforest,
            _subforest_map_type, subforest::hash> forest_distance_table_type;
    struct subforest_pairs
    {
        subforest f1;
        subforest f2;
    };
    typedef std::unordered_map<size_t,
            std::unordered_map<size_t, size_t>> tree_distance_table_type;
    struct decomposition_type
    {
        typedef std::vector<tree_type::iterator> relevant_subtrees;
        typedef std::vector<tree_type::iterator> path_type;

        path_type path;
        relevant_subtrees subtrees;
    };
public:
    gted(const tree_type& _t1, const tree_type& _t2);
    void run_gted();
private:
    decomposition_type path_decomposition(tree_type::iterator it,
                                        const tree_type& t,
                                        const rted::map_type& t_sizes,
                                        path_strategy s) const;

    size_t biggest_subtree_child(tree_type::iterator root,
                                const tree_type& t,
                                const rted::map_type& t_sizes) const;

    void init_tree_dist_table();
    void init_forest_dist_table(forest_distance_table_type& table,
                                subforest_pairs p,
                                graph who_first) const;

    void compute_forest_distances_recursive(tree_type::iterator root1,
                                            tree_type::iterator root2);
    void compute_forest_distances(forest_distance_table_type& table,
                                    subforest_pairs p,
                                    graph who_first);
    void fill_distance_tables(forest_distance_table_type& table,
                            const subforest_pairs& root_forests,
                            const subforest_pairs& prevs,
                            tree_type::iterator tree_root1,
                            tree_type::iterator tree_root2,
                            graph who_first);

    inline void set_forest_distance_table_value(forest_distance_table_type& table,
                                                const subforest& index1,
                                                const subforest& index2,
                                                size_t value,
                                                graph who_first) const;
    inline size_t get_forest_distance_table_value(const forest_distance_table_type& table,
                                                const subforest& index1,
                                                const subforest& index2,
                                                graph who_first) const;

    void print_TDist() const;
    void print_FDist(const forest_distance_table_type& table) const;
    void pretty_printF(const subforest& index1,
                        const subforest& index2,
                        const forest_distance_table_type& forest_dist,
                        graph who_first) const;
    void pretty_print(const subforest& index1,
                    const subforest& index2,
                    graph who_first) const;
private:
    tree_type t1;
    tree_type t2;
    rted::map_type t1_sizes;
    rted::map_type t2_sizes;
    rted::strategy_map_type strategies;
    tree_distance_table_type tree_distances;
};

#endif /* !GTED_HPP */

