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
#define get_str "GET: " F_str
#define set_str "SET: " F_str

#define labels_LR(s) label(s.left), label(s.right)

void print_subforest(const gted::subforest& f)
{
    DEBUG("[%s, %s, %s, %s]", label(f.left), label(f.right),
            label(f.path_node), label(f.root));
}


















#ifdef NODEF

bool gted::do_decompone_LR(tree_type::iterator& it_ref,
                    tree_type::iterator root,
                    path_strategy str) const
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);
    // nazaciatku ma dostat v it_ref vrchol na ceste podla str-strategy
    APP_DEBUG_FNAME;
    DEBUG("it=%s, end=%s", label(it_ref), label(root));

    assert(str != path_strategy::heavy && "only LR strategies");
    assert(id(it_ref) <= id(root));

    if (it_ref == root)
    {
        DEBUG("itref == root, return = false");
        return false;
    }

    tree_type::sibling_iterator it = it_ref;

    if (str == path_strategy::left)
    {
        // prechadzam bratov zlava doprava..
        // ak som na pravom kraji, idem do otca..
        //
        DEBUG("str = left");

        if (tree_type::is_last_child(it))
            it = tree_type::parent(it);
        else
            ++it;

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
        // prechadzam bratov zprava dolava. 
        // ak som na lavom kraji, idem do otca..
        assert(str == path_strategy::right);
        DEBUG("str = right");

        if (tree_type::is_first_child(it))
            it = tree_type::parent(it);
        else
            --it;

        while (it != root && tree_type::is_last_child(it))
        {
            DEBUG("while it=%s", label(it));
            assert(id(it) < id(root));

            if (tree_type::is_first_child(it))
                it = tree_type::parent(it);
            else
                --it;
        }
    }
    
    it_ref = it;

    bool output = root != it_ref;
    DEBUG("itout=%s, return %s", label(it_ref), output?"true":"false");
    return output;
    //return root != it_ref;
}

void gted::compute_distances_recursive(tree_type::iterator root1,
                                    tree_type::iterator root2)
{
    APP_DEBUG_FNAME;

    // pouzivam STRATEGIES...
    //
    tree_type::iterator it;
    rted::strategy_map_type::mapped_type::mapped_type spair;
    spair = make_pair(T1, path_strategy::left);
    //spair = make_pair(T2, path_strategy::left);
    //spair = make_pair(T1, path_strategy::right);
    //spair = make_pair(T2, path_strategy::right);
    //spair = make_pair(T1, path_strategy::heavy);
    //spair = make_pair(T2, path_strategy::heavy);
    //
    //spair = strategies.at(id(root1)).at(id(root2));

    if (spair.second == path_strategy::heavy)
    {
        logger.error("not implemented yet");
        abort();
    }
    else
    {
        // L/R
        if (spair.first == T1)
        {
            // rozkladam T1, zacinam vrcholom na ceste,
            // v do_decompone() ho menim
            if (spair.second == path_strategy::left)
                it = tree_type::leftmost_child(root1);
            else
                it = tree_type::rightmost_child(root1);

            while (do_decompone_LR(it, root1, spair.second))
                compute_distances_recursive(it, root2);
            assert(it == root1);
        }
        else
        {
            assert(false);
            // rozkladam T2
            if (spair.second == path_strategy::left)
                it = tree_type::leftmost_child(root2);
            else
                it = tree_type::rightmost_child(root2);

            while (do_decompone_LR(it, root2, spair.second))
                compute_distances_recursive(root1, it);
            assert(it == root2);
        }
        // mam uz dopocitane rekurzivne vsetky dekompozicie {T1/T2}
        // uz len doratat vzdialenost root1, root2
        single_path_function_LR(root1, root2, spair.second, spair.first);
    }
}

bool gted::do_decompone_LR_recursive(tree_type::iterator& it_ref,
                            tree_type::iterator end,
                            path_strategy str) const
{
    return false;

    //LOGGER_PRIORITY_ON_FUNCTION(INFO);
    APP_DEBUG_FNAME;
    DEBUG("it=%s, end=%s, str=%s", label(it_ref), label(end), to_string(str));

    assert(str != path_strategy::heavy && "only LR strategies");

    if (str == path_strategy::left)
    {
        assert(id(end) < id(it_ref));

        // idem zprava dolava
        tree_type::reverse_post_order_iterator it = it_ref;
        ++it;
        while(it != end && tree_type::is_first_child(it))
        {
            DEBUG("while ");
            ++it;
        }
        it_ref = it;
    }
    else
    {
        assert(str == path_strategy::right);
        assert(id(end) > id(it_ref));

        // idem zlava doprava
        tree_type::post_order_iterator it = it_ref;
        ++it;
        it_ref = it;
    }

    bool output = end != it_ref;
    DEBUG("itout=%s, return %s", label(it_ref), output?"true":"false");
    return output;

    return it_ref != end;
}

