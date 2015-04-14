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


#undef NDEBUG
#include "gted.hpp"
#include <algorithm>
#include <iomanip>

/*
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_for.h>
#include <tbb/atomic.h>
#include <tbb/parallel_invoke.h>
using namespace tbb;

atomic<size_t> LEFT, RIGHT, HEAVY;
*/


using namespace std;

#ifdef NDEBUG
#define GTED_CHECKS_DISABLED
#define NO_LOGGER_DEBUG_GTED_MESSAGES
#endif


#define GTED_CHECKS_DISABLED

#ifndef NO_LOGGER_DEBUG_GTED_MESSAGES
#define LOGGER_DEBUG_GTED_PRINT_TABLES
#define LOGGER_DEBUG_GTED_HASH

#undef DEBUG
#define DEBUG(...)
#endif


#define INDEXES_LEFT    0
#define INDEXES_RIGHT   1
#define INDEXES_HEAVY   2

#define BAD_INDEX       0xBADF00D
#define _BAD            0xBADF00D





size_t LEFT, RIGHT, HEAVY;




gted::~gted()
{
    logger.notice("~GTED");
}

gted::gted(const tree_type& _t1, const tree_type& _t2)
    : t1(_t1), t2(_t2)
{
    APP_DEBUG_FNAME;
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    auto merge = [this](rted& r)
    {
        // merge sizes to one table
        //
        auto m1 = r.get_t1_sizes();
        auto m2 = r.get_t2_sizes();
        if (t2.size() > t1.size())
            m1.swap(m2);

        t_sizes = std::move(m1);
        t_sizes.insert(m2.begin(), m2.end());
    };
    auto init_TDist = [this]()
    {
        // create final tree_distances table -> M x N
        //
        tree_distances.reserve(t1.size());
        for (auto it1 = t1.begin_post(); it1 != t1.end_post(); ++it1)
        {
            tree_distances[id(it1)].reserve(t2.size());
            for (auto it2 = t2.begin_post(); it2 != t2.end_post(); ++it2)
            {
                tree_distances[id(it1)][id(it2)] = _BAD;
            }
        }
    };

    rted r(t1, t2);

    logger.info("rted_begin");
    r.run_rted();
    strategies = r.get_strategies();
    merge(r);
    logger.info("rted_end");

    logger.info("INIT_begin");
    precompute_tables();
    init_TDist();
    logger.info("INIT_end");
}

void gted::run_gted()
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    assert(t1.begin().number_of_children() == 1 &&
            t2.begin().number_of_children() == 1);

    iterator_pair roots;

    roots.it1 = tree_type::first_child(t1.begin());
    roots.it2 = tree_type::first_child(t2.begin());

    LEFT =
        RIGHT =
        HEAVY = 0;

    logger.notice("GTED: BEGIN");
    compute_distances_recursive(roots);
    logger.notice("GTED: END");

    //print_TDist();

    logger.info("# of used strategies:\n"
            "LEFT: %lu\n"
            "RIGHT: %lu\n"
            "HEAVY: %lu",
            (size_t)LEFT, (size_t)RIGHT, (size_t)HEAVY);
    logger.info("distance = %lu",
            tree_distances.at(id(roots.it1)).at(id(roots.it2)));
}



