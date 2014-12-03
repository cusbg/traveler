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

#ifndef RTED_HPP
#define RTED_HPP

#include "types.hpp"
#include "rna_tree.hpp"
#include <unordered_map>



enum graph
{
    T1, T2
};

const char* to_string(graph h);
const char* to_string(path_strategy p);

template <typename tree_type>
class rted
{
/*
 * v poradi LEFT/RIGHT/HEAVY
 * pretoze pre L/R viem hned urcit syna, pre H ho musim spocitat..
 */
#define RTED_VECTOR_T1_LEFT_INDEX   0
#define RTED_VECTOR_T2_LEFT_INDEX   1
#define RTED_VECTOR_T1_RIGHT_INDEX  2
#define RTED_VECTOR_T2_RIGHT_INDEX  3
#define RTED_VECTOR_T1_HEAVY_INDEX  4
#define RTED_VECTOR_T2_HEAVY_INDEX  5

private:
    //typedef tree_base<node_base<std::string>> tree_type;
    typedef std::unordered_map<size_t, size_t> map_type;
public:
    typedef std::unordered_map<size_t, std::unordered_map<size_t, 
                std::pair<graph, path_strategy>>>
                    strategy_map_type;
private:
    /* check if map contains each child of it */
    void check_map_contains_children(
                const tree_type& t,
                typename tree_type::iterator it,
                const map_type& m) const;