void gted::init_subforest_pair(subforest_pair& forests,
                    tree_type::iterator root1,
                    tree_type::iterator root2,
                    path_strategy str,
                    graph g) const
{
    assert(heavy_paths.T1_heavy.find(id(root1)) ==
            heavy_paths.T1_heavy.end() ||
            heavy_paths.T2_heavy.find(id(root1)) ==
            heavy_paths.T2_heavy.end());
    assert(heavy_paths.T1_heavy.find(id(root2)) ==
            heavy_paths.T1_heavy.end() ||
            heavy_paths.T2_heavy.find(id(root2)) ==
            heavy_paths.T2_heavy.end());

    tree_type::iterator first, second;

    if (g == T1)
    {
        switch (str)
        {
            // T1.subtrees<->T2 bolo uz spocitane, teraz idem z T1.path
            // a rekurzim podla T2.subtrees
            //      (teda v T2 zacinam na opacnej strane ako path)
            // vo forests vymienam T1 a T2... musim iterovat cez T2!!
            case path_strategy::left:
                first = tree_type::rightmost_child(root2);
                second = tree_type::leftmost_child(root1);
                break;
            default:
                abort();
        }
        forests.f1.root = root2;
        forests.f2.root = root1;
    }
    else
    {
        abort();
        switch (str)
        {
            case path_strategy::left:
                break;
            default:
                abort();
        }
    }
    forests.f1.left =
        forests.f1.right =
        forests.f1.path_node = first;
    forests.f2.left =
        forests.f2.right =
        forests.f2.path_node = second;


    assert(forests.f1.root != tree_type::iterator() &&
            forests.f2.root != tree_type::iterator());
}

void gted::compute_distance_LR(subforest_pair forests,
                        graph who_first)
{
    APP_DEBUG_FNAME;
    print_subforest(forests.f1);
    print_subforest(forests.f2);
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
        compute_distance_LR(forests, who_first);
    }

    while(do_decompone_LR_recursive(it, end_it, str))
    {
        if(tree_type::is_leaf(it))
            leaf = it;

        all_subforest_nodes_init(forests.f1, leaf);
        forests.f1.root = it;

        compute_distance_LR(forests, who_first);
    }
    all_subforest_nodes_init(forests.f1, end_it);
    forests.f1.root = (who_first == T1) ? root2 : root1;
    DEBUG("compute between roots in single_path_f");
    compute_distance_LR(forests, who_first);

    return;
}


//////////////////////////////////////////////////////////////////






void gted::compute_distances_recursive(tree_type::iterator root1,
                                    tree_type::iterator root2)
{
    APP_DEBUG_FNAME;

    // pouzivam STRATEGIES...
    //
    tree_type::iterator it;
    rted::strategy_map_type::mapped_type::mapped_type spair;
    spair = make_pair(T1, path_strategy::left);
    //spair = make_pair(T2, path_strategy::left);
    //spair = make_pair(T1, path_strategy::right);
    //spair = make_pair(T2, path_strategy::right);
    //spair = make_pair(T1, path_strategy::heavy);
    //spair = make_pair(T2, path_strategy::heavy);
    //
    //spair = strategies.at(id(root1)).at(id(root2));

    if (spair.second == path_strategy::heavy)
    {
        logger.error("not implemented yet");
        abort();


        tree_type::iterator it_path;
        // TODO inicializovat s heavy_path_node-om
        logger.warn("este nebol INIT !!!");
        if (spair.first == T1)
        {
            it_path = it = tree_type::rightmost_child(root1);
            it_path = it = tree_type::parent(it).begin();
            while(do_decompone_H(it, it_path, root1))
                compute_distances_recursive(it, root2);
        }
        else
        {
            it_path = it = tree_type::leftmost_child(root2);
            while(do_decompone_H(it, it_path, root2))
                compute_distances_recursive(root1, it);
        }
        single_path_function_H(root1, root2, spair.first);
    }
    else
    {
        // L/R
        if (spair.first == T1)
        {
            // rozkladam T1, zacinam vrcholom na ceste,
            // v do_decompone() ho menim
            if (spair.second == path_strategy::left)
                it = tree_type::leftmost_child(root1);
            else
                it = tree_type::rightmost_child(root1);

            while (do_decompone_LR(it, root1, spair.second))
                compute_distances_recursive(it, root2);
        }
        else
        {
            // rozkladam T2
            if (spair.second == path_strategy::left)
                it = tree_type::leftmost_child(root2);
            else
                it = tree_type::rightmost_child(root2);

            while (do_decompone_LR(it, root2, spair.second))
                compute_distances_recursive(root1, it);
        }
        // mam uz dopocitane rekurzivne vsetky dekompozicie {T1/T2}
        // uz len doratat vzdialenost root1, root2
        single_path_function_LR(root1, root2, spair.second, spair.first);
    }
}

bool gted::do_decompone_LR(tree_type::iterator& it_ref,
                    tree_type::iterator root,
                    path_strategy str) const
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);
    // nazaciatku ma dostat v it_ref vrchol na ceste podla str-strategy
    APP_DEBUG_FNAME;
    DEBUG("it=%s, end=%s", label(it_ref), label(root));

    assert(str != path_strategy::heavy && "only LR strategies");
    assert(id(it_ref) <= id(root));

    if (it_ref == root)
    {
        DEBUG("itref == root, return = false");
        return false;
    }

    tree_type::sibling_iterator it = it_ref;

    if (str == path_strategy::left)
    {
        // prechadzam bratov zlava doprava..
        // ak som na pravom kraji, idem do otca..
        //
        DEBUG("str = left");

        if (tree_type::is_last_child(it))
            it = tree_type::parent(it);
        else
            ++it;

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
        // prechadzam bratov zprava dolava. 
        // ak som na lavom kraji, idem do otca..
        assert(str == path_strategy::right);
        DEBUG("str = right");

        if (tree_type::is_first_child(it))
            it = tree_type::parent(it);
        else
            --it;

        while (it != root && tree_type::is_last_child(it))
        {
            DEBUG("while it=%s", label(it));
            assert(id(it) < id(root));

            if (tree_type::is_first_child(it))
                it = tree_type::parent(it);
            else
                --it;
        }
    }
    
    it_ref = it;

    bool output = root != it_ref;
    DEBUG("itout=%s, return %s", label(it_ref), output?"true":"false");
    return output;
    //return root != it_ref;
}

