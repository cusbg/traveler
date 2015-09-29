/*
 * File: compact_utils.hpp
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

#ifndef COMPACT_UTILS_HPP
#define COMPACT_UTILS_HPP

#include "compact.hpp"

struct compact::intervals
{
    struct interval;
    enum rna_structure_type
    {
        hairpin,
        interior_loop,
        multibranch_loop
    };

    void init(iterator parent);
    point get_circle_direction() const;

    std::vector<interval> vec;
    rna_structure_type type;
};

struct compact::intervals::interval
{
    struct
    {
        iterator it;
        bool index;
    } beg, end;
    nodes_vec vec;
    bool remake = false;

};

std::string to_string(const compact::intervals::interval& i);

#endif /* !COMPACT_UTILS_HPP */