    /* 
     * upgrade full_decomposition tables
     *
     * ALeft == left_decomposition
     * ARight == right_decomposition
     * A[parent] = sum(ALeft[ch1] * ARight[ch2]) + sum(A[ch]) + 1
     *  where ch, ch1, ch2 are children, and ch1<ch2
     * ALeft[parent] = sum(ALeft[ch]) + 1
     * ARight[parent] = sum(ARight[ch]) + 1
     */
    void compute_full_decomposition(const tree_type& t,
                                    typename tree_type::iterator it,
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
    void compute_relevant_subforrests(const tree_type& t,
                                    typename tree_type::iterator it,
                                    map_type& FLeft,
                                    map_type& FRight,
                                    map_type& Size);
    /*
     * update Size table from children:
     *
     * Size[parent] = sum(Size[ch]) + 1
     *  where ch are children
     */
    void compute_subtree_size(const tree_type& t,
                                typename tree_type::iterator it,
                                map_type& Size);

    /*
     * == ekvivalent to line 20, 21 in rted_opt_strategy(F,G)
     *
     * T2_Lw[parent_it] += (it == leftmost_child) ? T2_Lw[it] : c_min;
     * T2_Rw[parent_it] += (it == rightmost_child) ? T2_Rw[it] : c_min;
     */
    void update_T2_LR_w_tables(typename tree_type::iterator it,
                                size_t c_min);
    /*
     * == ekvivalent to line 22 in rted_opt_strategy(F,G)
     *
     * T2_Hw[parent_it] += (it is on heavy_parents_path) ? T2_Hw[it] : c_min;
     *
     * it cant be predicted if term is true or false, so i am using another map
     * T2_Hw_partials, where is stored tuple (subtree_size, c_min, H_value)
     * of heaviest child i saw. if i find another ch2, that 
     * Size[ch2] > subtree_size, i compute
     * T2_Hw[parent_it] += c_min - H_value + T2_Hw[ch2]
     * and store values from ch2:
     *      subtree_size = Size[ch2];
     *      c_min = function_c_min;
     *      H_value = T2_Hw[ch2]
     */
    void update_T2_H_w_table(typename tree_type::iterator it,
                            size_t c_min);
    
    /*
     * == ekvivalent to line 16, 17 in rted_opt_strategy(F,G)
     *
     * T1_Lv[parent_it1][it2] += (it1 == leftmost_child) ?
     *                              T1_Lv[it1][it2] : c_min;
     * T1_Rv[parent_it1][it2] += (it1 == leftmost_child) ?
     *                              T1_Rv[it1][it2] : c_min;
     */
    void update_T1_LR_v_tables(typename tree_type::iterator it1, 
                                typename tree_type::iterator it2,
                                size_t c_min);
    /*
     * == ekvivalent to line 18 in rted_opt_strategy(F,G)
     *
     * T1_Hv[parent_it1][it2] += (it1 is on heavy_parents_path) ?
     *                              T2_Hw[it1][it2] : c_min;
     * 
     * as in update_T2_H_w_table, we use map T1_Hv_partials,
     * where we store tuple (subtree_size, c_min, H_value) of heaviest child
     * if i find heavier ch2 (Size[ch2] > subtree_size) i compute
     * T1_Hv[parent1_id][it2_id] = c_min - H_value + T1_Hv[it1_id][it2_id]
     * and store values from ch2:
     *      subtree_size = Size[ch2];
     *      c_min = function_c_min;
     *      H_value = T1_Hv[it1_id][it2_id];
     */
    void update_T1_H_v_table(typename tree_type::iterator it1, 
                                typename tree_type::iterator it2,
                                size_t c_min);

    /*
     * initialize L/R/H_v tables for leaf it1
     *
     * T1_{L,R,H}v[it1_id][it2_id] = 0;
     */
    void init_T1_LRH_v_tables(typename tree_type::iterator it1, 
                                typename tree_type::iterator it2);
    /*
     * initialize L/R/H_w tables for leaf it
     *
     * T2_{L,R,H}w[it_id] = 0;
     */
    void init_T2_LRH_w_tables(typename tree_type::iterator it);

    /*
     * compute C from rted_opt_strategy(F,G) (== lines 7-12)
     * find minimum and stores minimal_path
     * returns c_min
     */
    size_t update_STR_table(typename tree_type::iterator it1,
                            typename tree_type::iterator it2);

    /*
     * tests if there is a m[index1][index2] value, and returns it
     */
    size_t get_value(size_t index1,
                    size_t index2,
                    const std::unordered_map<size_t, map_type>& m);
    /*
     * tests if there is a m[index] value and returns it
     */
    size_t get_value(size_t index, 
                    const map_type& m);

public:
    rted(const rna_tree& _t1, const rna_tree& _t2);
    strategy_map_type run_rted();

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


    partial_result_map
            T2_Hw_partials;
    std::unordered_map<size_t, partial_result_map>
            T1_Hv_partials;

    strategy_map_type STR;
};




// RTED DEFINICIE:


//TODO: uvolnovat pamet po precitani z mapy...
template<typename tree_type>
rted<tree_type>::rted(const rna_tree& _t1, const rna_tree& _t2)
    : t1(_t1), t2(_t2)
{
    APP_DEBUG_FNAME;
}

template<typename tree_type>
void rted<tree_type>::check_map_contains_children(const tree_type& t,
                                    typename tree_type::iterator it,
                                    const map_type& m) const
{
    if (tree_type::is_leaf(it))
        return;

    //TODO auto=, ale treba preistotu kontrolovat typ..
    typename tree_type::sibling_iterator ch = t.child(it, 0);
    for (; ch != ch.end(); ++ch)
    {
        if (m.find(ch->get_id()) == m.end())
        {
            logger.error("tree_node[%u](=='%s') was not found in the map",
                        ch->get_id(),
                        label(ch));
            exit(1);
        }
    }
}

template<typename tree_type>
void rted<tree_type>::compute_full_decomposition(const tree_type& t,
                                        typename tree_type::iterator it,
                                        map_type& A,
                                        map_type& ALeft,
                                        map_type& ARight)
{
    //TODO: prepisat vsetko na .at() alebo [] pri tabulkach. resp. nejaky debug mod
    check_map_contains_children(t, it, A);
    check_map_contains_children(t, it, ALeft);
    check_map_contains_children(t, it, ARight);

    size_t it_id = it->get_id();
    A[it_id]        = 1;
    ALeft[it_id]    = 1;
    ARight[it_id]   = 1;

    // if is_leaf(it) -> set A* = 1, what is done ..
    if (!tree_type::is_leaf(it))
    {
        auto ch = t.child(it, 0);
        while(ch != ch.end())
        {
            size_t ch_id = ch->get_id();
            
            A[it_id]        += A.at(ch_id);
            ALeft[it_id]    += ALeft.at(ch_id);
            ARight[it_id]   += ARight.at(ch_id);
            
            auto ch2 = ch;
            ++ch2;
            for (; ch2 != ch2.end(); ++ch2)
                A.at(it_id) += ALeft.at(ch_id) * ARight.at(ch2->get_id());

            ++ch;
        }
    }
#ifdef LOGGER_DEBUG_RTED_COMPUTE_DECOMPOSITION
    logger.debug("A\t[%s]\t== %lu",
                label(it),
                A[it_id]);
    logger.debug("ALeft\t[%s]\t== %lu",
                label(it),
                ALeft[it_id]);
    logger.debug("ARight\t[%s]\t== %lu",
                label(it),
                ARight[it_id]);
#endif
}

template<typename tree_type>
void rted<tree_type>::compute_subtree_size(const tree_type& t,
                                typename tree_type::iterator it,
                                map_type& m)
{
    check_map_contains_children(t, it, m);
    size_t it_id = it->get_id();

    m[it_id] = 1;
    if (!tree_type::is_leaf(it))
    {
        for (auto ch = t.child(it, 0); ch != ch.end(); ++ch)
            m[it_id] += m[ch->get_id()];
    }

#ifdef LOGGER_DEBUG_RTED_COMPUTE_SIZE
    logger.debug("subtree_size[%s] = %lu",
            label(it),
            m[it_id]);
#endif
}

template<typename tree_type>
void rted<tree_type>::compute_relevant_subforrests(const tree_type& t, 
                                        typename tree_type::iterator it,
                                        map_type& FLeft, 
                                        map_type& FRight, 
                                        map_type& Size)
{
    compute_subtree_size(t, it, Size);
    check_map_contains_children(t, it, FLeft);
    check_map_contains_children(t, it, FRight);
    check_map_contains_children(t, it, Size);

    // Pr:
    // FLeft[it_id] = 1 + F[child_left] + 
    //      + sum(size(children_right) + F[children_right]) 
    //          // child_left == first_child (==1); children_right == 2..n

    size_t it_id = it->get_id();
    FLeft[it_id]   = 1;
    FRight[it_id]  = 1;

    if (!tree_type::is_leaf(it))
    {
        auto ch = t.child(it, 0);

        // FLeft
        FLeft[it_id] += FLeft[ch->get_id()];  // == leftmost
        ++ch;
        for (; ch != ch.end(); ++ch)
            FLeft[it_id] += Size[ch->get_id()] + FLeft[ch->get_id()];

        // FRight
        ch = t.child(it, 0);
        if (ch != ch.end())
        {
            auto ch2 = ch;
            ++ch2;

            for (; ch2 != ch2.end(); ++ch2, ++ch) // == ch != --ch.end()
                FRight[it_id] += Size[ch->get_id()] + FRight[ch->get_id()];
            assert(ch2 == ch2.end());
            FRight[it_id] += FRight[ch->get_id()];
        }
    }

#ifdef LOGGER_DEBUG_RTED_COMPUTE_SUBFOREST
    logger.debug("FLeft\t[%s]\t == %lu",
                label(it),
                FLeft[it_id]);
    logger.debug("FRight\t[%s]\t== %lu",
                label(it),
                FRight[it_id]);
#endif
}

template<typename tree_type>
void rted<tree_type>::update_T2_LR_w_tables(typename tree_type::iterator it,
                                size_t c_min)
{

    size_t it_id = it->get_id();
    size_t parent_id = t2.parent(it)->get_id();
    
    get_value(it_id, T2_Lw);
    get_value(it_id, T2_Rw);
    // ^^ == test ci je inicializovana hodnota..

    T2_Lw[parent_id] += (tree_type::is_first_child(it) ? //it =?= leftmost_child
                T2_Lw[it_id] :
                c_min
            );

    T2_Rw[parent_id] += (tree_type::is_last_child(it) ? //it =?= rigtmost_child
                T2_Rw[it_id] :
                c_min
            );

#ifdef LOGGER_DEBUG_RTED_UPDATE_TABLE
    logger.debug("update: T2_Lw[%s]\t= %lu",
                label(tree_type::parent(it)),
                T2_Lw[parent_id]);
    logger.debug("update: T2_Rw[%s]\t= %lu",
                label(tree_type::parent(it)),
                T2_Rw[parent_id]);
#endif
}

template<typename tree_type>
void rted<tree_type>::update_T1_LR_v_tables(typename tree_type::iterator it1, 
                                typename tree_type::iterator it2, 
                                size_t c_min)
{
    size_t it1_id = it1->get_id();
    size_t it2_id = it2->get_id();
    size_t parent1_id = t1.parent(it1)->get_id();

    get_value(it1_id, it2_id, T1_Lv);
    get_value(it1_id, it2_id, T1_Rv);
    // ^^ == test ci je inicializovana hodnota..

    T1_Lv[parent1_id][it2_id] += (tree_type::is_first_child(it1) ? // it1 =?= leftmost_child
                T1_Lv[it1_id][it2_id] :
                c_min
            );

    T1_Rv[parent1_id][it2_id] += (tree_type::is_last_child(it1) ? // it1 =?= rightmost_child
                T1_Rv[it1_id][it2_id] :
                c_min
            );
    
#ifdef LOGGER_DEBUG_RTED_UPDATE_TABLE
    logger.debug("update: T1_Lv[%s][%s]\t= %lu",
                label(tree_type::parent(it1)),
                label(it2),
                T1_Lv[parent1_id][it2_id]);
    logger.debug("update: T1_Rv[%s][%s]\t= %lu",
                label(tree_type::parent(it1)),
                label(it2),
                T1_Rv[parent1_id][it2_id]);
#endif
}

template<typename tree_type>
void rted<tree_type>::update_T2_H_w_table(typename tree_type::iterator it, 
                                size_t c_min)
{
    size_t it_id = it->get_id();
    size_t parent_id = t2.parent(it)->get_id();

    get_value(it_id, T2_Hw);
    // ^^ == test ci je inicializovana hodnota..

    assert(T2_Size.find(it_id) != T2_Size.end());

    auto& res = T2_Hw_partials[parent_id];

    if (T2_Size[it_id] > res.subtree_size)
    {
        // terajsi podstrom je vacsi, odcitam povodne Hw, pricitam povodne c_min a svoje Hw.
        T2_Hw[parent_id] += T2_Hw[it_id] - res.H_value + res.c_min;
        // a nastavim nove hodnoty podla aktualneho maxima.
        res.subtree_size = T2_Size[it_id];
        res.c_min = c_min;
        res.H_value = T2_Hw[it_id];
    }
    else
        T2_Hw[parent_id] += c_min;

#ifdef LOGGER_DEBUG_RTED_UPDATE_TABLE
    logger.debug("update: T2_Hw[%s]\t= %lu",
                label(tree_type::parent(it)),
                T2_Hw[parent_id]);
#endif
}

template<typename tree_type>
void rted<tree_type>::update_T1_H_v_table(typename tree_type::iterator it1, 
                                typename tree_type::iterator it2, 
                                size_t c_min)
{
    size_t it1_id = it1->get_id();
    size_t it2_id = it2->get_id();
    size_t parent1_id = t1.parent(it1)->get_id();

    get_value(it1_id, it2_id, T1_Hv);
    // ^^ == test ci je inicializovana hodnota..
    
    assert(T1_Size.find(it1_id) != T1_Size.end() && "size should be computed");

    auto& res = T1_Hv_partials[parent1_id][it2_id];

    if (T1_Size[it1_id] > res.subtree_size)
    {
        T1_Hv[parent1_id][it2_id] += T1_Hv[it1_id][it2_id] - res.H_value + res.c_min;

        res.subtree_size = T1_Size[it1_id];
        res.c_min = c_min;
        res.H_value = T1_Hv[it1_id][it2_id];

    }
    else
        T1_Hv[parent1_id][it2_id] += c_min;
    
#ifdef LOGGER_DEBUG_RTED_UPDATE_TABLE
    logger.debug("update: T1_Hv[%s][%s]\t= %lu",
            label(tree_type::parent(it1)),
            label(it2),
            T1_Hv[parent1_id][it2_id]);
#endif
}

template<typename tree_type>
void rted<tree_type>::init_T1_LRH_v_tables(typename tree_type::iterator it1, 
                                typename tree_type::iterator it2)
{
    assert(tree_type::is_leaf(it1));

    size_t it1_id = it1->get_id();
    size_t it2_id = it2->get_id();

    T1_Lv[it1_id] =
        T1_Rv[it1_id] =
            T1_Hv[it1_id] = map_type();

    T1_Lv[it1_id][it2_id] =
        T1_Rv[it1_id][it2_id] = 
            T1_Hv[it1_id][it2_id] = 0;

#ifdef LOGGER_DEBUG_RTED_INIT
    logger.debug("initializing T1_{LRH}v_tables for '%s:%s' (=0)",
                label(it1),
                label(it2));
#endif
}

template<typename tree_type>
void rted<tree_type>::init_T2_LRH_w_tables(typename tree_type::iterator it)
{
    assert(tree_type::is_leaf(it));

    size_t it_id = it->get_id();

    T2_Lw[it_id] =
        T2_Rw[it_id] = 
            T2_Hw[it_id] = 0;

#ifdef LOGGER_DEBUG_RTED_INIT
    logger.debug("initializing T2_{LRH}w_tables for '%s' (=0)",
                label(it));
#endif
}

template<typename tree_type>
size_t rted<tree_type>::get_value(size_t index1,
                        size_t index2,
                        const std::unordered_map<size_t, map_type>& m)
{
    if (m.find(index1) == m.end() ||
            m.at(index1).find(index2) == m.at(index1).end())
    {
        logger.error("get_value_err: map dont have key(-s) %lu || "
                    "%lu, maybe not initialized yet", index1, index2);
        exit(1);
    }
    return m.at(index1).at(index2);
}

template<typename tree_type>
size_t rted<tree_type>::get_value(size_t index, 
                        const map_type& m)
{
    if (m.find(index) == m.end())
    {
        logger.error("get_value_err: map dont have key %lu, "
                    "maybe not initialized yet", index);
        exit(1);
    }
    return m.at(index);
}

template<typename tree_type>
size_t rted<tree_type>::update_STR_table(typename tree_type::iterator it1,
                            typename tree_type::iterator it2)
{
    std::vector<size_t> vec(6, 0xBADF00D);
    size_t it1_id = it1->get_id();
    size_t it2_id = it2->get_id();

    // compute C from rted_opt_strategy(F,G), linex 7-12

    //      |T1v| * |A(T2w)| + Hv[v,w]
    vec[RTED_VECTOR_T1_HEAVY_INDEX] = 
            (get_value(it1_id, T1_Size) * get_value(it2_id, T2_A)) +
                get_value(it1_id, it2_id, T1_Hv);
    //      |T2w| * |A(T1v)| + Hw[w]
    vec[RTED_VECTOR_T2_HEAVY_INDEX] = 
            (get_value(it2_id, T2_Size) * get_value(it1_id, T1_A)) +
                get_value(it2_id, T2_Hw);
    //      |T1v| * |FLeft(T2w)| + Lv[v,w]
    vec[RTED_VECTOR_T1_LEFT_INDEX] = 
            (get_value(it1_id, T1_Size) * get_value(it2_id, T2_FLeft)) +
                get_value(it1_id, it2_id, T1_Lv);
    //      |T2w| * |FLeft(T1v)| + Lw[w]
    vec[RTED_VECTOR_T2_LEFT_INDEX] =
            (get_value(it2_id, T2_Size) * get_value(it1_id, T1_FLeft)) +
                get_value(it2_id, T2_Lw);
    //      |T1v| * |FRight(T2w)| + Rv[v,w]
    vec[RTED_VECTOR_T1_RIGHT_INDEX] =
            (get_value(it1_id, T1_Size) * get_value(it2_id, T2_FRight)) +
                get_value(it1_id, it2_id, T1_Rv);
    //      |T2w| * |FRight(T1v)| + Rw[w]
    vec[RTED_VECTOR_T2_RIGHT_INDEX] =
            (get_value(it2_id, T2_Size) * get_value(it1_id, T1_FRight)) +
                get_value(it2_id, T2_Rw);


    auto c_min_it = std::min_element(vec.begin(), vec.end());
    size_t index = distance(vec.begin(), c_min_it);
    size_t c_min = vec[index];

#ifdef LOGGER_DEBUG_RTED_STRATEGY_MIN_VECTOR
    stringstream s;
    for (auto val : vec)
        s << val << " ";
    logger.debug("vec.values = %s", s.str().c_str());
    logger.debug("minimum_vec = %lu", c_min);
#endif

    switch(index)
    {
        case RTED_VECTOR_T1_HEAVY_INDEX:
        case RTED_VECTOR_T1_LEFT_INDEX:
        case RTED_VECTOR_T1_RIGHT_INDEX:
            STR[it1_id][it2_id].first = graph::T1;
            break;
        case RTED_VECTOR_T2_HEAVY_INDEX:
        case RTED_VECTOR_T2_LEFT_INDEX:
        case RTED_VECTOR_T2_RIGHT_INDEX:
            STR[it1_id][it2_id].first = graph::T2;
            break;
        default:
            logger.error("i should not be there, %lu == c_min_index != 0..5", index);
            exit(1);
    }
    switch(index)
    {
        case RTED_VECTOR_T1_HEAVY_INDEX:
        case RTED_VECTOR_T2_HEAVY_INDEX:
            STR[it1_id][it2_id].second = path_strategy::heavy;
            break;
        case RTED_VECTOR_T1_LEFT_INDEX: 
        case RTED_VECTOR_T2_LEFT_INDEX: 
            STR[it1_id][it2_id].second = path_strategy::left;
            break;
        case RTED_VECTOR_T1_RIGHT_INDEX:
        case RTED_VECTOR_T2_RIGHT_INDEX:
            STR[it1_id][it2_id].second = path_strategy::right;
            break;
        default:
            logger.error("i should not be there, %lu == c_min_index != 0..5", index);
            exit(1);
    }
/*
    switch (index)
    {
        case RTED_VECTOR_T1_HEAVY_INDEX:
            STR[it1_id][it2_id].first = graph::T1;
            STR[it1_id][it2_id].second = path_strategy::heavy;
            break;
        case RTED_VECTOR_T2_HEAVY_INDEX:
            STR[it1_id][it2_id].first = graph::T2;
            STR[it1_id][it2_id].second = path_strategy::heavy;
            break;
        case RTED_VECTOR_T1_LEFT_INDEX:
            STR[it1_id][it2_id].first = graph::T1;
            STR[it1_id][it2_id].second = path_strategy::left;
            break;
        case RTED_VECTOR_T2_LEFT_INDEX:
            STR[it1_id][it2_id].first = graph::T2;
            STR[it1_id][it2_id].second = path_strategy::left;
            break;
        case RTED_VECTOR_T1_RIGHT_INDEX:
            STR[it1_id][it2_id].first = graph::T1;
            STR[it1_id][it2_id].second = path_strategy::right;
            break;
        case RTED_VECTOR_T2_RIGHT_INDEX:
            STR[it1_id][it2_id].first = graph::T2;
            STR[it1_id][it2_id].second = path_strategy::right;
            break;
        default:
            logger.error("i should not be there, %lu == c_min_index != 0..5", index);
            exit(1);
    }
*/
#ifdef LOGGER_DEBUG_RTED_STRATEGY

    const char* _str = to_string(STR[it1_id][it2_id].second);
    const char* _g = to_string(STR[it1_id][it2_id].first);
    
    logger.debug("STR_strategy[%s][%s] = %s",
            label(it1),
            label(it2),
            _str);
    logger.debug("STR_graph[%s][%s] = %s",
            label(it1),
            label(it2),
            _g);
#endif

    return c_min;
}

template<typename tree_type>
typename rted<tree_type>::strategy_map_type rted<tree_type>::run_rted()
{
    APP_DEBUG_FNAME;

    logger.notice("RTED: BEGIN");
    logger.notice("computing full_decomposition & relevant_subforests for T2");
    for (auto it2 = t2.tree_ptr->begin_post(); it2 != t2.tree_ptr->end_post(); ++it2)
    {
        compute_full_decomposition(t2, it2, T2_A, T2_ALeft, T2_ARight);
        compute_relevant_subforrests(t2, it2, T2_FLeft, T2_FRight, T2_Size);
    }

    logger.notice("full_decomposition & relevant_subforests for T2 computing done");
    logger.notice("full_decomposition & relevant_subforests for T1 will be computed in main cycle..");
    logger.notice("starting main algorithm cycle");

    for (auto it1 = t1.tree_ptr->begin_post(); it1 != t1.tree_ptr->end_post(); ++it1)
    {
        if (it1->is_root())
            break;
        compute_full_decomposition(t1, it1, T1_A, T1_ALeft, T1_ARight);
        compute_relevant_subforrests(t1, it1, T1_FLeft, T1_FRight, T1_Size);

        T2_Hw.clear();
        T2_Lw.clear();
        T2_Rw.clear();
        T2_Hw_partials.clear();

        for (auto it2 = t2.tree_ptr->begin_post(); it2 != t2.tree_ptr->end_post(); ++it2)
        {
            if (it2->is_root())
                break;

            if (tree_type::is_leaf(it1))
                init_T1_LRH_v_tables(it1, it2);
            if (tree_type::is_leaf(it2))
                init_T2_LRH_w_tables(it2);

            size_t c_min = update_STR_table(it1, it2);


            update_T1_LR_v_tables(it1, it2, c_min);
            update_T1_H_v_table(it1, it2, c_min);

            update_T2_LR_w_tables(it2, c_min);
            update_T2_H_w_table(it2, c_min);
        }
    }

    logger.notice("RTED: END");

    return STR;
}






#endif /* !RTED_HPP */