void gted::precompute_tables()
{
    APP_DEBUG_FNAME;

#define is_heavy(iter) \
        (!iter->is_root() && heavy_child(tree_type::parent(iter)) == iter)
#define is_first(iter) \
        (tree_type::is_first_child(iter))
#define is_last(iter) \
        (tree_type::is_last_child(iter))

#define lies_on_path(iter, STR) \
        (precomputed.leafs.at(id(iter)).STR == \
         precomputed.leafs.at(id(tree_type::parent(iter))).STR)

    auto heavy_child = [&](const tree_type::iterator& root)
    {
        LOGGER_PRIORITY_ON_FUNCTION(INFO);

        assert(!tree_type::is_leaf(root));
        tree_type::sibling_iterator it(tree_type::first_child(root));
        tree_type::iterator out;
        size_t index = 0;
        size_t biggest_subtree = 0;
        size_t i = 0;

        while(it != it.end())
        {
            auto val = t_sizes.at(id(it));
            if (val > biggest_subtree)
            {
                index = i;
                biggest_subtree = val;
                out = it;
            }
            ++it;
            ++i;
        }
        i = index;
        // ^^ inac dava warning -Wunused-but-set-variable,
        // lebo DEBUG je prazdne makro
        DEBUG("child #'%lu - %s' of node '%s' has biggest subtree (size == %lu)",
                index, label(out), label(root), biggest_subtree);
        return out;
    };
    auto push_back = [&](tables::vector_type& vec,
            tree_type::iterator val)
    {
        DEBUG("push_back: %s", label(val));
        vec.push_back(val);
    };
    auto comp_leafs = [&](const tree_type& t)
    {
        size_t it_id;
        tree_type::post_order_iterator it;
        tables::LRH lrh;

        for (it = t.begin_post(); it != t.end_post(); ++it)
        {
            it_id = id(it);

            if (tree_type::is_leaf(it))
                lrh = {it, it, it};
            else
            {
                lrh.left = precomputed.leafs.at(
                        id(tree_type::leftmost_child(it))).left;
                lrh.right = precomputed.leafs.at(
                        id(tree_type::rightmost_child(it))).right;
                lrh.heavy = precomputed.leafs.at(
                        id(heavy_child(it))).heavy;
            }
            DEBUG("leafs for '%s' = [%s, %s, %s]",
                    label(it), label(lrh.left),
                    label(lrh.right), label(lrh.heavy));
            precomputed.leafs[it_id] = lrh;
        }
    };
    auto comp_keyroots = [&](const tree_type& t, bool T1)
    {
        // potrebujem mat uz LEAFS vypocitane
        tree_type::iterator root, path_node;
        tree_type::sibling_iterator sibling;
        tables::vector_type& left = precomputed.keyroots[
            (T1 ? PATH_STRATEGY_LEFT_T1 : PATH_STRATEGY_LEFT_T2)];
        tables::vector_type& right = precomputed.keyroots[
            (T1 ? PATH_STRATEGY_RIGHT_T1 : PATH_STRATEGY_RIGHT_T2)];
        tables::vector_type& heavy = precomputed.keyroots[
            (T1 ? PATH_STRATEGY_HEAVY_T1 : PATH_STRATEGY_HEAVY_T2)];

        enum lrh_list
        {
            L, R, H
        };
        auto do_loop = [&push_back, &t](tables::vector_type& tbl, tables::root_leaf_path_type& leafs, tables::indexes_type& indexes, lrh_list l)
        {
            bool was_first;
            size_t i = 0;
            tree_type::iterator root, path_node, it;
            tree_type::sibling_iterator sibling;
            while(i != tbl.size())
            {
                root = tbl.at(i);
                tables::LRH lrh = leafs.at(id(root));
                path_node = (l == L ? lrh.left : 
                        (l == R ? lrh.right : lrh.heavy));
                was_first = false;

                //DEBUG("root = %s, path_node = %s", label(root), label(path_node));

                indexes[id(root)].keyroot = BAD_INDEX;
                if (path_node == root)
                {}
                else
                {
                    while(true)
                    {
                        // get first sibling..
                        sibling = tree_type::first_child(tree_type::parent(path_node));
                        indexes[id(path_node)].keyroot = BAD_INDEX;

                        while(sibling != sibling.end())
                        {
                            if (sibling != path_node)
                            {
                                push_back(tbl, sibling);
                                if (!was_first)
                                {
                                    indexes[id(root)].keyroot = tbl.size() - 1;
                                    was_first = true;
                                }
                            }
                            ++sibling;
                        }
                        path_node = tree_type::parent(path_node);

                        if (path_node == root)
                            break;
                    }
                    path_node = (l == L ? lrh.left : 
                            (l == R ? lrh.right : lrh.heavy));
                    indexes[id(path_node)].keyroot = BAD_INDEX; // is leaf...
                    path_node = tree_type::parent(path_node);
                    while (tree_type::first_child(path_node) == tree_type::last_child(path_node) &&
                            path_node != root)
                        path_node = tree_type::parent(path_node);
                    while (path_node != root)
                    {
                        indexes[id(path_node)].keyroot = indexes[id(root)].keyroot;
                        path_node = tree_type::parent(path_node);
                    }
                }
                size_t j = indexes[id(root)].keyroot;
                stringstream s;
                if (j == BAD_INDEX)
                    s << "<null>";
                else
                    while(j < tbl.size())
                        s << label(tbl.at(j++)) << " ";
                DEBUG("keyroots for %s: \t%s", label(root), s.str().c_str());

                ++i;
            }
        };

        auto begin = ++t.begin();
        left.push_back( begin);
        right.push_back(begin);
        heavy.push_back(begin);

        DEBUG("LEFT");
        do_loop(left,  precomputed.leafs, precomputed.indexes.at(INDEXES_LEFT), L);
        DEBUG("RIGHT");
        do_loop(right, precomputed.leafs, precomputed.indexes.at(INDEXES_RIGHT), R);
        DEBUG("HEAVY");
        do_loop(heavy, precomputed.leafs, precomputed.indexes.at(INDEXES_HEAVY), H);
    };
    auto comp_subforests = [&](const tree_type& t, bool T1)
    {
        tables::vector_type& left = precomputed.subforests[
            (T1 ? PATH_STRATEGY_LEFT_T1 : PATH_STRATEGY_LEFT_T2)];
        tables::vector_type& right = precomputed.subforests[
            (T1 ? PATH_STRATEGY_RIGHT_T1 : PATH_STRATEGY_RIGHT_T2)];
        tables::vector_type& heavy = precomputed.subforests[
            (T1 ? PATH_STRATEGY_HEAVY_T1 : PATH_STRATEGY_HEAVY_T2)];

        tables::indexes_type& ind_left  = precomputed.indexes.at(INDEXES_LEFT);
        tables::indexes_type& ind_right = precomputed.indexes.at(INDEXES_RIGHT);
        tables::indexes_type& ind_heavy = precomputed.indexes.at(INDEXES_HEAVY);


        tree_type::post_order_iterator it;
        tree_type::sibling_iterator child;
        size_t index;
        
        for (it = t.begin_post(); it != --t.end_post(); ++it)
        {
            // if it does not lie on path, we add it to vector
            // Left  -> !is_first
            // Right -> !is_last
            // Heavy -> !is_heavy
            //
            // and then we copy value of first non-path-node (first/second child)
            //
            if (!is_first(it))
            {
                DEBUG("left");
                if (tree_type::is_only_child(it))
                    index = left.size();
                else
                {
                    // we want child that does not lie on my path
                    child = tree_type::leftmost_child(it);
                    if (lies_on_path(child, left))
                        ++child;

                    index = ind_left.at(id(child)).subforest;
                }
                left.push_back(it);
                DEBUG("sub %s", label(it));
                ind_left.at(id(it)).subforest = index;
                DEBUG("sub %s -> %lu:%s", label(it), index, label(left.at(index)));
            }
            if (!is_last(it))
            {
                DEBUG("right");
                if (tree_type::is_only_child(it))
                    index = right.size();
                else
                {
                    child = tree_type::leftmost_child(it);
                    if (lies_on_path(child, right))
                        child = child.node->next_sibling;

                    index = ind_right.at(id(child)).subforest;
                }
                right.push_back(it);
                ind_right.at(id(it)).subforest = index;
                DEBUG("sub %s -> %lu:%s", label(it), index, label(right.at(index)));
            }
            if (!is_heavy(it))
            {
                DEBUG("heavy");
                if (tree_type::is_only_child(it))
                    index = heavy.size();
                else
                {
                    child = tree_type::leftmost_child(it);
                    if (lies_on_path(child, heavy))
                        child = child.node->next_sibling;

                    index = ind_heavy.at(id(child)).subforest;
                }
                heavy.push_back(it);
                ind_heavy.at(id(it)).subforest = index;
                DEBUG("sub %s -> %lu:%s", label(it), index, label(heavy.at(index)));
            }
        }
        assert(it == --t.end_post());
        it = tree_type::first_child(it);

        left.push_back(it);
        right.push_back(it);
        heavy.push_back(it);

        ind_left.at(id(it)).subforest = 0;
        ind_right.at(id(it)).subforest = 0;
        ind_heavy.at(id(it)).subforest = 0;

        DEBUG("sub LRH %s -> <0:%s, 0:%s, 0:%s>",
                label(it), label(left.at(0)),
                label(right.at(0)), label(heavy.at(0)));
    };

    assert(t1.begin().number_of_children() == 1);
    assert(t2.begin().number_of_children() == 1);

    logger.notice("GTED: BEGIN precompute");

    // da sa nejak paralelizovat...
    // keyroots so subforests
    // ale zavisia na leafs
    logger.info("leafs_begin");
    comp_leafs(t1);
    comp_leafs(t2);
    logger.info("leafs_end");
    logger.info("keyroots_begin");
    comp_keyroots(t1, true);
    comp_keyroots(t2, false);
    logger.info("keyroots_end");
    logger.info("subforests_begin");
    comp_subforests(t1, true);
    comp_subforests(t2, false);
    logger.info("subforests_end");

    logger.notice("GTED: END precompute");

    //print_precomputed();
}



