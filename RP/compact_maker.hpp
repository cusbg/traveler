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
    using iterator          = rna_tree::iterator;
    using sibling_iterator  = rna_tree::sibling_iterator;
    struct intervals;
    struct circle;
    struct init;
public:
    compact(const rna_tree& _rna);
    void make_compact();

private:

    template <typename interval>
        void redraw(interval in);
    void redraw(
                iterator beg,
                iterator end);
    void reinsert(
                std::vector<iterator> nodes,
                const circle& c);
    void remake(
                iterator it);





#ifdef NODEF
    /*
     * set points to lie on circle
     */
    void reinsert(
                std::vector<sibling_iterator> nodes,
                const circle& c);


    void init();
    void init_points(
                iterator it);
    bool init_points_recursive(
                iterator it);

    void make_inserted();
    void make_deleted();

    circle create(
                const interval& in);


    void remake(
                const interval& in,
                bool is_hairpin = false);
    void remake_interial_loops(
                const std::vector<interval> vec);
    void remake_multibranch_loops(
                const std::vector<interval> vec);

#endif

public:
    rna_tree& get() {return rna;}


private:
    rna_tree rna;
};

#define branches_count(_iter) \
    (count_children_if(_iter, [](iterator _sib) {return _sib->get_label().is_paired();}))

#endif /* !COMPACT_MAKER_HPP */

