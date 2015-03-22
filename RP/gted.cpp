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
#include <algorithm>
#include <iomanip>

using namespace std;

// TODO nazaciatku vsetkeho skontrolovat ci
// strategies.graph su z {T1, T2}
// strategies.path_strategy z {L, R, H}

bool gted::subforest::operator==(const gted::subforest& other) const
{
    return left == other.left &&
        right == other.right;
}

size_t gted::subforest::hash::operator()(const gted::subforest& s) const
{
    // TODO:
    //return 0;

    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    // zdroj hash funkcie:
    // http://stackoverflow.com/questions/682438/hash-function-providing-unique-uint-from-an-integer-coordinate-pair
    assert(s.right.node != NULL);

    int out;
    if (s.left.node == NULL)
        out = 0;
    else if (s.left == s.right)
        out = s.right->get_id();
    else
        out = (s.left->get_id() * 0x1F1F1F1F) ^ s.right->get_id();

    if (s.right == NULL)
        logger.warn("right iterator is NULL..!!");

    static vector<subforest> hashes;
    if (find(hashes.begin(), hashes.end(), s) == hashes.end())
    {
        hashes.push_back(s);
        DEBUG("HASH [%s, %s] = %i", label(s.left), label(s.right), out);
    }
    return out;
}

void gted::precompute_heavy_paths()
{
    APP_DEBUG_FNAME;

    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    tree_type::post_order_iterator it;
    tree_type::sibling_iterator child;
    int i;

    it = t1.begin_post();
    while (it != t1.end_post())
    {
        if (tree_type::is_leaf(it))
        {
            DEBUG("is leaf %s", label(it));
            heavy_paths.T1_heavy[id(it)] = it;
        }
        else
        {
            i = biggest_subtree_child(it, t1, t1_sizes);
            child = it.begin();
            while(i--)
                ++child;
            DEBUG("heavy child %s", label(child));
            heavy_paths.T1_heavy[id(it)] = heavy_paths.T1_heavy.at(id(child));
        }
        ++it;
    }
    it = t2.begin_post();
    while(it != t2.end_post())
    {
        if (tree_type::is_leaf(it))
        {
            DEBUG("is leaf %s", label(it));
            heavy_paths.T2_heavy[id(it)] = it;
        }
        else
        {
            i = biggest_subtree_child(it, t2, t2_sizes);
            child = it.begin();
            while(i--)
                ++child;
            DEBUG("heavy child %s", label(child));
            heavy_paths.T2_heavy[id(it)] = heavy_paths.T2_heavy.at(id(child));
        }
        ++it;
    }
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
}

void gted::pretty_printT(const subforest& index1,
                    const subforest& index2,
                    graph who_first) const
{
    if (who_first == T1)
        DEBUG("[%s, %s];\t[%s, %s]",
            label(index1.left), label(index1.right),
            label(index2.left), label(index2.right));
    else
        DEBUG("[%s, %s];\t[%s, %s]",
            label(index2.left), label(index2.right),
            label(index1.left), label(index1.right));
}

void gted::print_TDist() const
{
    APP_DEBUG_FNAME;

    int odsadenie = 22;
    cout << "TREE_DISTANCE_TABLE:" << endl;
    cout << setw(odsadenie) << "";
    for (auto it2 = t2.tree_ptr->begin_post(); it2 != --t2.tree_ptr->end_post(); ++it2)
        cout << setw(odsadenie) << std::left << *it2;
    cout << endl;
    for (auto it1 = t1.tree_ptr->begin_post(); it1 != --t1.tree_ptr->end_post(); ++it1)
    {
        cout << setw(odsadenie) << std::left << *it1;
        for (auto it2 = t2.tree_ptr->begin_post(); it2 != --t2.tree_ptr->end_post(); ++it2)
        {
            try
            {
                cout << std::left << setw(odsadenie) << tree_distances.at(it1->get_id()).at(it2->get_id());
            } catch (...)
            {
                cout << std::left << setw(odsadenie) << tree_distances.at(it2->get_id()).at(it1->get_id());
            }
        }
        cout << endl;
    }
}

