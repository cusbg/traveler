/*
 * File: gted_LR.cpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard@ba30.eu>
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

#include "gted.hpp"

using namespace std;

#define all_subforest_nodes_init(Subforest, value) \
    Subforest.left = Subforest.right = Subforest.path_node = value
#define F_str "F[%s, %s][%s, %s]"
#define T_str "T[%s][%s]"

#define labels_LR(s) label(s.left), label(s.right)

bool gted::do_decompone_LR(tree_type::iterator& it_ref,
                    tree_type::iterator root,
                    path_strategy str) const
{
    /*
     * podla strategie vracia keyroots
     */
    APP_DEBUG_FNAME;
    DEBUG("it=%s, end=%s", label(it_ref), label(root));

    assert(str != path_strategy::heavy && "only LR strategies");
    assert(id(it_ref) <= id(root));

    if (it_ref == root)
    {
        DEBUG("it_ref == root, return=false");
        return false;
    }

    tree_type::sibling_iterator it = it_ref;
    
    if (str == path_strategy::left)
    {
        // prechadzam bratov zlava doprava
        // ak som nakraji, idem do otca.
        //
        DEBUG("str == left");
        
        if (tree_type::is_last_child(it))
            it = tree_type::parent(it);
        else
            ++it;

        // sice som sa posunul, ale som na ceste, musim is to dalsieho vrcholu
        while(it != root && tree_type::is_first_child(it))
        {
            DEBUG("while, it=%s", label(it));
            assert(id(it) < id(root));

            if (tree_type::is_last_child(it))
                it = tree_type::parent(it);
            else
                ++it;
        }
    }
    else
    {
        // prechadzam bratov zprava dolava, obdoba ::left
        DEBUG("str == right");

        if (tree_type::is_first_child(it))
            it = tree_type::parent(it);
        else
            --it;

        while(it != root && tree_type::is_last_child(it))
        {
            DEBUG("while, it=%s", label(it));
            assert(id(it) < id(root));

            if (tree_type::is_first_child(it))
                it = tree_type::parent(it);
            else
                --it;
        }
    }

    it_ref = it;

    bool output = (root != it_ref);
    DEBUG("itout=%s, return %s", label(it_ref), output?"true":"false");
    return output;
}

bool gted::do_decompone_LR_recursive(tree_type::iterator& it_ref,
                                tree_type::iterator& leaf,
                                tree_type::iterator end,
                                path_strategy str) const
{
    APP_DEBUG_FNAME;
    DEBUG("it=%s, end=%s, str=%s",
            label(it_ref), label(end), to_string(str));

    assert(str != path_strategy::heavy && "only LR strategies");

    if (str == path_strategy::left)
    {
        assert(id(end) < id(it_ref));

        // idem zprava dolava
        tree_type::reverse_post_order_iterator it = it_ref;
        ++it;
        while(it != end && tree_type::is_first_child(it))
        {
            if (tree_type::is_leaf(it))
                leaf = it;

            DEBUG("while, it=%s", label(it));
            ++it;
        }
        it_ref = it;
    }
    else
    {
        DEBUG("right str");
        // idem zlava doprava..
        assert(id(end) > id(it_ref));

        tree_type::post_order_iterator it = it_ref;
        ++it;
        while(it != end && tree_type::is_last_child(it))
        {
            if (tree_type::is_leaf(it))
                leaf = it;

            DEBUG("while, it=%s", label(it));
            ++it;
        }
        it_ref = it;
    }
    // pripad ze while neprebehne ani raz..
    if (tree_type::is_leaf(it_ref))
        leaf = it_ref;

    bool output = end != it_ref;
    DEBUG("itout=%s, return %s", label(it_ref), output?"true":"false");
    return output;
}

