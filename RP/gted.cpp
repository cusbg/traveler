/*
 * File: gted.cpp
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

#include "gted.hpp"
#include "tree_hh/tree_util.hh"
#include <algorithm>

using namespace std;

/**
 * ZHANG-SHASHA generalization alg:
 *
 * T{1,2}_keyroots contains node_ids from trees T1, T2, node_id != 0
 *
 *
 * Main loop:
 *  main(T1_keyroots, T2_keyroots):
 *      foreach (root1 in T1_keyroots)
 *          foreach (root2 in T2_keyroots)
 *              Compute treedist(root1, root2)
 *
 *
 * Treedist computation loop: 
 *  treedist(root1, root2):
 *      forestdist(0, 0) = 0;
 */


gted::decomposition_type gted::path_decomposition(tree_type::iterator root,
            const tree_type& t, const map_type& t_size, path_strategy s)
{
    set_logger_priority_to_return_function l(log4cpp::Priority::INFO);

    APP_DEBUG_FNAME;

    if (s == path_strategy::left)
        return left_decomposition(root);
    if (s == path_strategy::right)
        return right_decomposition(root);
    if (s == path_strategy::heavy)
        return heavy_decomposition(root, t, t_size);

    logger.error("only LRH path strategies are supported!");
    return decomposition_type();
}

gted::decomposition_type gted::left_decomposition(tree_type::iterator it)
{
    APP_DEBUG_FNAME;

    decomposition_type decomposition;
    relevant_subtrees& subtrees = decomposition.subtrees;
    path_type& path = decomposition.path;
    while (!tree_type::is_leaf(it))
    {
        logger.debug("going down the path node '%s'", label(it));

        path[it->get_id()] = it;
        tree_type::sibling_iterator sib(tree_type::first_child(it));
        while (++sib != sib.end())
        {
            logger.debug("adding '%s' to relevat subtrees", label(sib));

            subtrees.push_back(sib);
        }
        it = tree_type::child(it, 0);
    }
    path[it->get_id()] = it;

    return decomposition;
}

gted::decomposition_type gted::right_decomposition(tree_type::iterator it)
{
    APP_DEBUG_FNAME;

    decomposition_type decomposition;
    relevant_subtrees& subtrees = decomposition.subtrees;
    path_type& path = decomposition.path;
    while (!tree_type::is_leaf(it))
    {
        logger.debug("going down the path node '%s'", label(it));

        path[it->get_id()] = it;
        tree_type::sibling_iterator sib(tree_type::last_child(it));
        while (!tree_type::is_first_child(sib))
        {
            --sib;
            logger.debug("adding '%s' to relevat subtrees", label(sib));

            subtrees.push_back(sib);
        }
        it = tree_type::last_child(it);
    }
    path[it->get_id()] = it;
    
    return decomposition;
}

gted::decomposition_type gted::heavy_decomposition(tree_type::iterator it,
            const tree_type& t, const map_type& t_size)
{
    APP_DEBUG_FNAME;

    decomposition_type decomposition;
    relevant_subtrees& subtrees = decomposition.subtrees;
    path_type& path = decomposition.path;
    while (!tree_type::is_leaf(it))
    {
        logger.debug("going down the path node '%s'", label(it));

        path[it->get_id()] = it;    //momentalne som na heavy_path

        size_t index = biggest_subtree_child(it, t, t_size);
        size_t i = 0;
        tree_type::sibling_iterator sib = tree_type::first_child(it);

        while (sib != sib.end())
        {
            if (i != index)
            {
                logger.debug("adding '%s' to relevat subtrees", label(sib));

                subtrees.push_back(sib);
            }
            else
            {
                logger.debug("jumping over '%s', node lie on the heavy path", label(sib));
            }
            ++i;
            ++sib;
        }
        it = tree_type::child(it, index);
    }
    path[it->get_id()] = it;

    return decomposition;
}

bool gted::node_lies_on_path(tree_type::iterator it, const path_type& path) const
{
    return path.find(it->get_id()) != path.end();
}

size_t gted::biggest_subtree_child(tree_type::iterator root,
            const tree_type& t, const map_type& t_size) const
{
    assert(!tree_type::is_leaf(root));
    tree_type::sibling_iterator it(tree_type::first_child(root));
    size_t index = 0;
    size_t biggest_subtree = 0;
    size_t i = 0;

    while(it != it.end())
    {
        auto val = t_size.at(it->get_id());
        if (val > biggest_subtree)
        {
            index = i;
            biggest_subtree = val;
        }
        ++it;
        ++i;
    }
    logger.debug("child no. '%lu' of node '%s' has biggest subtree (size == %lu)",
            index, label(root), biggest_subtree);
    return index;
}