void gted::compute_distances_recursive(
                iterator_pair roots)
{
    // rozkladam pomocou keyrootov
    //
    APP_DEBUG_FNAME;

    DEBUG("pair: <%s, %s>", label(roots.it1), label(roots.it2));

    tree_type::iterator it;
    tree_type::iterator leaf;
    size_t i;
    strategy_pair str;

    str = strategies.at(id(roots.it1)).at(id(roots.it2));
    //str = rand() % 6;

    if (is_left_path(str))
        ++LEFT;
    else if (is_right_path(str))
        ++RIGHT;
    else
        ++HEAVY;

    DEBUG("strategy: %s", strategy_to_string(str).c_str());

    if (is_T2(str))
        swap(roots.it1, roots.it2);
    print_keyroots(roots.it1, str);

    i = precomputed.get_indexes(roots.it1, str).keyroot;
    if (i != BAD_INDEX)
    {
        leaf = precomputed.get_leaf(roots.it1, str);
        it = precomputed.get_keyroot(i, str);

        while(it != empty_iterator() && is_keyroot(roots.it1, it, str))
        {
            iterator_pair p = {it, roots.it2};
            if (is_T2(str))
                swap(p.it1, p.it2);
            compute_distances_recursive(p);
            it = precomputed.get_keyroot(++i, str);
        }
    }
    else
        DEBUG("BAD_INDEX");
    single_path_function(roots, str);
}

void gted::single_path_function(
                iterator_pair roots,
                strategy_pair str)
{
#define all_subforest_nodes_init(Subforest, value) \
    Subforest.left = Subforest.right = Subforest.path_node = value

    DEBUG("SPF: pair: <%s, %s>", label(roots.it1), label(roots.it2));

    subforest_pair forests;
    size_t i;
    tree_type::iterator it;

    swap(roots.it1, roots.it2);
    print_subforests(roots.it1, str);

    i = precomputed.get_indexes(roots.it1, str).subforest;
    it = precomputed.get_subforest(i, str);

    while(is_in_subtree(roots.it1, it))
    {
        all_subforest_nodes_init(forests.f1, precomputed.get_leaf(it, str));
        all_subforest_nodes_init(forests.f2, precomputed.get_leaf(roots.it2, str));
        forests.f2.last = forests.f1.last = subforest::undef;
        forests.f1.root = it;
        forests.f2.root = roots.it2;

        compute_distance(forests, str);

        ++i;
        it = precomputed.get_subforest(i, str);
    }

    all_subforest_nodes_init(forests.f1, precomputed.get_leaf(roots.it1, str));
    all_subforest_nodes_init(forests.f2, precomputed.get_leaf(roots.it2, str));
    forests.f1.root = roots.it1;
    forests.f2.root = roots.it2;
    forests.f2.last = forests.f1.last = subforest::undef;

    compute_distance(forests, str);
}