bool gted::do_decompone_LR_recursive(tree_type::iterator& it_ref,
                            tree_type::iterator end,
                            path_strategy str) const
{
    //LOGGER_PRIORITY_ON_FUNCTION(INFO);
    APP_DEBUG_FNAME;
    DEBUG("it=%s, end=%s, str=%s", label(it_ref), label(end), to_string(str));

    assert(str != path_strategy::heavy && "only LR strategies");

    if (str == path_strategy::left)
    {
        assert(id(end) < id(it_ref));

        // idem zprava dolava
        tree_type::reverse_post_order_iterator it = it_ref;
        ++it;
        it_ref = it;
    }
    else
    {
        assert(str == path_strategy::right);
        assert(id(end) > id(it_ref));

        // idem zlava doprava
        tree_type::post_order_iterator it = it_ref;
        ++it;
        it_ref = it;
    }

    bool output = end != it_ref;
    DEBUG("itout=%s, return %s", label(it_ref), output?"true":"false");
    return output;

    return it_ref != end;
}


void gted::single_path_function_LR(tree_type::iterator root1,
                                tree_type::iterator root2,
                                path_strategy str,
                                graph who_first)
{
    APP_DEBUG_FNAME;
    DEBUG("roots: %s %s, str=%s, whofirst=%s", label(root1), label(root2), to_string(str), to_string(who_first));
    
    assert(str != path_strategy::heavy);

    tree_type::iterator it;
    tree_type::iterator end_it;
    subforest_pair forests;
    tree_type::iterator leaf;

    if (who_first == T1)
    {
        // distance(T1.subtrees, T2) je uz vyratana.
        // teraz pocitam distance(T1.root, T2)
        init_subforest_pair(forests, root2, root1, str, who_first);
    }
    else
    {
        init_subforest_pair(forests, root1, root2, str, who_first);
    }

    end_it = (str == path_strategy::left ? 
            tree_type::leftmost_child (forests.f1.root) :
            tree_type::rightmost_child(forests.f1.root));

    it = forests.f1.path_node;
    forests.f1.root = it;

    if (it != end_it)
    {
        leaf = it;
        if ((str == path_strategy::left && !tree_type::is_first_child(it)) ||
            (str == path_strategy::right && !tree_type::is_last_child(it)))
        {
            compute_distance(forests, who_first);
        }

        while(do_decompone_LR_recursive(it, end_it, str))
        {
            if(tree_type::is_leaf(it))
                leaf = it;

            forests.f1.root = it;
            forests.f1.left =
                forests.f1.right =
                forests.f1.path_node = leaf;

            compute_distance(forests, who_first);
        }
    }
    forests.f1.root = (who_first == T1 ? root2 : root1);
    forests.f1.left = 
        forests.f1.right =
        forests.f1.path_node = end_it;

    DEBUG("compute between roots in single_path_f");
    compute_distance(forests, who_first);
}


void gted::init_subforest_pair(subforest_pair& forests,
                        tree_type::iterator root1,
                        tree_type::iterator root2,
                        path_strategy str,
                        graph g) const
{
    //TODO skontrolovat...!!!
    logger.warn("nefunkcna!?!?!? %s", __PRETTY_FUNCTION__);
    //abort();

    forests.f1.root = root1;
    forests.f2.root = root2;

    tree_type::iterator first, second;

    assert(heavy_paths.T1_heavy.find(id(root1)) ==
            heavy_paths.T1_heavy.end() ||
            heavy_paths.T2_heavy.find(id(root1)) ==
            heavy_paths.T2_heavy.end());
    assert(heavy_paths.T1_heavy.find(id(root2)) ==
            heavy_paths.T1_heavy.end() ||
            heavy_paths.T2_heavy.find(id(root2)) ==
            heavy_paths.T2_heavy.end());

    switch(str)
    {
        case path_strategy::left:
            if (g == T1)
            {
                // skor naopak...!!!
                first  = tree_type::rightmost_child(root1);
                second = tree_type::leftmost_child(root2);
            }
            else
            {
                first  = tree_type::leftmost_child(root1);
                second = tree_type::rightmost_child(root2);
            }
            break;
        default:
            logger.error("heavy: not implemented yet");
            abort();
    }

    forests.f1.left = 
        forests.f1.right =
        forests.f1.path_node = first;
    forests.f2.left = 
        forests.f2.right =
        forests.f2.path_node = second;
}

