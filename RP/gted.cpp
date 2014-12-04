/*
 * File: gted.cpp
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

#include "gted.hpp"
#include "tree_hh/tree_util.hh"

using namespace std;

gted::gted(const tree_type& _t1, const tree_type& _t2, const rted::strategy_map_type& _strategies)
    : t1(_t1), t2(_t2), strategies(_strategies)
{}

void gted::run_gted()
{
    relevant_subtrees vec;

    vec.push_back(++t1.tree_ptr->begin());
    while (!vec.empty())
        vec = path_decomposition(vec[0], path_strategy::right);
}

gted::relevant_subtrees gted::path_decomposition(tree_type::iterator root,
            path_strategy s)
{
    APP_DEBUG_FNAME;

    stringstream stream;
    stream << "TREE: ";
    kptree::print_subtree_bracketed(*t1.tree_ptr, root, stream);
    logger.debug(stream.str());


    if (s == path_strategy::left)
        return left_decomposition(root);
    if (s == path_strategy::right)
        return right_decomposition(root);

    return relevant_subtrees();
}

gted::relevant_subtrees gted::left_decomposition(tree_type::iterator root)
{
    tree_type::iterator it(root);
    relevant_subtrees subtrees;

    while (!tree_type::is_leaf(it))
    {
        logger.debug("going down the path node '%s'", label(it));

        tree_type::sibling_iterator sib = ++tree_type::first_child(it);
        while (sib != sib.end())
        {
            logger.debug("adding '%s' to relevat subtrees", label(sib));

            subtrees.push_back(sib++);
        }
        it = tree_type::child(it, 0);
    }
    
    return subtrees;
}

gted::relevant_subtrees gted::right_decomposition(tree_type::iterator root)
{
    tree_type::iterator it(root);
    relevant_subtrees subtrees;

    while (!tree_type::is_leaf(it))
    {
        logger.debug("going down the path node '%s'", label(it));

        tree_type::sibling_iterator sib = tree_type::last_child(it);

        if (sib == sib.begin()) // ! moze nastat --sib < .begin()
            continue;

        do
        {
            --sib;
            logger.debug("adding '%s' to relevat subtrees", label(sib));

            subtrees.push_back(sib);
        }
        while (!tree_type::is_first_child(sib));

        it = tree_type::last_child(it);
    }
    
    return subtrees;
}


