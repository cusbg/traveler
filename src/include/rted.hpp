/*
 * File: rted.hpp
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

#ifndef RTED_HPP
#define RTED_HPP

#include "strategy.hpp"
#include "rna_tree.hpp"


// TODO: prepisat na templatovanu verziu, nepouzivat rna_tree, ale radsej tree_base<T>
class rted
{
public:
    typedef rna_tree                                    tree_type;
    typedef typename tree_type::iterator                iterator;
    typedef typename tree_type::post_order_iterator     post_order_iterator;
    typedef typename tree_type::sibling_iterator        sibling_iterator;

    typedef std::vector<size_t>                         table_type;

public:
    rted(
                const tree_type& _t1,
                const tree_type& _t2);
    void run();

private:
    /**
     * initializes tables to their needed size;
     * compute:
     *  subtree size
     *  full decomposition,
     *  relevant subforest tables
     */
    void init();

    /**
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
     * after computing, ALeft[ch1] and ARight[ch1] is not needed
     */
    void compute_full_decomposition(
                iterator it,
                table_type& A,
                table_type& ALeft,
                table_type& ARight);

    /**
     * upgrade relevant_subforests tables of it with respect to left/right path
     *
     * FLeft[parent] = 1 + F[mostleft_child] +
     *                  sum(Size[other_children] + F[other_children])
     * FRight[parent] = 1 + F[mostright_child] +
     *                  sum(Size[other_children] + F[other_children])
     */
    void compute_relevant_subforrests(
                iterator it,
                table_type& FLeft,
                table_type& FRight,
                table_type& Size);

    /**
     * update Size table from children:
     *
     * Size[parent] = sum(Size[ch]) + 1
     *  where ch are children
     */
    void compute_subtree_size(
                iterator it,
                table_type& Size);

    /**
     * initialize L/R/H_v tables for leaf it1
     *
     * T1_{L,R,H}v[it1_id][it2_id] = 0;
     */
    void init_T1_LRH_v_tables(
                iterator it1,
                iterator it2);

    /**
     * initialize L/R/H_w tables for leaf it
     *
     * T2_{L,R,H}w[it_id] = 0;
     */
    void init_T2_LRH_w_tables(
                iterator it2);

    /**
     * checks initialization for *LRH* tables
     * and for parents of it-s too,
     * if parents are not initalized, init parent
     *      -- visiting first_child(parent)
     */
    void first_visit(
                iterator it1,
                iterator it2);

    /**
     * compute C from rted_opt_strategy(F,G) (== lines 7-12)
     * find minimum and stores minimal_path
     * returns c_min
     */
    size_t update_STR_table(
                iterator it1,
                iterator it2);

    /**
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
     *  as in update_T2_H_w_table, we use table T1_Hv_partials,
     *  where we store tuple (subtree_size, c_min, H_value) of heaviest child
     *  if i find heavier ch2 (Size[ch2] > subtree_size) i compute
     *  T1_Hv[parent1_id][it2_id] = c_min - H_value + T1_Hv[it1_id][it2_id]
     *  and store values from ch2:
     *       subtree_size = Size[ch2];
     *       c_min = function_c_min;
     *       H_value = T1_Hv[it1_id][it2_id];
     */
    void update_T1_LRH_v_tables(
                iterator it1,
                iterator it2,
                size_t c_min);

    /**
     * == ekvivalent to lines 20, 21, 22 in rted_opt_strategy(F,G)
     *
     * T2_Lw[parent_it] += (it == leftmost_child) ? T2_Lw[it] : c_min;
     * T2_Rw[parent_it] += (it == rightmost_child) ? T2_Rw[it] : c_min;
     * T2_Hw[parent_it] += (it is on heavy_parents_path) ? T2_Hw[it] : c_min;
     *
     * How to predict we are on heavy_parent_path???
     *      ... See update_T1_LRH_v_tables()
     */
    void update_T2_LRH_w_tables(
                iterator it2,
                size_t c_min);

private:
    void check_postorder();

public:
    strategy_table_type& get_strategies();

private:
    tree_type
                t1,
                t2;

    strategy_table_type
                STR;

    //tables for A(Gw), .., F(Gw), ..
    table_type
                // A == full decomposition
                T1_A,
                T2_A,

                // F* = relevant subforests tables
                T1_FLeft,
                T1_FRight,

                T2_FLeft,
                T2_FRight,

                // subtree sizes
                T1_Size,
                T2_Size,

                //main loop, {LRH}w
                T2_Lw,
                T2_Rw,
                T2_Hw;

    // 2D tables: {LRH}v[v_id][w_id] == value
    std::vector<table_type>
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
    typedef std::vector<t2_hw_partial_result> 
                partial_result_arr;

    partial_result_arr
                T2_Hw_partials;
    std::vector<partial_result_arr>
                T1_Hv_partials;

};



#endif /* !RTED_HPP */

