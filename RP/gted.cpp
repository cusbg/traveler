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
#include <iomanip>

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
    for (auto it2 = t2.begin_post(); it2 != t2.end_post(); ++it2)
        inner[id(it2)] = 0xDEAD;

    for (auto it1 = t1.begin_post(); it1 != t1.end_post(); ++it1)
        tree_distances[id(it1)] = inner;
}

gted::decomposition_type gted::path_decomposition(tree_type::iterator it,
        const tree_type& t, const rted::map_type& t_size, path_strategy s) const
{
    set_logger_priority_to_return_function l(log4cpp::Priority::INFO);

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
    p.t1.path_node = p.t1.left = p.t1.right = t1.begin_post_reverse();
    p.t2.path_node = p.t2.left = p.t2.right = t2.begin_post_reverse();

    compute_forest_distances_recursive(p.t1.root, p.t2.root);

    print_distances();
}

void gted::print_FDist(const forest_distance_table& table) const
{
    for (auto val : table)
    {
        cout << "\t\t[" << label(val.first.left) << "," << label(val.first.right) << "]" << endl;
        for (auto v2 : val.second)
        {
            cout << setw(5);
            cout << "[" << label(v2.first.left) << "," << label(v2.first.right) << "]\t";
            cout << v2.second << "\t";
            cout << endl;
        }
        cout << endl;
    }
}


void gted::compute_forest_distances_recursive(
        tree_type::iterator root1, tree_type::iterator root2)
{
    APP_DEBUG_FNAME;

    decomposition_type decomposition;

    decomposition = path_decomposition(root1, t1, t1_sizes, path_strategy::left);
    for (auto val : decomposition.subtrees)
        compute_forest_distances_recursive(val, root2);
    
    decomposition = path_decomposition(root2, t2, t2_sizes, path_strategy::left);
    for (auto val : decomposition.subtrees)
        compute_forest_distances_recursive(root1, val);

    forest_distance_table forest_dist;
    subtree_pairs p;
    p.t1.root = root1;
    p.t2.root = root2;
    p.t1.right = p.t1.left = p.t1.path_node = tree_type::leftmost_child(root1);
    p.t2.right = p.t2.left = p.t2.path_node = tree_type::leftmost_child(root2);

    init_forest_dist_table(forest_dist, p);
    compute_forest_distances(forest_dist, p);

    if (p.t1.root->is_root() && p.t2.root->is_root())
    {
        print_FDist(forest_dist);
    }

}





