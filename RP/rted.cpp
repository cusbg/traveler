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
{}

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
            logger.error("tree_node[%u](=='%s') was not found in the map", ch->get_id(), ch->get_label().c_str());
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

            //TODO: delete children from table...
            ++ch;
        }
    }
    //const char* label = it->get_label().c_str();
    //logger.debug("A\t[%s]\t== %lu",       label, A        [it_id]);
    //logger.debug("ALeft\t[%s]\t== %lu",   label, ALeft    [it_id]);
    //logger.debug("ARight\t[%s]\t== %lu",  label, ARight   [it_id]);
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
    //logger.debug("subtree_size[%s] = %lu", it->get_label().c_str(), m[it_id]);
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

    //const char* label = it->get_label().c_str();
    //logger.debug("m_left\t[%s]\t== %lu",    label, m_left[it_id]);
    //logger.debug("m_right\t[%s]\t== %lu",   label, m_right[it_id]);
}

void rted::update_T2_LR_w_tables(tree_type::iterator it, size_t c_min)
{

    size_t it_id = it->get_id();
    size_t parent_id = t2.parent(it)->get_id();
    
    if (T2_Lw.find(parent_id) == T2_Lw.end())
    {
        T2_Lw[parent_id] = 0;

        //logger.debug("initializing T2_L_w[%s]", t2.parent(it)->get_label().c_str());
    }
    if (T2_Rw.find(parent_id) == T2_Rw.end())
    {
        T2_Rw[parent_id] = 0;

        //logger.debug("initializing T2_R_w[%s]", t2.parent(it)->get_label().c_str());
    }

    T2_Lw[parent_id] += (it == it.begin() ? //it =?= leftmost_child
                T2_Lw[it_id] :
                c_min
            );

    auto it2 = it;
    ++it2;
    T2_Rw[parent_id] += (it2 == it2.end() ? //it =?= rigtmost_child
                T2_Rw[it_id] :
                c_min
            );
    //logger.debug("update: T2_L_w[%s]\t= %lu", t2.parent(it)->get_label().c_str(), T2_Lw[parent_id]);
    //logger.debug("update: T2_R_w[%s]\t= %lu", t2.parent(it)->get_label().c_str(), T2_Rw[parent_id]);
}

void rted::update_T1_LR_v_tables(tree_type::iterator it1, tree_type::iterator it2, size_t c_min)
{
    size_t it1_id = it1->get_id();
    size_t it2_id = it2->get_id();
    size_t parent1_id = t1.parent(it1)->get_id();

    if (T1_Lv.find(parent1_id) == T1_Lv.end())
    {
        T1_Lv[parent1_id] = map_type();

        //logger.debug("initializing T1_Lv[%s]", t1.parent(it1)->get_label().c_str());
    }
    if (T1_Lv[parent1_id].find(it2_id) == T1_Lv[parent1_id].end())
    {
        T1_Rv[parent1_id][it2_id] = 0;

        //logger.debug("initializing T1_Lv[%s][%s]", t1.parent(it1)->get_label().c_str(), t2.parent(it2)->get_label().c_str());
    }

    T1_Lv[parent1_id][it2_id] += (it1 == it1.begin() ? // it1 =?= leftmost_child
                T1_Lv[it1_id][it2_id] :
                c_min
            );

    auto it1_2 = it1;
    ++it1_2;
    T1_Rv[parent1_id][it2_id] += (it1_2 == it1_2.end() ? // it1 =?= rightmost_child
                T1_Rv[it1_id][it2_id] :
                c_min
            );
    

    //logger.debug("update: T1_Lv[%s][%s]\t= %lu", 
                //t1.parent(it1)->get_label().c_str(), 
                //t2.parent(it2)->get_label().c_str(), 
                //T1_Lv[parent1_id][it2_id]);
    //logger.debug("update: T1_Rv[%s][%s]\t= %lu", 
                //t1.parent(it1)->get_label().c_str(), 
                //t2.parent(it2)->get_label().c_str(), 
                //T1_Rv[parent1_id][it2_id]);
}

void rted::update_T2_H_w_table(tree_type::iterator it, size_t c_min)
{
    size_t it_id = it->get_id();
    size_t parent_id = t2.parent(it)->get_id();

    if (T2_Hw.find(parent_id) == T2_Hw.end())
    {
        assert (T2_Hw_partials.find(parent_id) == T2_Hw_partials.end());
        T2_Hw[parent_id] = 0;
        T2_Hw_partials[parent_id] = {0, 0, 0};

        //logger.debug("initializing T2_Hw[%s]", t2.parent(it)->get_label().c_str());
        //logger.debug("initializing  T2_Hw_partials[%s]", t2.parent(it)->get_label().c_str());
    }

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

    //logger.debug("update: T2_Hw[%s]\t= %lu", t2.parent(it)->get_label().c_str(), T2_Hw[parent_id]);
}

