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
#include "rted.hpp"
#include <unordered_map>


class gted
{
private:
public: // TODO << remove public >>
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
                std::unordered_map<subforest, size_t, subforest::hash>,
                    subforest::hash> forest_distance_table_type;
    struct subforest_pair
    {
        subforest f1;
        subforest f2;
    };
    typedef std::unordered_map<size_t,
            std::unordered_map<size_t, size_t>> tree_distance_table_type;
    struct iterator_pair
    {
        tree_type::iterator it1;
        tree_type::iterator it2;

        iterator_pair() = default;
        iterator_pair(const subforest& other);

        bool operator==(const iterator_pair& other) const;
        struct hash
        {
            size_t operator()(const iterator_pair& other) const;
        };
    };
    struct tables
    {
        typedef std::vector<tree_type::iterator>
                                        relevant_subforests_type;
        typedef std::vector<tree_type::iterator>
                                        keyroots_type;
        typedef std::unordered_map<size_t, std::pair<size_t, size_t>>
                                        vector_indexes_type;
        typedef std::unordered_map<size_t, tree_type::iterator>
                                        root_leaf_path_type;

        struct
        {
            /*
             * relevant subforests for tree
             *  index, where we should start our computation, is
             *      i = indexes[id(path_leaf(node))].first
             *  and we compute while
             *      id(subforests[i]) <= id(node)
             */
            relevant_subforests_type
                        subforests;

            /*
             * keyroots for tree
             *  indes, where we should start our computation, is
             *      i = indexes[id(path_leaf(node))].second
             *  and we compute while
             *      path_leaf(parent(keyroots[i])) == path_leaf(node)
             */
            keyroots_type
                        keyroots;

            /*
             * indexes to subforests and keyroots tables
             *  .first  -> subforests
             *  .second -> keyroots
             * defined only for path_leaf nodes
             */
            vector_indexes_type
                        indexes;

            /*
             * leafs on root-leaf path
             * defined for all nodex in tree.
             */
            root_leaf_path_type
                        leafs;

        } left, right, heavy;

    } t1_tables, t2_tables;
    typedef std::vector<iterator_pair> mapping_table_type;
public:
    gted(const tree_type& _t1, const tree_type& _t2);
    void run_gted();
private:
    void precompute_paths();



    /**
     * rekurzivne rozkladam stromy podla strategie z rted-u
     * nakoniec pustim single_path_function(root1, root2)
     * ktora vyrata
     *      T[root1][root2]
     *      T[i][j], pre i, j vrcholy na root-leaf path
     */
    void compute_distances_recursive(
                    tree_type::iterator root1,
                    tree_type::iterator root2);
    /*{
        compute_distances_recursive(root1, root2);
        single_path_function();
    }*/

    void single_path_function();
    /*{
        // decompone T2
        compute_distance();
    }*/
    void compute_distance();
    //{}


    tree_type::iterator get_path_leaf(
                    tree_type::iterator root,
                    strategy_pair str);



#ifdef NODEF

    bool do_decompone_H(tree_type::iterator& it_ref,
                        tree_type::iterator root,
                        tree_type::iterator& it_path_node) const;
    bool do_decompone_H_recursive(tree_type::iterator& it_ref,
                                    tree_type::iterator& leaf,
                                    tree_type::iterator end) const;
    void single_path_function_H(tree_type::iterator root1,
                                tree_type::iterator root2,
                                graph who_first);


    bool do_decompone_LR(tree_type::iterator& it_ref,
                        tree_type::iterator root,
                        path_strategy str) const;
    bool do_decompone_LR_recursive(tree_type::iterator& it_ref,
                                    tree_type::iterator& leaf,
                                    tree_type::iterator end,
                                    path_strategy str) const;
    void single_path_function_LR(tree_type::iterator root1,
                                    tree_type::iterator root2,
                                    path_strategy str,
                                    graph who_first);

    void fill_table(forest_distance_table_type& forest_dist,
                    const subforest_pair& roots,
                    const subforest_pair& prevs,
                    iterator_pair prev_roots,
                    graph who_first);

    forest_distance_table_type compute_distance(subforest_pair pair,
                                                graph who_first);
    void compute_mapping();



    /**
     * zinicializuje tabulku forest_dist na vzdialenosti pri mazani vrcholov
     * teda pre kazdy f = podles T1 -> F[f][EMPTY] = |f|
     * a naopak kazdy f = podles T2 -> F[EMPTY][f] = |f|
     */
    void init_FDist_table(forest_distance_table_type& forest_dist,
                        subforest_pair subforests);

    /**
     * zinicializuje pair tak, ze
     *  .left == .right == .path_node == list na root-leaf-path
     *  .root == root
     */
    void init_subforest_pair(subforest_pair& pair,
                        tree_type::iterator root1,
                        tree_type::iterator root2,
                        path_strategy str,
                        graph who_first) const;

    void precompute_heavy_paths();

    /**
     * vrati index potomka s najvacsim podstromom
     */
    size_t biggest_subtree_child(tree_type::iterator root,
                                const tree_type& t,
                                const rted::map_type& t_sizes) const;

    void print_TDist() const;
    void print_TDist(tree_distance_table_type distances);
    void print_FDist(const forest_distance_table_type& table) const;
    void print_subforest(const subforest& f);

    size_t get_Fdist(const subforest& index1,
                    const subforest& index2,
                    const forest_distance_table_type& table) const;
    size_t get_Tdist(tree_type::iterator index1,
                    tree_type::iterator index2,
                    graph who_first) const;
    void set_Fdist(const subforest& index1,
                    const subforest& index2,
                    forest_distance_table_type& table,
                    size_t value) const;
    void set_Tdist(tree_type::iterator index1,
                    tree_type::iterator index2,
                    size_t value,
                    graph who_first);
#endif

    using empty_iterator = tree_type::iterator;

#ifdef TESTS
public:
    static void test();
#endif

private:
    tree_type t1;
    tree_type t2;
    rted::map_type t_sizes;
    rted::strategy_map_type strategies;
    tree_distance_table_type tree_distances;
    path_tables paths;
    mapping_table_type mapping;
};

#endif /* !GTED_HPP */

