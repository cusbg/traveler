/*
 * rted.cpp
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
 *
 * Distributed under terms of the MIT license.
 */

#include "rted.hpp"
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>
#include <math.h>

using namespace std;

rted::rted(const rna_tree& _t1, const rna_tree& _t2)
    : t1(*_t1.tree_ptr), t2(*_t2.tree_ptr)
{
    APP_DEBUG_FNAME;
}

size_t cost(const node_base<std::string>& n1, const node_base<std::string>& n2)
{
    return 0;
}

rted::tree_type::iterator rted::most_left(const rted::tree_type& t) const
{
    return t.begin_post();
}

rted::tree_type::iterator rted::most_right(const rted::tree_type& t) const
{
    assert (t.begin() != t.end());
    return --t.end();
}

bool rted::is_leftmost_child(const tree_type::iterator& it) const
{
    return tree_type::child(parent(it), 0) == it;
}

bool rted::is_rightmost_child(const tree_type::iterator& it) const
{
    size_t n = parent(it).number_of_children();
    return tree_type::child(parent(it), n - 1) == it;
}

rted::tree_type::iterator rted::parent(const tree_type::iterator& it) const
{
    return tree_type::parent(it);
}

const char* rted::label(const tree_type::iterator& it) const
{
    return it->get_label().c_str();
}

bool rted::is_leaf(rted::tree_type::iterator_base it) const
{
    return it.number_of_children() == 0;
}

void rted::check_map_contains_children(const tree_type& t, tree_type::iterator it, const map_type& m) const
{
    if (is_leaf(it))
        return;
    auto ch = t.child(it, 0);
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

void rted::compute_full_decomposition(const tree_type& t, tree_type::iterator it, map_type& A, map_type& ALeft, map_type& ARight)
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
    if (!is_leaf(it))
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
#ifdef LOGGER_DEBUG_COMPUTE_DECOMPOSITION
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

void rted::compute_subtree_size(const tree_type& t, tree_type::iterator it, map_type& m)
{
    check_map_contains_children(t, it, m);
    size_t it_id = it->get_id();

    m[it_id] = 1;
    if (!is_leaf(it))
    {
        for (auto ch = t.child(it, 0); ch != ch.end(); ++ch)
            m[it_id] += m[ch->get_id()];
    }

#ifdef LOGGER_DEBUG_COMPUTE_SIZE
    logger.debug("subtree_size[%s] = %lu",
            label(it),
            m[it_id]);
#endif
}

void rted::compute_relevant_subforrests(const tree_type& t, tree_type::iterator it, map_type& m_left, map_type& m_right, map_type& m_size)
{
    compute_subtree_size(t, it, m_size);
    check_map_contains_children(t, it, m_left);
    check_map_contains_children(t, it, m_right);

    // Pr:
    // m_left[it_id] = 1 + F[child_left] + 
    //      + sum(size(children_right) + F[children_right]) 
    //          // child_left == first_child (==1); children_right == 2..n

    size_t it_id = it->get_id();
    m_left[it_id]   = 1;
    m_right[it_id]  = 1;

    if (!is_leaf(it))
    {
        auto ch = t.child(it, 0);

        // m_left
        m_left[it_id] += m_left[ch->get_id()];  // == leftmost
        ++ch;
        for (; ch != ch.end(); ++ch)
            m_left[it_id] += m_size[ch->get_id()] + m_left[ch->get_id()];

        // m_right
        ch = t.child(it, 0);
        if (ch != ch.end())
        {
            auto ch2 = ch;
            ++ch2;

            for (; ch2 != ch2.end(); ++ch2, ++ch) // == ch != --ch.end()
                m_right[it_id] += m_size[ch->get_id()] + m_right[ch->get_id()];
            assert(ch2 == ch2.end());
            m_right[it_id] += m_right[ch->get_id()];
        }
    }

#ifdef LOGGER_DEBUG_COMPUTE_SUBFOREST
    logger.debug("m_left\t[%s]\t == %lu",
                label(it),
                m_left[it_id]);
    logger.debug("m_right\t[%s]\t== %lu",
                label(it),
                m_right[it_id]);
#endif
}

void rted::update_T2_LR_w_tables(tree_type::iterator it, size_t c_min)
{

    size_t it_id = it->get_id();
    size_t parent_id = t2.parent(it)->get_id();
    
    get_value(it_id, T2_Lw);
    get_value(it_id, T2_Rw);
    // ^^ == test ci je inicializovana hodnota..

    T2_Lw[parent_id] += (is_leftmost_child(it) ? //it =?= leftmost_child
                T2_Lw[it_id] :
                c_min
            );

    T2_Rw[parent_id] += (is_rightmost_child(it) ? //it =?= rigtmost_child
                T2_Rw[it_id] :
                c_min
            );

#ifdef LOGGER_DEBUG_UPDATE_TABLE
    logger.debug("update: T2_Lw[%s]\t= %lu",
                label(parent(it)),
                T2_Lw[parent_id]);
    logger.debug("update: T2_Rw[%s]\t= %lu",
                label(parent(it)),
                T2_Rw[parent_id]);
#endif
}

void rted::update_T1_LR_v_tables(tree_type::iterator it1, tree_type::iterator it2, size_t c_min)
{
    size_t it1_id = it1->get_id();
    size_t it2_id = it2->get_id();
    size_t parent1_id = t1.parent(it1)->get_id();

    get_value(it1_id, it2_id, T1_Lv);
    get_value(it1_id, it2_id, T1_Rv);
    // ^^ == test ci je inicializovana hodnota..

    T1_Lv[parent1_id][it2_id] += (is_leftmost_child(it1) ? // it1 =?= leftmost_child
                T1_Lv[it1_id][it2_id] :
                c_min
            );

    T1_Rv[parent1_id][it2_id] += (is_rightmost_child(it1) ? // it1 =?= rightmost_child
                T1_Rv[it1_id][it2_id] :
                c_min
            );
    
#ifdef LOGGER_DEBUG_UPDATE_TABLE
    logger.debug("update: T1_Lv[%s][%s]\t= %lu",
                label(parent(it1)),
                label(it2),
                T1_Lv[parent1_id][it2_id]);
    logger.debug("update: T1_Rv[%s][%s]\t= %lu",
                label(parent(it1)),
                label(it2),
                T1_Rv[parent1_id][it2_id]);
#endif
}

void rted::update_T2_H_w_table(tree_type::iterator it, size_t c_min)
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

#ifdef LOGGER_DEBUG_UPDATE_TABLE
    logger.debug("update: T2_Hw[%s]\t= %lu",
                label(parent(it)),
                T2_Hw[parent_id]);
#endif
}

