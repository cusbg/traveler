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


#define F_str "F[%s, %s][%s, %s]"
#define T_str "T[%s][%s]"

#define labels_LR(s) label(s.left), label(s.right)

#define all_subforest_nodes_init(Subforest, value) \
    Subforest.left = Subforest.right = Subforest.path_node = value


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

void gted::print_TDist() const
{
    APP_DEBUG_FNAME;

    stringstream out;
    bool was_catch = false;
    int odsadenie = 22;
    out << "TREE_DISTANCE_TABLE:" << endl;
    out << setw(odsadenie) << "";
    for (auto it2 = t2.tree_ptr->begin_post(); it2 != --t2.tree_ptr->end_post(); ++it2)
        out << setw(odsadenie) << std::left << *it2;
    out << endl;
    for (auto it1 = t1.tree_ptr->begin_post(); it1 != --t1.tree_ptr->end_post(); ++it1)
    {
        out << setw(odsadenie) << std::left << *it1;
        for (auto it2 = t2.tree_ptr->begin_post(); it2 != --t2.tree_ptr->end_post(); ++it2)
        {
            try
            {
                out << std::left << setw(odsadenie) << tree_distances.at(it1->get_id()).at(it2->get_id());
            } catch (...)
            {
                was_catch = true;
                out << std::left << setw(odsadenie) << tree_distances.at(it2->get_id()).at(it1->get_id());
            }
        }
        out << endl;
    }
    logger.infoStream() << out.str();
    if (was_catch)
        logger.warn("CATCH in TDist");
}

void gted::print_FDist(const forest_distance_table_type& table) const
{
    //APP_DEBUG_FNAME;

    stringstream out;
    out << "FOREST_DISTANCE_TABLE:" << endl;

    int odsadenie = 15;
    for (auto val : table)
    {
        string s = string() +  "[" + label(val.first.left) + "," + label(val.first.right) + "]";
        out << setw(odsadenie) << "";
        out << s << endl;
        for (auto v2 : val.second)
        {
            s = string() + "[" + label(v2.first.left) + "," + label(v2.first.right) + "]";
            out << setw(odsadenie) << s;
            out << v2.second << "\t";
            out << endl;
        }
        out << endl;
    }
    logger.debugStream() << out.str();
}

/* GET/SET functions: */
size_t gted::get_Fdist(const subforest& index1,
                    const subforest& index2,
                    const forest_distance_table_type& forest_dist) const
{
    // vzdy vracia v danom poradi, table[index1][index2].
    try
    {
        DEBUG("GET: " F_str,
                labels_LR(index1), labels_LR(index2));
        return forest_dist.at(index1).at(index2);
    }
    catch(...)
    {
        logger.error("CATCHed exception");
        DEBUG("mozno vymenit indexy???");
        print_FDist(forest_dist);
        abort();
    }
}

size_t gted::get_Tdist(tree_type::iterator index1,
                    tree_type::iterator index2,
                    graph who_first) const
{
    size_t out;
    if (who_first == T1)
    {
        DEBUG("GET: " T_str,
                label(index2), label(index1));
        out = tree_distances.at(id(index2)).at(id(index1));
    }
    else
    {
        DEBUG("GET: " T_str,
                label(index1), label(index2));
        out = tree_distances.at(id(index1)).at(id(index2));
    }
    return out;
}

void gted::set_Fdist(const subforest& index1,
                    const subforest& index2,
                    forest_distance_table_type& forest_dist,
                    size_t value,
                    graph who_first) const
{
    if (who_first == T1)
    {
        DEBUG("SET: " F_str " = %lu",
                labels_LR(index2), labels_LR(index1), value);
        forest_dist[index2][index1] = value;
    }
    else
    {
        DEBUG("SET: " F_str " = %lu",
                labels_LR(index1), labels_LR(index2), value);
        forest_dist[index1][index2] = value;
    }
}

void gted::set_Tdist(tree_type::iterator index1,
                    tree_type::iterator index2,
                    size_t value,
                    graph who_first)
{
    if (who_first == T1)
    {
        DEBUG("SET: " T_str " = %lu",
                label(index2), label(index1), value);
        tree_distances[id(index2)][id(index1)] = value;
    }
    else
    {
        DEBUG("SET: " T_str " = %lu",
                label(index1), label(index2), value);
        tree_distances[id(index1)][id(index2)] = value;
    }
}



