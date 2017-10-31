/*
 * File: rted.cpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard.elias@matfyz.cz>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */


#include "rted.hpp"

#define RTED_BAD        size_t(-0xBADF00D)
#define isbad(value)    ((value) == RTED_BAD)

// TODO: mozno prepisat podobne ako gted - vytvorit iny rted_tree,
//  v ktorom budu ulozene vsetky potrebne tabulky

using namespace std;

rted::rted(
           const tree_type& _t1,
           const tree_type& _t2)
: t1(_t1), t2(_t2)
{
    APP_DEBUG_FNAME;
    
    check_postorder();
}

void rted::run()
{
    APP_DEBUG_FNAME;
    
    init();
    
    INFO("BEG: Computing RTED between RNAs %s and %s",
         t1.name(), t2.name());
    
    for (post_order_iterator it1 = t1.begin_post(); it1 != t1.end_post(); ++it1)
    {
        for (post_order_iterator it2 = t2.begin_post(); it2 != t2.end_post(); ++it2)
        {
            first_visit(it1, it2);
            
            size_t c_min = update_STR_table(it1, it2);
            
            if (!tree_type::is_root(it1))
                update_T1_LRH_v_tables(it1, it2, c_min);
            if (!tree_type::is_root(it2))
                update_T2_LRH_w_tables(it2, c_min);
            
     
            
        }
    }
    DEBUG("Strategy computed, STR=%s", STR[id(t1.begin())][id(t2.begin())]);
    
    INFO("END: Computing RTED between RNAs %s and %s",
         t1.name(), t2.name());
}

void rted::init()
{
    APP_DEBUG_FNAME;
    
    size_t size1, size2;
    
    DEBUG("BEG prepare tables");
    
    strategy_table_type::value_type inner_str;
    table_type inner_v;
    
    size1 = t1.size();
    size2 = t2.size();
    
    t1.print_tree();
    t2.print_tree();
    
    // STR table:
    inner_str.resize(size2);
    STR.resize(size1, inner_str);
    
    // {L,R,H}v tables:
    inner_v.resize(size2, RTED_BAD);
    for (auto table : {&T1_Lv, &T1_Rv, &T1_Hv})
        table->resize(size1, inner_v);
    
    // {L, R, H}w tables:
    for (auto table : {&T2_Lw, &T2_Rw, &T2_Hw})
        table->resize(size2, RTED_BAD);
    
    // partial tables:
    T2_Hw_partials.resize(size2);
    T1_Hv_partials.resize(size1, partial_result_arr(size2));
    
    // A* = decomposition tables.
    // ALeft/ARight == left/right decomposition
    // T_ALeft/ARight -> only to compute T_A
    table_type T1_ALeft, T1_ARight, T2_ALeft, T2_ARight;
    
    // initialize all tables to size of tree..
    for (auto table : {&T1_ALeft, &T1_ARight, &T1_A,
        &T1_FLeft, &T1_FRight, &T1_Size})
        table->resize(size1, RTED_BAD);
    for (auto table : {&T2_ALeft, &T2_ARight, &T2_A,
        &T2_FLeft, &T2_FRight, &T2_Size})
        table->resize(size2, RTED_BAD);
    
    DEBUG("END prepare tables");
    DEBUG("BEG precomputation");
    
    // precompute full-decomposition/relevant-subforest tables:
    for (post_order_iterator it = t1.begin_post(); it != t1.end_post(); ++it)
    {
        compute_full_decomposition(it, T1_A, T1_ALeft, T1_ARight);
        compute_subtree_size(it, T1_Size);
        compute_relevant_subforrests(it, T1_FLeft, T1_FRight, T1_Size);
    }
    for (post_order_iterator it = t2.begin_post(); it != t2.end_post(); ++it)
    {
        compute_full_decomposition(it, T2_A, T2_ALeft, T2_ARight);
        compute_subtree_size(it, T2_Size);
        compute_relevant_subforrests(it, T2_FLeft, T2_FRight, T2_Size);
    }
    assert(T1_Size[id(t1.begin())] == t1.size());
    assert(T2_Size[id(t2.begin())] == t2.size());
    
    DEBUG("END precomputation");
}