void gted::print_FDist(const forest_distance_table_type& table) const
{
    //APP_DEBUG_FNAME;

    int odsadenie = 15;
    for (auto val : table)
    {
        string s = string() +  "[" + label(val.first.left) + "," + label(val.first.right) + "]";
        cout << setw(odsadenie) << "";
        cout << s << endl;
        for (auto v2 : val.second)
        {
            s = string() + "[" + label(v2.first.left) + "," + label(v2.first.right) + "]";
            cout << setw(odsadenie) << s;
            cout << v2.second << "\t";
            cout << endl;
        }
        cout << endl;
    }
}

/*

bool gted::do_decompone_H(tree_type::iterator& it_ref,
                        tree_type::iterator& it_path_node,
                        tree_type::iterator root) const
{
    //LOGGER_PRIORITY_ON_FUNCTION(INFO);

    APP_DEBUG_FNAME;

    DEBUG("it=%s, pathnode=%s, root=%s", label(it_ref), label(it_path_node), label(root));
    
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





void gted::single_path_function_H(tree_type::iterator root1,
                                tree_type::iterator root2,
                                graph who_first)
{
    APP_DEBUG_FNAME;
    DEBUG("roots: %s %s", label(root1), label(root2));
    
}

*/



size_t gted::biggest_subtree_child(tree_type::iterator root,
                                const tree_type& t,
                                const rted::map_type& t_sizes) const
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    assert(!tree_type::is_leaf(root));
    tree_type::sibling_iterator it(tree_type::first_child(root));
    size_t index = 0;
    size_t biggest_subtree = 0;
    size_t i = 0;

    while(it != it.end())
    {
        auto val = t_sizes.at(it->get_id());
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


void gted::init_FDist_table(forest_distance_table_type& forest_dist,
                            subforest_pair forests)
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    struct
    {
        tree_type::iterator it1, it2;
    } parents, leafs;
    int i;

    assert(forest_dist.empty());

    leafs.it1 = forests.f1.right;
    leafs.it2 = forests.f2.right;
    parents.it1 = tree_type::parent(leafs.it1);
    parents.it2 = tree_type::parent(leafs.it2);

    // INIT
    DEBUG("INIT 0");
    forests.f1.left = forests.f2.left = tree_type::iterator();  // zneplatni cely subtree -> hash() = 0
    assert(forest_dist.hash_function()(forests.f1) == 0 &&
            forest_dist.hash_function()(forests.f2) == 0);

#define LOGGER_PRINT_FDIST(First, Second) \
    DEBUG("F[%s, %s][%s, %s] = %lu", \
            label(First.left), label(First.right), \
            label(Second.left), label(Second.right), \
            forest_dist.at(First).at(Second))

    forest_dist[forests.f1][forests.f2] = 0;
    LOGGER_PRINT_FDIST(forests.f1, forests.f2);

    DEBUG("INIT 1");
    i = 1;
    forests.f1.left = leafs.it1;
    forest_dist[forests.f1][forests.f2] = i++;
    LOGGER_PRINT_FDIST(forests.f1, forests.f2);

    while(forests.f1.right != forests.f1.root)
    {
        ++forests.f1.left;
        while(forests.f1.left != parents.it1)
        {
            forest_dist[forests.f1][forests.f2] = i++;
            LOGGER_PRINT_FDIST(forests.f1, forests.f2);
            ++forests.f1.left;
        }
        forests.f1.left = tree_type::first_child(parents.it1);
        ++forests.f1.right;

        while(forests.f1.right != parents.it1)
        {
            forest_dist[forests.f1][forests.f2] = i++;
            LOGGER_PRINT_FDIST(forests.f1, forests.f2);
            ++forests.f1.right;
        }
        forests.f1.left = parents.it1;
        // left == right == parent
        forest_dist[forests.f1][forests.f2] = i++;
        LOGGER_PRINT_FDIST(forests.f1, forests.f2);

        parents.it1 = tree_type::parent(parents.it1);
    }

    DEBUG("INIT 2");
    i = 1;
    forests.f1.left = tree_type::iterator();
    forests.f1.right = leafs.it1;
    forests.f2.left = leafs.it2;
    forest_dist[forests.f1][forests.f2] = i++;
    LOGGER_PRINT_FDIST(forests.f1, forests.f2);

    while(forests.f2.right != forests.f2.root)
    {
        ++forests.f2.left;
        while(forests.f2.left != parents.it2)
        {
            forest_dist[forests.f1][forests.f2] = i++;
            LOGGER_PRINT_FDIST(forests.f1, forests.f2);
            ++forests.f2.left;
        }
        forests.f2.left = tree_type::first_child(forests.f2.left);
        ++forests.f2.right;

        while(forests.f2.right != parents.it2)
        {
            forest_dist[forests.f1][forests.f2] = i++;
            LOGGER_PRINT_FDIST(forests.f1, forests.f2);
            ++forests.f2.right;
        }
        forests.f2.left = parents.it2;
        // left == right == parent
        forest_dist[forests.f1][forests.f2] = i++;
        LOGGER_PRINT_FDIST(forests.f1, forests.f2);

        parents.it2 = tree_type::parent(parents.it2);
    }
    //print_FDist(forest_dist);
}




