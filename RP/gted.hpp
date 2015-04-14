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
    typedef rted::tree_type tree_type;
    struct subforest
    {
        tree_type::reverse_post_order_iterator  left;
        tree_type::post_order_iterator          right;
        tree_type::iterator                     root;
        tree_type::iterator                     path_node;

        enum last_added {Lnode, Rnode, undef}   last = undef;
    };
    struct subforest_pair
    {
        subforest f1;
        subforest f2;
    };
    struct iterator_pair
    {
        iterator_pair() = default;
        iterator_pair(std::initializer_list<tree_type::iterator> const list)
            : it1(*list.begin()), it2(*(list.begin() + 1)) {}
        iterator_pair(const subforest& other)
            : it1(other.left), it2(other.right) {}

        inline bool operator==(const iterator_pair& other) const;
        struct hash
        {
            inline size_t operator()(const iterator_pair& other) const;
        };


        tree_type::iterator it1;
        tree_type::iterator it2;
    };
    typedef std::unordered_map<iterator_pair,
                std::unordered_map<iterator_pair, size_t, iterator_pair::hash>,
                    iterator_pair::hash> forest_distance_table_type;
    typedef std::unordered_map<size_t,
                std::unordered_map<size_t, size_t>> tree_distance_table_type;
    typedef std::vector<iterator_pair> mapping_table_type;


public:
    gted(const tree_type& _t1, const tree_type& _t2);
    ~gted();
    void run_gted();

private:
    /**
     * predpocita tabulky leafs, keyroots, subforests
     */
    void precompute_tables();

    /**
     * hlavna funkcia, v danych korenoch sa pozrie do tabulky strategii
     * a dekomponuje podla nej strom
     *  -> vola compute_dustances_recursive(root_i, keyroot(root_i2))
     *  pre nejake poradie vrcholov root_i, root_i2.
     * takto prejde vsetky podstromy susediace s path_node podla strategie
     */
    void compute_distances_recursive(
                    iterator_pair roots);
    /**
     * vyrata tree_distance(root1, root2)
     * dekomponuje druhy strom (nie ten z rekurzie) pomocou tabulky subforests,
     *  na tie vola compute_distance(root_i, subforest(root_i2))
     */
    void single_path_function(
                    iterator_pair roots,
                    strategy_pair str);

    /**
     * dopocita vzdialenost vsetkych vrcholov na path
     * iteruje stromamy tak, ze zacne v liste,
     *  pridava najprv lave vrcholy kym nedojde do otca
     *  potom sa vrati o krok spat, a pridava prave vrcholy kym nedojde do otca
     *  potom opakuje tie 2 body az kym niesom v root-och
     */
    forest_distance_table_type compute_distance(
                    subforest_pair forests,
                    strategy_pair str);
    /**
     * inicializacia tabulky, v podstate iteruje ako pri compute_distance
     * ale pocita iba mazanie vrcholov.
     */
    void init_FDist_table(
                    forest_distance_table_type& forest_dist,
                    subforest_pair forests) const;
    /**
     * samotne hladanie vyslednej minimalnej hodnoty a dosadenie do tabuliek
     *  (blizsie info priamo vo funkcii compute_distance())
     */
    void fill_table(
                    forest_distance_table_type& table,
                    const subforest_pair& roots,
                    const subforest_pair& prevs,
                    const iterator_pair& prev_roots,
                    strategy_pair str);

    //void compute_mapping();

    inline bool is_in_subtree(
                    tree_type::iterator root,
                    tree_type::iterator it) const;
    inline bool is_keyroot(
                    tree_type::iterator root,
                    tree_type::iterator it,
                    strategy_pair str) const;

    inline size_t get_Fdist(
                    forest_distance_table_type& table,
                    const subforest& index1,
                    const subforest& index2) const;
    inline void set_Fdist(
                    forest_distance_table_type& table,
                    const subforest& index1,
                    const subforest& index2,
                    size_t value) const;
    inline size_t get_Tdist(
                    tree_type::iterator it1,
                    tree_type::iterator it2,
                    strategy_pair str);
    inline void set_Tdist(
                    tree_type::iterator it1,
                    tree_type::iterator it2,
                    strategy_pair str,
                    size_t value);

    inline void print_TDist() const;
    inline void print_FDist(
                    const forest_distance_table_type& table) const;

    inline void print_precomputed() const;
    inline void print_subforests(
                    tree_type::iterator root,
                    strategy_pair str) const;
    inline void print_keyroots(
                    tree_type::iterator root,
                    strategy_pair str) const;
    inline void print_subforest(
                    const subforest& f);