void rted::compute_full_decomposition(
                                      iterator it,
                                      table_type& A,
                                      table_type& ALeft,
                                      table_type& ARight)
{
    size_t it_id = id(it);
    size_t a, left, right;
    size_t ch_id;
    sibling_iterator ch, ch2;
    
    a =
    left =
    right = 1;
    
    
    for (ch = it.begin(); ch != it.end(); ++ch)
    {
        ch_id = id(ch);
        
        a       += A[ch_id];
        left    += ALeft[ch_id];
        right   += ARight[ch_id];
        
        ch2 = ch;
        ++ch2;
        for (; ch2 != it.end(); ++ch2)
            a += ALeft[ch_id] * ARight[id(ch2)];
    }
    
    A[it_id]        = a;
    ALeft[it_id]    = left;
    ARight[it_id]   = right;
}

void rted::compute_relevant_subforrests(
                                        iterator it,
                                        table_type& FLeft,
                                        table_type& FRight,
                                        table_type& Size)
{
    size_t it_id = id(it);
    size_t left, right;
    sibling_iterator beg, end;
    
    left = 1;
    right = 1;
    
    if (!tree_type::is_leaf(it))
    {
        // FLeft
        left += FLeft[id(tree_type::first_child(it))];
        beg = ++it.begin();
        end = it.end();
        for (; beg != end; ++beg)
            left += Size[id(beg)] + FLeft[id(beg)];
        
        // FRight
        right += FRight[id(tree_type::last_child(it))];
        beg = it.begin();
        end = tree_type::last_child(it);
        for (; beg != end; ++beg)
            right += Size[id(beg)] + FRight[id(beg)];
    }
    FLeft[it_id]    = left;
    FRight[it_id]   = right;
}

void rted::compute_subtree_size(
                                iterator it,
                                table_type& Size)
{
    size_t it_id = id(it);
    size_t s = 1;
    
    for (sibling_iterator ch = it.begin(); ch != it.end(); ++ch)
        s += Size[id(ch)];
    
    Size[it_id] = s;
}

void rted::init_T1_LRH_v_tables(
                                iterator it1,
                                iterator it2)
{
    assert(tree_type::is_leaf(it1));
    
    size_t it1_id = id(it1);
    size_t it2_id = id(it2);
    
    T1_Lv[it1_id][it2_id] =
    T1_Rv[it1_id][it2_id] =
    T1_Hv[it1_id][it2_id] = 0;
}

void rted::init_T2_LRH_w_tables(
                                iterator it2)
{
    size_t it2_id = id(it2);
    
    T2_Lw[it2_id] =
    T2_Rw[it2_id] =
    T2_Hw[it2_id] = 0;
}

