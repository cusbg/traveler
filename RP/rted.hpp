/*
 * File: rted.hpp
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

#ifdef NODEF

#ifndef RTED_HPP
#define RTED_HPP

#include "types.hpp"
#include "tree/rna_tree.hpp"
#include <unordered_map>


class rted
{
public:
    typedef std::unordered_map<size_t,
                std::unordered_map<size_t,
                    strategy_pair>>        strategy_map_type;
    typedef tree_base<node_base<std::string>>   tree_type;
    typedef std::unordered_map<size_t, size_t>  map_type;

private:
    /*
     * upgrade full_decomposition tables
     *
     * ALeft == left_decomposition
     * ARight == right_decomposition
     * A[parent] = sum(ALeft[ch1] * ARight[ch2]) + sum(A[ch]) + 1
     *  where ch, ch1, ch2 are children, and ch1<ch2
     *
     * ALeft[parent] = sum(ALeft[ch]) + 1
     * ARight[parent] = sum(ARight[ch]) + 1
     *
     * we can delete ALeft[ch1] and ARight[ch1]
     */
    void compute_full_decomposition(
                    tree_type::iterator it,
                    map_type& A,
                    map_type& ALeft,
                    map_type& ARight);

    /*
     * upgrade relevant_subforests tables of it with respect to left/right path
     *
     * FLeft[parent] = 1 + F[mostleft_child] +
     *                  sum(Size[other_children] + F[other_children])
     * FRight[parent] = 1 + F[mostright_child] +
     *                  sum(Size[other_children] + F[other_children])
     *
     *  where   mostleft_child_index == 0,
     *          other_children_index == 1,..,n-1,
     *          rightmost_child_index == n-1
     */
    void compute_relevant_subforrests(
                    tree_type::iterator it,
                    map_type& FLeft,
                    map_type& FRight,
                    map_type& Size);

    /*
     * update Size table from children:
     *
     * Size[parent] = sum(Size[ch]) + 1
     *  where ch are children
     */
    inline void compute_subtree_size(
                    tree_type::iterator it,
                    map_type& Size);

    /*
     * == ekvivalent to lines 20, 21, 22 in rted_opt_strategy(F,G)
     *
     * T2_Lw[parent_it] += (it == leftmost_child) ? T2_Lw[it] : c_min;
     * T2_Rw[parent_it] += (it == rightmost_child) ? T2_Rw[it] : c_min;
     * T2_Hw[parent_it] += (it is on heavy_parents_path) ? T2_Hw[it] : c_min;
     *
     * How to predict we are on heavy_parent_path???
     *  it cant be predicted if term is true or false, so i am using another map
     *  T2_Hw_partials, where is stored tuple (subtree_size, c_min, H_value)
     *  of heaviest child i saw. if i find another ch2, that
     *  Size[ch2] > subtree_size, i compute
     *  T2_Hw[parent_it] += c_min - H_value + T2_Hw[ch2]
     *  and store values from ch2:
     *       subtree_size = Size[ch2];
     *       c_min = function_c_min;
     *       H_value = T2_Hw[ch2]
     */
    inline void update_T2_LRH_w_tables(
                    tree_type::iterator it,
                    size_t c_min);

    /*
     * == ekvivalent to lines 16, 17, 18 in rted_opt_strategy(F,G)
     *
     * T1_Lv[parent_it1][it2] += (it1 == leftmost_child) ?
     *                              T1_Lv[it1][it2] : c_min;
     * T1_Rv[parent_it1][it2] += (it1 == leftmost_child) ?
     *                              T1_Rv[it1][it2] : c_min;
     * T1_Hv[parent_it1][it2] += (it1 is on heavy_parents_path) ?
     *                              T2_Hw[it1][it2] : c_min;
     *
     * How to predict we are on heavy_parent_path???
     *  as in update_T2_H_w_table, we use map T1_Hv_partials,
     *  where we store tuple (subtree_size, c_min, H_value) of heaviest child
     *  if i find heavier ch2 (Size[ch2] > subtree_size) i compute
     *  T1_Hv[parent1_id][it2_id] = c_min - H_value + T1_Hv[it1_id][it2_id]
     *  and store values from ch2:
     *       subtree_size = Size[ch2];
     *       c_min = function_c_min;
     *       H_value = T1_Hv[it1_id][it2_id];
     */
    inline void update_T1_LRH_v_tables(
                    tree_type::iterator it1,
                    tree_type::iterator it2,
                    size_t c_min);

    /*
     * initialize L/R/H_v tables for leaf it1
     *
     * T1_{L,R,H}v[it1_id][it2_id] = 0;
     */
    inline void init_T1_LRH_v_tables(
                    tree_type::iterator it1,
                    tree_type::iterator it2);

    /*
     * initialize L/R/H_w tables for leaf it
     *
     * T2_{L,R,H}w[it_id] = 0;
     */
    inline void init_T2_LRH_w_tables(
                    tree_type::iterator it);

    /*
     * initializes tables to their needed size
     */
    inline void init();

    /*
     * compute C from rted_opt_strategy(F,G) (== lines 7-12)
     * find minimum and stores minimal_path
     * returns c_min
     */
    size_t update_STR_table(
                    tree_type::iterator it1,
                    tree_type::iterator it2);

    /*
     * check if for each child of it, map contains it
     */
    inline void check_map_contains_children(
                    tree_type::iterator it,
                    const map_type& m) const;

    inline void tables_precomputation();

#ifdef TESTS
public:
    static void test();
#endif

public:
    rted(const tree_type& _t1, const tree_type& _t2);
    void run_rted();
    strategy_map_type&& get_strategies()
        { return std::move(STR); }
    map_type&& get_t1_sizes()
        { return std::move(T1_Size); }
    map_type&& get_t2_sizes()
        { return std::move(T2_Size); }

    /**
     * prints STR array to logger.debugStream()
     */
    inline void print_strategies() const;

private:
    tree_type
                t1,
                t2;

    //tables for A(Gw), .., F(Gw), ..
    map_type
                // A* = decomposition tables.
                // ALeft/ARight == left/right decomposition
                // A == full decomposition
                T1_ALeft,
                T1_ARight,
                T1_A,

                T2_ALeft,
                T2_ARight,
                T2_A,

                // F* = relevant subforests tables
                T1_FLeft,
                T1_FRight,

                T2_FLeft,
                T2_FRight,

                // subtree sizes
                T1_Size,
                T2_Size,

                //main loop, {L,R,H}w
                T2_Lw,
                T2_Rw,
                T2_Hw;

    // 2D maps: _map[v_id][w_id] == value
    std::unordered_map<size_t, map_type>
                T1_Lv,
                T1_Rv,
                T1_Hv;


    struct t2_hw_partial_result {
        // for more details, see functions:
        // update_T2_LRH_w_tables or update_T1_LRH_v_tables
        size_t subtree_size;
        size_t c_min;
        size_t H_value;
    };
    typedef std::unordered_map<size_t, t2_hw_partial_result>
                partial_result_map;

    partial_result_map
                T2_Hw_partials;
    std::unordered_map<size_t, partial_result_map>
                T1_Hv_partials;

    strategy_map_type
                STR;
};

#endif /* !RTED_HPP */

#endif