void gted::compute_distance(subforest_pair forests,
                            graph who_first)
{
    //APP_DEBUG_FNAME;
    DEBUG("computing distance between '%s' '%s'", label(forests.f1.root), label(forests.f2.root));
    print_subforest(forests.f1);
    print_subforest(forests.f2);
    //return;

    auto inited_subforest = [](const subforest& f) {
        cout << label(f.left) << " " <<
                label(f.right) << " " <<
                label(f.path_node) << " " <<
                label(f.root) << endl;

        return f.right == f.left &&
            f.right == f.path_node &&
            tree_type::is_leaf(f.right) &&
            id(f.root) >= id(f.right);
    };
    assert (inited_subforest(forests.f1) && inited_subforest(forests.f2));

    subforest_pair prevs;
    forest_distance_table_type forest_dist;
    iterator_pair leafs, parents, last_tree_roots;

    init_FDist_table(forest_dist, forests);

    // init premennych
    prevs.f1.right = forests.f1.right;
    prevs.f2.right = forests.f2.right;
    leafs.it1 = forests.f1.right;
    leafs.it2 = forests.f2.right;
    parents.it1 = tree_type::parent(leafs.it1);
    parents.it2 = tree_type::parent(leafs.it2);
    
#define compute_fdist_command() \
    fill_table(forest_dist, forests, prevs, last_tree_roots, who_first)

    bool loop = true;
    do
    {
        DEBUG("NEW CYCLE");
        if (forests.f2.root == forests.f2.right)
            loop = false;

        // inicializuj premenne na novu loop
        forests.f1.path_node =
            forests.f1.left =
            forests.f1.right = leafs.it1;

        forests.f1.last = subforest::undef;
        parents.it1 = tree_type::parent(leafs.it1);
        prevs.f1.right = leafs.it1;
        prevs.f1.left = tree_type::iterator();  // prevs ma byt neplatny, hash==0

        compute_fdist_command();

        while(forests.f1.root != forests.f1.right)
        {
            prevs.f1.left = forests.f1.left++;
            forests.f1.last = subforest::Lnode;

            while(forests.f1.left != parents.it1)
            {
                // nastav predchadzajuci podstrom, v ktorom som bol..
                if (tree_type::is_leaf(forests.f1.left))
                {
                    last_tree_roots.it1 = prevs.f1.left;
                    DEBUG("last_tree = %s", label(last_tree_roots.it1));
                }
                compute_fdist_command();
                prevs.f1.left = forests.f1.left++;
            }

            forests.f1.left = prevs.f1.left;
            prevs.f1.right = forests.f1.right++;
            forests.f1.last = subforest::Rnode;

            while(forests.f1.right != parents.it1)
            {
                // nastav predchadzajuci podstrom, v ktorom som bol..
                if (tree_type::is_leaf(forests.f1.right))
                {
                    last_tree_roots.it1 = prevs.f1.right;
                    DEBUG("last_tree = %s", label(last_tree_roots.it1));
                }
                compute_fdist_command();
                prevs.f1.right = forests.f1.right++;
            }

            forests.f1.path_node = 
                forests.f1.left = forests.f1.right;
            forests.f1.last = subforest::undef;
            // left == right == path_node
            
            last_tree_roots.it1 = tree_type::iterator();
            /*last_tree_roots.it1 = forests.f1.right;*/ // ???

            parents.it1 = tree_type::parent(forests.f1.right);
            compute_fdist_command();

            prevs.f1.left =
                prevs.f1.right = forests.f1.right;
        }

        prevs.f2.left = forests.f2.left++;
        forests.f2.last = subforest::Lnode;

        DEBUG("LNODE");
        if (prevs.f2.left == forests.f2.path_node)
        {
            DEBUG("left == pathnode");
            prevs.f2.right = forests.f2.path_node;
        }
        if (tree_type::is_leaf(forests.f2.left))
            last_tree_roots.it2 = prevs.f2.left;

        if (forests.f2.left == parents.it2)
        {
            DEBUG("t2.left == parent");
            // dosiel som az do rodica, musim znizit iterator a zvysit .right
            forests.f2.left = prevs.f2.left;
            prevs.f2.right = forests.f2.right++;
            forests.f2.last = subforest::Rnode;
            if (tree_type::is_leaf(forests.f2.right))
                last_tree_roots.it2 = prevs.f2.right;

            if (forests.f2.right == parents.it2)
            {
                // parent = right = left = path_node
                DEBUG("t2.right == parent");
                forests.f2.left =
                    forests.f2.path_node = forests.f2.right;
                forests.f2.last = subforest::undef;
                parents.it2 = tree_type::parent(parents.it2);
                last_tree_roots.it2 = tree_type::iterator();
                //prevs.f2.left =
                    //prevs.f2.right = forests.f2.right;
            }
        }
    }
    while(loop);

    print_FDist(forest_dist);
}