void gted::print_distances() const
{
    cout << "TREE_DISTANCE_TABLE:" << endl;
    for (auto it2 = t2.tree_ptr->begin_post(); it2 != --t2.tree_ptr->end_post(); ++it2)
        cout << "\t" << *it2;
    cout << endl;
    for (auto it1 = t1.tree_ptr->begin_post(); it1 != --t1.tree_ptr->end_post(); ++it1)
    {
        cout << *it1;
        for (auto it2 = t2.tree_ptr->begin_post(); it2 != --t2.tree_ptr->end_post(); ++it2)
        {
            cout << "\t" << tree_distances.at(it1->get_id()).at(it2->get_id());
            //cout << *it1 << " " << *it2 << endl;
            //cout << tree_distances[it1->get_id()][it2->get_id()] << "[" << label(it1) << "," << label(it2) << "]" << " ";
        }
        cout << endl;
    }
}

void gted::init_tree_dist_table()
{
    //TODO: zmenit 2cykly na kopirovanie map ..
    for (auto it1 = t1.tree_ptr->begin_post(); it1 != t1.tree_ptr->end_post(); ++it1)
    {
        for (auto it2 = t2.tree_ptr->begin_post(); it2 != t2.tree_ptr->end_post(); ++it2)
            tree_distances[it1->get_id()][it2->get_id()] = 0xDEAD;
    }
}




gted::gted(const tree_type& _t1, const tree_type& _t2)
    : t1(_t1), t2(_t2)
{
    APP_DEBUG_FNAME;
    set_logger_priority_to_return_function p(log4cpp::Priority::WARN);

    rted r(t1, t2);
    r.run_rted();
    strategies = r.get_strategies();
    t1_sizes = r.get_t1_sizes();
    t2_sizes = r.get_t2_sizes();
    init_tree_dist_table();
}

void gted::run_gted()
{
    APP_DEBUG_FNAME;
    logger.debug("GTED: BEGIN");

    auto root1 = ++t1.tree_ptr->begin();
    auto root2 = ++t2.tree_ptr->begin();

    SUBTREE_DEBUG_PRINT(*t1.tree_ptr, root1);
    SUBTREE_DEBUG_PRINT(*t2.tree_ptr, root2);
    //compute_distance(t1.tree_ptr->begin(), t2.tree_ptr->begin());
    compute_distance_recursive(root1, root2);

    //print_distances();

    logger.debug("GTED: END");
}




void gted::init_forest_dist_table(distance_table& forest_dist, tree_type::iterator root1, tree_type::iterator root2)
{
    APP_DEBUG_FNAME;

    stringstream out;
    out << "INIT!!!" << endl;
    forest_dist[0][0] = 0;

    SUBTREE_DEBUG_PRINT(*t1.tree_ptr, root1);
    SUBTREE_DEBUG_PRINT(*t2.tree_ptr, root2);

    size_t i;

    tree_type::post_order_iterator it1(root1);
    if (!tree_type::is_leaf(root1))
        it1 = tree_type::post_order_iterator(tree_type::leftmost_child(root1));

    out << "init FDist[";
    i = 0;
    while (it1 != root1)
    {
        out << it1->get_id() << " ";
        forest_dist[it1->get_id()][0] = ++i;
        ++it1;
    }
    out << root1->get_id();
    out << "][0]" << endl;
    forest_dist[root1->get_id()][0] = ++i;

    tree_type::post_order_iterator it2(root2);
    if (!tree_type::is_leaf(root2))
        it2 = tree_type::post_order_iterator(tree_type::leftmost_child(root2));

    out << "init FDist[0][";
    i = 0;
    while (it2 != root2)
    {
        out << it2->get_id() << " ";
        forest_dist[0][it2->get_id()] = ++i;
        ++it2;
    }
    out << root2->get_id();
    out << "]" << endl;
    forest_dist[0][root2->get_id()] = ++i;

    //cout << out.str();
}

size_t gted::index_from_ids(tree_type::iterator from, tree_type::iterator to, bool is_leftmost) const
{
    assert(!from->is_root() &&
            !to->is_root() &&
            from->get_id() != 0 &&
            to->get_id() != 0);

    //cout << is_leftmost << endl;
    size_t out = (is_leftmost || from->get_id() > to->get_id() ? 0 : to->get_id());
    //logger.debug("from %s, to %s", label(from), label(to));
    //logger.debug("index = '%lu'", out);
    return out;
}




