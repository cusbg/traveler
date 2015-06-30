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

#include "util.hpp"
#include "point.hpp"


class compact
{
public: //TODO: remove
    using iterator = rna_tree::iterator;
    using sibling_iterator = rna_tree::sibling_iterator;
    struct interval;
    struct circle;
public:
    compact(const document& _doc) : doc(_doc) {}
    void make_compact();

private:
    void shift_nodes(iterator it, Point vector);

    void init();
    void init_points(iterator it);
    void normalize_pair_distance(iterator it);

    void make_inserted();
    void make_deleted();

    void make_pairs();
    void make(std::vector<interval> vec);

    void normalize_branch_distance(iterator parent, iterator child, size_t bases_count);

    void reinsert(interval i, circle& c);

private:
    document doc;
};

#define branches_count(_iter) \
    (count_children_if(_iter, [](iterator _sib) {return _sib->get_label().is_paired();}))

#endif /* !COMPACT_MAKER_HPP */