void rted::first_visit(
                       iterator it1,
                       iterator it2)
{
    size_t it1_id = id(it1);
    size_t it2_id = id(it2);
    std::vector<bool> vec;
    
    if (tree_type::is_leaf(it1))
        init_T1_LRH_v_tables(it1, it2);
    if (tree_type::is_leaf(it2))
        init_T2_LRH_w_tables(it2);
    
#define all_same(v) (v[0] == v[1] && v[1] == v[2])
    auto init_parent_w_tables = [this](iterator iter2) {
        if (tree_type::is_root(iter2))
            return;
        
        size_t parent2_id = id(tree_type::parent(iter2));
        
        std::vector<bool> vec = {
            isbad(T2_Lw[parent2_id]),
            isbad(T2_Rw[parent2_id]),
            isbad(T2_Hw[parent2_id]),
        };
        if (!all_same(vec))
        {   // should be all inited/not-inited
            ERR("all_same");
            LOGGER_PRINT_CONTAINER(vec, "isbad w");
            abort();
        }
        if (vec[0] == true)
        {
            assert(tree_type::is_first_child(iter2));
            
            T2_Lw[parent2_id] =
            T2_Rw[parent2_id] =
            T2_Hw[parent2_id] = 0;
        }
    };
    auto init_parent_v_tables = [this](iterator iter1, iterator iter2) {
        if (tree_type::is_root(iter1))
            return;
        
        size_t parent1_id = id(tree_type::parent(iter1));
        size_t it2_id = id(iter2);
        
        std::vector<bool> vec = {
            isbad(T1_Lv[parent1_id][it2_id]),
            isbad(T1_Rv[parent1_id][it2_id]),
            isbad(T1_Hv[parent1_id][it2_id]),
        };
        if (!all_same(vec))
        {   // should be all inited/not-inited
            ERR("all_same");
            LOGGER_PRINT_CONTAINER(vec, "isbad v");
            abort();
        }
        if (vec[0] == true)
        {   // init parent
            assert(tree_type::is_first_child(iter1));
            
            T1_Lv[parent1_id][it2_id] =
            T1_Rv[parent1_id][it2_id] =
            T1_Hv[parent1_id][it2_id] = 0;
        }
    };
    
    init_parent_w_tables(it2);
    init_parent_v_tables(it1, it2);
    
    { // it1 should be inited yet
        vec = {
            isbad(T1_Lv[it1_id][it2_id]),
            isbad(T1_Rv[it1_id][it2_id]),
            isbad(T1_Hv[it1_id][it2_id]),
        };
        if (all_same(vec) && vec[0] == true)
        {
            ERR("all_same");
            LOGGER_PRINT_CONTAINER(vec, "isbad");
            abort();
        }
        vec = {
            isbad(T2_Lw[it2_id]),
            isbad(T2_Rw[it2_id]),
            isbad(T2_Hw[it2_id]),
        };
        if (all_same(vec) && vec[0] == true)
        {
            ERR("all_same");
            LOGGER_PRINT_CONTAINER(vec, "isbad");
            abort();
        }
    }
#undef all_same
}

size_t rted::update_STR_table(
                              iterator it1,
                              iterator it2)
{
    std::vector<size_t> vec(6);
    size_t it1_id = id(it1);
    size_t it2_id = id(it2);
    
    //      |T1v| * |FLeft(T2w)| + Lv[v,w]
    vec[RTED_T1_LEFT] =
    T1_Size[it1_id] * T2_FLeft[it2_id] + T1_Lv[it1_id][it2_id];
    //      |T2w| * |FLeft(T1v)| + Lw[w]
    vec[RTED_T2_LEFT] =
    T2_Size[it2_id] * T1_FLeft[it1_id] + T2_Lw[it2_id];
    //      |T1v| * |FRight(T2w)| + Rv[v,w]
    vec[RTED_T1_RIGHT] =
    T1_Size[it1_id] * T2_FRight[it2_id] + T1_Rv[it1_id][it2_id];
    //      |T2w| * |FRight(T1v)| + Rw[w]
    vec[RTED_T2_RIGHT] =
    T2_Size[it2_id] * T1_FRight[it1_id] + T2_Rw[it2_id];
    //      |T1v| * |A(T2w)| + Hv[v,w]
    vec[RTED_T1_HEAVY] =
    T1_Size[it1_id] * T2_A[it2_id] + T1_Hv[it1_id][it2_id];
    //      |T2w| * |A(T1v)| + Hw[w]
    vec[RTED_T2_HEAVY] =
    T2_Size[it2_id] * T1_A[it1_id] + T2_Hw[it2_id];
    
    auto c_min_it = min_element(vec.begin(), vec.end());
    size_t c_min = *c_min_it;
    size_t index = distance(vec.begin(), c_min_it);
    
    STR[it1_id][it2_id] = strategy(index);
    
    return c_min;
}