void rted::update_T1_H_v_table(tree_type::iterator it1, tree_type::iterator it2, size_t c_min)
{
    size_t it1_id = it1->get_id();
    size_t it2_id = it2->get_id();
    size_t parent1_id = t1.parent(it1)->get_id();

    // initialize if maps dont exist
    if (T1_Hv.find(parent1_id) == T1_Hv.end())
    {
        assert (T1_Hv_partials.find(parent1_id) == T1_Hv_partials.end());

        T1_Hv[parent1_id] = map_type();
        T1_Hv_partials[parent1_id] = partial_result_map();

        //logger.debug("initializing T1_Hv[%s]", t2.parent(it1)->get_label().c_str());
        //logger.debug("initializing T1_Hv_partials[%s]", t2.parent(it1)->get_label().c_str());
    }
    if (T1_Hv[parent1_id].find(it2_id) == T1_Hv[parent1_id].end())
    {
        assert(T1_Hv_partials[parent1_id].find(it2_id) == T1_Hv_partials[parent1_id].end());

        T1_Hv[parent1_id][it2_id] = 0;
        T1_Hv_partials[parent1_id][it2_id] = {0, 0, 0};

        //logger.debug("initializing T1_Hv[%s][%s]", t2.parent(it1)->get_label().c_str(), it2->get_label().c_str());
        //logger.debug("initializing T1_Hv_partials[%s][%s]", t2.parent(it1)->get_label().c_str(), it2->get_label().c_str());
    }

    assert(T1_Size.find(it1_id) != T1_Size.end());

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
    
    //logger.debug("update: T1_Hv[%s][%s]\t= %lu", 
            //t2.parent(it1)->get_label().c_str(),
            //it2->get_label().c_str(),
            //T1_Hv[parent1_id][it2_id]);
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

    //logger.debug("initializing T1_{LRH}_v_tables");
}

void rted::init_T2_LRH_w_tables(tree_type::iterator it)
{
    assert(is_leaf(it));

    size_t it_id = it->get_id();

    T2_Lw[it_id] =
        T2_Rw[it_id] = 
            T2_Hw[it_id] = 0;

    //logger.debug("initializing T2_{LRH}_w_tables");
}

size_t rted::get_value(size_t index1, size_t index2, const std::unordered_map<size_t, map_type>& m)
{
    if (m.find(index1) == m.end() || m.at(index1).find(index2) == m.at(index1).end())
    {
        logger.error("get_value_err: map dont have key(-s) %lu || %lu", index1, index2);
        exit(1);
    }
    return m.at(index1).at(index2);
}

size_t rted::get_value(size_t index, const map_type& m)
{
    if (m.find(index) == m.end())
    {
        logger.error("get_value_err: map dont have key %lu", index);
        exit(1);
    }
    return m.at(index);
}

void rted::run_rted()
{
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

            if (is_leaf(it1))
                init_T1_LRH_v_tables(it1, it2);
            if (is_leaf(it2))
                init_T2_LRH_w_tables(it2);

            vector<size_t> vec(6, 0xBADF00D);
            size_t it1_id = it1->get_id();
            size_t it2_id = it2->get_id();

            //      |T1v| * |A(T2w)| + Hv[v,w]
            vec[0] = (get_value(it1_id, T1_Size) * get_value(it2_id, T2_A)) +
                        get_value(it1_id, it2_id, T1_Hv);
            //      |T2w| * |A(T1v)| + Hw[w]
            vec[1] = (get_value(it2_id, T2_Size) * get_value(it1_id, T1_A)) +
                        get_value(it2_id, T2_Hw);
            //      |T1v| * |FLeft(T2w)| + Lv[v,w]
            vec[2] = (get_value(it1_id, T1_Size) * get_value(it2_id, T2_FLeft)) +
                        get_value(it1_id, it2_id, T1_Lv);
            //      |T2w| * |FLeft(T1v)| + Lw[w]
            vec[3] = (get_value(it2_id, T2_Size) * get_value(it1_id, T1_FLeft)) +
                        get_value(it2_id, T2_Lw);
            //      |T1v| * |FRight(T2w)| + Rv[v,w]
            vec[4] = (get_value(it1_id, T1_Size) * get_value(it2_id, T2_FRight)) +
                        get_value(it1_id, it2_id, T1_Rv);
            //      |T2w| * |FRight(T1v)| + Rw[w]
            vec[5] = (get_value(it2_id, T2_Size) * get_value(it1_id, T1_FRight)) +
                        get_value(it2_id, T2_Rw);


            for (auto val : vec)
                cout << val << " ";
            cout << endl;

            auto c_min_it = std::min_element(vec.begin(), vec.end());
            size_t index = distance(vec.begin(), c_min_it);
            size_t c_min = vec[index];

            update_T1_LR_v_tables(it1, it2, c_min);
            update_T2_LR_w_tables(it2, c_min);

            update_T1_H_v_table(it1, it2, c_min);
            update_T2_H_w_table(it2, c_min);
        }
    }
}

void rted::test1()
{
    for (auto it1 = t1.begin_post(); it1 != t1.end_post(); ++it1)
    {
        for (auto it2 = t2.begin_post(); it2 != t2.end_post(); ++it2)
        {
            compute_relevant_subforrests(t1, it1, T1_FLeft, T1_FRight, T1_Size);
            compute_full_decomposition(t1, it1, T1_A, T1_ALeft, T1_ARight);
        }
    }
}






