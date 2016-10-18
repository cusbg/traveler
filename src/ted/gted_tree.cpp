/*
 * File: gted_tree.cpp
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */


#include "gted_tree.hpp"

using namespace std;

#define heavy_child(iter)   (heavy_children[id(iter)])
#define insert(from, to)    ((to).insert((to).end(), (from).begin(), (from).end()))

gted_tree::gted_tree(
                const rna_tree& t)
    : rna_tree(t)
{
    init();
}

void gted_tree::init()
{
    APP_DEBUG_FNAME;
    assert(id(begin()) == size() - 1);

    size_t s = size();
    post_order_iterator it;
    sibling_iterator ch;
    LRH lrh;

    sizes.resize(s, 0);
    leafs.resize(s);
    heavy_children.resize(s);
    keyroots.resize(s);
    subforests.resize(s);

    for (it = begin_post(); it != end_post(); ++it)
    {
        // compute sizes
        sizes[id(it)] = 1;
        for (ch = it.begin(); ch != it.end(); ++ch)
            sizes[id(it)] += sizes[id(ch)];

        // compute heavy_child
        if (!is_leaf(it))
        {
            iterator h;
            size_t max;

            ch = it.begin();
            max = sizes[id(ch)];
            h = ch;

            for (; ch != it.end(); ++ch)
                if (sizes[id(ch)] > max)
                {
                    h = ch;
                    max = sizes[id(ch)];
                }
            heavy_child(it) = h;
        }

        // compute leafs
        if (is_leaf(it))
        {
            lrh.left =
                lrh.right =
                lrh.heavy = it;
        }
        else
        {
            lrh.left = leafs[id(first_child(it))].left;
            lrh.right = leafs[id(last_child(it))].right;
            lrh.heavy = leafs[id(heavy_child(it))].heavy;
        }
        leafs[id(it)] = lrh;

        // compute keyroots && subforests
        //
        // (iter == keyroot(parent)) <=>
        //      (iter == child(parent) && !lie_on_path(iter))
        // (iter == subforest(parent)) <=>
        //      ((iter == subforest(child(parent))) ||
        //      (iter == child(parent) && !lie_on_path(iter)))
        for (ch = it.begin(); ch != it.end(); ++ch)
        {
            auto &subforest = subforests[id(it)];
            auto &keyroot   = keyroots[id(it)];
            size_t chid = id(ch);

            insert(subforests[chid].left,  subforest.left);
            insert(subforests[chid].right, subforest.right);
            insert(subforests[chid].heavy, subforest.heavy);

            if (!is_left(ch))
            {
                keyroot.left.push_back(ch);
                subforest.left.push_back(ch);
            }
            else
                insert(keyroots[chid].left, keyroot.left);

            if (!is_right(ch))
            {
                keyroot.right.push_back(ch);
                subforest.right.push_back(ch);
            }
            else
                insert(keyroots[chid].right, keyroot.right);

            if (!is_heavy(ch))
            {
                keyroot.heavy.push_back(ch);
                subforest.heavy.push_back(ch);
            }
            else
                insert(keyroots[chid].heavy, keyroot.heavy);
        }
    }

    assert(size() == get_size(begin()));
}



