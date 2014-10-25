/*
 * rted.hpp
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef RTED_HPP
#define RTED_HPP

#include "types.hpp"
#include "rna_tree.hpp"
#include <unordered_map>




class rted
{
private:
    typedef tree<node_base<std::string>> tree_type;
    typedef std::unordered_map<size_t, size_t> map_type;
    tree_type::iterator most_left(const tree_type& t) const;
    tree_type::iterator most_right(const tree_type& t) const;
    bool is_leaf(tree_type::iterator_base it) const;

    void check_map_contains_children(const tree_type& t, tree_type::iterator it, const map_type& m) const;
    //void check_same_tree(const tree_type& t, tree_type::iterator it) const; // NEFUNGUJE!!! z iteratora sa neda zistit ci ukazuje do daneho stromu..

    void compute_full_decomposition(const tree_type& t, tree_type::iterator it, map_type& A, map_type& ALeft, map_type& ARight);
    void compute_relevant_subforrests(const tree_type& t, tree_type::iterator it, map_type& m_left, map_type& m_right, map_type& m_size);
    void compute_subtree_size(const tree_type& t, tree_type::iterator it, map_type& m);

    void update_T2_LR_w_tables(tree_type::iterator it, size_t c_min);
    void update_T2_H_w_table(tree_type::iterator it, size_t c_min);
    
    void update_T1_LR_v_tables(tree_type::iterator it1, tree_type::iterator it2, size_t c_min);
    void update_T1_H_v_table(tree_type::iterator it1, tree_type::iterator it2, size_t c_min);

    void init_T1_LRH_v_tables(tree_type::iterator it1, tree_type::iterator it2);
    void init_T2_LRH_w_tables(tree_type::iterator it);

    size_t get_value(size_t index1, size_t index2, const std::unordered_map<size_t, map_type>& m);
    size_t get_value(size_t index, const map_type& m);

public:
    rted(const rna_tree& _t1, const rna_tree& _t2);
    void run_rted();
    void test1();

private:
    tree_type t1;
    tree_type t2;
    
              //tables for A(Gw), .., F(Gw), ..
    map_type  T1_ALeft,
              T1_ARight,
              T1_A,
              
              T2_ALeft,
              T2_ARight,
              T2_A,
              
              T1_FLeft,
              T1_FRight,
              
              T2_FLeft,
              T2_FRight,

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


    struct t2_hw_partial_result{
        size_t subtree_size;
        size_t c_min;
        size_t H_value;
    };

    typedef std::unordered_map<size_t, t2_hw_partial_result> partial_result_map;

    partial_result_map T2_Hw_partials;
    std::unordered_map<size_t, partial_result_map> T1_Hv_partials;

    std::unordered_map<size_t, std::unordered_map<size_t, path_strategy>> STR;
};


#endif /* !RTED_HPP */

