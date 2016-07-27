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

#include "rna_tree.hpp"

/**
 * precompute and store some frequently needed values in gted
 */
struct gted_tree : public rna_tree
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
    gted_tree(
                const rna_tree& t);

    /**
     * initialize all table values
     */
    void init();

private:
    inline size_t id(
                iterator it) const
    {
        // should be here, because of tree_base::id() function without arguments
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
    /**
     * returns `it`s heavy child
     */
    inline iterator get_heavy_child(
                const iterator& it) const
    {
        check_same_tree(it);
        return heavy_children[id(it)];
    }

    /**
     * returns `it`s subtree size
     */
    inline size_t get_size(
                const iterator& it) const
    {
        check_same_tree(it);
        return sizes[id(it)];
    }

    /**
     * returns `it`s leafs (left/right/heavy)
     */
    inline LRH get_leafs(
                const iterator& it) const
    {
        check_same_tree(it);
        return leafs[id(it)];
    }

    /**
     * returns `it`s keyroots
     */
    inline LRH_table get_keyroots(
                const iterator& it) const
    {
        check_same_tree(it);
        return keyroots[id(it)];
    }

    /**
     * returns `it`s subforests
     */
    inline LRH_table get_subforests(
                const iterator& it) const
    {
        check_same_tree(it);
        return subforests[id(it)];
    }

    /**
     * returns if `it` is left child
     */
    inline bool is_left(
                iterator it) const
    {
        return is_first_child(it);
    }

    /**
     * returns if `it` is right child
     */
    inline bool is_right(
                iterator it) const
    {
        return is_last_child(it);
    }

    /**
     * returns if `it` is heavy child
     */
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