void gted::run_gted()
{
    APP_DEBUG_FNAME;

    precompute_heavy_paths();
    compute_distances_recursive(t1.begin(), t2.begin());

    //init_tree_dist_table();
    //print_TDist();

    //compute_forest_distances_recursive(++t1.begin(), ++t2.begin());
    //print_TDist();
}


/*
gted::decomposition_type gted::path_decomposition(tree_type::iterator it,
        const tree_type& t, const rted::map_type& t_size, path_strategy s) const
{

    APP_DEBUG_FNAME;

    DEBUG("decomponing by strategy '%s'", to_string(s));

    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    size_t index;
    size_t i;
    decomposition_type decomposition;

    while(!tree_type::is_leaf(it))
    {
        logger.debug("going down the path node '%s'", label(it));

        if (s == path_strategy::left)
            index = 0;
        // TODO: .number_of_children je draha operacia. ..
        // tak radsej namiesto indexu davat iterator na dany vrchol..
        // .last_child() aj .first_child() je konstantne, ale zase .child_at() bude drahe..
        // tak sa da skombinovat index aj iterator.. if (index == x || iterator ==y) ...
        if (s == path_strategy::right)
            index = it.number_of_children() - 1;
        if (s == path_strategy::heavy)
            index = biggest_subtree_child(it, t, t_size);

        decomposition.path.push_back(it);
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
    decomposition.path.push_back(it);   // posledny list..

    return decomposition;
}




void gted::set_forest_distance_table_value(forest_distance_table_type& table,
                                        const subforest& index1,
                                        const subforest& index2,
                                        size_t value,
                                        graph who_first) const
{
    //APP_DEBUG_FNAME;

    if (who_first == T1)
        table[index1][index2] = value;
    else
        table[index2][index1] = value;
}

size_t gted::get_forest_distance_table_value(const forest_distance_table_type& table,
                                            const subforest& index1,
                                            const subforest& index2,
                                            graph who_first) const
{
    //APP_DEBUG_FNAME;

    //cout << "whofirst: " << to_string(who_first) << endl;
    if (who_first == T1)
    {
        DEBUG("GET: F[%s, %s][%s, %s]",
            label(index1.left), label(index1.right),
            label(index2.left), label(index2.right)
            );
        return table.at(index1).at(index2);
    }
    else
    {
        DEBUG("GET: F[%s, %s][%s, %s]",
            label(index2.left), label(index2.right),
            label(index1.left), label(index1.right)
            );
        return table.at(index2).at(index1);
    }
}


void gted::init_tree_dist_table()
{
    APP_DEBUG_FNAME;

    rted::map_type inner;
    for (auto it2 = t2.begin_post(); it2 != t2.end_post(); ++it2)
        inner[id(it2)] = -0xDEAD;

    for (auto it1 = t1.begin_post(); it1 != t1.end_post(); ++it1)
        tree_distances[id(it1)] = inner;
}

void gted::init_forest_dist_table(forest_distance_table_type& forest_distance,
                                subforest_pairs forests,
                                graph who_first) const
{
    APP_DEBUG_FNAME;
    DEBUG("INIT BEGIN");

    // vsetky iteratory right/left/path_node musia ukazovat 
    // na ten isty vrchol, ktory je zaroven listom.
    assert(tree_type::is_leaf(forests.f1.right) &&
            tree_type::is_leaf(forests.f2.right) &&
            forests.f1.right == forests.f1.left &&
            forests.f2.right == forests.f2.left &&
            forests.f1.path_node == forests.f1.right &&
            forests.f2.path_node == forests.f2.right &&
            forests.f1.root != tree_type::iterator() &&
            forests.f2.root != tree_type::iterator() &&
            (who_first == T1 || who_first == T2));

    size_t distance;
    tree_type::iterator parent;

    // zneplatni subtrees, aby hash() == 0
    forests.f1.left = tree_type::reverse_post_order_iterator();
    forests.f2.left = tree_type::reverse_post_order_iterator();

    assert(forest_distance.hash_function()(forests.f1) == 0 &&
            forest_distance.hash_function()(forests.f2) == 0);

#define INIT_FDIST_OUTPUT \
    if (who_first == T1) \
        DEBUG("F[%s, %s][%s, %s] = %lu", \
            label(forests.f1.left), label(forests.f1.right), \
            label(forests.f2.left), label(forests.f2.right), \
            forest_distance.at(forests.f1).at(forests.f2)); \
    else \
        DEBUG("F[%s, %s][%s, %s] = %lu", \
            label(forests.f2.left), label(forests.f2.right), \
            label(forests.f1.left), label(forests.f1.right), \
            forest_distance.at(forests.f2).at(forests.f1));


#define INIT_SETFDIST_COMMAND \
    set_forest_distance_table_value(forest_distance, forests.f1, forests.f2, distance++, who_first)

    DEBUG("NULL INIT");
    distance = 0;
    INIT_SETFDIST_COMMAND;
    INIT_FDIST_OUTPUT;

    DEBUG("FIRST INIT");
    forests.f1.left = forests.f1.right;
    parent = tree_type::parent(forests.f1.right);
    distance = 1;

    //forest_distance[forests.f1][forests.f2] = i++;
    INIT_SETFDIST_COMMAND;
    INIT_FDIST_OUTPUT;
    while(forests.f1.right != forests.f1.root)
    {
        ++forests.f1.left;
        while(forests.f1.left != parent)
        {
            //forest_distance[forests.f1][forests.f2] = i++;
            INIT_SETFDIST_COMMAND;
            INIT_FDIST_OUTPUT;
            ++forests.f1.left;
        }
        // potrebujem teraz sa vratit o krok spat, na indexaciu pre .right
        //
        //--forests.f1.left; NEFUNGUJE, ale ekvivalent je dat tam prveho syna...
        forests.f1.left = tree_type::first_child(parent);
        ++forests.f1.right;

        while(forests.f1.right != parent)
        {
            //forest_distance[forests.f1][forests.f2] = i++;
            INIT_SETFDIST_COMMAND;
            INIT_FDIST_OUTPUT;
            ++forests.f1.right;
        }
        forests.f1.left = parent;
        // .left == .right == parent
        //forest_distance[forests.f1][forests.f2] = i++;
        INIT_SETFDIST_COMMAND;
        INIT_FDIST_OUTPUT;

        parent = tree_type::parent(parent);
    }

    DEBUG("SECOND INIT");
    // to iste, len pre 2. strom..
    forests.f1.left = tree_type::reverse_post_order_iterator();
    forests.f1.right = forests.f1.path_node;    // zresetuj .right na leaf_node
    forests.f2.left = forests.f2.right;
    parent = tree_type::parent(forests.f2.right);
    distance = 1;

    //forest_distance[forests.f1][forests.f2] = i++;
    INIT_SETFDIST_COMMAND;
    INIT_FDIST_OUTPUT;
    while(forests.f2.right != forests.f2.root)
    {
        ++forests.f2.left;
        while(forests.f2.left != parent)
        {
            //forest_distance[forests.f1][forests.f2] = i++;
            INIT_SETFDIST_COMMAND;
            INIT_FDIST_OUTPUT;
            ++forests.f2.left;
        }

        forests.f2.left = tree_type::first_child(parent);
        ++forests.f2.right;

        while(forests.f2.right != parent)
        {
            //forest_distance[forests.f1][forests.f2] = i++;
            INIT_SETFDIST_COMMAND;
            INIT_FDIST_OUTPUT;
            ++forests.f2.right;
        }
        forests.f2.left = parent;
        // .left == .right == parent
        //forest_distance[forests.f1][forests.f2] = i++;
        INIT_SETFDIST_COMMAND;
        INIT_FDIST_OUTPUT;

        parent = tree_type::parent(parent);
    }
    DEBUG("INIT END");
}




void gted::compute_forest_distances_recursive(tree_type::iterator root1,
                                            tree_type::iterator root2)
{
    APP_DEBUG_FNAME;

    SUBTREE_DEBUG_PRINT(*t1.tree_ptr, root1);
    SUBTREE_DEBUG_PRINT(*t2.tree_ptr, root2);

    decomposition_type decomposition;
    auto strategy_pair = strategies.at(id(root1)).at(id(root2));

    strategy_pair.second = path_strategy::left;

    subforest_pairs p;
    graph who_first = strategy_pair.first;
    cout << "strategy: " << to_string(strategy_pair.second) << " graph: " << to_string(strategy_pair.first) << endl;
    cout << "roots: " << *root1 << " " << *root2 << endl;

[>
    decomposition = path_decomposition(root1, t1, t1_sizes, strategy_pair.second);
    for (auto node : decomposition.subtrees)
        compute_forest_distances_recursive(node, root2);

    p.f1.right = p.f1.left = p.f1.path_node = decomposition.path.back();

    decomposition = path_decomposition(root2, t2, t2_sizes, strategy_pair.second);
    for (auto node : decomposition.subtrees)
        compute_forest_distances_recursive(root1, node);
    p.f2.right = p.f2.left = p.f2.path_node = decomposition.path.back();
    who_first = graph::T1;
<]

    if (who_first == graph::T1)
    {
        decomposition = path_decomposition(root1, t1, t1_sizes, path_strategy::left);
        //decomposition = path_decomposition(root1, t1, t1_sizes, strategy_pair.second);
        for (auto node : decomposition.subtrees)
            compute_forest_distances_recursive(node, root2);

        // v F1 zacnem s vrcholom na ceste,
        // v F2 treba zacat s lubovolnym, napr najlavejsim...
        // najprv sa hybem s iteratormi na F1 
        p.f1.right = p.f1.left = p.f1.path_node = decomposition.path.back();
        p.f2.right = p.f2.left = p.f2.path_node = tree_type::leftmost_child(root2);
        p.f1.root = root1;
        p.f2.root = root2;

        for (int i = decomposition.path.size() - 1; i > 1; ++i)
            compute_forest_distances_recursive(decomposition.path.at(i), root2);
    }
    else
    {
        decomposition = path_decomposition(root2, t2, t2_sizes, path_strategy::left);
        //decomposition = path_decomposition(root2, t2, t2_sizes, strategy_pair.second);
        for (auto node : decomposition.subtrees)
            compute_forest_distances_recursive(root1, node);

        // v F1 zacinam lubovolnym vrhcolom, napr najlavejsim
        // v F2 zacinam na ceste
        // .. najprv sa hybem s iteratormi na F2
        p.f1.right = p.f1.left = p.f1.path_node = decomposition.path.back();
        p.f2.right = p.f2.left = p.f2.path_node = tree_type::leftmost_child(root1);
        p.f1.root = root2;
        p.f2.root = root1;

        for (int i = decomposition.path.size() - 1; i > 1; --i)
            compute_forest_distances_recursive(root1, decomposition.path.at(i));
    }


    DEBUG("COMPUTING DISTANCES");
    SUBTREE_DEBUG_PRINT(*t1.tree_ptr, p.f1.root);
    SUBTREE_DEBUG_PRINT(*t2.tree_ptr, p.f2.root);
    cout << "strategy: " << to_string(strategy_pair.second) << " graph: " << to_string(who_first) << endl;
    cout << "roots: " << *root1 << " " << *root2 << endl;

    forest_distance_table_type forest_dist;
    init_forest_dist_table(forest_dist, p, who_first);
    compute_forest_distances(forest_dist, p, who_first);
    print_FDist(forest_dist);
    print_TDist();
}

void gted::compute_forest_distances(forest_distance_table_type& forest_dist,
                                subforest_pairs forests,
                                graph who_first)
{
    APP_DEBUG_FNAME;

    assert (tree_type::is_leaf(forests.f1.left) && 
            tree_type::is_leaf(forests.f2.left) &&
            forests.f1.left == forests.f1.right && 
            forests.f2.left == forests.f2.right &&
            forests.f1.path_node == forests.f1.left &&
            forests.f2.path_node == forests.f2.left &&
            forests.f1.root != tree_type::iterator() &&
            forests.f2.root != tree_type::iterator());

    struct 
    {
        tree_type::iterator it1;
        tree_type::iterator it2;
    } leafs, parents, last_tree_roots;

    subforest_pairs prevs = forests;

    // zneplatni prevs*
    prevs.f1.left = prevs.f2.left = tree_type::reverse_post_order_iterator();

    leafs.it1 = forests.f1.right;
    leafs.it2 = forests.f2.right;
    parents.it1 = tree_type::parent(leafs.it1);
    parents.it2 = tree_type::parent(leafs.it2);
    last_tree_roots.it2 = leafs.it2;

#define COMPUTE_FDIST_COMMAND \
    fill_distance_tables(forest_dist, forests, prevs, last_tree_roots.it1, last_tree_roots.it2, who_first)

    bool end_loop = false;
    do
    {
        if (forests.f2.root == forests.f2.right)
            end_loop = true;

        // inicializuj premenne:
        forests.f1.path_node = forests.f1.left = forests.f1.right = leafs.it1;
        forests.f1.last = subforest::undef;
        parents.it1 = tree_type::parent(leafs.it1);
        // prevs ma byt zatial neplatne -> hash() == 0
        prevs.f1.right = leafs.it1;
        prevs.f1.left = tree_type::reverse_post_order_iterator();
        last_tree_roots.it1 = leafs.it1;

        DEBUG("NEW CYCLE");
        COMPUTE_FDIST_COMMAND;
        while(forests.f1.root != forests.f1.right)
        {
            prevs.f1.left = forests.f1.left++;
            forests.f1.last = subforest::Lnode;

            while(forests.f1.left != parents.it1)
            {
                COMPUTE_FDIST_COMMAND;

                prevs.f1.left = forests.f1.left++;
                if (tree_type::is_leaf(forests.f1.left))
                    last_tree_roots.it1 = prevs.f1.left;
            }

            // som aktualne v otcovi, ale indexovat musim este s 1. synom (predchadzajucim vrcholom).. 
            // => treba sa vratit o krok spat
            forests.f1.left = prevs.f1.left;
            // posun o 1 doprava
            prevs.f1.right = forests.f1.right++;
            forests.f1.last = subforest::Rnode;

            while(forests.f1.right != parents.it1)
            {
                COMPUTE_FDIST_COMMAND;
                
                prevs.f1.right = forests.f1.right++;
                if(tree_type::is_leaf(forests.f1.right))
                    last_tree_roots.it1 = prevs.f1.right;
            }
            // som uz v otcovi, potrebujem na neho nastavit aj .left
            forests.f1.left = forests.f1.right;
            // zaroven je to novy cely strom, nastavujem aj last_tree_root
            last_tree_roots.it1 = forests.f1.right;
            // posun sa o 1 patro vyssie..
            parents.it1 = tree_type::parent(parents.it1);
            // nastav aktualny vrhcol, ze lezi na PATH
            forests.f1.path_node = forests.f1.right;
            forests.f1.last = subforest::undef;
            // dopocitaj este aktualny vrchol
            COMPUTE_FDIST_COMMAND;
            prevs.f1.right = forests.f1.right;
        }

        // dopocital som vzdialenosti pre (f2.left, f2.right) <-> f1.root_subtree.. 
        // teraz uprav F2.
        prevs.f2.left = forests.f2.left++;
        forests.f2.last = subforest::Lnode;
        DEBUG("LNODE");
        if (tree_type::is_leaf(forests.f2.left))
            last_tree_roots.it2 = prevs.f2.left;

        if (forests.f2.left == parents.it2)
        {
            //DEBUG("t2.left == parents.it2");
            // som v otcovi, ale indexovat musim vzdy este z predchadzajuceho vrcholu.. 
            // to predchadzajuce .left++ stoji malo, kedze iba prejdem od 1. syna k otcovi
            // takze to nevadi, ze to volam aj viackrat za cyklus
            
            forests.f2.left = prevs.f2.left;

            // zvys teda .right
            prevs.f2.right = forests.f2.right++;
            forests.f2.last = subforest::Rnode;
            DEBUG("RNODE");
            if (tree_type::is_leaf(forests.f2.right))
                last_tree_roots.it2 = prevs.f2.right;
            
            if (forests.f2.right == parents.it2)
            {
                //DEBUG("t2.right == parents.it2");
                // posuvam sa o 1 patro vyssie..
                forests.f2.left = forests.f2.right;
                parents.it2 = tree_type::parent(parents.it2);
                forests.f2.path_node = forests.f2.right;
                forests.f2.last = subforest::undef;
                DEBUG("undef");
            }
        }
        print_TDist();
        print_FDist(forest_dist);
        // a skonci az po loop z T2.root
    }
    while(!end_loop);
}

void gted::fill_distance_tables(forest_distance_table_type& forest_dist,
                            const subforest_pairs& root_forests,
                            const subforest_pairs& prevs,
                            tree_type::iterator tree_root1,
                            tree_type::iterator tree_root2,
                            graph who_first)
{
    APP_DEBUG_FNAME;

#define GTED_VECTOR_DELETE_LEFT     0
#define GTED_VECTOR_DELETE_RIGHT    1
#define GTED_VECTOR_DELETE_BOTH     2

#define GTED_COST_MODIFY            0
#define GTED_COST_DELETE            1

    DEBUG("ROOTS + PREVS:");
    pretty_print(root_forests.f1, root_forests.f2, who_first);
    pretty_print(prevs.f1, prevs.f2, who_first);

    // pocitam forest_dist[root_forests.f1][root_forests.f2]..
    vector<size_t> vec(3, 0xBADF00D);

    //DEBUG("A");
    vec[GTED_VECTOR_DELETE_LEFT] =
        get_forest_distance_table_value(forest_dist, prevs.f1, root_forests.f2, who_first) + GTED_COST_DELETE;
    //DEBUG("A");
    vec[GTED_VECTOR_DELETE_RIGHT] = 
        get_forest_distance_table_value(forest_dist, root_forests.f1, prevs.f2, who_first) + GTED_COST_DELETE;
    //DEBUG("A");

    auto is_path_node_funct = [](const subforest& p){
        return p.path_node == p.left && p.path_node == p.right;
    };

    string l1, l2;

    //TODO skontrolovat ci je ekvivalentne is_path_node_funct(...) a .last = subforest::undef

    if (is_path_node_funct(root_forests.f1) &&
            is_path_node_funct(root_forests.f2))
    {
        DEBUG("both nodes are on path..");
        vec[GTED_VECTOR_DELETE_BOTH] =
            get_forest_distance_table_value(forest_dist, prevs.f1, prevs.f2, who_first) + GTED_COST_MODIFY;
    }
    else
    {
        size_t id1, id2;
        subforest_pairs other = prevs;

        DEBUG("F1:");
        if (root_forests.f1.last == subforest::Lnode)
        {
            DEBUG("last == Lnode, %s", label(root_forests.f1.left));
            other.f1.left = tree_root1;
            id1 = id(root_forests.f1.left);
            l1 = label(root_forests.f1.left);
        }
        else if (root_forests.f1.last == subforest::Rnode)
        {
            DEBUG("last == Rnode, %s", label(root_forests.f1.right));
            other.f1.right = tree_root1;
            id1 = id(root_forests.f1.right);
            l1 = label(root_forests.f1.right);
        }
        else
        {
            logger.warn("ELSE??");
            id1 = id(root_forests.f1.right);
            l1 = label(root_forests.f1.right);
        }

        DEBUG("F2:");
        if (root_forests.f2.last == subforest::Lnode)
        {
            DEBUG("last == Lnode, %s", label(root_forests.f1.left));
            other.f2.left = tree_root2;
            id2 = id(root_forests.f2.left);
            l2 = label(root_forests.f2.left);
        }
        else if (root_forests.f2.last == subforest::Rnode)
        {
            DEBUG("last == Rnode, %s", label(root_forests.f1.right));
            other.f2.right = tree_root2;
            id2 = id(root_forests.f2.right);
            l2 = label(root_forests.f2.right);
        }
        else
        {
            logger.warn("ELSE");
            id2 = id(root_forests.f2.right);
            l2 = label(root_forests.f2.right);
        }

        if (who_first == T2)
        {
            swap(id1, id2);
            swap(l1, l2);
        }

        DEBUG("old: T[%s][%s] == %i", l1.c_str(), l2.c_str(), tree_distances.at(id1).at(id2));
        vec[GTED_VECTOR_DELETE_BOTH] =
            get_forest_distance_table_value(forest_dist, other.f1, other.f2, who_first) +
            tree_distances.at(id1).at(id2);
    }

    size_t c_min = *min_element(vec.begin(), vec.end());

    LOGGER_PRINT_CONTAINER(vec, "vec ");

    set_forest_distance_table_value(forest_dist, root_forests.f1, root_forests.f2, c_min, who_first);

    pretty_printF(root_forests.f1, root_forests.f2, forest_dist, who_first);

    if (is_path_node_funct(root_forests.f1) &&
            is_path_node_funct(root_forests.f2))
    {
        if (who_first == T1)
        {
            DEBUG("T[%s][%s] = %lu",
                    label(root_forests.f1.right), label(root_forests.f2.right), c_min);
            tree_distances[id(root_forests.f1.right)][id(root_forests.f2.right)] = c_min;
        }
        else
        {
            DEBUG("T[%s][%s] = %lu",
                    label(root_forests.f2.right), label(root_forests.f1.right), c_min);
            tree_distances[id(root_forests.f2.right)][id(root_forests.f1.right)] = c_min;
        }
    }
}

*/

/*
void gted::pretty_printF(const subforest& index1,
                    const subforest& index2,
                    const forest_distance_table_type& forest_dist,
                    graph who_first) const
{
    if (who_first == T1)
        DEBUG("F[%s, %s][%s, %s] = %lu",
            label(index1.left), label(index1.right),
            label(index2.left), label(index2.right),
            get_forest_distance_table_value(forest_dist, index1, index2, who_first));
    else
        DEBUG("F[%s, %s][%s, %s] = %lu",
            label(index2.left), label(index2.right),
            label(index1.left), label(index1.right),
            get_forest_distance_table_value(forest_dist, index1, index2, who_first));
}
*/

