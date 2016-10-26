/*
 * File: tree_base_node.cpp
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


#include "tree_base_node.hpp"
#include "types.hpp"

using namespace std;

/* static */
size_t node_base::ID = 0;

size_t node_base::id() const
{
    return _id;
}

void node_base::reset_id()
{
    _id = ID++;
}

/* static */
void node_base::reset_ID()
{
    INFO("Reseting tree IDs");

    ID = 0;
}

