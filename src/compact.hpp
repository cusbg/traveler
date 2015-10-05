/*
 * File: compact.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard.elias@matfyz.cz>
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

#ifndef COMPACT_HPP
#define COMPACT_HPP

#include "types.hpp"
#include "rna_tree.hpp"


#define PAIRS_DISTANCE              20
#define BASES_DISTANCE              8

class compact
{
public:
    struct circle;
    struct intervals;
    struct interval;

public:
    typedef rna_tree::iterator                  iterator;
    typedef rna_tree::sibling_iterator          sibling_iterator;
    typedef std::vector<point>                  points_vec;
    typedef std::vector<circle>                 circles_vec;
    typedef std::vector<sibling_iterator>       nodes_vec;

//private:

public:
    compact(
                const rna_tree& _rna);
    void run();

private:
    static void set_distance(
                iterator parent,
                iterator child,
                double distance);
    static void shift_branch(
                iterator parent,
                point vec);
    static void rotate_branch(
                iterator parent,
                double angle);

private:
    // INIT functions:
    void init();

    sibling_iterator get_onlyone_branch(
                sibling_iterator it);

    point init_branch_recursive(
                sibling_iterator it,
                point from);
    void even_branch(
                sibling_iterator it);
    // INIT ^^

private:
    void make();
    circles_vec get_circles(
                const intervals& in);
    void reinsert(
                const circle& c,
                const nodes_vec& nodes);
    void remake(
                const interval& in,
                point direction);
    void set_distances(
                intervals& in);
    void set_distance_interior_loop(
                intervals& in);
    void set_distance_multibranch_loop(
                intervals& in);


    double get_length(
                const interval& in);
    void split(
                interval& in);
private:
    void init_branches();

private:
    rna_tree rna;
};



#endif /* !COMPACT_HPP */

