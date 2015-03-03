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



//template <typename tree_type>
class gted
{
    typedef rted::tree_type tree_type;
    // Pozn: relevant_subtrees su ekvivalentne (LR_keyroots-root_node) v zhang_shasha
    typedef std::vector<tree_type::iterator> relevant_subtrees;
    //typedef std::unordered_map<size_t, tree_type::iterator> path_type;
    typedef std::vector<tree_type::iterator> path_type;

    struct decomposition_type
    {
        relevant_subtrees subtrees;
        path_type path;
    };
    struct subtree
    {
        // ~> interval [left, right]
        // teda podstromy generovane begin/end-om
        tree_type::reverse_post_order_iterator left;    // included
        tree_type::post_order_iterator right;           // included
        tree_type::iterator root;
        tree_type::iterator path_node;

        bool operator==(const subtree& other) const;
        struct hash
        {
            size_t operator()(const subtree& s) const;
        };
    };
    typedef std::unordered_map<subtree, size_t, subtree::hash> subtree_map_type;
    typedef std::unordered_map<subtree, subtree_map_type, subtree::hash> forest_distance_table;

    struct subtree_pairs
    {
        subtree t1;
        subtree t2;
    };
    struct iterator_pairs
    {
        tree_type::iterator it1;
        tree_type::iterator it2;
    };
public:
    gted(const tree_type& _t1, const tree_type& _t2);
    void run_gted();
    void print_distances() const;

private:
    /**
     * decompone tree t by path_strategy s
     * returns struct decomposition_type as pair (siblings of path_nodes; path_nodes)
     */
    decomposition_type
    path_decomposition(tree_type::iterator it,
                        const tree_type& t,
                        const rted::map_type& t_size,
                        path_strategy s) const;


    size_t biggest_subtree_child(tree_type::iterator root,
                                const tree_type& t,
                                const rted::map_type& t_size) const;
    inline bool node_lies_on_path(tree_type::iterator it,
                                const path_type& path) const;


    void init_forest_dist_table(forest_distance_table& table,
                                subtree_pairs p) const;
    void init_tree_dist_table();

    void compute_forest_distances_recursive(
                                            tree_type::iterator root1,
                                            tree_type::iterator root2);
    void compute_forest_distances(forest_distance_table& table,
                                    subtree_pairs p);
    void fill_tables(forest_distance_table& table,
                    const subtree_pairs& roots,
                    const subtree_pairs& prevs);


    void print_FDist(const forest_distance_table& table) const;
private:
    tree_type t1;
    tree_type t2;
    rted::map_type t1_sizes;
    rted::map_type t2_sizes;
    rted::strategy_map_type strategies;
private:
    typedef std::unordered_map<size_t, rted::map_type> distance_table;
    distance_table tree_distances;
};





#endif /* !GTED_HPP */