void gted::init_forest_dist_table(forest_distance_table& forest_dist,
        subtree_pairs forests) const
{
    // TODO: prerobit len na klasicke iterovanie a zvysovanie hodnoty
    // int i = 0; forest[][] = i++;
    APP_DEBUG_FNAME;
    SUBTREE_DEBUG_PRINT(*t1.tree_ptr, forests.t1.root);
    SUBTREE_DEBUG_PRINT(*t2.tree_ptr, forests.t2.root);
    DEBUG("INIT BEGIN");

    // pairs by malo v .begin, .end ukazovat na list na konci cesty...
    //
    assert (tree_type::is_leaf(forests.t1.left) && 
            tree_type::is_leaf(forests.t2.left) &&
            forests.t1.left == forests.t1.right && 
            forests.t2.left == forests.t2.right &&
            forests.t1.path_node == forests.t1.left &&
            forests.t2.path_node == forests.t2.left);

    iterator_pairs parents, leafs;
    subtree prev1, prev2;

    // prev*.left zostava neinicializovane, aby boli prev* neplatne
    // z dovodu hashovania.. ->hash(prev1) = hash(prev2) = 0
    prev1.right = leafs.it1 = forests.t1.right;
    prev2.right = leafs.it2 = forests.t2.right;
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
    // moze sa stat, ze nazaciatku parent.it1 == root1..
    forest_dist[forests.t1][prev2] = forest_dist.at(prev1).at(prev2) + 1;

    GTED_OUTPUT_STRING(forests.t1, prev2);
    while(forests.t1.right != forests.t1.root)
    {
        // mazem vrcholy v T1, 2.index ma byt neplatny... 
        prev1.left = forests.t1.left;
        ++forests.t1.left;
        while(forests.t1.left != parents.it1)
        {
            forest_dist[forests.t1][prev2] = forest_dist.at(prev1).at(prev2) + 1;

            GTED_OUTPUT_STRING(forests.t1, prev2);

            prev1.left = forests.t1.left;
            ++forests.t1.left;
        }

        // musim pouzivat predchadzajuci left iterator.. v otcovi oficialne este niesom... 
        forests.t1.left = prev1.left;
        prev1.right = forests.t1.right;
        ++forests.t1.right;
        while(forests.t1.right != parents.it1)
        {
            forest_dist[forests.t1][prev2] = forest_dist.at(prev1).at(prev2) + 1;

            GTED_OUTPUT_STRING(forests.t1, prev2);
            
            prev1.right = forests.t1.right;
            ++forests.t1.right;
        }
        forests.t1.left = forests.t1.right;
        // plati: forests.t1.left == forests.t1.right == parents.it1;
        forest_dist[forests.t1][prev2] = forest_dist.at(prev1).at(prev2) + 1;

        GTED_OUTPUT_STRING(forests.t1, prev2);
        
        parents.it1 = tree_type::parent(parents.it1);
    }

    // zneplatni prev1...
    // + nastav *.right na povodnu hodnotu, aby sa zahashovali rovnako + operator== vratilo true
    prev1.right = leafs.it1;
    prev2.right = leafs.it2;
    prev1.left = tree_type::iterator();
    prev2.left = tree_type::iterator();
    // forests.t1 som posuval.. nastav tiez na default zozaciatku..
    forests.t1.right = forests.t1.left = leafs.it1;

    cout << "SECOND INIT" << endl;
    // preistotu, ak nazaciatku parent.it2 == root2..
    forest_dist[prev1][forests.t2] = forest_dist.at(prev1).at(prev2) + 1;
    GTED_OUTPUT_STRING(prev1, forests.t2);
    while(forests.t2.right != forests.t2.root)
    {
        // mazem vrcholy v T2, 1.index ma byt neplatny...
        prev2.left = forests.t2.left;
        ++forests.t2.left;
        while(forests.t2.left != parents.it2)
        {
            forest_dist[prev1][forests.t2] = forest_dist.at(prev1).at(prev2) + 1;

            GTED_OUTPUT_STRING(prev1, forests.t2);

            prev2.left = forests.t2.left;
            ++forests.t2.left;
        }

        // potrebujem pouzivat predchadzajuci left iterator.. do otca sa dostanem az ked tam dojde .right..
        forests.t2.left = prev2.left;
        prev2.right = forests.t2.right;
        ++forests.t2.right;
        while(forests.t2.right != parents.it2)
        {
            forest_dist[prev1][forests.t2] = forest_dist.at(prev1).at(prev2) + 1;

            GTED_OUTPUT_STRING(prev1, forests.t2);

            prev2.right = forests.t2.right;
            ++forests.t2.right;
        }
        forests.t2.left = forests.t2.right;
        // plati: forests.t2.left == forests.t2.right == parents.it2;
        forest_dist[prev1][forests.t2] = forest_dist.at(prev1).at(prev2) + 1;

        GTED_OUTPUT_STRING(prev1, forests.t2);

        parents.it2 = tree_type::parent(parents.it2);
    }

    DEBUG("INIT END");
    DEBUG("");
}