gted::forest_distance_table_type gted::compute_distance(
                subforest_pair forests,
                strategy_pair str)
{
    /*if (is_T1(str))
        logger.info("computing distance between roots <%s, %s>",
                label(forests.f2.root), label(forests.f1.root));
    else
        logger.info("computing distance between roots <%s, %s>",
                label(forests.f1.root), label(forests.f2.root));*/

    auto inited_subforest = [this](const subforest& f)
    {
        print_subforest(f);
        //cout << label(f.left) << " " <<
                //label(f.right) << " " <<
                //label(f.path_node) << " " <<
                //label(f.root) << endl;

        return f.right == f.left &&
            f.right == f.path_node &&
            tree_type::is_leaf(f.right) &&
            id(f.root) >= id(f.right);
    };

    assert(inited_subforest(forests.f1) && inited_subforest(forests.f2));

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
    fill_table(forest_dist, forests, prevs, last_tree_roots, str)

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
    init_FDist_table(forest_dist, forests);

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
        DEBUG("%s", out.str().c_str());
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
                // ak som v rodicovi, zmazem vsetkych synov ktori boli v zasobniku..
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
            del_stack_children(last_tree_roots_stacks.s1, forests.f1.right);
            last_tree_roots.it1 = get_back_f1();

            compute_fdist_command();

            prevs.f1.left =
                prevs.f1.right = forests.f1.right;
            parents.it1 = tree_type::parent(forests.f1.right);
        }

        forests.f2.last = subforest::Lnode;
        prevs.f2.left = forests.f2.left++;

        DEBUG("LNODE");
        // vynoril som sa z podstromu, ale .right este vzdy ukazuje do praveho syna...
        // Pr: 55(1, 44(2, 3)), ked prechadzam od 3ky
        //      A: pridal som uz 2, 3, som v 44.
        //      B: forest.left = 1, forest.right = 44
        //          prevs.left = 44, !!! prevs.right = 3 !!!
        if (prevs.f2.left == forests.f2.path_node)
        {
            DEBUG(".left == .path_node");
            prevs.f2.right = prevs.f2.left;
        }
        if (tree_type::is_leaf(forests.f2.left))
        {
            last_tree_roots_stacks.s2.push_back(prevs.f2.left);
            DEBUG("L2: last <- left, %s", label(prevs.f2.left));
        }

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
        else
        {
            del_stack_children(last_tree_roots_stacks.s2, forests.f2.left);
            last_tree_roots.it2 = get_back_f2();
        }
    }
    while(loop);

    return forest_dist;
}

void gted::init_FDist_table(
                forest_distance_table_type& forest_dist,
                subforest_pair forests) const
{
    //LOGGER_PRIORITY_ON_FUNCTION(INFO);
    APP_DEBUG_FNAME;

    struct
    {
        tree_type::iterator it1, it2;
    } parents, leafs;
    int i;
    size_t inner_size, outer_size;

    inner_size = t_sizes.at(id(forests.f2.root));
    outer_size = t_sizes.at(id(forests.f1.root));

    forest_distance_table_type::mapped_type inner;
    inner.reserve(inner_size);

    forest_dist.reserve(outer_size);

    assert(forest_dist.empty());

    leafs.it1 = forests.f1.right;
    leafs.it2 = forests.f2.right;
    parents.it1 = tree_type::parent(leafs.it1);
    parents.it2 = tree_type::parent(leafs.it2);

    // INIT
    DEBUG("INIT 0");
    forests.f1.left = forests.f2.left = tree_type::iterator();

#define init_command() \
    set_Fdist(forest_dist, forests.f1, forests.f2, i++)

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
            forest_dist[forests.f1].reserve(inner_size);
            init_command();
            ++forests.f1.left;
        }
        forests.f1.left = tree_type::first_child(parents.it1);
        ++forests.f1.right;

        while(forests.f1.right != parents.it1)
        {
            forest_dist[forests.f1].reserve(inner_size);
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
}

void gted::fill_table(
                forest_distance_table_type& forest_dist,
                const subforest_pair& roots,
                const subforest_pair& prevs,
                const iterator_pair& prev_roots,
                strategy_pair str)
{
    /*
     * do forest_dist zapisujem normalne v poradi [F1][F2]
     * do tree_distances zapisujem podla who_first:
     *      ak == T1, poradie je [F2][F1], inac [F1][F2]
     *
     * inac predpoklady su take, ze v prev_roots su:
     *      ak .last == undef, je tam vrchol na root-leaf ceste (tam kde som zacal)
     *      inac je tam predchadzajuci vrchol najblizsie k aktualnemu a nieje v jeho podstrome
     *          viz. poznamka v compute_distance funkcii
     */

#define GTED_VECTOR_DELETE_LEFT     0
#define GTED_VECTOR_DELETE_RIGHT    1
#define GTED_VECTOR_DELETE_BOTH     2

#define GTED_COST_MODIFY            0
#define GTED_COST_DELETE            1

#define labels_LR(s) label(s.left), label(s.right)

    vector<size_t> vec(3, _BAD);
    size_t c_min;
    // pocitam F[p.it1][p.it2]..
    iterator_pair p;
    if (roots.f1.last == subforest::Lnode)
        p.it1 = roots.f1.left;
    else
        p.it1 = roots.f1.right;
    if (roots.f2.last == subforest::Lnode)
        p.it2 = roots.f2.left;
    else
        p.it2 = roots.f2.right;

    vec[GTED_VECTOR_DELETE_LEFT]  =
        get_Fdist(forest_dist, prevs.f1, roots.f2);
    vec[GTED_VECTOR_DELETE_RIGHT] =
        get_Fdist(forest_dist, roots.f1, prevs.f2);

    vec[GTED_VECTOR_DELETE_LEFT]  += GTED_COST_DELETE;
    vec[GTED_VECTOR_DELETE_RIGHT] += GTED_COST_DELETE;

    if (p.it1 == roots.f1.path_node &&
            p.it2 == roots.f2.path_node)
    {
        DEBUG("path node");
        vec[GTED_VECTOR_DELETE_BOTH] =
            get_Fdist(forest_dist, prevs.f1, prevs.f2);

        vec[GTED_VECTOR_DELETE_BOTH] +=
            GTED_COST_MODIFY;
    }
    else
    {
        DEBUG("not path node");
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

        vec[GTED_VECTOR_DELETE_BOTH] =
            get_Fdist(forest_dist, other.f1, other.f2);

        vec[GTED_VECTOR_DELETE_BOTH] +=
            get_Tdist(t_it1, t_it2, str);
    }

    LOGGER_PRINT_CONTAINER(vec, "vec");

#ifndef GTED_CHECKS_DISABLED
    assert(find(vec.begin(), vec.end(), _BAD) == vec.end());
#endif

    c_min = *min_element(vec.begin(), vec.end());
    set_Fdist(forest_dist, roots.f1, roots.f2, c_min);

    if (p.it1 == roots.f1.path_node &&
        p.it2 == roots.f2.path_node)
    {
        set_Tdist(p.it1, p.it2, str, c_min);
    }
}