void gted::fill_table(forest_distance_table_type& forest_dist,
                    const subforest_pair& roots,
                    const subforest_pair& prevs,
                    iterator_pair prev_roots,
                    graph who_first)
{
    //return;
    APP_DEBUG_FNAME;

#define GTED_VECTOR_DELETE_LEFT     0
#define GTED_VECTOR_DELETE_RIGHT    1
#define GTED_VECTOR_DELETE_BOTH     2

#define GTED_COST_MODIFY            0
#define GTED_COST_DELETE            1

#define labels_LR(s) label(s.left), label(s.right)
#define get_str "get F[%s, %s][%s, %s]"
#define set_str "set F[%s, %s][%s, %s]"

    auto get_dist = [&](const subforest& index1, const subforest& index2){
        size_t out;
        //cout << "A" << endl;
        //DEBUG(get_str, labels_LR(index1), labels_LR(index2));
        try
        {
            out = forest_dist.at(index1).at(index2);
        } catch(...)
        {
            DEBUG("CATCH");
            print_FDist(forest_dist);
            abort();
        }
        //cout << "B" << endl;
        return out;
    };
    auto set_dist = [&](const subforest& index1, const subforest& index2, size_t value) {
        DEBUG(set_str " = %lu", labels_LR(index1), labels_LR(index2), value);
        forest_dist[index1][index2] = value;
    };

    print_subforest(roots.f1);
    print_subforest(roots.f2);
    print_subforest(prevs.f1);
    print_subforest(prevs.f2);

    vector<size_t> vec(3, 0xBADF00D);
    size_t c_min;

    vec[GTED_VECTOR_DELETE_LEFT]  = (who_first == T1) ?
        get_dist(prevs.f1, roots.f2) :
        get_dist(prevs.f2, roots.f1);
    vec[GTED_VECTOR_DELETE_RIGHT] = (who_first == T1) ?
        get_dist(roots.f1, prevs.f2) :
        get_dist(roots.f2, prevs.f1);

    vec[GTED_VECTOR_DELETE_LEFT]  += GTED_COST_DELETE;
    vec[GTED_VECTOR_DELETE_RIGHT] += GTED_COST_DELETE;

    if (roots.f1.right == roots.f1.path_node &&
            roots.f2.right == roots.f2.path_node)
    {
        //DEBUG("path node");
        vec[GTED_VECTOR_DELETE_BOTH] = (who_first == T1) ?
            get_dist(prevs.f1, prevs.f2) :
            get_dist(prevs.f2, prevs.f1);
    }
    else
    {
        //DEBUG("not path node");
        //assert(roots.f1.last != subforest::undef &&
                //roots.f2.last != subforest::undef);
        DEBUG("prevroots=[%s, %s]", label(prev_roots.it1), label(prev_roots.it2));

        subforest_pair other = prevs;
        if (roots.f1.last == subforest::Lnode)
        {
            DEBUG("A1");
            other.f1.left = prev_roots.it1;
        }
        else if (roots.f1.last == subforest::Rnode)
        {
            DEBUG("A2");
            other.f1.right = prev_roots.it1;
        }
        else
        {
            DEBUG("A3");
            other.f1.left = prev_roots.it1;
            //other.f1.left = other.f1.right = prev_roots.it1;
        }

        if (roots.f2.last == subforest::Lnode)
        {
            DEBUG("B1");
            other.f2.left = prev_roots.it2;
        }
        else if (roots.f2.last == subforest::Rnode)
        {
            DEBUG("B2");
            other.f2.right = prev_roots.it2;
        }
        else
        {
            DEBUG("B3");
            other.f2.left = prev_roots.it2;
            //other.f2.left  = other.f2.right = prev_roots.it2;
        }

        DEBUG("other:");
        print_subforest(other.f1);
        print_subforest(other.f2);
        logger.warn("not implemented yet");

        vec[GTED_VECTOR_DELETE_BOTH] = (who_first == T1) ?
            get_dist(other.f1, other.f2) :
            get_dist(other.f2, other.f1);


        //abort();
    }
    vec[GTED_VECTOR_DELETE_BOTH] += GTED_COST_MODIFY;

    LOGGER_PRINT_CONTAINER(vec, "vec");

    c_min = *min_element(vec.begin(), vec.end());

    if (who_first == T1)
        set_dist(roots.f1, roots.f2, c_min);
    else
        set_dist(roots.f2, roots.f1, c_min);
    //print_FDist(forest_dist);
}

#endif





















void gted::compute_distances_recursive(tree_type::iterator root1,
                                    tree_type::iterator root2)
{
    APP_DEBUG_FNAME;

    // pouzivam STRATEGIES...
    //
    tree_type::iterator it;
    rted::strategy_map_type::mapped_type::mapped_type spair;
    spair = make_pair(T1, path_strategy::left);
    //spair = make_pair(T2, path_strategy::left);
    //spair = make_pair(T1, path_strategy::right);
    //spair = make_pair(T2, path_strategy::right);
    //spair = make_pair(T1, path_strategy::heavy);
    //spair = make_pair(T2, path_strategy::heavy);
    //
    //spair = strategies.at(id(root1)).at(id(root2));

    if (spair.second == path_strategy::heavy)
    {
        logger.error("not implemented yet");
        abort();
    }
    else
    {
        // L/R
        if (spair.first == T1)
        {
            // rozkladam T1, zacinam vrcholom na ceste,
            // v do_decompone() ho menim
            if (spair.second == path_strategy::left)
                it = tree_type::leftmost_child(root1);
            else
                it = tree_type::rightmost_child(root1);

            while (do_decompone_LR(it, root1, spair.second))
                compute_distances_recursive(it, root2);
            assert(it == root1);
        }
        else
        {
            abort();
            // rozkladam T2
            if (spair.second == path_strategy::left)
                it = tree_type::leftmost_child(root2);
            else
                it = tree_type::rightmost_child(root2);

            while (do_decompone_LR(it, root2, spair.second))
                compute_distances_recursive(root1, it);
            assert(it == root2);
        }
        // mam uz dopocitane rekurzivne vsetky dekompozicie {T1/T2}
        // uz len doratat vzdialenost root1, root2
        single_path_function_LR(root1, root2, spair.second, spair.first);
    }
}