void gted::compute_distance_recursive(tree_type::iterator root1, tree_type::iterator root2)
{
    set_logger_priority_to_return_function p(log4cpp::Priority::WARN);
    // TODO: otestovat co to robi pri strategiach != left... 
    APP_DEBUG_FNAME;
    SUBTREE_DEBUG_PRINT(*t1.tree_ptr, root1);
    SUBTREE_DEBUG_PRINT(*t2.tree_ptr, root2);

    //path_strategy strategy = path_strategy::right;
    path_strategy strategy = path_strategy::left;
    //path_strategy strategy = strategies.at(root1->get_id()).at(root2->get_id()).second;
    logger.debug("using strategy: %s", to_string(strategy));
    if (strategy != path_strategy::left)
        logger.warn("TREBA ESTE OTESTOVAT CO TO ROBI!!!"
                "asi treba zmenit prev iteratory, ze nejdu iba -- ktovie");

    decomposition_type decomposition1 = path_decomposition(root1, t1, t1_sizes, strategy);
    decomposition_type decomposition2 = path_decomposition(root2, t2, t2_sizes, strategy);

    // RECURSION:
    //

    for (auto node : decomposition1.subtrees)
        compute_distance_recursive(node, root2);
    for (auto node : decomposition2.subtrees)
        compute_distance_recursive(root1, node);

    // DISTANCE COMPUTING:
    //

#define GTED_VECTOR_DEL_LEFT	0
#define GTED_VECTOR_DEL_RIGHT	1
#define GTED_VECTOR_DEL_BOTH	2

#define COST_DELETE             1
#define COST_MODIFY             0

    distance_table forest_dist;
    init_forest_dist_table(forest_dist, root1, root2);

    typedef tree_type::post_order_iterator post_it;

    post_it left1(tree_type::leftmost_child(root1));
    post_it it1(left1);
    do
    {
        post_it left2(tree_type::leftmost_child(root2));
        post_it it2(left2);
        do
        {
            post_it prev1 = it1;
            post_it prev2 = it2;
            bool is_mostleft1 = (prev1-- == left1);
            bool is_mostleft2 = (prev2-- == left2);

            bool both_lies_on_decomposition_path = 
                node_lies_on_path(it1, decomposition1.path) &&
                node_lies_on_path(it2, decomposition2.path);

            vector<size_t> vec(3, 0xBADF00D);

            size_t index1, index2;

            {
                // del it1 from first tree..
                index1 = index_from_ids(left1, prev1, is_mostleft1);
                index2 = index_from_ids(left2, it2);
                //cout << index1 << " " << index2 << endl;

                vec[GTED_VECTOR_DEL_LEFT] = forest_dist.at(index1).at(index2) + COST_DELETE;
            }

            {
                // del it2 from second tree..
                index1 = index_from_ids(left1, it1);
                index2 = index_from_ids(left2, prev2, is_mostleft2);
                //cout << index1 << " " << index2 << endl;
                
                vec[GTED_VECTOR_DEL_RIGHT] = forest_dist.at(index1).at(index2) + COST_DELETE;
            }

            {
                if (both_lies_on_decomposition_path)
                {
                    // modify it1 ~> it2
                    index1 = index_from_ids(left1, prev1, is_mostleft1);
                    index2 = index_from_ids(left2, prev2, is_mostleft2);
                    //cout << index1 << " " << index2 << endl;

                    vec[GTED_VECTOR_DEL_BOTH] = forest_dist.at(index1).at(index2) + COST_MODIFY;
                }
                else
                {
                    // modify subtree(it1) ~> subtree(it2)
                    prev1 = post_it(tree_type::leftmost_child(it1));
                    prev2 = post_it(tree_type::leftmost_child(it2));
                    is_mostleft1 = (prev1-- == left1);
                    is_mostleft2 = (prev2-- == left2);

                    index1 = index_from_ids(left1, prev1, is_mostleft1);
                    index2 = index_from_ids(left2, prev2, is_mostleft2);
                    //cout << "ELSE:" << endl;
                    //cout << index1 << " " << index2 << endl;
                    
                    vec[GTED_VECTOR_DEL_BOTH] = forest_dist.at(index1).at(index2) + tree_distances.at(it1->get_id()).at(it2->get_id());
                }
            }

            auto c_min_it = min_element(vec.begin(), vec.end());
            size_t index = distance(vec.begin(), c_min_it);
            size_t c_min = vec[index];

            LOGGER_PRINT_CONTAINER(vec, "VEC");

            forest_dist[it1->get_id()][it2->get_id()] = c_min;
            logger.info("Fdist[%s][%s] == %lu", label(it1), label(it2), c_min);
            if (both_lies_on_decomposition_path)
            {
                logger.info("Tdist[%s][%s] == %lu", label(it1), label(it2), c_min);
                tree_distances[it1->get_id()][it2->get_id()] = c_min;
            }
        }
        while (it2++ != root2);
    }
    while (it1++ != root1);

    print_distances();
}