void gted::init_FDist_table(forest_distance_table_type& forest_dist,
                            subforest_pair forests,
                            graph who_first)
{
    //LOGGER_PRIORITY_ON_FUNCTION(INFO);
    APP_DEBUG_FNAME;

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

#define init_command() \
    set_Fdist(forests.f1, forests.f2, forest_dist, i++, who_first)

    i = 0;
    init_command();

    DEBUG("INIT 1");
    i = 1;
    forests.f1.left = leafs.it1;
    init_command();

    while(forests.f1.right != forests.f1.root)
    {
        ++forests.f1.left;
        while(forests.f1.left != parents.it1)
        {
            init_command();
            ++forests.f1.left;
        }
        forests.f1.left = tree_type::first_child(parents.it1);
        ++forests.f1.right;

        while(forests.f1.right != parents.it1)
        {
            init_command();
            ++forests.f1.right;
        }
        forests.f1.left = parents.it1;
        // left == right == parent
        init_command();

        parents.it1 = tree_type::parent(parents.it1);
    }

    DEBUG("INIT 2");
    i = 1;
    forests.f1.left = tree_type::iterator();
    forests.f1.right = leafs.it1;
    forests.f2.left = leafs.it2;
    init_command();

    while(forests.f2.right != forests.f2.root)
    {
        ++forests.f2.left;
        while(forests.f2.left != parents.it2)
        {
            init_command();
            ++forests.f2.left;
        }
        forests.f2.left = tree_type::first_child(forests.f2.left);
        ++forests.f2.right;

        while(forests.f2.right != parents.it2)
        {
            init_command();
            ++forests.f2.right;
        }
        forests.f2.left = parents.it2;
        // left == right == parent
        init_command();

        parents.it2 = tree_type::parent(parents.it2);
    }
    //print_FDist(forest_dist);
#undef init_command
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





void gted::compute_distance(subforest_pair forests,
                            graph who_first)
{
    if (who_first == T1)
        DEBUG("computing distance between '%s' '%s'",
                label(forests.f2.root), label(forests.f1.root));
    else
        DEBUG("computing distance between '%s' '%s'",
                label(forests.f1.root), label(forests.f2.root));
    
    auto inited_subforest = [](const subforest& f)
    {
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

    // variables:
    subforest_pair              prevs;
    forest_distance_table_type  forest_dist;
    iterator_pair               leafs,
                                parents,
                                last_tree_roots;
    struct
    {
        vector<tree_type::iterator> s1;
        vector<tree_type::iterator> s2;
    } last_tree_roots_stacks;

#define compute_fdist_command() \
    fill_table(forest_dist, forests, prevs, \
            last_tree_roots, who_first)

    /*
     * if stack is empty, return leaf
     * in stack there are last_tree roots, that are needed in computation
     *  of F[][] when we are not on the path with (1-2) nodes.
     *  for detailed formula, see formula below.
     */
#define get_back_f1() \
    (last_tree_roots_stacks.s1.empty()) ? \
        leafs.it1 : last_tree_roots_stacks.s1.back()
#define get_back_f2() \
    (last_tree_roots_stacks.s2.empty()) ? \
        leafs.it2 : last_tree_roots_stacks.s2.back()

    // VARS INIT:
    init_FDist_table(forest_dist, forests, who_first);

    // in prevs, we use only right/left...
    prevs.f1.right = forests.f1.right;
    prevs.f2.right = forests.f2.right;
    leafs.it1 = forests.f1.right;
    leafs.it2 = forests.f2.right;
    parents.it1 = tree_type::parent(leafs.it1);
    parents.it2 = tree_type::parent(leafs.it2);
    last_tree_roots.it2 = get_back_f2();

    auto del_stack_children = [](vector<tree_type::iterator>& stack, tree_type::iterator it)
    {
        // for given node, removes all his children on the top of stack
        //
        stringstream out;
        out << "STACK:" << "\t";
        for (auto i : stack)
            out << *i << " ";
        out << endl;
        logger.debugStream() << out.str();
        while(!stack.empty())
        {
            if (tree_type::parent(stack.back()) != it)
                break;
            assert(!stack.empty());
            DEBUG("stack.back == it, deleting %s", label(stack.back()));
            stack.pop_back();
        }
    };

    bool loop = true;
    do
    {
        /*
         * ABOUT: vseobecne principy:
         *  iterujem najprv cez F1, vonkajsi cyklus je cez F2.
         *  vzdy pridavam najprv lave vrcholy, az kym nedojdem do otca, potom sa 
         *  o 1 krok vratim spat (1.syn == najlavejsi) a pridavam prave vrcholy.
         *  ked aj tam dojdem do otca, nastavim aj laveho na otca a idem novy (vnutorny) cyklus
         *  az kym nedojdem do korena. 
         *  vonkajsi cyklus funguje na rovnakom principe
         *
         *  v premennych last_tree_roots su:
         *      ak je .last == undef (stojim na path_node): je tam pointer na zaciatocny list,
         *          aby sa dalo pouzit mazanie vrcholu -> index do tabulky F: (<null>, list)
         *      inac je tam posledny subtreevrchol ktory je mimo aktualneho podstromu
         *          a teda da sa pouzit index F:
         *              (<?begin?>, last_tree_root) || (last_tree_root, <?begin?>)
         *          a index T: (this.node)
         *
         *  aby fungovala prva moznost, treba si ukladat tie vrcholy do zasobnika, 
         *  a vzdy ak zasobnik.back == child(node), tak musim dat zasobnik.pop()
         *  inac pouzivam dany vrchol ako last_tree_root
         *  ak je zasobnik prazdny, tak pouzijem list z cesty.. (leafs.it)
         */

        /*
         * ABOUT: princip fungovania algoritmu:
         * ak stojim v F1, F2 na ceste:
         *      F[forests.f1][forests.f2] = MIN {
         *          F[prevs.f1  ][forests.f2] + COST_DELETE,
         *          F[forests.f1][prevs.f2  ] + COST_DELETE,
         *          F[prevs.f1  ][prevs.f2  ] + COST_MODIFY
         *      }
         *
         * ak som mimo cesty, iba s jednym, BUNO F2, .last == Rnode
         *      F[forests.f1][forests.f2] = MIN {
         *          F[prevs.f1  ][forests.f2] + COST_DELETE,
         *          F[forests.f1][prevs.f2  ] + COST_DELETE,
         *          F[EMPTY     ][prevs.left .. last_tree_root.it2]
         *                  + T[forests.f1][forests.f2]
         *      }
         *
         * obdobna situacia je pre .last == Lnode, iba sa berie podstrom
         *      [][last_tree_root.it2 .. prevs.f2.right]
         *          Pozn: prevs.right == forests.f2.right
         * a podobne aj ked su oba mimo cesty, vyberam dane podstromy..
         */

        DEBUG("******************** NEW CYCLE ********************");
        if (forests.f2.root == forests.f2.right)
            loop = false;

        // init premennych
        all_subforest_nodes_init(forests.f1, leafs.it1);

        forests.f1.last = subforest::undef;
        parents.it1 = tree_type::parent(leafs.it1);
        prevs.f1.right = forests.f1.right;
        prevs.f1.left = empty_iterator();
        last_tree_roots_stacks.s1.clear();
        last_tree_roots.it1 = get_back_f1();

        compute_fdist_command();

        while(forests.f1.root != forests.f1.right)
        {
            prevs.f1.left = forests.f1.left++;
            forests.f1.last = subforest::Lnode;

            while(parents.it1 != forests.f1.left)
            {
                if (tree_type::is_leaf(forests.f1.left))
                {
                    last_tree_roots_stacks.s1.push_back(prevs.f1.left);
                    DEBUG("L1: last_tree <- %s", label(prevs.f1.left));
                }
                // ak som v rodicovi, zmazem vsetkych synov ktore boli v zasobniku..
                del_stack_children(last_tree_roots_stacks.s1, forests.f1.left);
                last_tree_roots.it1 = get_back_f1();

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
                    last_tree_roots_stacks.s1.push_back(prevs.f1.right);
                    DEBUG("R1: last_tree <- %s", label(prevs.f1.right));
                }
                del_stack_children(last_tree_roots_stacks.s1, forests.f1.right);
                last_tree_roots.it1 = get_back_f1();

                compute_fdist_command();
                prevs.f1.right = forests.f1.right++;
            }

            forests.f1.last = subforest::undef;
            forests.f1.left =
                forests.f1.path_node = forests.f1.right;
            parents.it1 = tree_type::parent(forests.f1.right);
            del_stack_children(last_tree_roots_stacks.s1, forests.f1.right);
            last_tree_roots.it1 = get_back_f1();

            compute_fdist_command();

            prevs.f1.left =
                prevs.f1.right = forests.f1.right;
        }

        forests.f2.last = subforest::Lnode;
        prevs.f2.left = forests.f2.left++;

        DEBUG("LNODE");
        if (tree_type::is_leaf(forests.f2.left))
        {
            last_tree_roots_stacks.s2.push_back(prevs.f2.left);
            DEBUG("L2: last <- left, %s", label(prevs.f2.left));
        }
        del_stack_children(last_tree_roots_stacks.s2, forests.f2.left);
        last_tree_roots.it2 = get_back_f2();

        if (forests.f2.left == parents.it2)
        {
            DEBUG("t2.left == parent");

            forests.f2.left = prevs.f2.left;
            forests.f2.last = subforest::Rnode;
            prevs.f2.right = forests.f2.right++;

            if (tree_type::is_leaf(forests.f2.right))
            {
                last_tree_roots_stacks.s2.push_back(prevs.f2.right);
                DEBUG("R2: last <- right, %s", label(prevs.f2.right));
            }
            del_stack_children(last_tree_roots_stacks.s2, forests.f2.right);
            last_tree_roots.it2 = get_back_f2();

            if (forests.f2.right == parents.it2)
            {
                DEBUG("t2.right == parent");
                assert(last_tree_roots_stacks.s2.empty());
                last_tree_roots.it2 = get_back_f2();

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