bool gted::do_decompone_LR(tree_type::iterator& it_ref,
                    tree_type::iterator root,
                    path_strategy str) const
{
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
        abort();
    }

    it_ref = it;

    bool output = (root != it_ref);
    DEBUG("itout=%s, return %s", label(it_ref), output?"true":"false");
    return output;
}

void gted::init_subforest_pair(subforest_pair& forests,
                    tree_type::iterator root1,
                    tree_type::iterator root2,
                    path_strategy str,
                    graph g) const
{
    assert(heavy_paths.T1_heavy.find(id(root1)) ==
            heavy_paths.T1_heavy.end() ||
            heavy_paths.T2_heavy.find(id(root1)) ==
            heavy_paths.T2_heavy.end());
    assert(heavy_paths.T1_heavy.find(id(root2)) ==
            heavy_paths.T1_heavy.end() ||
            heavy_paths.T2_heavy.find(id(root2)) ==
            heavy_paths.T2_heavy.end());

    tree_type::iterator first, second;

    if (g == T1)
    {
        switch (str)
        {
            // T1.subtrees<->T2 bolo uz spocitane, teraz idem z T1.path
            // a rekurzim podla T2.subtrees
            //      (teda v T2 zacinam na opacnej strane ako path)
            // vo forests vymienam T1 a T2... musim iterovat cez T2!!
            case path_strategy::left:
                first = tree_type::rightmost_child(root2);
                second = tree_type::leftmost_child(root1);
                break;
            default:
                abort();
        }
        forests.f1.root = root2;
        forests.f2.root = root1;
    }
    else
    {
        abort();
        switch (str)
        {
            case path_strategy::left:
                break;
            default:
                abort();
        }
    }
    forests.f1.left =
        forests.f1.right =
        forests.f1.path_node = first;
    forests.f2.left =
        forests.f2.right =
        forests.f2.path_node = second;


    assert(forests.f1.root != tree_type::iterator() &&
            forests.f2.root != tree_type::iterator());
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
        if (tree_type::is_leaf(it))
            leaf = it;

        it_ref = it;
    }
    else
    {
        abort();
    }

    bool output = end != it_ref;
    DEBUG("itout=%s, return %s", label(it_ref), output?"true":"false");
    return output;
}