/*
void gted::compute_distance(tree_type::iterator root1, tree_type::iterator root2)
{
#define GTED_VECTOR_DEL_LEFT	0
#define GTED_VECTOR_DEL_RIGHT	1
#define GTED_VECTOR_DEL_BOTH	2

// TODO: dopisat cost_funct()
#define COST_DELETE             1
#define COST_MODIFY             0

    //TODO: skontrolovat co robi funkcia.. 
    //
    APP_DEBUG_FNAME;
    //SUBTREE_DEBUG_PRINT(*t1.tree_ptr, root1);
    //SUBTREE_DEBUG_PRINT(*t2.tree_ptr, root2);

    distance_table forest_dist;
    init_forest_dist_table(forest_dist, tree_type::iterator(root1), tree_type::iterator(root2));
    //return;
    path_strategy strategy = path_strategy::left; //strategies.at(root1->get_id()).at(root2->get_id()).second;

    decomposition_type decomposition1 = path_decomposition(root1, t1, t1_sizes, strategy);
    decomposition_type decomposition2 = path_decomposition(root2, t2, t2_sizes, strategy);

    tree_type::post_order_iterator left1(tree_type::leftmost_child(root1));
    tree_type::post_order_iterator left2(tree_type::leftmost_child(root2));

    // na do-while cyklus:
    tree_type::post_order_iterator it1 = left1;
    tree_type::post_order_iterator it2 = left2;

    do
    //for (tree_type::post_order_iterator it1 = left1; it1 != root1; ++it1)
    {
        do
        //for (tree_type::post_order_iterator it2 = left2; it2 != root2; ++it2)
        {
            // su len 2 rozdiely vo vetvach if(both_lies_on_decomposition_path) {} else {}
            // a to pricitanie pri delete_both_nodes a v ulozeni vypocitanej hodnoty do tree_dist..
            // => vetvy sa daju rovno zlucit..

            logger.debug("NODES: '%s' '%s'", label(it1), label(it2));
            logger.debug("NODES_ID: %lu %lu", it1->get_id(), it2->get_id());

            vector<size_t> vec(3, 0xBADF00D);
            tree_type::post_order_iterator prev1 = it1;
            tree_type::post_order_iterator prev2 = it2;
            bool is_mostleft1 = false;
            bool is_mostleft2 = false;
            bool both_lies_on_decomposition_path =
                    node_lies_on_path(it1, decomposition1.path) &&
                    node_lies_on_path(it2, decomposition2.path);

            cout << boolalpha << "both_lies_on_decomposition_path = " << both_lies_on_decomposition_path << endl;
            // pri --prev mozem vypadnut zo stromu... 
            if (prev1 != left1)
                --prev1;
            else
                is_mostleft1 = true;
            if (prev2 != left2)
                --prev2;
            else
                is_mostleft2 = true;

            // TODO: pri pocitani vzdialenosti lesa (xz) zo stromu (x-y-z) dava zle vysledky!
            cout << label(left1) << " " << label(prev1) << " " << is_mostleft1 << endl;
            cout << label(left2) << " " << label(prev2) << " " << is_mostleft2 << endl;

            size_t index1, index2;

            // TODO:
            // POZOR!!! raz volam funkcie s is_mostleft a raz bez! => niekedy je tam implicitne false...
            // .. to preto, ze raz sa snazim dany vrchol zmazat (volam s prev) a inokedy ho tam nechavam (volam s it)

            {
                cout << "A" << endl;
                // ==forest_dist[F1 - it1][F2] + cost(delete)
                index1 = index_from_ids(left1, prev1, is_mostleft1);
                index2 = index_from_ids(left2, it2);
                cout << index1 << " " << index2 << endl;
                vec[GTED_VECTOR_DEL_LEFT]   = forest_dist.at(index1).at(index2) + COST_DELETE;
            }

            {
                cout << "A" << endl;
                // == forest_dist[F1][F2 - it2] + cost(delete)
                index1 = index_from_ids(left1, it1);
                index2 = index_from_ids(left2, prev2, is_mostleft2);
                cout << index1 << " " << index2 << endl;
                vec[GTED_VECTOR_DEL_RIGHT]  = forest_dist.at(index1).at(index2) + COST_DELETE;
            }

            {
                cout << "A" << endl;
                if (both_lies_on_decomposition_path)
                {
                    // == forest_dist[F1 - it1][F2 - it2] + cost(modify)
                    index1 = index_from_ids(left1, prev1, is_mostleft1);
                    index2 = index_from_ids(left2, prev2, is_mostleft2);
                    cout << index1 << " " << index2 << endl;
                    vec[GTED_VECTOR_DEL_BOTH] = forest_dist.at(index1).at(index2) + COST_MODIFY;
                }
                else
                {
                    tree_type::post_order_iterator prevleft1(tree_type::leftmost_child(it1));
                    tree_type::post_order_iterator prevleft2(tree_type::leftmost_child(it2));
                    bool mostleft1 = false;
                    bool mostleft2 = false;
                    if (prevleft1-- == left1)
                        mostleft1 = true;
                    if (prevleft2-- == left2)
                        mostleft2 = true;

                    index1 = index_from_ids(left1, prevleft1, mostleft1);
                    index2 = index_from_ids(left2, prevleft2, mostleft2);

                    //tree_distances[it1->get_id()][it2->get_id()] = 0;
                    //if (it1->get_label() == "3" && it2->get_label() == "22")
                        //tree_distances[it1->get_id()][it2->get_id()] = 1;

                    cout << "FDist = " << forest_dist.at(index1).at(index2) << endl;
                    cout << "TDist = " << tree_distances.at(it1->get_id()).at(it2->get_id()) << endl;
                    vec[GTED_VECTOR_DEL_BOTH] = forest_dist.at(index1).at(index2) +
                                tree_distances.at(it1->get_id()).at(it2->get_id());
                }
            }


            auto c_min_it = min_element(vec.begin(), vec.end());
            size_t index =  distance(vec.begin(), c_min_it);
            size_t c_min =  vec[index];

            cout << "VEC: ";
            for (auto value : vec)
                cout << value << " ";
            cout << endl;
            //logger.debug("minimal index == %lu, c_min == %lu", index, c_min);

            forest_dist[it1->get_id()][it2->get_id()] = c_min;
            if (both_lies_on_decomposition_path)
            {
                logger.debug("Tree_distance[%s][%s] == %lu", label(it1), label(it2), c_min);
                tree_distances[it1->get_id()][it2->get_id()] = c_min;
            }

            logger.debug("Forrest_distance[%s][%s] == %lu", label(it1), label(it2), forest_dist.at(it1->get_id()).at(it2->get_id()));
            cout << endl;
        }
        while (it2 != root2);
    }
    while (it1 != root1);
}

*/
/*
void gted::compute_distance_recursive(tree_type::iterator root1, tree_type::iterator root2)
{
    APP_DEBUG_FNAME;
    SUBTREE_DEBUG_PRINT(*t1.tree_ptr, root1);
    SUBTREE_DEBUG_PRINT(*t2.tree_ptr, root2);

    path_strategy strategy = path_strategy::left;
    //logger.info("using decomposition strategy %s", to_string(strategy));

    decomposition_type decomposition1 = path_decomposition(root1, t1, t1_sizes, strategy);
    decomposition_type decomposition2 = path_decomposition(root2, t2, t2_sizes, strategy);

    cout << "decomposition1: ";
    for (auto node : decomposition1.subtrees)
        cout << *node << " ";
    cout << endl;
    cout << "decomposition2: ";
    for (auto node : decomposition2.subtrees)
        cout << *node << " ";
    cout << endl;

    for (auto node : decomposition1.subtrees)
        compute_distance_recursive(node, root2);
    for (auto node : decomposition2.subtrees)
        compute_distance_recursive(root1, node);

    if ("3" != root1->get_label())
        return;

    // vyjdem z rekurzie, tree_distances su vypocitane pre vsetky podstromy..
    compute_distance(root1, root2);
}
*/