void gted::compute_forest_distances(forest_distance_table& forest_dist,
        subtree_pairs forests)
{
    APP_DEBUG_FNAME;

    assert (tree_type::is_leaf(forests.t1.left) && 
            tree_type::is_leaf(forests.t2.left) &&
            forests.t1.left == forests.t1.right && 
            forests.t2.left == forests.t2.right &&
            forests.t1.path_node == forests.t1.left &&
            forests.t2.path_node == forests.t2.left);

    iterator_pairs parents, leafs;
    subtree_pairs prevs = forests;

    // zneplati prev* iteratory..
    prevs.t1.left = tree_type::reverse_post_order_iterator();
    prevs.t2.left = tree_type::reverse_post_order_iterator();

    leafs.it1 = forests.t1.right;
    leafs.it2 = forests.t2.right;
    parents.it1 = tree_type::parent(leafs.it1);
    parents.it2 = tree_type::parent(leafs.it2);



    bool end_loop = false;
    do
    {
        if (forests.t2.root == forests.t2.right)
            end_loop = true;

        // zresetuj T1 strom
        forests.t1.path_node = forests.t1.left = forests.t1.right = leafs.it1;
        prevs.t1.right = leafs.it1;
        prevs.t1.left = tree_type::iterator();
        parents.it1 = tree_type::parent(leafs.it1);

        // vyrataj aktualny vrchol..
        fill_tables(forest_dist, forests, prevs);
        while(forests.t1.root != forests.t1.right)
        {
            // pridavam najprv vsetky lave uzly
            //DEBUG("a");
            prevs.t1.left = forests.t1.left;
            ++forests.t1.left;
            while(forests.t1.left != parents.it1)
            {
                fill_tables(forest_dist, forests, prevs);

                prevs.t1.left = forests.t1.left;
                ++forests.t1.left;
            }

            //DEBUG("b");
            // som v otcovi, ale indexovat musim este s predchadzajucim vrcholom... => vrat sa o jeden krok spat.
            forests.t1.left = prevs.t1.left;
            // posun sa o jedno doprava..
            prevs.t1.right = forests.t1.right;
            ++forests.t1.right;

            // pridavam vsetky prave uzly
            while(forests.t1.right != parents.it1)
            {
                fill_tables(forest_dist, forests, prevs);

                prevs.t1.right = forests.t1.right;
                ++forests.t1.right;
            }
            //DEBUG("c");
            forests.t1.left = forests.t1.right;
            // a posun sa o 1 patro vyssie..
            parents.it1 = tree_type::parent(parents.it1);
            // nastav aktualny vrchol, ze je leziaci na PATH.
            forests.t1.path_node = forests.t1.right;

            // a dopocitaj dany vrchol (parent node z minula)
            fill_tables(forest_dist, forests, prevs);
        }

        // teraz uprav T2.
        prevs.t2.left = forests.t2.left;
        ++forests.t2.left;

        if (forests.t2.left == parents.it2)
        {
            DEBUG("t2.left == parent");
            // vrat t2.left na predchadzajuci vrchol a zvys t2.right
            // to predchadzajuce ++ a nasledne vratenie vrcholu na prev nestoji vela, kedze prev je lavy syn left.
            forests.t2.left = prevs.t2.left;

            prevs.t2.right = forests.t2.right;
            ++forests.t2.right;
            if (forests.t2.right == parents.it2)
            {
                DEBUG("t2.right == parent; parent<-parent(parent)");
                forests.t2.left = forests.t2.right;
                parents.it2 = tree_type::parent(parents.it2);
                // nastav este path_node.. (uz sa do podstromu nikdy nevratim..)
                forests.t2.path_node = forests.t2.right;
            }
        }
        
        // a skonci az potom co som uz presiel jeden loop z T2.root
    }
    while(!end_loop);
}

