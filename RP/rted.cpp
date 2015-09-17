/*
 * File: rted.cpp
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

#include "rted.hpp"

using namespace std;

#define BAD 0xBADF00D

rted::rted(tree_type& _t1, tree_type& _t2)
    : t1(_t1), t2(_t2)
{ }


void rted::run_rted()
{
    APP_DEBUG_FNAME;

    post_order_iterator it1, it2;

    init();

    INFO("BEG: RTED");
    for (it1 = t1.begin_post(); !tree_type::is_root(it1); ++it1)
    {
        for (it2 = t2.begin_post(); !tree_type::is_root(it2); ++it2)
        {
            DEBUG("%s:%lu <-> %s:%lu", clabel(it1), id(it1), clabel(it2), id(it2));

            if (tree_type::is_leaf(it1))
                init_T1_LRH_v_tables(it1, it2);
            if (tree_type::is_leaf(it2))
                init_T2_LRH_w_tables(it2);

            size_t c_min = update_STR_table(it1, it2);

            update_T1_LRH_v_tables(it1, it2, c_min);
            update_T2_LRH_w_tables(it2, c_min);
        }
    }
    INFO("END: RTED");

    DEBUG("STR: %s", to_cstr(STR[id(t1.begin())][id(t2.begin())]));
}

void rted::tree_ids_postorder()
{
    post_order_iterator it;
    
    for (auto t : {&t1, &t2})
    {
        node_base::reset_ID();
        for (it = t->begin_post(); it != t->end_post(); ++it)
            it->reset_id();

        assert(t->size() - 1 == id(t->begin()));
    }
}

void rted::init()
{
    APP_DEBUG_FNAME;

    size_t size1, size2;

    INFO("BEG prepare tables");

    strategy_table_type::value_type inner_str;
    table_type inner_v;

    size1 = t1.size();
    size2 = t2.size();

    tree_ids_postorder();

    t1.print_tree();
    t2.print_tree();

    // STR table:
    inner_str.resize(size2);
    STR.resize(size1, inner_str);

    // {L,R,H}v tables:
    inner_v.resize(size2);
    for (auto table : {&T1_Lv, &T1_Rv, &T1_Hv})
        table->resize(size1, inner_v);

    // {L, R, H}w tables:
    for (auto table : {&T2_Lw, &T2_Rw, &T2_Hw})
        table->resize(size2);

    // partial tables:
    T2_Hw_partials.resize(size2);
    T1_Hv_partials.resize(size1, partial_result_arr(size2));

    // initialize all tables to size of tree..
    for (auto table : {&T1_ALeft, &T1_ARight, &T1_A,
                    &T1_FLeft, &T1_FRight, &T1_Size})
        table->resize(size1);
    for (auto table : {&T2_ALeft, &T2_ARight, &T2_A,
                    &T2_FLeft, &T2_FRight, &T2_Size})
        table->resize(size2);

    INFO("END prepare tables");
    INFO("BEG precomputation");

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

    // do not need anymore
    T1_ALeft =
        T2_ALeft =
        T1_ARight =
        T2_ARight = table_type();

    INFO("END precomputation");
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

    DEBUG("A\t[%s]\t == %lu",
            clabel(it),
            A[it_id]);
    DEBUG("ALeft\t[%s]\t == %lu",
            clabel(it),
            ALeft[it_id]);
    DEBUG("ARight\t[%s]\t == %lu",
            clabel(it),
            ARight[it_id]);
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

    DEBUG("FLeft\t[%s]\t == %lu",
            clabel(it), FLeft[it_id]);
    DEBUG("FRight\t[%s]\t == %lu",
            clabel(it), FRight[it_id]);
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

    DEBUG("Size\t[%s]\t == %lu",
            clabel(it),
            Size[it_id]);
}

void rted::init_T1_LRH_v_tables(
                iterator it1,
                iterator it2)
{
    //APP_DEBUG_FNAME;

    assert(tree_type::is_leaf(it1));

    size_t it1_id = id(it1);
    size_t it2_id = id(it2);

    T1_Lv[it1_id][it2_id] =
        T1_Rv[it1_id][it2_id] =
        T1_Hv[it1_id][it2_id] = 0;

    DEBUG("T1_{LRH}v\t[%s][%s]\t == 0 (init)",
            clabel(it1),
            clabel(it2));
}

void rted::init_T2_LRH_w_tables(
                iterator it)
{
    //APP_DEBUG_FNAME;

    size_t it_id = id(it);

    T2_Lw[it_id] =
        T2_Rw[it_id] =
        T2_Hw[it_id] = 0;

    DEBUG("T2_{LRH}w\t[%s]\t == 0 (init)",
            clabel(it));
}

size_t rted::update_STR_table(
                iterator it1,
                iterator it2)
{
    vector<size_t> vec(6);
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

    DEBUG("c_min %lu, index %lu", c_min, index);
    STR[it1_id][it2_id] = strategy::from_index(index);

    DEBUG("STR\t[%s][%s]\t == %s",
            clabel(it1),
            clabel(it2),
            to_cstr(STR[it1_id][it2_id]));

    return c_min;
}

void rted::update_T2_LRH_w_tables(
                iterator it,
                size_t c_min)
{
    size_t it_id = id(it);
    size_t parent_id = id(tree_type::parent(it));

    // Lw:
    T2_Lw[parent_id] +=
        tree_type::is_first_child(it) ?
        T2_Lw[it_id] : c_min;

    // Rw:
    T2_Rw[parent_id] +=
        tree_type::is_last_child(it) ?
        T2_Rw[it_id] : c_min;

    // Hw:
    auto res = T2_Hw_partials[parent_id];

    if (T2_Size[it_id] > res.subtree_size)
    {
        T2_Hw[parent_id] +=
            T2_Hw[it_id] - res.H_value + res.c_min;

        res.subtree_size = T2_Size[it_id];
        res.c_min = c_min;
        res.H_value = T2_Hw[it_id];

        T2_Hw_partials[parent_id] = res;
    }
    else
        T2_Hw[parent_id] += c_min;


    DEBUG("T2_Lw\t[%s]\t = %lu \t (update)",
            clabel(tree_type::parent(it)),
            T2_Lw[parent_id]);
    DEBUG("T2_Rw\t[%s]\t = %lu \t (update)",
            clabel(tree_type::parent(it)),
            T2_Rw[parent_id]);
    DEBUG("T2_Hw\t[%s]\t = %lu \t (update)",
            clabel(tree_type::parent(it)),
            T2_Hw[parent_id]);
}

void rted::update_T1_LRH_v_tables(
                iterator it1,
                iterator it2,
                size_t c_min)
{
    size_t it1_id = id(it1);
    size_t it2_id = id(it2);
    size_t parent1_id = id(tree_type::parent(it1));

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

    if (T1_Size[it1_id] > res.subtree_size)
    {
        T1_Hv[parent1_id][it2_id] +=
            T1_Hv[it1_id][it2_id] - res.H_value + res.c_min;

        res.subtree_size = T1_Size[it1_id];
        res.c_min = c_min;
        res.H_value = T1_Hv[it1_id][it2_id];

        T1_Hv_partials[parent1_id][it2_id] = res;
    }
    else
        T1_Hv[parent1_id][it2_id] += c_min;


    DEBUG("T1_Lv\t[%s][%s]\t = %lu \t (update)",
            clabel(tree_type::parent(it1)),
            clabel(it2),
            T1_Lv[parent1_id][it2_id]);
    DEBUG("T1_Rv\t[%s][%s]\t = %lu \t (update)",
            clabel(tree_type::parent(it1)),
            clabel(it2),
            T1_Rv[parent1_id][it2_id]);
    DEBUG("T1_Hv\t[%s][%s]\t = %lu \t (update)",
            clabel(tree_type::parent(it1)),
            clabel(it2),
            T1_Hv[parent1_id][it2_id]);
}


