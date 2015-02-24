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


bool gted::subtree::operator==(const gted::subtree& other) const
{
    return left == other.left &&
        right == other.right;
}

size_t gted::subtree::hash::operator()(const gted::subtree& s) const
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);
    // zdroj hash funkcie:
    // http://stackoverflow.com/questions/682438/hash-function-providing-unique-uint-from-an-integer-coordinate-pair
    
    //assert(s.begin.node != NULL && s.end.node != NULL);
    //assert(s.begin->get_id() <= s.end->get_id());

    int out;
    if (s.left.node == NULL || s.right.node == NULL)
        out = 0;
    else if (s.left == s.right)
        out = s.right->get_id();
    else
        out = (s.left->get_id() * 0x1F1F1F1F) ^ s.right->get_id();

    if (s.right == NULL)
        logger.warn("right iterator is NULL..!!");

    static vector<subtree> hashes;
    if (find(hashes.begin(), hashes.end(), s) == hashes.end())
    {
        hashes.push_back(s);
        DEBUG("HASH [%s, %s] = %i", label(s.left), label(s.right), out);
    }
    return out;
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

bool gted::node_lies_on_path(tree_type::iterator it, const path_type& path) const
{
    return find(path.begin(), path.end(), it) != path.end();
    //return path.find(it->get_id()) != path.end();
}

size_t gted::biggest_subtree_child(tree_type::iterator root,
            const tree_type& t, const rted::map_type& t_size) const
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
    //logger.debug("child no. '%lu' of node '%s' has biggest subtree (size == %lu)",
            //index, label(root), biggest_subtree);
    return index;
}

void gted::init_tree_dist_table()
{
    rted::map_type inner;
    for (auto it2 = t2.tree_ptr->begin_post(); it2 != t2.tree_ptr->end_post(); ++it2)
        inner[it2->get_id()] = 0xDEAD;

    for (auto it1 = t1.tree_ptr->begin_post(); it1 != t1.tree_ptr->end_post(); ++it1)
    {
        for (auto it2 = t2.tree_ptr->begin_post(); it2 != t2.tree_ptr->end_post(); ++it2)
            tree_distances[it1->get_id()][it2->get_id()] = 0xDEAD;
    }
}

gted::decomposition_type gted::path_decomposition(tree_type::iterator it,
        const tree_type& t, const rted::map_type& t_size, path_strategy s) const
{
    //set_logger_priority_to_return_function l(log4cpp::Priority::INFO);

    APP_DEBUG_FNAME;

    SUBTREE_DEBUG_PRINT(*t.tree_ptr, it);

    if (s != path_strategy::left &&
            s != path_strategy::right &&
            s != path_strategy::heavy)
    {
        logger.error("only LRH strategies are supported");
        exit(1);
    }

    size_t index;
    size_t i;
    decomposition_type decomposition;

    while(!tree_type::is_leaf(it))
    {
        logger.debug("going down the path node '%s'", label(it));

        if (s == path_strategy::left)
            index = 0;
        if (s == path_strategy::right)
            index = it.number_of_children() - 1;
        if (s == path_strategy::heavy)
            index = biggest_subtree_child(it, t, t_size);

        decomposition.path.push_back(it);
        //decomposition.path[it->get_id()] = it;
        tree_type::sibling_iterator sib(tree_type::first_child(it));
        i = 0;

        while (sib != sib.end())
        {
            if (i != index)
            {
                logger.debug("adding '%s' to relevat subtrees", label(sib));

                decomposition.subtrees.push_back(sib);
            }
            else
            {
                logger.debug("jumping over '%s', node lie on the path", label(sib));
            }
            ++i;
            ++sib;
        }
        it = tree_type::child(it, index);
    }
    return decomposition;
}

gted::gted(const tree_type& _t1, const tree_type& _t2)
    : t1(_t1), t2(_t2)
{
    APP_DEBUG_FNAME;
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    rted r(t1, t2);
    r.run_rted();
    strategies = r.get_strategies();
    t1_sizes = r.get_t1_sizes();
    t2_sizes = r.get_t2_sizes();
    init_tree_dist_table();
}

void gted::run_gted()
{
    path_decomposition(t1.tree_ptr->begin(), t1, t1_sizes, path_strategy::heavy);
    forest_distance_table f;
    subtree_pairs p;
    p.t1.root = t1.begin();
    p.t2.root = t2.begin();
    p.t1.left = p.t1.right = t1.begin_post_reverse();
    p.t2.left = p.t2.right = t2.begin_post_reverse();

    cout << *p.t1.left << "  " << *p.t2.left << endl;
    init_forest_dist_table(f, p);
}







