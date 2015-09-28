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
#define PAIRS_DISTANCE_PRECISION    1
#define BASES_DISTANCE              8
#define BASES_RATIO                 1.4

class compact
{
public:
    typedef rna_tree::iterator                  iterator;
    typedef rna_tree::sibling_iterator          sibling_iterator;

//private:
    struct circle;

public:
    compact(
                const rna_tree& _rna);
    void run();

private:
    // INIT functions:
    void init();
    static void shift_branch(
                iterator parent,
                point vec);
    static void rotate_branch(
                iterator parent,
                double angle);

    sibling_iterator get_onlyone_branch(
                sibling_iterator it);

    point direction_vector_from(
                iterator it);
    sibling_iterator get_prev(
                sibling_iterator it);
    sibling_iterator get_next(
                sibling_iterator it);

    point init_branch_recursive(
                sibling_iterator it,
                point from);
    void adjust_branch(
                sibling_iterator it);


private:
    void init_branches();
private:
    rna_tree rna;
};



#endif /* !COMPACT_HPP */

