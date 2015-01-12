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
    //typedef RNA_tree_type tree_type;
    typedef tree_base<node_base<std::string>> tree_type;
    // Pozn: relevant_subtrees su ekvivalentne (LR_keyroots-root_node) v zhang_shasha
    typedef std::vector<tree_type::iterator> relevant_subtrees;
    typedef std::unordered_map<size_t, tree_type::iterator> path_type;
    typedef rted::map_type map_type;
    typedef std::unordered_map<size_t, map_type> distance_table;
    struct decomposition_type
    {
        relevant_subtrees subtrees;
        path_type path;
    };
private:
    tree_type t1;
    tree_type t2;
    rted::map_type t1_sizes;
    rted::map_type t2_sizes;
    rted::strategy_map_type strategies;
public:
    gted(const tree_type& _t1, const tree_type& _t2);
    void run_gted();
    void print_distances() const;

private:
    /**
     * decompone tree t by path_strategy s
     * returns struct decomposition_type as pair (siblings of path_nodes; path_nodes)
     */
    decomposition_type path_decomposition(tree_type::iterator it,
            const tree_type& t, const map_type& t_size, path_strategy s);
    decomposition_type left_decomposition(tree_type::iterator it);
    decomposition_type right_decomposition(tree_type::iterator it);
    decomposition_type heavy_decomposition(tree_type::iterator it,
            const tree_type& t, const map_type& t_size);


    size_t biggest_subtree_child(tree_type::iterator root,
            const tree_type& t, const map_type& t_size) const;


    /** return 0 if (is_leftmost || from > to) or to.id() */
    size_t index_from_ids(tree_type::iterator it1, tree_type::iterator it2, bool is_leftmost = false) const;

    void init_forest_dist_table(distance_table& table, tree_type::iterator root1, tree_type::iterator root2);
    void init_tree_dist_table();

    void compute_distance_recursive(tree_type::iterator it1, tree_type::iterator it2);
    void compute_distance(tree_type::iterator it1, tree_type::iterator it2);

    void left(tree_type::iterator root1, tree_type::iterator root2, const decomposition_type& dec1, const decomposition_type& dec2);
    void right(tree_type::iterator root1, tree_type::iterator root2, const decomposition_type& dec1, const decomposition_type& dec2);
    void heavy(tree_type::iterator root1, tree_type::iterator root2, const decomposition_type& dec1, const decomposition_type& dec2);
/*
    void compute_distance_left(tree_type::iterator it1, tree_type::iterator it2);
    void compute_distance_right(tree_type::iterator it1, tree_type::iterator it2);
    void compute_distance_heavy(tree_type::iterator it1, tree_type::iterator it2);
*/
    inline bool node_lies_on_path(tree_type::iterator it, const path_type& path) const;

private:
    distance_table tree_distances;
};





#endif /* !GTED_HPP */