void gted::init_forest_dist_table(forest_distance_table& forest_dist,
        subtree_pairs pairs) const
{
    APP_DEBUG_FNAME;

    // pairs by malo v .begin, .end ukazovat na list na konci cesty...
    //
    assert (tree_type::is_leaf(pairs.t1.left) && 
            tree_type::is_leaf(pairs.t2.left) &&
            pairs.t1.left == pairs.t1.right && 
            pairs.t2.left == pairs.t2.right);

    /*
    struct
    {
        tree_type::iterator it1;
        tree_type::iterator it2;
    } parents, leafs;
*/
    iterator_pairs parents, leafs;

    subtree prev1, prev2;

    // prev*.left zostava neinicializovane, aby boli prev* neplatne
    // z dovodu hashovania..
    prev1.right = leafs.it1 = pairs.t1.right;
    prev2.right = leafs.it2 = pairs.t2.right;
    parents.it1 = tree_type::parent(leafs.it1);
    parents.it2 = tree_type::parent(leafs.it2);

    assert(forest_dist.hash_function()(prev1) == 0 &&
            forest_dist.hash_function()(prev2) == 0);

    forest_dist[prev1][prev2] = 0;

#define GTED_OUTPUT_STRING(First1, First2) \
    DEBUG("F[%s, %s][%s, %s] == F[%s, %s][%s, %s] + 1 == %lu", \
            label(First1.left), label(First1.right), \
            label(First2.left), label(First2.right), \
            label(prev1.left), label(prev1.right), \
            label(prev2.left), label(prev2.right), \
            forest_dist.at(First1).at(First2))


    DEBUG("F[%s, %s][%s, %s]==\t%lu",
            label(prev1.left), label(prev1.right),
            label(prev2.left), label(prev2.right),
            forest_dist.at(prev1).at(prev2));

    cout << "FIRST INIT" << endl;
    while(parents.it1 != pairs.t1.root)
    {
        // mazem vrcholy v T1, 2.index ma byt neplatny... 
        while(pairs.t1.left != parents.it1)
        {
            forest_dist[pairs.t1][prev2] = forest_dist.at(prev1).at(prev2) + 1;

            GTED_OUTPUT_STRING(pairs.t1, prev2);

            prev1.left = pairs.t1.left;
            ++pairs.t1.left;
        }

        // musim pouzivat predchadzajuci left iterator.. v otcovi oficialne este niesom... 
        pairs.t1.left = prev1.left;
        prev1.right = pairs.t1.right;
        ++pairs.t1.right;
        while(pairs.t1.right != parents.it1)
        {
            forest_dist[pairs.t1][prev2] = forest_dist.at(prev1).at(prev2) + 1;

            GTED_OUTPUT_STRING(pairs.t1, prev2);
            
            prev1.right = pairs.t1.right;
            ++pairs.t1.right;
        }
        pairs.t1.left = pairs.t1.right;
        // pairs.t1.left == pairs.t1.right == parents.it1;
        forest_dist[pairs.t1][prev2] = forest_dist.at(prev1).at(prev2) + 1;

        GTED_OUTPUT_STRING(pairs.t1, prev2);
        
        parents.it1 = tree_type::parent(parents.it1);
    }
    // moze sa stat, ze nazaciatku parent.it1 == root1..
    forest_dist[pairs.t1][prev2] = forest_dist.at(prev1).at(prev2) + 1;

    GTED_OUTPUT_STRING(pairs.t1, prev2);

    // zneplatni prev1...
    // + nastav *.right na povodnu hodnotu, aby sa zahashovali rovnako + operator== vratilo true
    prev1.right = leafs.it1;
    prev2.right = leafs.it2;
    prev1.left = tree_type::iterator();
    prev2.left = tree_type::iterator();
    // pairs.t1 som posuval.. nastav tiez na default zozaciatku..
    pairs.t1.right = pairs.t1.left = leafs.it1;

    cout << "SECOND INIT" << endl;
    while(parents.it2 != pairs.t2.root)
    {
        // mazem vrcholy v T2, 1.index ma byt neplatny...
        while(pairs.t2.left != parents.it2)
        {
            forest_dist[prev1][pairs.t2] = forest_dist.at(prev1).at(prev2) + 1;

            GTED_OUTPUT_STRING(prev1, pairs.t2);

            prev2.left = pairs.t2.left;
            ++pairs.t2.left;
        }

        // potrebujem pouzivat predchadzajuci left iterator.. do otca sa dostanem az ked tam dojde .right..
        pairs.t2.left = prev2.left;
        prev2.right = pairs.t2.right;
        ++pairs.t2.right;
        while(pairs.t2.right != parents.it2)
        {
            forest_dist[prev1][pairs.t2] = forest_dist.at(prev1).at(prev2) + 1;

            GTED_OUTPUT_STRING(prev1, pairs.t2);

            prev2.right = pairs.t2.right;
            ++pairs.t2.right;
        }
        pairs.t2.left = pairs.t2.right;
        // pairs.t1.left == pairs.t1.right == parents.it1;
        forest_dist[prev1][pairs.t2] = forest_dist.at(prev1).at(prev2) + 1;

        GTED_OUTPUT_STRING(prev1, pairs.t2);

        parents.it2 = tree_type::parent(parents.it2);
    }
    // preistotu, ak nazaciatku parent.it2 == root2..
    forest_dist[prev1][pairs.t2] = forest_dist.at(prev1).at(prev2) + 1;
    GTED_OUTPUT_STRING(prev1, pairs.t2);
}

