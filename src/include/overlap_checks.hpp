/*
 * File: overlap_checks.hpp
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

#ifndef OVERLAP_CHECKS_HPP
#define OVERLAP_CHECKS_HPP

#include <vector>
#include "point.hpp"
#include "rna_tree.hpp"

class overlap_checks
{
public:
    struct edge
    {
        point p1, p2;
    };
    struct overlapping
    {
        point centre;
        double radius;
    };
    
    typedef std::vector<edge> edges;
    typedef std::vector<overlapping> overlaps;
    
public:
    overlap_checks();
    
    /**
     * run overlap checks
     */
    overlaps run(
                 rna_tree& _rna);
    
    static edges get_edges(const rna_tree::iterator& node);
    static overlaps get_overlaps(const edges &e1, const edges &e2);
    
private:
    /**
     * create edges of rna
     * for tree: (12(3, 4, 5))
     *      where 12 is pair node (with labels 1 and 2) and 3,4,5 are leafs
     *      created edges:
     *          (1,3),(3,4),(4,5),(5,2)
     *          => going pre_post_order in tree
     */
    edges get_edges(
                    rna_tree& rna);
    
    /**
     * run checks for edges
     */
    overlaps run(
                 const edges& e);
    
#ifdef TESTS
public:
#endif
    /**
     * find point in which edges are intersecting each other
     * if no point exist, return point::bad_point
     */
    static point intersection(
                              const edge& e1,
                              const edge& e2);
    
};

#endif /* !OVERLAP_CHECKS_HPP */