void gted::single_path_function_LR(tree_type::iterator root1,
                                tree_type::iterator root2,
                                path_strategy str,
                                graph who_first)
{
    APP_DEBUG_FNAME;
    DEBUG("roots: %s %s, str=%s, whofirst=%s", 
            label(root1), label(root2), to_string(str), to_string(who_first));
    
    assert(str != path_strategy::heavy);

    tree_type::iterator it;
    tree_type::iterator end_it;
    subforest_pair forests;
    tree_type::iterator leaf;

    init_subforest_pair(forests, root1, root2, str, who_first);
    print_subforest(forests.f1);
    print_subforest(forests.f2);

    end_it = (str == path_strategy::left ? 
            tree_type::leftmost_child (forests.f1.root) :
            tree_type::rightmost_child(forests.f1.root));
    it = forests.f1.path_node;
    forests.f1.root = it;
    leaf = it;

    if ((str == path_strategy::left && !tree_type::is_first_child(it)) ||
        (str == path_strategy::right && !tree_type::is_last_child(it)))
    {
        compute_distance(forests, who_first);
    }

    while(do_decompone_LR_recursive(it, leaf, end_it, str))
    {
        all_subforest_nodes_init(forests.f1, leaf);
        forests.f1.root = it;

        //print_subforest(forests.f1);

        compute_distance(forests, who_first);
    }
    all_subforest_nodes_init(forests.f1, end_it);
    forests.f1.root = (who_first == T1) ? root2 : root1;
    DEBUG("compute between roots in single_path_f");
    compute_distance(forests, who_first);
}

bool gted::do_decompone_H(tree_type::iterator& it_ref,
                        tree_type::iterator root,
                        tree_type::iterator& it_path_node) const
{
    //LOGGER_PRIORITY_ON_FUNCTION(INFO);

    APP_DEBUG_FNAME;

    DEBUG("it=%s, pathnode=%s, root=%s",
            label(it_ref), label(it_path_node), label(root));
    
    assert(id(it_ref) <= id(root) &&
            id(it_path_node) <= id(root));

    if (it_ref == root)
    {
        DEBUG("itref == root, return false");
        assert(it_ref == it_path_node);
        return false;
    }

    tree_type::sibling_iterator it = it_ref;

    if (it == it_path_node)
        // it <- 1. brat it.
        it = tree_type::first_child(tree_type::parent(it));
    else if (tree_type::is_last_child(it))
    {
        // it <- 1. brat od rodica..
        it = tree_type::parent(it);
        it_path_node = it;
    }
    else
        ++it;

    while(it != root && it == it_path_node)
    {
        DEBUG("while, it=%s", label(it));

        if (tree_type::is_last_child(it))
        {
            // go to first sibling of parent..
            it = tree_type::parent(it);
            it_path_node = it;

            // ak nieje root, tak ma este otca, a chcem jeho prveho syna..
            // inac, root je jediny, takze to nepotrebujem riesit..
            if (!it->is_root())
                it = tree_type::first_child(tree_type::parent(it));
        }
        else
            ++it;
    }
    it_ref = it;

    bool output = root != it_ref;
    DEBUG("itout=%s, return %s", label(it_ref), output?"true":"false");
    return output;
}

bool gted::do_decompone_H_recursive(tree_type::iterator& it_ref,
                                    tree_type::iterator& leaf,
                                    tree_type::iterator end) const
{
    APP_DEBUG_FNAME;
    DEBUG("it=%s, end=%s, str=heavy",
            label(it_ref), label(end));

    // budem vzdy zacinat v najlavejsom vrchole
    // a postupne budem prechadzat strom a ak
    //  heavy_node(parent(it_ref)) != leaf,
    //      tak to znamena, ze it_ref nieje na heavy_path otca
    //      a teda ze mam dany vrchol vyratat. inac pokracujem dalej.

    tree_type::post_order_iterator it = it_ref;

    ++it;

    //while(it != end && leaf == 
    //TODO: potrebujem tu aj tabulku heavy_paths...
    //
    //moznob y stalo zato zlucit T1/T2 heavy_paths do 1 tabulky
    //.. iba najprv skontrolovat, ze IDcka v oboch stromoch su rozne...
    //a usetri sa potom kopa roboty s parametramy a tak..

    it_ref = it;

    if (tree_type::is_leaf(it_ref))
        leaf = it_ref;

    return false;
}

void gted::single_path_function_H(tree_type::iterator root1,
                                tree_type::iterator root2,
                                graph who_first)
{
    APP_DEBUG_FNAME;
    DEBUG("roots: %s %s, whofirst=%s", 
            label(root1), label(root2), to_string(who_first));

    tree_type::iterator it;
    tree_type::iterator end_it;
    subforest_pair forests;
    tree_type::iterator leaf;

    init_subforest_pair(forests, root1, root2, path_strategy::heavy, who_first);
    print_subforest(forests.f1);
    print_subforest(forests.f2);
}




























