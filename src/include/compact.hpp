/*
 * File: compact.hpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard.elias@matfyz.cz>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */

#ifndef COMPACT_HPP
#define COMPACT_HPP

#include "rna_tree.hpp"

class compact
{
#ifdef TEST
public:
#endif
    struct circle;
    struct intervals;
    struct interval;

public:
    typedef rna_tree::iterator                  iterator;
    typedef rna_tree::post_order_iterator       post_order_iterator;
    typedef rna_tree::pre_post_order_iterator   pre_post_order_iterator;
    typedef rna_tree::sibling_iterator          sibling_iterator;
    typedef std::vector<point>                  points_vec;
    typedef std::vector<sibling_iterator>       nodes_vec;

public:
    compact(
                rna_tree& _rna);

    /**
     * run compact algorithm.
     * After run all nodes will be initialized
     * and layout can be visualized
     */
    void run();

private:
    /**
     * sets distance between `parent` and `child`
     */
    static void set_distance(
                iterator parent,
                iterator child,
                double distance);

    static void set_distance(
                iterator it,
                point from,
                double distance);

    /**
     * shift full subtree rooted at `parent` with vector `vec`
     */
    static void shift_branch(
                iterator parent,
                point vec);

    /**
     * rotate branch from `parent` arount circle `c` with angle `alpha`
     */
    static void rotate_branch(
                iterator parent,
                circle c,
                double alpha);

    /**
     * returns if child number `n` should be remade
     * that means, that interval
     *  (branch_prev..parent..branch_next) was modified
     *      for example branch or other leaf-node was inserted/deleted
     */
    static bool remake_child(
                iterator parent,
                size_t n);

    /**
     * return if parent-s children should be remaked
     * => parent was inserted or has any changes in his subtree
     *      (ins/del children)
     */
    static bool to_remake_children(
                iterator parent);

private:
    // INIT functions:
    /**
     * initialize all branch-nodes in trees
     * (only primitive initialization)
     */
    void init();

    /**
     * make all branches lie on straight line
     */
    void init_even_branches();

    /**
     * recursively find inited only-branch node,
     * make place for new node by shifting full subtree
     * and return direction vector with which `it` should be shifted
     * (normalized vector (inited_node->from) * DEFAULT_DISTANCE)
     */
    point init_branch_recursive(
                sibling_iterator it,
                point from);

    /**
     * init branch botton-up
     * if stem is inited in subtree, shift it and
     * make place for new node and return its position
     * otherwise return badpoint
     */
    point init_branch_recursive(
                sibling_iterator it);

    /**
     * init with parent and its parent position
     */
    void init_by_ancestor(
                sibling_iterator it);

    /**
     * set branch nodes to lie on straight line
     */
    void make_branch_even(
                sibling_iterator it);

    /**
     * lay out nodes on circle, rotate and shift branches if needed
     */
    void init_multibranch(
                sibling_iterator parent);
    // INIT ^^

private:
    /**
     * remake tree-parts when needed (insert/delete parent/sibling, ..)
     */
    void make();

    /**
     * set distances between nodes in interval
     * .. see set_distances_*_loop functions
     */
    void set_distances(
                intervals& in);

    /**
     * set distance between branch nodes
     * (distance = function(#nodes in each branch loop))
     */
    void set_distance_interior_loop(
                intervals& in);

    /**
     * we do not change any distances,
     * if #nodes between branches is > MIN
     *  split the area between all nodes
     *      for example: B1 |1|2|3*|4*|5*| B2
     *          B1, B2 - branches (inited)
     *          i* - not inited leaf nodes
     *              splits area of length (|B1,1| + |1,2| + |2,B2|)
     *              between (B1,1), (1,2), (2,3), (3,4), (4,5), (5, B2)
     *      if many  nodes are added, or many deleted, distances 
     *          could be too small/large
     *      if many nodes are added, output could be strange, because
     *          we are traveling around initialized points...
     *
     *      !!! sets interval.remake to FALSE !!!
     */
    void set_distance_multibranch_loop(
                intervals& in);

    /**
     * see set_distance_multibranch_loop
     */
    void split(
                const interval& in);

    /**
     * remake interval -> create circle for all nodes to lie on
     * and call reinsert()
     */
    void remake(
                const interval& in,
                point direction);

    /**
     * reinserts nodes to lie on circle
     */
    void reinsert(
                const points_vec& points,
                const nodes_vec& nodes);

    /**
     * return sum of distances between initialized points in interval
     * (including (begin, fist), (last, end))
     */
    double get_length(
                const interval& in);

    /**
     * check all nodes are inited
     */
    inline void checks();


private:
    // for debugging:
    friend std::ostream& operator<<(
                std::ostream& out,
                const circle& c);

private:
    rna_tree &rna;
};

#endif /* !COMPACT_HPP */