void rted::update_T1_LRH_v_tables(
                                  iterator it1,
                                  iterator it2,
                                  size_t c_min)
{
    size_t it1_id = id(it1);
    size_t it2_id = id(it2);
    size_t parent1_id = id(tree_type::parent(it1));
    
    {   // checks:
        std::vector<bool> vec = {
            isbad(T1_Lv[parent1_id][it2_id]),
            isbad(T1_Rv[parent1_id][it2_id]),
            isbad(T1_Hv[parent1_id][it2_id]),
            isbad(T1_Lv[it1_id][it2_id]),
            isbad(T1_Rv[it1_id][it2_id]),
            isbad(T1_Hv[it1_id][it2_id])
        };
        if (std::find(vec.begin(), vec.end(), true) != vec.end())
        {
            ERR("isbad()");
            LOGGER_PRINT_CONTAINER(vec, "isbad");
            abort();
        }
    }
    
    // Lv:
    T1_Lv[parent1_id][it2_id] +=
    tree_type::is_first_child(it1) ?
    T1_Lv[it1_id][it2_id] : c_min;
    
    // Rv:
    T1_Rv[parent1_id][it2_id] +=
    tree_type::is_last_child(it1) ?
    T1_Rv[it1_id][it2_id] : c_min;
    
    // Hv:
    auto res = T1_Hv_partials[parent1_id][it2_id];
    size_t val;
    
    if (T1_Size[it1_id] > res.subtree_size)
    {
        val = T1_Hv[it1_id][it2_id] - res.H_value + res.c_min;
        
        res.subtree_size = T1_Size[it1_id];
        res.c_min = c_min;
        res.H_value = T1_Hv[it1_id][it2_id];
        
        T1_Hv_partials[parent1_id][it2_id] = res;
    }
    else
        val = c_min;
    
    T1_Hv[parent1_id][it2_id] += val;
}

void rted::update_T2_LRH_w_tables(
                                  iterator it2,
                                  size_t c_min)
{
    size_t it2_id = id(it2);
    size_t parent2_id = id(tree_type::parent(it2));
    
    {   // checks:
        std::vector<bool> vec = {
            isbad(T2_Lw[parent2_id]),
            isbad(T2_Rw[parent2_id]),
            isbad(T2_Hw[parent2_id]),
            isbad(T2_Lw[it2_id]),
            isbad(T2_Rw[it2_id]),
            isbad(T2_Hw[it2_id])
        };
        if (std::find(vec.begin(), vec.end(), true) != vec.end())
        {
            ERR("isbad() w[%s], w[%s]",
                clabel(tree_type::parent(it2)), clabel(it2));
            LOGGER_PRINT_CONTAINER(vec, "isbad");
            abort();
        }
    }
    
    // Lw:
    T2_Lw[parent2_id] +=
    tree_type::is_first_child(it2) ?
    T2_Lw[it2_id] : c_min;
    
    // Rw:
    T2_Rw[parent2_id] +=
    tree_type::is_last_child(it2) ?
    T2_Rw[it2_id] : c_min;
    
    // Hw:
    auto res = T2_Hw_partials[parent2_id];
    
    if (T2_Size[it2_id] > res.subtree_size)
    {
        T2_Hw[parent2_id] +=
        T2_Hw[it2_id] - res.H_value + res.c_min;
        
        res.subtree_size = T2_Size[it2_id];
        res.c_min = c_min;
        res.H_value = T2_Hw[it2_id];
        
        T2_Hw_partials[parent2_id] = res;
    }
    else
        T2_Hw[parent2_id] += c_min;
}

void rted::check_postorder()
{
    size_t i;
    bool b = true;
    i = 0;
    for (post_order_iterator it = t1.begin_post(); it != t1.end_post(); ++it, ++i)
        if (id(it) != i)
            b = false;
    i = 0;
    for (post_order_iterator it = t2.begin_post(); it != t2.end_post(); ++it, ++i)
        if (id(it) != i)
            b = false;
    
    if (!b)
    {
        ERR("trees arent postorder");
        abort();
    }
}

strategy_table_type& rted::get_strategies()
{
    return STR;
}
