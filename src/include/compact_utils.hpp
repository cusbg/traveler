/*
 * File: compact_utils.hpp
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

#ifndef COMPACT_UTILS_HPP
#define COMPACT_UTILS_HPP

#include "compact.hpp"

struct compact::intervals
{
    enum rna_structure_type
    {
        hairpin,
        interior_loop,
        multibranch_loop
    };
    
    /**
     * create intervals for `parent`-s children
     */
    void init(
              iterator parent);
    point get_circle_direction() const;
    
    std::vector<interval> vec;
    rna_structure_type type;
};

/**
 * interval between two stems
 * vector contains all non-paired bases between stems
 */
struct compact::interval
{
    struct
    {
        /**
         * which node
         */
        iterator it;
        /**
         * index of base in pair
         */
        bool index;
    } beg, end;
    nodes_vec vec;
    /**
     * if interval should be remade
     */
    bool remake = false;
};


#endif /* !COMPACT_UTILS_HPP */