void gted::compute_distance(subforest_pair forests,
                            graph who_first)
{
    DEBUG("computing distance between '%s' '%s'",
            label(forests.f1.root), label(forests.f2.root));
    
    auto inited_subforest = [](const subforest& f) {
        //cout << label(f.left) << " " <<
                //label(f.right) << " " <<
                //label(f.path_node) << " " <<
                //label(f.root) << endl;

        return f.right == f.left &&
            f.right == f.path_node &&
            tree_type::is_leaf(f.right) &&
            id(f.root) >= id(f.right);
    };
    assert (inited_subforest(forests.f1) && inited_subforest(forests.f2));

    subforest_pair prevs;
    forest_distance_table_type forest_dist;
    iterator_pair leafs, parents, last_tree_roots;

    init_FDist_table(forest_dist, forests);

    prevs.f1.right = forests.f1.right;
    prevs.f2.right = forests.f2.right;
    leafs.it1 = forests.f1.right;
    leafs.it2 = forests.f2.right;
    parents.it1 = tree_type::parent(leafs.it1);
    parents.it2 = tree_type::parent(leafs.it2);

    last_tree_roots.it2 = leafs.it2;
/*
    struct
    {
        vector<tree_type::iterator> s1, s2;
    } last_tree_roots_stacks;
    auto del_stack_children = [](vector<tree_type::iterator>& stack, tree_type::iterator it){
        cout << "STACK:" << endl;
        for (auto i : stack)
            cout << *i << " ";
        cout << endl;
        while(tree_type::parent(stack.back()) ==  it)
        {
            assert(!stack.empty());
            DEBUG("stack.s1.back == it, deleting %s", label(stack.back()));
            stack.pop_back();
        }
    };

    last_tree_roots_stacks.s2 = {leafs.it2};
*/
#define compute_fdist_command() \
    fill_table(forest_dist, forests, prevs, \
            last_tree_roots, who_first)

    bool loop = true;
    do
    {
        /*
         *  iterujem najprv cez F1, vonkajsia loop je cez F2.
         *  vzdy pridavam najprv lave vrcholy, az kym nedojdem do otca, potom sa 
         *  o 1 krok vratim spat (1.syn, najlavejsi) a pridavam prave vrcholy.
         *  ked aj tam dojdem do otca, nastavim aj laveho na otca a idem novy (vnutorny) cyklus
         *  az kym nedojdem do korena. 
         *  vonkajsi cyklus funguje na rovnakom principe
         *
         *  v premennych last_tree_roots su:
         *      ak je .last == undef: je tam pointer na zaciatocny list, 
         *          aby sa dalo pouzit mazanie vrcholu -> index do tabulky: (<null>, list)
         *      inac je tam posledny subtreevrchol ktory je mimo aktualneho podstromu
         *          a teda da sa pouzit index F: (<?begin?>, last_tree_root)
         *          a index T: (this.node)
         *
         *  aby fungovala prva moznost, treba si ukladat tie vrcholy do zasobnika, 
         *  a vzdy ak zasobnik.back == child(node), tak musim dat zasobnik.pop()
         *  inac pouzivam dany vrchol ako last_tree_root
         *  ak je zasobnik prazdny, tak pouzijem list z cesty.. (leafs.it)
         */

        DEBUG("******************** NEW CYCLE ********************");
        if (forests.f2.root == forests.f2.right)
            loop = false;

        // init premennych
        all_subforest_nodes_init(forests.f1, leafs.it1);

        forests.f1.last = subforest::undef;
        parents.it1 = tree_type::parent(leafs.it1);
        prevs.f1.right = forests.f1.right;
        prevs.f1.left = tree_type::iterator();
        //last_tree_roots_stacks.s1 = { leafs.it1 };
        //last_tree_roots.it1 = last_tree_roots_stacks.s1.back();
        last_tree_roots.it1 = leafs.it1;

        compute_fdist_command();

        while(forests.f1.root != forests.f1.right)
        {
            prevs.f1.left = forests.f1.left++;
            forests.f1.last = subforest::Lnode;

            while(parents.it1 != forests.f1.left)
            {
                if (tree_type::is_leaf(forests.f1.left))
                {
                    last_tree_roots.it1 = prevs.f1.left;
                    //last_tree_roots_stacks.s1.push_back(prevs.f1.left);
                    DEBUG("L: last_tree <- %s", label(last_tree_roots.it1));
                }
                // ak som v rodicovi, zmazem vsetkych synov ktore boli v zasobniku..
                //del_stack_children(last_tree_roots_stacks.s1, forests.f1.left);
                //last_tree_roots.it1 = last_tree_roots_stacks.s1.back();

                compute_fdist_command();
                prevs.f1.left = forests.f1.left++;
            }
            forests.f1.left = prevs.f1.left;
            prevs.f1.right = forests.f1.right++;
            forests.f1.last = subforest::Rnode;

            while(parents.it1 != forests.f1.right)
            {
                if (tree_type::is_leaf(forests.f1.right))
                {
                    //last_tree_roots_stacks.s1.push_back(prevs.f1.right);
                    DEBUG("R: last_tree <- %s", label(last_tree_roots.it1));
                }
                //del_stack_children(last_tree_roots_stacks.s1, forests.f1.right);
                //last_tree_roots.it1 = last_tree_roots_stacks.s1.back();
                compute_fdist_command();
                prevs.f1.right = forests.f1.right++;
            }

            // ak v T2 som mimo cesty:
            //      F[forests.f1][forests.f2] = MIN {
            //          F[prevs.f1][forests.f2] + DELETE,
            //          F[forests.f1][prevs.f2] + DELETE,
            //          F[EMPTY][prevs.f2 from last_tree_root.it1]
            //              + T[forests.f1][forests.f2]
            //      }
            // inac 3. podmienka je iba
            //          F[prevs.f1][prevs.f2] + MODIFY
            //
            parents.it1 = tree_type::parent(forests.f1.right);
            last_tree_roots.it1 = leafs.it1;
            forests.f1.last = subforest::undef;
            forests.f1.left =
                forests.f1.path_node = forests.f1.right;

            compute_fdist_command();

            prevs.f1.left =
                prevs.f1.right = forests.f1.right;
        }

        forests.f2.last = subforest::Lnode;
        prevs.f2.left = forests.f2.left++;

        DEBUG("LNODE");
        /*if (prevs.f2.left == forests.f2.path_node)
        {
            // TODO: zistit preco!!!
            cout << (prevs.f2.left == parents.it2) << endl;
            DEBUG("prevs: left == path_node, right<-left");
            prevs.f2.right = prevs.f2.left;
            abort();
        }*/
        if (tree_type::is_leaf(forests.f2.left))
        {
            last_tree_roots.it2 = prevs.f2.left;
            DEBUG("L2: last <- left, %s", label(prevs.f2.left));
            //last_tree_roots_stacks.s2.push_back(prevs.f2.left);
        }
        //del_stack_children(last_tree_roots_stacks.s2, forests.f2.left);
        //last_tree_roots.it2 = last_tree_roots_stacks.s2.back();

        if (forests.f2.left == parents.it2)
        {
            DEBUG("t2.left == parent");

            forests.f2.left = prevs.f2.left;
            forests.f2.last = subforest::Rnode;
            prevs.f2.right = forests.f2.right++;

            if (tree_type::is_leaf(forests.f2.right))
            {
                last_tree_roots.it2 = prevs.f2.right;
                DEBUG("L2: last <- right, %s", label(prevs.f2.right));
                //last_tree_roots_stacks.s2.push_back(prevs.f2.right);
            }
            //del_stack_children(last_tree_roots_stacks.s2, forests.f2.right);
            //last_tree_roots.it2 = last_tree_roots_stacks.s2.back();

            if (forests.f2.right == parents.it2)
            {
                DEBUG("t2.right == parent");
                last_tree_roots.it2 = leafs.it2;
                forests.f2.last = subforest::undef;
                forests.f2.left =
                    forests.f2.path_node = forests.f2.right;
                parents.it2 = tree_type::parent(parents.it2);
            }
        }
    }
    while(loop);
    print_FDist(forest_dist);
    if (forests.f1.root->is_root() && forests.f2.root->is_root())
        print_TDist();
}


