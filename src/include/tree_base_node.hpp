/*
 * File: tree_base_node.hpp
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

#ifndef TREE_BASE_NODE_HPP
#define TREE_BASE_NODE_HPP

#include <cstddef>

#define TREE_BASE_NODE_NOT_MAPPED_ID 0

class node_base
{
public:

    virtual ~node_base() = default;
protected:
    node_base() = default;
    
public:
    size_t id() const;
    void reset_id();
    size_t seq_id_mapped();
    void set_seq_id_mapped(size_t id);
    static void reset_ID();

private:
    static size_t ID;
protected:
    size_t _id = ID++;
    size_t _seq_id_mapped = TREE_BASE_NODE_NOT_MAPPED_ID; //used to store sequence position of the matched residue (if any) in template
};

#endif /* !TREE_BASE_NODE_HPP */
