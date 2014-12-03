/*
 * File: gted.hpp
 *
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
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

#ifndef GTED_HPP
#define GTED_HPP

#include "types.hpp"
#include "rna_tree.hpp"
#include "rted.hpp"


//template <typename tree_type>
class gted
{
    typedef tree_base<node_base<std::string>> tree_type;
    typedef std::vector<tree_type::iterator> relevant_subtrees;
private:
    tree_type t1;
    tree_type t2;
    typename rted<tree_type>::strategy_map_type strategies;
public:
    gted(const tree_type& _t1, const tree_type& _t2,
            const typename rted<tree_type>::strategy_map_type& _strategies);
    void run_gted();

private:
    relevant_subtrees path_decomposition(tree_type::iterator it,
            path_strategy s);
    relevant_subtrees left_decomposition(tree_type::iterator it);
    relevant_subtrees right_decomposition(tree_type::iterator it);
};





#endif /* !GTED_HPP */