/* inline */ bool gted::is_keyroot(
                tree_type::iterator root,
                tree_type::iterator it,
                strategy_pair str) const
{
    return is_in_subtree(root, it) &&
        precomputed.get_leaf(root, str) ==
            precomputed.get_leaf(tree_type::parent(it), str);
}

/* inline */ bool gted::is_in_subtree(
                tree_type::iterator root,
                tree_type::iterator it) const
{
    return id(root) > id(it) &&
        id(it) >= id(precomputed.get_leafs(root).left);
}



#define F_str "F[%s, %s][%s, %s]"
#define T_str "T[%s][%s]"
#define labels_LR(s) label(s.left), label(s.right)

/* inline */ size_t gted::get_Fdist(
                forest_distance_table_type& forest_dist,
                const subforest& index1,
                const subforest& index2) const
{
    DEBUG("GET: " F_str,
            labels_LR(index1), labels_LR(index2));

    return forest_dist[index1][index2];
}

/* inline */ void gted::set_Fdist(
                forest_distance_table_type& forest_dist,
                const subforest& index1,
                const subforest& index2,
                size_t value) const
{
    DEBUG("SET: " F_str "= %lu",
            labels_LR(index1), labels_LR(index2), value);
    forest_dist[index1][index2] = value;
}

/* inline */ size_t gted::get_Tdist(
                tree_type::iterator it1,
                tree_type::iterator it2,
                strategy_pair str)
{
    assert(is_path_strategy(str));

    if (is_T1(str))
        swap(it1, it2);
    DEBUG("GET: T[%s:%lu][%s:%lu]",
            label(it1), id(it1),
            label(it2), id(it2));

    if (tree_distances.at(id(it1)).at(id(it2)) == _BAD)
    {
        logger.error("GET: T[%s:%lu][%s:%lu] == %lu",
                label(it1), id(it1),
                label(it2), id(it2),
                _BAD);
        exit(1);
    }

    return tree_distances.at(id(it1)).at(id(it2));
}

/* inline */ void gted::set_Tdist(
                tree_type::iterator it1,
                tree_type::iterator it2,
                strategy_pair str,
                size_t value)
{
    assert(is_path_strategy(str));

    if (is_T1(str))
        swap(it1, it2);
    DEBUG("SET: T[%s:%lu][%s:%lu] = %lu",
            label(it1), id(it1),
            label(it2), id(it2),
            value);
    tree_distances.at(id(it1)).at(id(it2)) = value;
}




/* inline */ void gted::print_TDist() const
{
    APP_DEBUG_FNAME;

    stringstream out;
    bool was_catch = false;
    int odsadenie = 22;
    out << "TREE_DISTANCE_TABLE:" << endl;
    out << setw(odsadenie) << "";
    for (auto it2 = t2.begin_post(); it2 != --t2.end_post(); ++it2)
        out << setw(odsadenie) << std::left << *it2;
    out << endl;
    for (auto it1 = t1.begin_post(); it1 != --t1.end_post(); ++it1)
    {
        out << setw(odsadenie) << std::left << *it1;
        for (auto it2 = t2.begin_post(); it2 != --t2.end_post(); ++it2)
        {
            try
            {
                out << std::left << setw(odsadenie) << tree_distances.at(it1->get_id()).at(it2->get_id());
            } catch (...)
            {
                was_catch = true;
                try
                {
                    // mozno su len vymenene indexy... 
                    out << std::left << setw(odsadenie) << tree_distances.at(it2->get_id()).at(it1->get_id());
                }
                catch(...) {}
            }
        }
        out << endl;
    }
    logger.infoStream() << out.str();
    //cerr << out.str();
    if (was_catch)
        logger.warn("CATCH in TDist, mozno vymenene indexy v TDist");
}

/* inline */ void gted::print_FDist(
                const forest_distance_table_type& forest_dist) const
{
#ifndef LOGGER_DEBUG_GTED_PRINT_TABLES
    return;
#endif

    //APP_DEBUG_FNAME;

    int odsadenie = 15;
    stringstream out;
    out << "FOREST_DISTANCE_TABLE:" << endl;

    for (auto val : forest_dist)
    {
        string s = string() +  "[" + label(val.first.it1) + "," + label(val.first.it2) + "]";
        out << setw(odsadenie) << "";
        out << s << endl;
        for (auto v2 : val.second)
        {
            s = string() + "[" + label(v2.first.it1) + "," + label(v2.first.it2) + "]";
            out << setw(odsadenie) << s;
            out << v2.second << "\t";
            out << endl;
        }
        out << endl;
    }
    logger.debugStream() << out.str();
}




