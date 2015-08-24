/*
 * File: checks.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard@ba30.eu>
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

#ifndef CHECKS_HPP
#define CHECKS_HPP

#include "types.hpp"

class rna_tree;

class crossing_check
{
private:
    struct edge;
    typedef std::vector<crossing_check::edge> edges;
public:
//private:
    std::vector<edge> split_to_edges(rna_tree rna);
    bool intersect(rna_tree rna);

private:
    bool intersect(edge e1, edge e2);
};



#endif /* !CHECKS_HPP */

