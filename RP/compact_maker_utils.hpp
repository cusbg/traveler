/*
 * File: compact_maker_utils.hpp
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

#ifndef COMPACT_MAKER_UTILS_HPP
#define COMPACT_MAKER_UTILS_HPP

#include "compact_maker.hpp"
#include "compact_init.hpp"
#include "compact_maker_utils.hpp"
#include "compact_circle.hpp"

struct compact::intervals
{
    struct interval;
    enum rna_structure_type
    {
        hairpin,
        interior_loop,
        multibranch_loop
    };

    void create(iterator it);
    Point get_circle_direction() const;

    std::vector<interval> vec;
    rna_structure_type type;
};

struct compact::intervals::interval
{
    void print();
    Point get_circle_direction();

public:
    char b_index, e_index;
    sibling_iterator begin, end;
    std::vector<iterator> vec;
    bool remake = false;
};

template <>
        void compact::redraw<compact::intervals::interval>(intervals::interval in, Point dir);

Point get_direction(rna_tree::iterator it);

#endif /* !COMPACT_MAKER_UTILS_HPP */