#ifdef TESTS
public:
    static void test();
#endif

private:
    class tables
    {
    public:
        struct LRH
        {
            tree_type::iterator
                    left,
                    right,
                    heavy;
        };
        struct indexes_pair_type
        {
            size_t
                    subforest,
                    keyroot;
        };

        typedef std::unordered_map<size_t, LRH>
                                        root_leaf_path_type;
        typedef std::unordered_map<size_t, indexes_pair_type>
                                        indexes_type;
        typedef std::vector<tree_type::iterator>
                                        vector_type;


        inline indexes_pair_type get_indexes(const tree_type::iterator& it, strategy_pair str) const;
        inline tree_type::iterator get_leaf(const tree_type::iterator& it, strategy_pair str) const;
        inline LRH get_leafs(const tree_type::iterator& it) const;
        inline tree_type::iterator get_subforest(size_t index, strategy_pair str) const;
        inline tree_type::iterator get_keyroot(size_t index, strategy_pair str) const;

        friend void gted::precompute_tables();
        friend void gted::print_precomputed() const;

    private:
        // indexovane pomocou PATH_STRATEGY_* makier, [0-5]
        std::vector<vector_type>
                    subforests,
                    keyroots;

        // Left/Right/Heavy
        std::vector<indexes_type>
                    indexes;

        root_leaf_path_type
                    leafs;

    public:
        tables();

    /*struct tables
    {
        [>
         * tabulky s predpocitanymi hodnotamy
         *      subforests, keyroots
         * pre stromy T1 a T2.
         <]

        struct LRH
        {
            tree_type::iterator
                        left,
                        right,
                        heavy;
        };
        struct index_pair
        {
            size_t subforest, keyroot;
        };
        typedef std::vector<tree_type::iterator>
                                        relevant_subforests_type;
        typedef std::vector<tree_type::iterator>
                                        keyroots_type;
        typedef std::unordered_map<size_t, index_pair>
                                        vector_indexes_type;
        typedef std::unordered_map<size_t, LRH>
                                        root_leaf_path_type;

        struct sub_key_tables
        {
            relevant_subforests_type
                            sub_left,
                            sub_right,
                            sub_heavy;

            keyroots_type
                            key_left,
                            key_right,
                            key_heavy;

        } t1_tables, t2_tables;

        [>
         * indexes to t*_tables vectors
         <]
        vector_indexes_type
                            ind_left,
                            ind_right,
                            ind_heavy;

        [>
         * leafs on root-leaf path
         * (left, right, heavy) tupples
         <]
        root_leaf_path_type
                    leafs;

[>
        struct
        {
            [>
             * relevant subforests for tree
             *  index, where we should start our computation, is
             *      i = indexes[id(path_leaf(node))].first
             *  and we compute while
             *      id(subforests[i]) <= id(node)
             <]
            relevant_subforests_type
                            t1_subforests,
                            t2_subforests;

            [>
             * keyroots for tree
             *  indes, where we should start our computation, is
             *      i = indexes[id(path_leaf(node))].second
             *  and we compute while
             *      path_leaf(parent(keyroots[i])) == path_leaf(node)
             <]
            keyroots_type
                            t1_keyroots,
                            t2_keyroots;

            [>
             * indexes to subforests and keyroots tables
             *  .first  -> subforests
             *  .second -> keyroots
             * defined only for path_leaf nodes
             <]
            vector_indexes_type
                            t12_indexes;

        } left, right, heavy;
<]


        size_t get_keyroot_index    (tree_type::iterator it, strategy_pair str) const;
        size_t get_subforests_index (tree_type::iterator it, strategy_pair str) const;
        tree_type::iterator get_leaf(tree_type::iterator it, strategy_pair str) const;
        tree_type::iterator get_keyroot(size_t index, strategy_pair str) const;
        tree_type::iterator get_subforest(size_t index, strategy_pair str) const;
    };*/
    };

private:
    tree_type t1;
    tree_type t2;
    rted::map_type t_sizes;
    rted::strategy_map_type strategies;
    tree_distance_table_type tree_distances;
    mapping_table_type mapping;
    tables precomputed;

    using empty_iterator = tree_type::iterator;
};

#endif /* !GTED_HPP */