void gted::fill_table(forest_distance_table_type& forest_dist,
                const subforest_pair& roots,
                const subforest_pair& prevs,
                iterator_pair prev_roots,
                graph who_first)
{
#define GTED_VECTOR_DELETE_LEFT     0
#define GTED_VECTOR_DELETE_RIGHT    1
#define GTED_VECTOR_DELETE_BOTH     2

#define GTED_COST_MODIFY            0
#define GTED_COST_DELETE            1

#define labels_LR(s) label(s.left), label(s.right)

    auto get_dist = [&forest_dist, this](const subforest& index1, const subforest& index2){
        size_t out;
        //cout << "A" << endl;
        DEBUG(get_str,
                labels_LR(index1), labels_LR(index2));
        try
        {
            out = forest_dist.at(index1).at(index2);
        } catch(...)
        {
            DEBUG("CATCH");
            print_FDist(forest_dist);
            abort();
        }
        //cout << "B" << endl;
        return out;
    };
    auto set_dist = [&](const subforest& index1, const subforest& index2, size_t value) {
        DEBUG(set_str " = %lu",
                labels_LR(index1), labels_LR(index2), value);
        forest_dist[index1][index2] = value;
    };
    auto get_Tdist = [this](tree_type::iterator index1,tree_type::iterator index2){
        DEBUG("GET: " T_str,
                label(index1), label(index2));
        return tree_distances.at(id(index1)).at(id(index2));
    };
    auto set_Tdist = [this](tree_type::iterator index1,tree_type::iterator index2, size_t value){
        DEBUG("SET: " T_str " = %lu",
                label(index1), label(index2), value);
        tree_distances[id(index1)][id(index2)] = value;
    };

    assert(who_first == T1);
    set_dist(roots.f1, roots.f2, 123456);

    vector<size_t> vec(3, 0xBADF00D);
    size_t c_min;

    vec[GTED_VECTOR_DELETE_LEFT] = (who_first == T1) ?
        get_dist(prevs.f1, roots.f2) :
        get_dist(prevs.f2, roots.f1);
    vec[GTED_VECTOR_DELETE_RIGHT] = (who_first == T1) ?
        get_dist(roots.f1, prevs.f2) :
        get_dist(roots.f2, prevs.f1);

    vec[GTED_VECTOR_DELETE_LEFT]  += GTED_COST_DELETE;
    vec[GTED_VECTOR_DELETE_RIGHT] += GTED_COST_DELETE;

    if (roots.f1.right == roots.f1.path_node &&
            roots.f2.right == roots.f2.path_node)
    {
        DEBUG("path node");
        vec[GTED_VECTOR_DELETE_BOTH] = (who_first == T1) ?
            get_dist(prevs.f1, prevs.f2) :
            get_dist(prevs.f2, prevs.f1);
    }
    else
    {
        DEBUG("not path node");
        //print_subforest(roots.f1);
        //print_subforest(roots.f2);
        //print_subforest(prevs.f1);
        //print_subforest(prevs.f2);
        DEBUG("prevroots=[%s, %s]",
                label(prev_roots.it1), label(prev_roots.it2));

        subforest_pair other = prevs;
        tree_type::iterator t_it1, t_it2;

        if (roots.f1.last == subforest::Lnode)
        {
            DEBUG("f1.last = lnode");
            other.f1.left = prev_roots.it1;
            t_it1 = roots.f1.left;
        }
        else if (roots.f1.last == subforest::Rnode)
        {
            DEBUG("f1.last == rnode");
            other.f1.right = prev_roots.it1;
            t_it1 = roots.f1.right;
        }
        else
        {
            DEBUG("f1.last == undef");
            other.f1.left = empty_iterator();
            other.f1.right = prev_roots.it1;
            t_it1 = roots.f1.right;
        }

        if (roots.f2.last == subforest::Lnode)
        {
            DEBUG("f2.last = lnode");
            other.f2.left = prev_roots.it2;
            t_it2 = roots.f2.left;
        }
        else if (roots.f2.last == subforest::Rnode)
        {
            DEBUG("f2.last == rnode");
            other.f2.right = prev_roots.it2;
            t_it2 = roots.f2.right;
        }
        else
        {
            DEBUG("f2.last == undef");
            other.f2.left = empty_iterator();
            other.f2.right = prev_roots.it2;
            t_it2 = roots.f2.right;
        }

        //DEBUG("OTHER:");
        //print_subforest(other.f1);
        //print_subforest(other.f2);
        //DEBUG("its: %s, %s", label(t_it1), label(t_it2));

        vec[GTED_VECTOR_DELETE_BOTH] = (who_first == T1) ?
            get_dist(other.f1, other.f2) :
            get_dist(other.f2, other.f1);

        vec[GTED_VECTOR_DELETE_BOTH] += (who_first == T1) ?
            get_Tdist(t_it1, t_it2) :
            get_Tdist(t_it2, t_it1);
            //tree_distances.at(id(t_it1)).at(id(t_it2)) :
            //tree_distances.at(id(t_it2)).at(id(t_it1));
    }

    LOGGER_PRINT_CONTAINER(vec, "vec");

    c_min = *min_element(vec.begin(), vec.end());
    if (who_first == T1)
        set_dist(roots.f1, roots.f2, c_min);
    else
        set_dist(roots.f2, roots.f1, c_min);

    if (roots.f1.right == roots.f1.path_node &&
            roots.f2.right == roots.f2.path_node)
    {
        assert(who_first == T1);
        set_Tdist(roots.f1.right, roots.f2.right, c_min);
        //tree_distances[id(roots.f1.right)][id(roots.f2.right)] = c_min;
    }
    DEBUG("");
}