/* inline */ void gted::print_precomputed() const
{
#ifndef LOGGER_DEBUG_GTED_PRINT_TABLES
    return;
#endif

    auto print_vectors = [](const tables::vector_type& vec)
    {
        stringstream out;
        for (auto val : vec)
            out << label(val) << " ";
        DEBUG("%s", out.str().c_str());
    };
    auto print_keyroots = [&](strategy_pair str, size_t indexes)
    {
        auto& tbl = precomputed.keyroots[str];
        auto& t = (is_T1(str) ? t1 : t2);

        stringstream out;
        out << "keyroots:" << strategy_to_string(str) << endl;
        for (auto val : precomputed.indexes.at(indexes))
        {
            stringstream s;
            size_t j = val.second.keyroot;

            try
            {
                s << label(t.find(val.first)) << ":\t";
                if (j == BAD_INDEX)
                    s << "<null>";
                while(j < tbl.size())
                {
                    if (is_keyroot(t.find(val.first), tbl.at(j), str))
                        s << label(tbl.at(j)) << " ";
                    ++j;
                }
                out << s.str() << endl;
                //out << (string(label(t.find(val.first))) + s.str()) << endl;
            }
            catch(...){}
        }
        DEBUG("%s", out.str().c_str());
        print_vectors(tbl);
    };
    auto print_subforests = [&](strategy_pair str, size_t indexes)
    {
        auto& tbl = precomputed.subforests[str];
        auto& t = (is_T1(str) ? t1 : t2);

        stringstream out;
        out << "subforests:" << strategy_to_string(str) << endl;
        for (auto val : precomputed.indexes.at(indexes))
        {
            stringstream s;
            size_t j = val.second.subforest;

            try
            {
                s << label(t.find(val.first)) << ":\t";
                if (j == BAD_INDEX)
                    s << "<null>";
                while(j < tbl.size())
                {
                    if (is_in_subtree(t.find(val.first), tbl.at(j)))
                        s << label(tbl.at(j)) << " ";
                    ++j;
                }
                out << s.str() << endl;
            }
            catch(...){}
        }
        DEBUG("%s", out.str().c_str());
        print_vectors(tbl);
    };

    DEBUG("SUBFORESTS:");
    print_subforests(PATH_STRATEGY_LEFT_T1, INDEXES_LEFT);
    print_subforests(PATH_STRATEGY_LEFT_T2, INDEXES_LEFT);
    print_subforests(PATH_STRATEGY_RIGHT_T1, INDEXES_RIGHT);
    print_subforests(PATH_STRATEGY_RIGHT_T2, INDEXES_RIGHT);
    print_subforests(PATH_STRATEGY_HEAVY_T1, INDEXES_HEAVY);
    print_subforests(PATH_STRATEGY_HEAVY_T2, INDEXES_HEAVY);

    DEBUG("KEYROOTS:");
    print_keyroots(PATH_STRATEGY_LEFT_T1, INDEXES_LEFT);
    print_keyroots(PATH_STRATEGY_LEFT_T2, INDEXES_LEFT);
    print_keyroots(PATH_STRATEGY_RIGHT_T1, INDEXES_RIGHT);
    print_keyroots(PATH_STRATEGY_RIGHT_T2, INDEXES_RIGHT);
    print_keyroots(PATH_STRATEGY_HEAVY_T1, INDEXES_HEAVY);
    print_keyroots(PATH_STRATEGY_HEAVY_T2, INDEXES_HEAVY);

}

/* inline */ void gted::print_subforests(
                tree_type::iterator root,
                strategy_pair str) const
{
#ifndef LOGGER_DEBUG_GTED_PRINT_TABLES
    return;
#endif

    size_t i = precomputed.get_indexes(root, str).subforest;
    tree_type::iterator it;

    stringstream s;
    s << "subforests of: " << label(root) << ":\t";
    while (i != BAD_INDEX &&
            (it = precomputed.get_subforest(i, str)) != empty_iterator() &&
            is_in_subtree(root, it))
    {
        s << label(it) << " ";
        ++i;
    }
    DEBUG("%s", s.str().c_str());
}

/* inline */ void gted::print_keyroots(
                tree_type::iterator root,
                strategy_pair str) const
{
#ifndef LOGGER_DEBUG_GTED_PRINT_TABLES
    return;
#endif

    size_t i = precomputed.get_indexes(root, str).keyroot;
    tree_type::iterator it;

    stringstream s;
    s << "keyroots of: " << label(root) << ":\t";
    while (i != BAD_INDEX &&
            (it = precomputed.get_keyroot(i, str)) != empty_iterator() &&
            is_keyroot(root, it, str))
    {
        s << label(it) << " ";
        ++i;
    }
    DEBUG("%s", s.str().c_str());
}

/* inline */ void gted::print_subforest(
                const subforest& f)
{
#ifndef LOGGER_DEBUG_GTED_PRINT_TABLES
    return;
#endif

    DEBUG("[%s, %s, %s, %s]", label(f.left), label(f.right),
            label(f.path_node), label(f.root));
}



