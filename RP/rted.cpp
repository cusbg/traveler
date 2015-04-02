/*
 * File: rted.cpp
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


#include "rted.hpp"
#include <unordered_map>
#include <algorithm>
#include <iomanip>

#include <log4cpp/TimeStamp.hh>

using namespace std;

//#define NDEBUG

#ifdef NDEBUG
#define NO_LOGGER_RTED_DEBUG_MESSAGES
#define RTED_CHECKS_DISABLED
#endif

#define NO_LOGGER_RTED_DEBUG_MESSAGES

#ifndef NO_LOGGER_RTED_DEBUG_MESSAGES

#define LOGGER_DEBUG_RTED_INIT
#define LOGGER_DEBUG_RTED_UPDATE_TABLE
#define LOGGER_DEBUG_RTED_COMPUTE_DECOMPOSITION
#define LOGGER_DEBUG_RTED_COMPUTE_SUBFOREST
#define LOGGER_DEBUG_RTED_COMPUTE_SIZE
#define LOGGER_DEBUG_RTED_STRATEGY_MIN_VECTOR
#define LOGGER_DEBUG_RTED_STRATEGY

#endif

#undef NDEBUG



//TODO: uvolnovat pamet po precitani z mapy...

rted::rted(const tree_type& _t1, const tree_type& _t2)
    : t1(_t1), t2(_t2)
{
    APP_DEBUG_FNAME;
}

void rted::compute_full_decomposition(
                tree_type::iterator it,
                map_type& A,
                map_type& ALeft,
                map_type& ARight)
{
    check_map_contains_children(it, A);
    check_map_contains_children(it, ALeft);
    check_map_contains_children(it, ARight);

    size_t it_id = id(it);
    size_t a, left, right;
    a       = 1;
    left    = 1;
    right   = 1;

    // if tree_type::is_leaf(it) -> set A* = 1, what is done ..
    if (!tree_type::is_leaf(it))
    {
        tree_type::sibling_iterator ch = tree_type::first_child(it);
        while(ch != ch.end())
        {
            size_t ch_id = id(ch);
            size_t ch_left;

            ch_left = ALeft.at(ch_id);

            a       += A.at(ch_id);
            left    += ch_left;
            //left    += ALeft.at(ch_id);
            right   += ARight.at(ch_id);

            // MAZEM V tables_precomputation() naraz cele tabulky
            // we dont need ALeft, ARight [ch_id] anymore.
            //ALeft.erase(ch_id);
            //ARight.erase(ch_id);
            
            auto ch2 = ch;
            ++ch2;
            for (; ch2 != ch2.end(); ++ch2)
                a += ch_left * ARight.at(id(ch2));

            ++ch;
        }
    }
    A[it_id]        = a;
    ALeft[it_id]    = left;
    ARight[it_id]   = right;

#ifdef LOGGER_DEBUG_RTED_COMPUTE_DECOMPOSITION
    logger.debug("A\t[%s]\t== %lu",
                label(it),
                A.at(it_id));
    logger.debug("ALeft\t[%s]\t== %lu",
                label(it),
                ALeft.at(it_id));
    logger.debug("ARight\t[%s]\t== %lu",
                label(it),
                ARight.at(it_id));
#endif
}

void rted::compute_relevant_subforrests(
                tree_type::iterator it,
                map_type& FLeft, 
                map_type& FRight, 
                map_type& Size)
{
    compute_subtree_size(it, Size);
    check_map_contains_children(it, FLeft);
    check_map_contains_children(it, FRight);
    check_map_contains_children(it, Size);

    // Pr:
    // FLeft[it_id] = 1 + F[child_left] + 
    //      + sum(size(children_right) + F[children_right]) 
    //          // child_left == first_child (==1); children_right == 2..n

    size_t it_id = id(it);
    size_t left, right;
    left = 1;
    right = 1;

    if (!tree_type::is_leaf(it))
    {
        tree_type::sibling_iterator ch = tree_type::first_child(it);

        // FLeft
        left += FLeft.at(id(ch));
        ++ch;
        for (; ch != ch.end(); ++ch)
            left += Size.at(id(ch)) + FLeft.at(id(ch));

        // FRight
        ch = tree_type::first_child(it);
        if (ch != ch.end())
        {
            auto ch2 = ch;
            ++ch2;

            for (; ch2 != ch2.end(); ++ch2, ++ch) // == ch != --ch.end()
                right += Size.at(id(ch)) + FRight.at(id(ch));
            assert(ch2 == ch2.end());
            right += FRight.at(id(ch));
        }
    }
    FLeft[it_id] = left;
    FRight[it_id] = right;

#ifdef LOGGER_DEBUG_RTED_COMPUTE_SUBFOREST
    logger.debug("FLeft\t[%s]\t == %lu",
                label(it),
                FLeft.at(it_id));
    logger.debug("FRight\t[%s]\t== %lu",
                label(it),
                FRight.at(it_id));
#endif
}

void rted::compute_subtree_size(
                tree_type::iterator it,
                map_type& m)
{
    check_map_contains_children(it, m);
    size_t it_id = id(it);
    size_t val;

    // == it-node
    val = 1;
    if (!tree_type::is_leaf(it))
    {
        for (tree_type::sibling_iterator ch = tree_type::first_child(it);
                ch != ch.end(); ++ch)
            val += m.at(id(ch));
    }
    m[it_id] = val;

#ifdef LOGGER_DEBUG_RTED_COMPUTE_SIZE
    logger.debug("subtree_size[%s] = %lu",
            label(it),
            m.at(it_id));
#endif
}

void rted::update_T2_LRH_w_tables(
                tree_type::iterator it,
                size_t c_min)
{
    size_t it_id = id(it);
    size_t parent_id = id(tree_type::parent(it));

    // Lw:
    T2_Lw[parent_id] += (tree_type::is_first_child(it) ? //it =?= leftmost_child
                T2_Lw.at(it_id) :
                c_min
            );

    // Rw:
    T2_Rw[parent_id] += (tree_type::is_last_child(it) ? //it =?= rigtmost_child
                T2_Rw.at(it_id) :
                c_min
            );

    // Hw:
    //
    // ak som v parent_id este nebol, vytvor partials
    auto res = T2_Hw_partials[parent_id];

    if (T2_Size.at(it_id) > res.subtree_size)
    {
        // terajsi podstrom je vacsi, odcitam povodne Hw,
        //  pricitam povodne c_min a svoje Hw.
        T2_Hw[parent_id] += T2_Hw.at(it_id) - res.H_value + res.c_min;
        // a nastavim nove hodnoty podla aktualneho maxima.
        res.subtree_size = T2_Size.at(it_id);
        res.c_min = c_min;
        res.H_value = T2_Hw.at(it_id);

        T2_Hw_partials[parent_id] = res;
    }
    else
        T2_Hw[parent_id] += c_min;


#ifdef LOGGER_DEBUG_RTED_UPDATE_TABLE
    logger.debug("update: T2_Lw[%s]\t= %lu",
                label(tree_type::parent(it)),
                T2_Lw.at(parent_id));
    logger.debug("update: T2_Rw[%s]\t= %lu",
                label(tree_type::parent(it)),
                T2_Rw.at(parent_id));
    logger.debug("update: T2_Hw[%s]\t= %lu",
                label(tree_type::parent(it)),
                T2_Hw.at(parent_id));
#endif
}

void rted::update_T1_LRH_v_tables(
                tree_type::iterator it1, 
                tree_type::iterator it2, 
                size_t c_min)
{
    size_t it1_id = id(it1);
    size_t it2_id = id(it2);
    size_t parent1_id = id(tree_type::parent(it1));

    // Lv:
    T1_Lv[parent1_id][it2_id] += (tree_type::is_first_child(it1) ? // it1 =?= leftmost_child
                T1_Lv.at(it1_id).at(it2_id) :
                c_min
            );

    // Rv:
    T1_Rv[parent1_id][it2_id] += (tree_type::is_last_child(it1) ? // it1 =?= rightmost_child
                T1_Rv.at(it1_id).at(it2_id) :
                c_min
            );

    // Hv:
    //
    // vytvor partials, ak este neexistuje..
    auto res = T1_Hv_partials[parent1_id][it2_id];

    if (T1_Size.at(it1_id) > res.subtree_size)
    {
        T1_Hv[parent1_id][it2_id] += T1_Hv.at(it1_id).at(it2_id)
                                        - res.H_value + res.c_min;

        res.subtree_size = T1_Size.at(it1_id);
        res.c_min = c_min;
        res.H_value = T1_Hv.at(it1_id).at(it2_id);

        T1_Hv_partials[parent1_id][it2_id] = res;
    }
    else
        T1_Hv[parent1_id][it2_id] += c_min;



#ifdef LOGGER_DEBUG_RTED_UPDATE_TABLE
    logger.debug("update: T1_Lv[%s][%s]\t= %lu",
                label(tree_type::parent(it1)),
                label(it2),
                T1_Lv.at(parent1_id).at(it2_id));
    logger.debug("update: T1_Rv[%s][%s]\t= %lu",
                label(tree_type::parent(it1)),
                label(it2),
                T1_Rv.at(parent1_id).at(it2_id));
    logger.debug("update: T1_Hv[%s][%s]\t= %lu",
                label(tree_type::parent(it1)),
                label(it2),
                T1_Hv.at(parent1_id).at(it2_id));
#endif
}

size_t rted::update_STR_table(
                tree_type::iterator it1,
                tree_type::iterator it2)
{
#define RTED_VECTOR_T1_LEFT_INDEX   0
#define RTED_VECTOR_T2_LEFT_INDEX   1
#define RTED_VECTOR_T1_RIGHT_INDEX  2
#define RTED_VECTOR_T2_RIGHT_INDEX  3
#define RTED_VECTOR_T1_HEAVY_INDEX  4
#define RTED_VECTOR_T2_HEAVY_INDEX  5

    vector<size_t> vec(6, 0xBADF00D);
    size_t it1_id = id(it1);
    size_t it2_id = id(it2);

    // compute C from rted_opt_strategy(F,G), linex 7-12

    //      |T1v| * |A(T2w)| + Hv[v,w]
    vec[RTED_VECTOR_T1_HEAVY_INDEX] = 
            (T1_Size.at(it1_id) * T2_A.at(it2_id)) +
                T1_Hv.at(it1_id).at(it2_id);
    //      |T2w| * |A(T1v)| + Hw[w]
    vec[RTED_VECTOR_T2_HEAVY_INDEX] = 
        (T2_Size.at(it2_id) * T1_A.at(it1_id)) +
            T2_Hw.at(it2_id);
    //      |T1v| * |FLeft(T2w)| + Lv[v,w]
    vec[RTED_VECTOR_T1_LEFT_INDEX] = 
        (T1_Size.at(it1_id) * T2_FLeft.at(it2_id)) +
            T1_Lv.at(it1_id).at(it2_id);
    //      |T2w| * |FLeft(T1v)| + Lw[w]
    vec[RTED_VECTOR_T2_LEFT_INDEX] =
        (T2_Size.at(it2_id) * T1_FLeft.at(it1_id)) +
            T2_Lw.at(it2_id);
    //      |T1v| * |FRight(T2w)| + Rv[v,w]
    vec[RTED_VECTOR_T1_RIGHT_INDEX] =
        (T1_Size.at(it1_id) * T2_FRight.at(it2_id)) + 
            T1_Rv.at(it1_id).at(it2_id);
    //      |T2w| * |FRight(T1v)| + Rw[w]
    vec[RTED_VECTOR_T2_RIGHT_INDEX] =
        (T2_Size.at(it2_id) * T1_FRight.at(it1_id)) +
            T2_Rw.at(it2_id);


    auto c_min_it = std::min_element(vec.begin(), vec.end());
    size_t index = distance(vec.begin(), c_min_it);
    size_t c_min = vec.at(index);
    size_t value;

#ifdef LOGGER_DEBUG_RTED_STRATEGY_MIN_VECTOR
    LOGGER_PRINT_CONTAINER(vec, "rted_str_vec_values");
    logger.debug("minimum_vec = %lu", c_min);
#endif

    switch(index)
    {
        case RTED_VECTOR_T1_LEFT_INDEX:
            value = PATH_STRATEGY_LEFT_T1;
            break;
        case RTED_VECTOR_T2_LEFT_INDEX:
            value = PATH_STRATEGY_LEFT_T2;
            break;
        case RTED_VECTOR_T1_RIGHT_INDEX:
            value = PATH_STRATEGY_RIGHT_T1;
            break;
        case RTED_VECTOR_T2_RIGHT_INDEX:
            value = PATH_STRATEGY_RIGHT_T2;
            break;
        case RTED_VECTOR_T1_HEAVY_INDEX:
            value = PATH_STRATEGY_HEAVY_T1;
            break;
        case RTED_VECTOR_T2_HEAVY_INDEX:
            value = PATH_STRATEGY_HEAVY_T2;
            break;
        default:
            logger.error("i should not be there, %lu == c_min_index != 0..5", index);
            abort();
    }
    STR[it1_id][it2_id] = value;

#ifdef LOGGER_DEBUG_RTED_STRATEGY
    DEBUG("STR[%s][%s] = %s",
            label(it1),
            label(it2),
            all_to_string(STR.at(it1_id).at(it2_id)).c_str());
#endif

    return c_min;
}



void rted::init_T1_LRH_v_tables(
                tree_type::iterator it1, 
                tree_type::iterator it2)
{
    assert(tree_type::is_leaf(it1));

    size_t it1_id = id(it1);
    size_t it2_id = id(it2);

    // v .init() uz rezervujem
    //      Tv.reserve(t1.size())
    //      Tv[*].reserve(t2.size())
    T1_Lv[it1_id][it2_id] =
        T1_Rv[it1_id][it2_id] =
            T1_Hv[it1_id][it2_id] = 0;

#ifdef LOGGER_DEBUG_RTED_INIT
    logger.debug("initializing T1_{LRH}v_tables for [%s][%s] (=0)",
                label(it1),
                label(it2));
#endif
}

void rted::init_T2_LRH_w_tables(
                tree_type::iterator it)
{
    assert(tree_type::is_leaf(it));

    size_t it_id = id(it);

    T2_Lw.clear();
    T2_Rw.clear();
    T2_Hw.clear();
    T2_Hw_partials.clear();

    // !!! bez rezervovania to bezi o polovicu kratsie !!!
    //
    //T2_Lw.reserve(t2.size());
    //T2_Rw.reserve(t2.size());
    //T2_Hw.reserve(t2.size());

    T2_Lw[it_id] =
        T2_Rw[it_id] = 
            T2_Hw[it_id] = 0;

#ifdef LOGGER_DEBUG_RTED_INIT
    logger.debug("initializing T2_{LRH}w_tables for [%s](=0)",
                label(it));
#endif
}

void rted::init()
{
    APP_DEBUG_FNAME;

    // !!! volanim map.clear() sa neuvolni miesto, teda nedealokuje pamat

    // STR table:
    strategy_map_type::mapped_type inner_str;
    inner_str.reserve(t2.size());
    STR.reserve(t1.size());

    // {L,R,H}v tables:
    map_type inner_v;
    inner_v.reserve(t2.size());

    T1_Lv.reserve(t1.size());
    T1_Rv.reserve(t1.size());
    T1_Hv.reserve(t1.size());

    // T{1,2}_Size-s:
    T1_Size.reserve(t1.size());
    T2_Size.reserve(t2.size());

    for (auto it = t1.begin_post(); it != t1.end_post(); ++it)
    {
        size_t i = id(it);
        STR[i] = inner_str;
        T1_Lv[i] = inner_v;
        T1_Rv[i] = inner_v;
        T1_Hv[i] = inner_v;
    }
}

void rted::tables_precomputation()
{
    APP_DEBUG_FNAME;

    T1_FLeft.reserve(t1.size());
    T1_FRight.reserve(t1.size());
    T2_FLeft.reserve(t2.size());
    T2_FRight.reserve(t2.size());
    //
    // full decomposition tables:
    T1_A.reserve(t1.size());
    T2_A.reserve(t2.size());
    T1_ALeft.reserve(t1.size());
    T2_ALeft.reserve(t2.size());
    T1_ARight.reserve(t1.size());
    T2_ARight.reserve(t2.size());

    for (auto it1 = t1.begin_post(); it1 != t1.end_post(); ++it1)
    {
        compute_full_decomposition(it1, T1_A, T1_ALeft, T1_ARight);
        compute_relevant_subforrests(it1, T1_FLeft, T1_FRight, T1_Size);
    }
    for (auto it2 = t2.begin_post(); it2 != t2.end_post(); ++it2)
    {
        compute_full_decomposition(it2, T2_A, T2_ALeft, T2_ARight);
        compute_relevant_subforrests(it2, T2_FLeft, T2_FRight, T2_Size);
    }

    // uz ich viac potrebovat nebudem:
    T1_ALeft =
        T1_ARight =
        T2_ALeft =
        T2_ARight = map_type();
}

void rted::run_rted()
{
    APP_DEBUG_FNAME;

    logger.notice("BEGIN init");
    init();
    logger.notice("END init");

    logger.notice("BEGIN precomputation");
    tables_precomputation();
    logger.notice("END precomputation");

    logger.notice("RTED: BEGIN");
    for (auto it1 = t1.begin_post(); it1 != t1.end_post(); ++it1)
    {
        if (it1->is_root())
            break;

        for (auto it2 = t2.begin_post(); it2 != t2.end_post(); ++it2)
        {
            if (it2->is_root())
                break;

            if (tree_type::is_leaf(it1))
                init_T1_LRH_v_tables(it1, it2);
            if (tree_type::is_leaf(it2))
                init_T2_LRH_w_tables(it2);

            size_t c_min = update_STR_table(it1, it2);

            update_T1_LRH_v_tables(it1, it2, c_min);
            update_T2_LRH_w_tables(it2, c_min);
        }
    }
    logger.notice("RTED: END");

#ifdef LOGGER_DEBUG_RTED_STRATEGY
    print_strategies();
#endif
}


void rted::check_map_contains_children(
                tree_type::iterator it,
                const map_type& m) const
{
#ifndef RTED_CHECKS_DISABLED
    if (tree_type::is_leaf(it))
        return;
    tree_type::sibling_iterator ch = tree_type::first_child(it);
    for (; ch != ch.end(); ++ch)
    {
        if (m.find(id(ch)) == m.end())
        {
            logger.error("tree_node[%u](=='%s') was not found in the map",
                        id(ch),
                        label(ch));
            abort();
        }
    }
#endif
}

void rted::print_strategies() const
{
    APP_DEBUG_FNAME;
    stringstream out;
    out << "STRATEGY MAP:" << endl;
    int odsadenie = 15;
    for (tree_type::post_order_iterator it1 = t1.begin_post(); !it1->is_root(); ++it1)
    {
        for (tree_type::post_order_iterator it2 = t2.begin_post(); !it2->is_root(); ++it2)
        {
            string s = string() + "[" + label(it1) + "," + label(it2) + "]";
            out << setw(odsadenie) << s;
            auto val = STR.at(id(it1)).at(id(it2));
            out << setw(5) << graph_to_string(val) << setw(10) << path_to_string(val) << endl;
        }
        out << endl;
    }
    logger.debugStream() << out.str();
}