void rted::update_T1_H_v_table(tree_type::iterator it1, tree_type::iterator it2, size_t c_min)
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
    
#ifdef LOGGER_DEBUG_UPDATE_TABLE
    logger.debug("update: T1_Hv[%s][%s]\t= %lu",
            label(parent(it1)),
            label(it2),
            T1_Hv[parent1_id][it2_id]);
#endif
}

void rted::init_T1_LRH_v_tables(tree_type::iterator it1, tree_type::iterator it2)
{
    assert(is_leaf(it1));

    size_t it1_id = it1->get_id();
    size_t it2_id = it2->get_id();

    T1_Lv[it1_id] =
        T1_Rv[it1_id] =
            T1_Hv[it1_id] = map_type();

    T1_Lv[it1_id][it2_id] =
        T1_Rv[it1_id][it2_id] = 
            T1_Hv[it1_id][it2_id] = 0;

#ifdef LOGGER_DEBUG_INIT
    logger.debug("initializing T1_{LRH}v_tables for '%s:%s' (=0)",
                label(it1),
                label(it2));
#endif
}

void rted::init_T2_LRH_w_tables(tree_type::iterator it)
{
    assert(is_leaf(it));

    size_t it_id = it->get_id();

    T2_Lw[it_id] =
        T2_Rw[it_id] = 
            T2_Hw[it_id] = 0;

#ifdef LOGGER_DEBUG_INIT
    logger.debug("initializing T2_{LRH}w_tables for '%s' (=0)",
                label(it));
#endif
}

size_t rted::get_value(size_t index1, size_t index2, const std::unordered_map<size_t, map_type>& m)
{
    if (m.find(index1) == m.end() || m.at(index1).find(index2) == m.at(index1).end())
    {
        logger.error("get_value_err: map dont have key(-s) %lu || %lu, maybe not initialized yet", index1, index2);
        exit(1);
    }
    return m.at(index1).at(index2);
}

size_t rted::get_value(size_t index, const map_type& m)
{
    if (m.find(index) == m.end())
    {
        logger.error("get_value_err: map dont have key %lu, maybe not initialized yet", index);
        exit(1);
    }
    return m.at(index);
}