#ifdef NODEF
void gted::compute_mapping()
{
    // TODO!!!
    APP_DEBUG_FNAME;
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    using iterator = tree_type::iterator;

    print_TDist();

    forest_distance_table_type forest_dist;
    vector<iterator_pair> to_be_matched;
    iterator_pair current, leafs;
    subforest_pair forests, prevs;

    auto equal_funct = [&forest_dist](
            const subforest& first1, const subforest& first2,
            const subforest& second1, const subforest& second2,
            size_t plus_value)
    {
        return forest_dist.at(first1).at(first2) ==
            forest_dist.at(second1).at(second2) + plus_value;
    };
    auto print_mapping = [this]()
    {
        cout << "MAPPING:" << endl;
        for (auto val : mapping)
            cout << label(val.it1) << "\t" << label(val.it2) << endl;
    };
    auto comp_funct = [this](iterator root1, iterator root2)
    {
        LOGGER_PRIORITY_ON_FUNCTION(INFO);
        subforest_pair forests;
        // init:
        forests.f1.root = root1;
        forests.f2.root = root2;
        all_subforest_nodes_init(forests.f1, tree_type::leftmost_child(root1));
        all_subforest_nodes_init(forests.f2, tree_type::leftmost_child(root2));
        // POZOR: ^^ nieje to ekvivalentne volaniu init_subforest_pair..

        print_subforest(forests.f1);
        print_subforest(forests.f2);
        auto Ttable = tree_distances;

        logger.info("COMPFUNCT");
        auto table = compute_distance(forests, T2);

        // skontroluj ci sa nezmenila tabulka tree_distances
        //assert(tree_distances == Ttable);
        logger.info("COMPFUNCT_END");
        return table;
    };
    auto step = [](subforest& s)
    {
        subforest other = s;
        if (s.left == s.right)
        {   // som vo vrchole na ceste... musim ist o patro nizsie
            // ak som ale v liste, iba vyhodim .left
            if(tree_type::is_leaf(s.right))
                s.left = empty_iterator();
            else
            {
                s.left  = tree_type::first_child(s.right);
                s.right = tree_type::last_child (s.right);
            }
        }
        else if (s.left != empty_iterator())
        {   // inac idem iba s .right v post_order dozadu
            --s.right;
        }
        DEBUG("changing from [%s, %s], to [%s, %s]",
                label(other.left), label(other.right),
                label(s.left), label(s.right));
    };
    auto jump_tree = [this](subforest& s, iterator leaf)
    {
        DEBUG("jump, %s", label(s.right));
        print_subforest(s);
        subforest other = s;
        if (s.right == leaf)
        {
            DEBUG(".right == leaf");
            s.left = empty_iterator();
        }
        else if (s.right->is_root())
        {
            // TODO:
            // tu treba rovno zmenit aj prevs aj forests. 
            // oba na (<null>, leaf)
            // !!!
            DEBUG("is_root");
            logger.error("is_root, not implemented yet");
            cerr << "isroot" << endl;
            abort();
        }
        else
        {
            DEBUG("get ancestor previous sibling");
            while(tree_type::is_first_child(s.right))
                s.right = tree_type::parent(s.right);
            tree_type::sibling_iterator it = s.right;
            --it;
            s.right = it;
        }
        DEBUG("changing from [%s, %s], to [%s, %s]",
                label(other.left), label(other.right),
                label(s.left), label(s.right));
    };

    mapping.clear();
    to_be_matched.push_back({++t1.begin(), ++t2.begin()});


    while(!to_be_matched.empty())
    {
        DEBUG("WHILE");

        current = to_be_matched.back();
        to_be_matched.pop_back();
        forest_dist = comp_funct(current.it1, current.it2);
        //print_FDist(forest_dist);

        leafs.it1 = tree_type::leftmost_child(current.it1);
        leafs.it2 = tree_type::leftmost_child(current.it2);

        forests.f1.left =
            forests.f1.right = current.it1;
        forests.f2.left =
            forests.f2.right = current.it2;

        if (tree_type::is_leaf(forests.f1.right))
            prevs.f1.right = forests.f1.right;
            // prevs.f1.left == empty_iterator()
        else
        {
            prevs.f1.left  = tree_type::first_child(forests.f1.left );
            prevs.f1.right = tree_type::last_child (forests.f1.right);
        }
        if (tree_type::is_leaf(forests.f2.right))
            prevs.f2.right = forests.f2.right;
            // prevs.f2.left == empty_iterator()
        else
        {
            prevs.f2.left  = tree_type::first_child(forests.f2.left );
            prevs.f2.right = tree_type::last_child (forests.f2.right);
        }

        bool loop = true;
        while (loop)
        {
            DEBUG("FORESTS: [%s, %s][%s, %s]",
                    label(forests.f1.left), label(forests.f1.right),
                    label(forests.f2.left), label(forests.f2.right));
            DEBUG("PREVS: [%s, %s][%s, %s]",
                    label(prevs.f1.left), label(prevs.f1.right),
                    label(prevs.f2.left), label(prevs.f2.right));

            // uz som v najlavejsom liste (na ceste)
            // tak este dopocitaj toto a skonci.
            if (prevs.f1.left == empty_iterator() &&
                    prevs.f2.left == empty_iterator())
                loop = false;

            // samotny vypocet mapovania:
            if (forests.f1.right == forests.f1.left &&
                    forests.f2.right == forests.f2.left)
            {   // edit:
                // som na ceste... takze mozem dane nody namatchovat na seba
                DEBUG("map %s, %s", label(forests.f1.right), label(forests.f2.right));
                mapping.push_back({forests.f1.right, forests.f2.right});
                forests = prevs;
                step(prevs.f1);
                step(prevs.f2);
            }
            else if (equal_funct(forests.f1, forests.f2, forests.f1, prevs.f2, GTED_COST_DELETE))
            {   // delete node from F1
                DEBUG("map <null>, %s", label(forests.f2.right));
                mapping.push_back({empty_iterator(), forests.f2.right});
                forests.f2 = prevs.f2;
                step(prevs.f2);
            }
            else if (equal_funct(forests.f1, forests.f2, prevs.f1, forests.f2, GTED_COST_DELETE))
            {   // delete node from F2
                DEBUG("map %s, <null>", label(forests.f1.right));
                mapping.push_back({forests.f1.right, empty_iterator()});
                forests.f1 = prevs.f1;
                step(prevs.f1);
            }
            else
            {   // inac musim vyratat a namatchovat dane stromy rekurzivne
                DEBUG("to match %s, %s", label(forests.f1.right), label(forests.f2.right));
                to_be_matched.push_back({forests.f1.right, forests.f2.right});
                jump_tree(forests.f1, leafs.it1);
                jump_tree(forests.f2, leafs.it2);
                prevs = forests;
                step(prevs.f1);
                step(prevs.f2);
            }
        }
    }
    print_mapping();
}
#endif