void gted::compute_forrest_distances(forest_distance_table& table,
        subtree_pairs p)
{
    APP_DEBUG_FNAME;

    /*struct
    {
        tree_type::iterator it1;
        tree_type::iterator it2;
    } parents, leafs;*/
    iterator_pairs parents, leafs;
    subtree prev1, prev2;
}


/*



void gted::run_gted()
{
    APP_DEBUG_FNAME;
    logger.debug("GTED: BEGIN");

    auto root1 = ++t1.tree_ptr->begin();
    auto root2 = ++t2.tree_ptr->begin();

    SUBTREE_DEBUG_PRINT(*t1.tree_ptr, root1);
    SUBTREE_DEBUG_PRINT(*t2.tree_ptr, root2);

    //path_decomposition(root1, t1, t1_sizes, path_strategy::right);
    //right_decomposition(root1);

    //path_decomposition(root1, t1, t1_sizes, path_strategy::heavy);
    //heavy_decomposition(root1, t1, t1_sizes);
    //compute_distance(t1.tree_ptr->begin(), t2.tree_ptr->begin());
 
    //compute_distance_recursive(root1, root2);

    //print_distances();

    logger.debug("GTED: END");
}





[>
void gted::compute_distance_recursive(tree_type::iterator root1, tree_type::iterator root2)
{
    // TODO FIX: mozno by stacilo len dopisat 3 funkcie na upravu iteratorov prevX, pri L_strategy sa urobi to co momentalne
    // pri R/H sa urobi nejaka ina operacia.. 
    // + bude treba este aj tie booleany zistit co znamenaju pri inych strategiach..
    //
    //set_logger_priority_to_return_function p(log4cpp::Priority::WARN);
    //
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
    
    auto s = strategies.at(root1->get_id()).at(root2->get_id());
    strategy = s.second;
    auto g = s.first;

    decomposition_type decomposition = (g == graph::T1 ?
            path_decomposition(root1, t1, t1_sizes, strategy) :
            path_decomposition(root2, t2, t2_sizes, strategy));

    if (g == graph::T1)
        for (auto node : decomposition.subtrees)
            compute_distance(node, root2);
    else
        for (auto node : decomposition.subtrees)
            compute_distance(root1, node);
[>
    decomposition_type decomposition1 = path_decomposition(root1, t1, t1_sizes, strategy);
    decomposition_type decomposition2 = path_decomposition(root2, t2, t2_sizes, strategy);

    /// RECURSION:
    //

    for (auto node : decomposition1.subtrees)
        compute_distance_recursive(node, root2);
    for (auto node : decomposition2.subtrees)
        compute_distance_recursive(root1, node);
<]

    // DISTANCE COMPUTING:
    //

#define GTED_VECTOR_DEL_LEFT	0
#define GTED_VECTOR_DEL_RIGHT	1
#define GTED_VECTOR_DEL_BOTH	2

#define COST_DELETE             1
#define COST_MODIFY             0

    //left(root1, root2, decomposition1, decomposition2);
    //right(root1, root2, decomposition1, decomposition2);


    distance_table forest_dist;
    init_forest_dist_table(forest_dist, root1, root2);

    typedef tree_type::post_order_iterator post_it;

    post_it left1(tree_type::leftmost_child(root1));
    post_it right1(tree_type::rightmost_child(root1));
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
<]
[>
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
<]











*/


