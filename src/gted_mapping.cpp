/*
 * File: gted_mapping.cpp
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

#include "gted.hpp"
#include "mapping.hpp"

using namespace std;


mapping gted::get_mapping()
{
    APP_DEBUG_FNAME;

    checks();

    typedef post_order_iterator iterator_type;
    mapping map;
    vector<pair<iterator_type,
            iterator_type>> to_be_matched;
    iterator_type root1, root2;
    iterator_type it1, it2;
    iterator_type beg1, beg2;
    const iterator_type empty;
    vector<vector<size_t>> fdist;

    auto get_begin_leaf =
        [](const tree_type& t, const iterator_type& root) {
            return t.get_leafs(root).left;
        };
    auto compute_distance_local =
        [&]() {
            LOGGER_PRIORITY_ON_FUNCTION(INFO);
            tree_distance_table_type old_tdist = tdist;
            INFO("str %s", to_cstr(actual_str));
            INFO("root1 %s, %lu, root2 %s, %lu", clabel(root1), t1.get_size(root1), clabel(root2), t2.get_size(root2));
            auto fdist = compute_distance(root1, root2);
            assert(old_tdist == tdist);
            assert(!fdist.empty());
            return fdist;
        };
#define prev(num) \
    ((it ##num == beg ##num) ? empty : --iterator_type(it ##num))
#define get_fdist(iter1, iter2) \
    get_fdist(fdist, iter1, iter2, id1, id2)

    to_be_matched.push_back({t1.begin(), t2.begin()});
    actual_str = strategy(RTED_T1_LEFT);

    while (!to_be_matched.empty())
    {
        root1 = to_be_matched.back().first;
        root2 = to_be_matched.back().second;
        to_be_matched.pop_back();

        logger.infoStream()
            << "matching roots:\n"
            << tree_type::print_subtree(root1) << "\n"
            << tree_type::print_subtree(root2);

        fdist = compute_distance_local();

        DEBUG("computed");

        beg1 = get_begin_leaf(t1, root1);
        beg2 = get_begin_leaf(t2, root2);
        size_t id1 = id(beg1);
        size_t id2 = id(beg2);
        it1 = root1;
        it2 = root2;

        DEBUG("while");

        assert(get_begin_leaf(t1, beg1) == beg1 &&
                get_begin_leaf(t2, beg2) == beg2);

        while (tree_type::is_valid(it1) || tree_type::is_valid(it2))
        {
            DEBUG("its: %s - %s", clabel(it1), clabel(it2));

            if (tree_type::is_valid(it1))
                assert(id(beg1) <= id(it1));
            if (tree_type::is_valid(it2))
                assert(id(beg2) <= id(it2));

            if (tree_type::is_valid(it1) &&
                    get_fdist(prev(1), it2) + GTED_COST_DELETE ==
                    get_fdist(it1, it2))
            {
                DEBUG("delete %s", clabel(it1));
                map.map.push_back({id(it1) + 1, 0});

                it1 = prev(1);
            }
            else if(tree_type::is_valid(it2) &&
                    get_fdist(it1, prev(2)) + GTED_COST_DELETE ==
                    get_fdist(it1, it2))
            {
                DEBUG("insert %s", clabel(it2));
                map.map.push_back({0, id(it2) + 1});

                it2 = prev(2);
            }
            else
            {
                if (get_begin_leaf(t1, it1) == beg1 &&
                        get_begin_leaf(t2, it2) == beg2)
                {
                    DEBUG("match %s -> %s",
                            clabel(it1), clabel(it2));

                    map.map.push_back({id(it1) + 1, id(it2) + 1});

                    it1 = prev(1);
                    it2 = prev(2);
                }
                else
                {
                    logger.debugStream()
                        << "to_be_matched:\n"
                        << tree_type::print_subtree(it1, false) << "\n"
                        << tree_type::print_subtree(it2, false);

                    to_be_matched.push_back({it1, it2});

                    // it <- left subtree
                    while (tree_type::is_first_child(it1) &&
                            it1 != root1)
                        it1 = tree_type::parent(it1);
                    while (tree_type::is_first_child(it2) &&
                            it2 != root2)
                        it2 = tree_type::parent(it2);

                    if (it1 == root1)
                        it1 = empty;
                    else if (!tree_type::is_first_child(it1))
                        it1 = --sibling_iterator(it1);
                    else
                        WAIT, it1 = empty;

                    if (it2 == root2)
                        it2 = empty;
                    else if (!tree_type::is_first_child(it2))
                        it2 = --sibling_iterator(it2);
                    else
                        WAIT, it2 = empty;
                }
            }
        }
        assert(!tree_type::is_valid(it1) && !tree_type::is_valid(it2));
/*
        //while (it1 != beg1 || it2 != beg2)
        //{
            //assert(id(beg1) <= id(it1));
            //assert(id(beg2) <= id(it2));

            //DEBUG("its: %s - %s", clabel(it1), clabel(it2));

            //if (it1 != beg1 &&
                    //get_fdist(prev(1), it2) + GTED_COST_DELETE ==
                    //get_fdist(it1, it2))

            //{ // node `it1` was deleted
                //DEBUG("delete %s", clabel(it1));
                //map.map.push_back({id(it1) + 1, 0});

                //--it1;
            //}
            //else if (it2 != beg2 &&
                    //get_fdist(it1, prev(2)) + GTED_COST_DELETE ==
                    //get_fdist(it1, it2))
            //{   // node `it1` was inserted
                //DEBUG("insert %s", clabel(it2));
                //map.map.push_back({0, id(it2) + 1});

                //--it2;
            //}
            //else
            //{
                //if (get_begin_leaf(t1, it1) == beg1 &&
                        //get_begin_leaf(t2, it2) == beg2)
                //{
                    //DEBUG("match %s -> %s",
                            //clabel(it1), clabel(it2));

                    //map.map.push_back({id(it1) + 1, id(it2) + 1});

                    //--it1;
                    //--it2;
                //}
                //else
                //{
                    //logger.debugStream()
                        //<< "to_be_matched:\n"
                        //<< tree_type::print_subtree(it1, false) << "\n"
                        //<< tree_type::print_subtree(it2, false);

                    //to_be_matched.push_back({it1, it2});

                    //if (!tree_type::is_first_child(it1))
                        //it1 = --sibling_iterator(it1);
                    //else
                        //WAIT, it1 = beg1;
                    //if (!tree_type::is_first_child(it2))
                        //it2 = --sibling_iterator(it2);
                    //else
                        //WAIT, it2 = beg2;
                //}
            //}
        //}
        */
    }

    for (auto it = t1.begin(); it != t1.end(); ++it)
    {
        bool cont = false;
        for (auto val : map.map)
            if (val.from - 1 == id(it))
                cont = true;

        if (cont == false)
        {
            DEBUG("!cont");
            DEBUG("%s", clabel(it));
            tree_type::print_subtree(tree_type::parent(it));
            abort();
        }
    }
    for (auto it = t2.begin(); it != t2.end(); ++it)
    {
        bool cont = false;
        for (auto val : map.map)
            if (val.to - 1 == id(it))
                cont = true;

        if (cont == false)
        {
            DEBUG("!cont");
            DEBUG("%s, %s", clabel(it), to_cstr(tree_type::is_first_child(it)));
            tree_type::print_subtree(tree_type::parent(it));
            abort();
        }
    }
    for (auto val : map.map)
    {
        bool cont;
        if (val.from != 0)
        {
            cont = false;
            for (auto it = t1.begin(); it != t1.end(); ++it)
                if (id(it) + 1 == val.from)
                    cont = true;
            if (cont == false)
            {
                DEBUG("!cont");
                abort();
            }
        }
        if (val.to != 0)
        {
            cont = false;
            for (auto it = t2.begin(); it != t2.end(); ++it)
                if (id(it) + 1 == val.to)
                    cont = true;
            if (cont == false)
            {
                DEBUG("!cont");
                abort();
            }
        }
    }

    for (auto val : map.map)
    {
        DEBUG("%lu -> %lu", val.from, val.to);
    }

    LOGGER_PRINT_CONTAINER(map.get_to_insert(), "inserted");
    LOGGER_PRINT_CONTAINER(map.get_to_remove(), "deleted") ;

    DEBUG("ins %lu, del %lu, m %lu, t1 %lu, t2 %lu",
            map.get_to_insert().size(),
            map.get_to_remove().size(),
            map.map.size(),
            t1.size(),
            t2.size());

    return map;
}


