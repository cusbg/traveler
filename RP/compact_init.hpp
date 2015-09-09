/*
 * File: compact_init.hpp
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

#ifndef COMPACT_INIT_HPP
#define COMPACT_INIT_HPP

#include "compact_maker.hpp"

struct compact::init
{
    static void init_points(
                intervals in);

    static void check_distance(
                const intervals& in);

    /*
     * normalize distance between base pairs
     * from     G --- C
     * to       G - C
     */
    static void normalize_pair_distance(
                iterator it);

    /*
     * shift all nodes in it-subtree
     */
    static void shift_branch(
                iterator it,
                Point vector);
    static void shift_branch_angle(
                iterator from,
                double alpha);
    static void set_distance(
                iterator parent,
                iterator child,
                double dist);

private:
    static void check_multibranch_loop(
                intervals in);
    static void check_interior_loop(
                intervals in);
};



#endif /* !COMPACT_INIT_HPP */