/* inline */ bool gted::iterator_pair::operator==(
                const gted::iterator_pair& other) const
{
    return it1 == other.it1 &&
        it2 == other.it2;
}

/* inline */ size_t gted::iterator_pair::hash::operator()(
                const gted::iterator_pair& s) const
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);
    // zdroj hash funkcie:
    // http://stackoverflow.com/questions/682438/hash-function-providing-unique-uint-from-an-integer-coordinate-pair
    auto hash_f = [](size_t x, size_t y) 
    {
        return (x * 0x1F1F1F1F) ^ y;
    };
    assert(s.it2 != empty_iterator());

    size_t out;
    if (s.it1 == s.it2 || s.it1 == empty_iterator())
        out = s.it2->get_id();
    else
        out = hash_f(s.it1->get_id(), s.it2->get_id());

#ifdef LOGGER_DEBUG_GTED_HASH
    static vector<iterator_pair> hashes;
    if (find(hashes.begin(), hashes.end(), s) == hashes.end())
    {
        hashes.push_back(s);
        DEBUG("HASH [%s, %s] = %i", label(s.it1), label(s.it2), out);
    }
#endif
    return out;
}




gted::tables::tables()
{
    subforests.resize(6);
    keyroots.resize(6);
    indexes.resize(3);
}

/* inline */ gted::tables::indexes_pair_type gted::tables::get_indexes(
                const tree_type::iterator& it,
                strategy_pair str) const
{
    //APP_DEBUG_FNAME;
    assert(is_path_strategy(str));

    size_t it_id = id(it);
    if (is_left_path(str))
        return indexes.at(INDEXES_LEFT).at(it_id);
    if (is_right_path(str))
        return indexes.at(INDEXES_RIGHT).at(it_id);
    return indexes.at(INDEXES_HEAVY).at(it_id);
}

/* inline */ gted::tree_type::iterator gted::tables::get_leaf(
                const tree_type::iterator& it,
                strategy_pair str) const
{
    //APP_DEBUG_FNAME;
    assert(is_path_strategy(str));

    LRH lrh = get_leafs(it);
    if (is_left_path(str))
        return lrh.left;
    if (is_right_path(str))
        return lrh.right;
    return lrh.heavy;
}

/* inline */ gted::tables::LRH gted::tables::get_leafs(
                const tree_type::iterator& it) const
{
    //APP_DEBUG_FNAME;

    return leafs.at(id(it));
}

/* inline */ gted::tree_type::iterator gted::tables::get_subforest(
                size_t index,
                strategy_pair str) const
{
    //APP_DEBUG_FNAME;
    assert(is_path_strategy(str));

    static vector<strategy_pair> vec = {
        PATH_STRATEGY_LEFT_T1,
        PATH_STRATEGY_RIGHT_T1,
        PATH_STRATEGY_HEAVY_T1,
        PATH_STRATEGY_LEFT_T2,
        PATH_STRATEGY_RIGHT_T2,
        PATH_STRATEGY_HEAVY_T2
    };
    static vector<strategy_pair> swapped = {
        PATH_STRATEGY_LEFT_T2,
        PATH_STRATEGY_RIGHT_T2,
        PATH_STRATEGY_HEAVY_T2,
        PATH_STRATEGY_LEFT_T1,
        PATH_STRATEGY_RIGHT_T1,
        PATH_STRATEGY_HEAVY_T1
    };

    size_t i = std::distance(vec.begin(), find(vec.begin(), vec.end(), str));
    str = swapped.at(i);
/*
    if (is_T1(str))
    {
        if (str == PATH_STRATEGY_LEFT_T1)
            str = PATH_STRATEGY_LEFT_T2;
        else if (str == PATH_STRATEGY_RIGHT_T1)
            str = PATH_STRATEGY_RIGHT_T2;
        else
            str = PATH_STRATEGY_HEAVY_T2;
    }
    else
    {
        if (str == PATH_STRATEGY_LEFT_T2)
            str = PATH_STRATEGY_LEFT_T1;
        else if (str == PATH_STRATEGY_RIGHT_T2)
            str = PATH_STRATEGY_RIGHT_T1;
        else
            str = PATH_STRATEGY_HEAVY_T1;
    }
*/
    return subforests.at(str).at(index);
}

/* inline */ gted::tree_type::iterator gted::tables::get_keyroot(
                size_t index,
                strategy_pair str) const
{
    //APP_DEBUG_FNAME;
    assert(is_path_strategy(str));

    if (keyroots.at(str).size() > index)
        return keyroots.at(str).at(index);

    return empty_iterator();
}