void gted::fill_tables(forest_distance_table& forest_dist,
        const subtree_pairs& forests, const subtree_pairs& prevs)
{
    //APP_DEBUG_FNAME;

#define GTED_OUTPUT_F_IDs(First1, First2) \
    DEBUG("[%s, %s]; [%s, %s]; [%s, %s]; [%s, %s]", \
            label(First1.left), label(First1.right), \
            label(First2.left), label(First2.right), \
            label(prevs.t1.left), label(prevs.t1.right), \
            label(prevs.t2.left), label(prevs.t2.right))

    GTED_OUTPUT_F_IDs(forests.t1, forests.t2);

#define GTED_VECTOR_DELETE_LEFT     0
#define GTED_VECTOR_DELETE_RIGHT    1
#define GTED_VECTOR_DELETE_BOTH     2

#define GTED_COST_MODIFY            0
#define GTED_COST_DELETE            1

    // pocitam forest_dist[forests.t1][forests.t2]

    vector<size_t> vec(3, 0xBADF00D);

    //DEBUG("a");
    vec[GTED_VECTOR_DELETE_LEFT] =
        forest_dist.at(prevs.t1).at(forests.t2) + GTED_COST_DELETE;
    //DEBUG("b");
    vec[GTED_VECTOR_DELETE_RIGHT] =
        forest_dist.at(forests.t1).at(prevs.t2) + GTED_COST_DELETE;
    //DEBUG("c");
    vec[GTED_VECTOR_DELETE_BOTH] =
        forest_dist.at(prevs.t1).at(prevs.t2);   // + bud COST_MODIFY, alebo tree_dist[X,Y]
        // TODO: ^^ toto nieje korektne.
        // Ak niektory les nieje strom, tak dovolujem pouzit niektore vrcholy viackrat.
        // musim odstranit vsetky vrcholy v podstrome kde prave stojim. 
        // Pr: vzdialenost [a,22] <-> [2,2] da v 3. bode 3ku, aj ked by mala hodit 2.

    //LOGGER_PRINT_CONTAINER(vec, "vec: ");

    auto is_path_node_funct = [](const subtree& p){
        return p.path_node == p.left && p.path_node == p.right;
    };

    if (is_path_node_funct(forests.t1) &&
            is_path_node_funct(forests.t2))
    {// pridavam iba COST_MODIFY
        DEBUG("is_path_node_funct(t1) && is_path_node_funct(t2)");
        vec[GTED_VECTOR_DELETE_BOTH] += GTED_COST_MODIFY;
    }
    else
    {// inac musim najst ktore X,Y pouzit v tree_dist_table..
        // pouzivam hodnotu vrcholu, ktory nieje na path_node..

        size_t id1, id2;

        if (is_path_node_funct(forests.t1))
        {
            //DEBUG("id1, is_path_node_funct(t1)");
            id1 = id(forests.t1.right);
        }
        else if (forests.t1.right == forests.t1.path_node)
        {
            //DEBUG("id1, right==path_node");
            id1 = id(forests.t1.left);
        }
        else
        {
            //DEBUG("id1, else");
            id1 = id(forests.t1.right);
        }

        if (is_path_node_funct(forests.t2))
        {
            //DEBUG("id2, is_path_node_funct(t2)");
            id2 = id(forests.t2.right);
        }
        else if (forests.t2.right == forests.t2.path_node)
        {
            //DEBUG("id2, right==path_node");
            id2 = id(forests.t2.left);
        }
        else
        {
            //DEBUG("id2, else");
            id2 = id(forests.t2.right);
        }

        //DEBUG("T[%i][%i] == %i", id1, id2, tree_distances.at(id1).at(id2));
        vec[GTED_VECTOR_DELETE_BOTH] +=
            tree_distances.at(id1).at(id2);
    }
    
    auto c_min_it = std::min_element(vec.begin(), vec.end());
    size_t index = distance(vec.begin(), c_min_it);
    size_t c_min = vec.at(index);

    LOGGER_PRINT_CONTAINER(vec, "vec: ");

    forest_dist[forests.t1][forests.t2] = c_min;

    if (is_path_node_funct(forests.t1) &&
            is_path_node_funct(forests.t2))
    {
        tree_distances[id(forests.t1.right)][id(forests.t2.right)] = c_min;
    }
}