void rted::run_rted()
{
    APP_DEBUG_FNAME;

    logger.notice("RTED");
    logger.notice("computing full_decomposition & relevant_subforests for T2");
    for (auto it2 = t2.begin_post(); it2 != t2.end_post(); ++it2)
    {
        compute_full_decomposition(t2, it2, T2_A, T2_ALeft, T2_ARight);
        compute_relevant_subforrests(t2, it2, T2_FLeft, T2_FRight, T2_Size);
    }

    logger.notice("full_decomposition & relevant_subforests for T2 computing done");
    logger.notice("full_decomposition & relevant_subforests for T1 will be computed in main cycle..");
    logger.notice("starting main algorithm cycle");

    for (auto it1 = t1.begin_post(); it1 != t1.end_post(); ++it1)
    {
        if (it1->is_root())
            break;
        compute_full_decomposition(t1, it1, T1_A, T1_ALeft, T1_ARight);
        compute_relevant_subforrests(t1, it1, T1_FLeft, T1_FRight, T1_Size);

        T2_Hw.clear();
        T2_Lw.clear();
        T2_Rw.clear();
        T2_Hw_partials.clear();

        for (auto it2 = t2.begin_post(); it2 != t2.end_post(); ++it2)
        {
            if (it2->is_root())
                break;


            size_t it1_id = it1->get_id();
            size_t it2_id = it2->get_id();

            if (is_leaf(it1))
                init_T1_LRH_v_tables(it1, it2);
            if (is_leaf(it2))
                init_T2_LRH_w_tables(it2);

            vector<size_t> vec(6, 0xBADF00D);

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


            size_t c_min = update_STR_table(vec, it1, it2);

            update_T1_LR_v_tables(it1, it2, c_min);
            update_T1_H_v_table(it1, it2, c_min);

            update_T2_LR_w_tables(it2, c_min);
            update_T2_H_w_table(it2, c_min);
        }
    }
}

size_t rted::update_STR_table(const std::vector<size_t>& vec, tree_type::iterator it1, tree_type::iterator it2)
{
    auto c_min_it = std::min_element(vec.begin(), vec.end());
    size_t index = distance(vec.begin(), c_min_it);
    size_t c_min = vec[index];

#ifdef LOGGER_DEBUG_STRATEGY_MIN_VECTOR
    stringstream s;
    for (auto val : vec)
        s << val << " ";
    logger.debug("vec.values = %s", s.str().c_str());
    logger.debug("minimum_vec = %lu", c_min);
#endif

    size_t it1_id = it1->get_id();
    size_t it2_id = it2->get_id();

    const char* str = nullptr;
    const char* g = nullptr;
    switch (index)
    {
        case RTED_VECTOR_T1_HEAVY_INDEX:
            STR[it1_id][it2_id].first = graph::T1;
            STR[it1_id][it2_id].second = path_strategy::heavy;
            str = "HEAVY";
            g = "T1";
            break;
        case RTED_VECTOR_T2_HEAVY_INDEX:
            STR[it1_id][it2_id].first = graph::T2;
            STR[it1_id][it2_id].second = path_strategy::heavy;
            str = "HEAVY";
            g = "T2";
            break;
        case RTED_VECTOR_T1_LEFT_INDEX:
            STR[it1_id][it2_id].first = graph::T1;
            STR[it1_id][it2_id].second = path_strategy::left;
            str = "LEFT";
            g = "T1";
            break;
        case RTED_VECTOR_T2_LEFT_INDEX:
            STR[it1_id][it2_id].first = graph::T2;
            STR[it1_id][it2_id].second = path_strategy::left;
            str = "LEFT";
            g = "T2";
            break;
        case RTED_VECTOR_T1_RIGHT_INDEX:
            STR[it1_id][it2_id].first = graph::T1;
            STR[it1_id][it2_id].second = path_strategy::right;
            str = "RIGHT";
            g = "T1";
            break;
        case RTED_VECTOR_T2_RIGHT_INDEX:
            STR[it1_id][it2_id].first = graph::T2;
            STR[it1_id][it2_id].second = path_strategy::right;
            str = "RIGHT";
            g = "T2";
            break;
        default:
            logger.error("i should not be there, %lu == c_min_index != 0..5", index);
            exit(1);
    }

#ifdef LOGGER_DEBUG_STRATEGY
    logger.debug("STR_strategy[%s][%s] = %s",
            label(it1),
            label(it2),
            str);
    logger.debug("STR_graph[%s][%s] = %s",
            label(it1),
            label(it2),
            g);
#endif

    return c_min;
}





