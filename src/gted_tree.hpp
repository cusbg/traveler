/*
 * File: gted_tree.hpp
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

#ifndef GTED_TREE_HPP
#define GTED_TREE_HPP

#include "types.hpp"
#include "rna_tree.hpp"

struct gted_tree
    : public rna_tree
{
public:
    struct LRH
    {
        iterator left, right, heavy;
    };
    struct LRH_table
    {
        std::vector<iterator> left, right, heavy;
    };

public:
    gted_tree(const rna_tree& t);
    void init();

private:
    inline size_t id(
                iterator it) const
    {
        return ::id(it);
    }

public:
    inline void check_same_tree(
                iterator it) const
    {
        while (!is_root(it))
            it = parent(it);
        assert(_tree.begin() == it);
    }
public:
    inline iterator get_heavy_child(
                const iterator& it) const
    {
        check_same_tree(it);
        return heavy_children[id(it)];
    }
    inline size_t get_size(
                const iterator& it) const
    {
        check_same_tree(it);
        return sizes[id(it)];
    }
    inline LRH get_leafs(
                const iterator& it) const
    {
        check_same_tree(it);
        return leafs[id(it)];
    }
    inline LRH_table get_keyroots(
                const iterator& it) const
    {
        check_same_tree(it);
        return keyroots[id(it)];
    }
    inline LRH_table get_subforests(
                const iterator& it) const
    {
        check_same_tree(it);
        return subforests[id(it)];
    }

public:
    inline bool is_left(
                iterator it) const
    {
        return is_first_child(it);
    }
    inline bool is_right(
                iterator it) const
    {
        return is_last_child(it);
    }
    inline bool is_heavy(
                iterator it) const
    {
        return is_root(it) ||
            get_heavy_child(parent(it)) == it;
    }

private:
    std::vector<iterator> heavy_children;
    std::vector<size_t> sizes;
    std::vector<LRH> leafs;
    std::vector<LRH_table> keyroots;
    std::vector<LRH_table> subforests;
};

#endif /* !GTED_TREE_HPP */

