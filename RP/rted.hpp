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

#include "types.hpp"
#include "rna_tree.hpp"
#include "strategy.hpp"


class rted
{
public:
    typedef rna_tree                            tree_type;
    typedef strategy_type                       strategy;
    typedef std::vector<std::vector<strategy>>  strategy_table_type;
    typedef std::vector<size_t>                 table_type;

    typedef tree_type::iterator                 iterator;
    typedef tree_type::post_order_iterator      post_order_iterator;
    typedef tree_type::sibling_iterator         sibling_iterator;

public:
    rted(
                tree_type& _t1,
                tree_type& _t2);
    void run_rted();

private:
    void init();
    void tree_ids_postorder();

    void compute_full_decomposition(
                iterator it,
                table_type& A,
                table_type& ALeft,
                table_type& ARight);
    void compute_relevant_subforrests(
                iterator it,
                table_type& FLeft,
                table_type& FRight,
                table_type& Size);
    void compute_subtree_size(
                iterator it,
                table_type& Size);
    void init_T1_LRH_v_tables(
                iterator it1,
                iterator it2);
    void init_T2_LRH_w_tables(
                iterator it);
    size_t update_STR_table(
                iterator it1,
                iterator it2);
    void update_T2_LRH_w_tables(
                iterator it,
                size_t c_min);
    void update_T1_LRH_v_tables(
                iterator it1,
                iterator it2,
                size_t c_min);

public:
    strategy_table_type& get_strategies()
        { return STR; } 
    table_type& get_t1_sizes()
        { return T1_Size; }
    table_type& get_t2_sizes()
        { return T2_Size; }

private:
    tree_type
                &t1,
                &t2;

    strategy_table_type
                STR;

    //tables for A(Gw), .., F(Gw), ..
    table_type
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

#ifdef TESTS
public:
    static void test();
#endif
};


#endif /* !RTED_HPP */

