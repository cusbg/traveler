/*
 * File: compact_maker.hpp
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

#ifndef COMPACT_MAKER_HPP
#define COMPACT_MAKER_HPP

#include "rna_tree.hpp"
#include "point.hpp"



class compact
{
public: //TODO: remove
    using iterator = rna_tree::iterator;
    using sibling_iterator = rna_tree::sibling_iterator;
    struct interval;
    struct circle;
public:
    compact(const rna_tree& _rna) : rna(_rna) {}
    void make_compact();

private:
    void shift_nodes(iterator it, Point vector);

    void make_pairs();
    void make_inserted();

    circle make_circle(iterator i);


    size_t bases_count(iterator it);


    bool is_normalized_dist(iterator it);
    void normalize_distance(iterator it);

private:
    rna_tree rna;
};


#endif /* !COMPACT_MAKER_HPP */

