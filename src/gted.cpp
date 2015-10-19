/*
 * File: gted.cpp
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

using namespace std;

#define BAD 0xBADF00D

#define get_table(str, tblname) \
    (str.is_left() ? (tblname).left : \
     (str.is_right() ? (tblname).right : \
      (tblname).heavy))

#define lies_on_path(iter, tree, path_name) \
    (!(tree).is_ ##path_name(iter))

#define lblid(iter) clabel(iter), id(iter)
#define valid(iter) (rna_tree::is_valid(iter))

gted::gted(
                const rna_tree& _t1,
                const rna_tree& _t2,
                const strategy_table_type& _str)
    : t1(_t1), t2(_t2), STR(_str)
{ }

void gted::checks()
{
    size_t i = 0;
    for (auto it = t1.begin_post(); it != t1.end_post(); ++it, ++i)
        assert(id(it) == i);
    i = 0;
    for (auto it = t2.begin_post(); it != t2.end_post(); ++it, ++i)
        assert(id(it) == i);
}

void gted::run()
{
    APP_DEBUG_FNAME;

    checks();

    tdist.resize(t1.size(), vector<size_t>(t2.size(), BAD));

    compute_distance_recursive(t1.begin(), t2.begin());
}

void gted::compute_distance_recursive(
                iterator it1,
                iterator it2)
{
    // using keyroots
    // if strategy == T1 -> divide t1

    APP_DEBUG_FNAME;

    t1.check_same_tree(it1);
    t2.check_same_tree(it2);

    DEBUG("recursion, its %s, %s",
            clabel(it1), clabel(it2));

    //actual_str = STR[id(it1)][id(it2)];
    actual_str = strategy(RTED_T1_LEFT);

    DEBUG("str = %s", to_cstr(actual_str));

    if (actual_str.is_T1())
    {
        DEBUG("decomponing T1 - keyroots");
        for (const auto& val :
                get_table(actual_str, t1.get_keyroots(it1)))
            compute_distance_recursive(val, it2);
    }
    else
    {
        for (const auto& val :
                get_table(actual_str, t2.get_keyroots(it1)))
            compute_distance_recursive(it1, val);
    }

    single_path_function(it1, it2);
}

void gted::single_path_function(
                iterator it1,
                iterator it2)
{
    // using subforests
    // if strategy == T1 -> divide t2...

    APP_DEBUG_FNAME;

    DEBUG("single_path_function %s, %s", clabel(it1), clabel(it2));

    if (actual_str.is_T1())
    {
        DEBUG("decomponing T2 - subforests");
        // use t2.subforests
        for (const auto& val :
                get_table(actual_str, t2.get_subforests(it2)))
            compute_distance(it1, val);
    }
    else
    {
        for (const auto& val :
                get_table(actual_str, t1.get_subforests(it1)))
            compute_distance(val, it2);
    }

    compute_distance(it1, it2);
}

void gted::compute_distance(
                iterator it1,
                iterator it2)
{
    APP_DEBUG_FNAME;

    tree_type *t1ptr = &t1;
    tree_type *t2ptr = &t2;

    if (actual_str.is_T2())
    {
        swap(t1ptr, t2ptr);
        swap(it1, it2);
    }

    t1ptr->check_same_tree(it1);
    t2ptr->check_same_tree(it2);

    compute_distance_L(it1, it2, *t1ptr, *t2ptr);
    //compute_distance_R(it1, it2, *t1ptr, *t2ptr);
}

void gted::compute_distance_L(
                iterator root1,
                iterator root2,
                tree_type& t1,
                tree_type& t2)
{
    // subtree has id-s (id(leafs.left) ... id(root1))

    LOGGER_PRIORITY_ON_FUNCTION(DEBUG);
    APP_DEBUG_FNAME;

    DEBUG("root1 %s:%lu, root2 %s:%lu",
            lblid(root1), lblid(root2));
    t1.print_subtree(root1);
    t2.print_subtree(root2);

    vector<vector<size_t>> fdist(
                t1.get_size(root1) + 1,
                    vector<size_t>(t2.get_size(root2) + 1, BAD));

    typedef post_order_iterator iterator_type;

    iterator_type it1, it2;
    const iterator_type empty;
    const iterator_type beg1 = t1.get_leafs(root1).left;    // LRH !!
    const iterator_type beg2 = t2.get_leafs(root2).left;
    const iterator_type end1 = ++iterator_type(root1);
    const iterator_type end2 = ++iterator_type(root2);
    const size_t id1 = id(t1.get_leafs(root1).left);
    const size_t id2 = id(t2.get_leafs(root2).left);
    vector<size_t> vec(3);

    auto get_tdist = [this](iterator iter1, iterator iter2) {
        assert(valid(iter1) && valid(iter2));

        size_t i1, i2, out;

        if (actual_str.is_T2())
            swap(iter1, iter2);

        i1 = id(iter1);
        i2 = id(iter2);

        DEBUG("\tget TDist[%s:%lu][%s:%lu] -> %lu",
                clabel(iter1), i1,
                clabel(iter2), i2,
                tdist.at(i1).at(i2));

        out = tdist[i1][i2];

        assert(out != BAD);
        return out;
    };
    auto set_tdist = [this](iterator iter1, iterator iter2, size_t value) {
        assert(valid(iter1) && valid(iter2));

        size_t i1, i2;

        if (actual_str.is_T2())
            swap(iter1, iter2);

        i1 = id(iter1);
        i2 = id(iter2);

        DEBUG("set TDist[%s:%lu][%s:%lu] = %lu",
                clabel(iter1), i1,
                clabel(iter2), i2,
                value);

        assert(i1 < tdist.size());
        assert(i2 < tdist[i1].size());

        tdist[i1][i2] = value;
    };
    auto get_fdist = [&id1, &id2, &fdist](iterator iter1, iterator iter2) {
        size_t i1, i2, out;

        i1 = i2 = 0;
        if (valid(iter1))
            i1 = id(iter1) - id1 + 1;
        if (valid(iter2))
            i2 = id(iter2) - id2 + 1;
        assert((int)i1 >= 0 && (int)i2 >= 0);

        DEBUG("\tget FDist[%s:%lu][%s:%lu] -> %lu",
                clabel(iter1), i1,
                clabel(iter2), i2,
                fdist.at(i1).at(i2));

        out = fdist[i1][i2];

        assert(out != BAD);
        return out;
    };
    auto set_fdist = [&id1, &id2, &fdist](iterator iter1, iterator iter2, size_t value) {
        size_t i1, i2;

        i1 = i2 = 0;
        if (valid(iter1))
            i1 = id(iter1) - id1 + 1;
        if (valid(iter2))
            i2 = id(iter2) - id2 + 1;
        assert((int)i1 >= 0 && (int)i2 >= 0);

        DEBUG("set FDist[%s:%lu][%s:%lu] = %lu",
                clabel(iter1), i1,
                clabel(iter2), i2,
                value);

        assert(i1 < fdist.size());
        assert(i2 < fdist[i1].size());

        fdist[i1][i2] = value;
    };

#define prev(num) \
    ((it ##num == beg ##num) ? empty : (--decltype(it ##num)(it ##num)))

    INFO("BEG init");

    set_fdist(empty, empty, 0);
    for (it1 = beg1; it1 != end1; ++it1)
        set_fdist(it1, empty, get_fdist(prev(1), empty) + GTED_COST_DELETE);
    for (it2 = beg2; it2 != end2; ++it2)
        set_fdist(empty, it2, get_fdist(empty, prev(2)) + GTED_COST_DELETE);

    INFO("END init");
    INFO("BEG main cycle");
    DEBUG("begs: %s, %s", clabel(beg1), clabel(beg2));

    for (it1 = beg1; it1 != end1; ++it1)
    {
        for (it2 = beg2; it2 != end2; ++it2)
        {
            DEBUG("its: %s - %s", clabel(it1), clabel(it2));

            size_t c;
            bool b = t1.get_leafs(it1).left == t1.get_leafs(root1).left &&
                t2.get_leafs(it2).left == t2.get_leafs(root2).left;

            vec[0] = get_fdist(prev(1), it2) + GTED_COST_DELETE;
            vec[1] = get_fdist(it1, prev(2)) + GTED_COST_DELETE;

            if (b)
                vec[2] = get_fdist(prev(1), prev(2)) + GTED_COST_MODIFY;
            else
            {
                post_order_iterator l1 = t1.get_leafs(it1).left;
                post_order_iterator l2 = t2.get_leafs(it2).left;

                if (l1 != beg1)
                    --l1;
                else
                    l1 = empty;
                if (l2 != beg2)
                    --l2;
                else
                    l2 = empty;

                vec[2] = get_tdist(it1, it2) + get_fdist(l1, l2);
            }
            c = *min_element(vec.begin(), vec.end());

            set_fdist(it1, it2, c);
            if (b)
                set_tdist(it1, it2, c);
        }
    }
    INFO("END main cycle");
}

void gted::compute_distance_R(
                iterator root1,
                iterator root2,
                tree_type& t1,
                tree_type& t2)
{
    // subtree has id-s (id(leafs.left) ... id(root1))

    LOGGER_PRIORITY_ON_FUNCTION(DEBUG);
    APP_DEBUG_FNAME;

    DEBUG("root1 %s:%lu, root2 %s:%lu",
            lblid(root1), lblid(root2));
    t1.print_subtree(root1);
    t2.print_subtree(root2);

    vector<vector<size_t>> fdist(
                t1.get_size(root1) + 1,
                    vector<size_t>(t2.get_size(root2) + 1, BAD));

    typedef rev_post_order_iterator iterator_type;

    iterator_type it1, it2;
    const iterator_type empty;
    const iterator_type beg1 = t1.get_leafs(root1).right;    // LRH !!
    const iterator_type beg2 = t2.get_leafs(root2).right;
    const iterator_type end1 = ++iterator_type(root1);
    const iterator_type end2 = ++iterator_type(root2);
    const size_t id1 = id(t1.get_leafs(root1).left);
    const size_t id2 = id(t2.get_leafs(root2).left);
    vector<size_t> vec(3);

    auto get_tdist = [this](iterator iter1, iterator iter2) {
        assert(valid(iter1) && valid(iter2));

        size_t i1, i2, out;

        if (actual_str.is_T2())
            swap(iter1, iter2);

        i1 = id(iter1);
        i2 = id(iter2);

        DEBUG("\tget TDist[%s:%lu][%s:%lu] -> %lu",
                clabel(iter1), i1,
                clabel(iter2), i2,
                tdist.at(i1).at(i2));

        out = tdist[i1][i2];

        assert(out != BAD);
        return out;
    };
    auto set_tdist = [this](iterator iter1, iterator iter2, size_t value) {
        assert(valid(iter1) && valid(iter2));

        size_t i1, i2;

        if (actual_str.is_T2())
            swap(iter1, iter2);

        i1 = id(iter1);
        i2 = id(iter2);

        DEBUG("set TDist[%s:%lu][%s:%lu] = %lu",
                clabel(iter1), i1,
                clabel(iter2), i2,
                value);

        assert(i1 < tdist.size());
        assert(i2 < tdist[i1].size());

        tdist[i1][i2] = value;
    };
    auto get_fdist = [&id1, &id2, &fdist](iterator iter1, iterator iter2) {
        size_t i1, i2, out;

        i1 = i2 = 0;
        if (valid(iter1))
            i1 = id(iter1) - id1 + 1;
        if (valid(iter2))
            i2 = id(iter2) - id2 + 1;
        assert((int)i1 >= 0 && (int)i2 >= 0);

        DEBUG("\tget FDist[%s:%lu][%s:%lu] -> %lu",
                clabel(iter1), i1,
                clabel(iter2), i2,
                fdist.at(i1).at(i2));

        out = fdist[i1][i2];

        assert(out != BAD);
        return out;
    };
    auto set_fdist = [&id1, &id2, &fdist](iterator iter1, iterator iter2, size_t value) {
        size_t i1, i2;

        i1 = i2 = 0;
        if (valid(iter1))
            i1 = id(iter1) - id1 + 1;
        if (valid(iter2))
            i2 = id(iter2) - id2 + 1;
        assert((int)i1 >= 0 && (int)i2 >= 0);

        DEBUG("set FDist[%s:%lu][%s:%lu] = %lu",
                clabel(iter1), i1,
                clabel(iter2), i2,
                value);

        assert(i1 < fdist.size());
        assert(i2 < fdist[i1].size());

        fdist[i1][i2] = value;
    };

#define prev(num) \
    ((it ##num == beg ##num) ? empty : (--decltype(it ##num)(it ##num)))

    INFO("BEG init");

    set_fdist(empty, empty, 0);
    for (it1 = beg1; it1 != end1; ++it1)
        set_fdist(it1, empty, get_fdist(prev(1), empty) + GTED_COST_DELETE);
    for (it2 = beg2; it2 != end2; ++it2)
        set_fdist(empty, it2, get_fdist(empty, prev(2)) + GTED_COST_DELETE);

    INFO("END init");
    INFO("BEG main cycle");
    DEBUG("begs: %s, %s", clabel(beg1), clabel(beg2));

    for (it1 = beg1; it1 != end1; ++it1)
    {
        for (it2 = beg2; it2 != end2; ++it2)
        {
            DEBUG("its: %s - %s", clabel(it1), clabel(it2));

            size_t c;
            bool b = t1.get_leafs(it1).right == t1.get_leafs(root1).right &&
                t2.get_leafs(it2).right == t2.get_leafs(root2).right;

            vec[0] = get_fdist(prev(1), it2) + GTED_COST_DELETE;
            vec[1] = get_fdist(it1, prev(2)) + GTED_COST_DELETE;

            if (b)
                vec[2] = get_fdist(prev(1), prev(2)) + GTED_COST_MODIFY;
            else
            {
                iterator_type l1 = t1.get_leafs(it1).right;   // !!LRH
                iterator_type l2 = t2.get_leafs(it2).right;

                if (l1 != beg1)
                    --l1;
                else
                    l1 = empty;
                if (l2 != beg2)
                    --l2;
                else
                    l2 = empty;

                vec[2] = get_fdist(l1, l2) + get_tdist(it1, it2);
            }
            c = *min_element(vec.begin(), vec.end());

            set_fdist(it1, it2, c);
            if (b)
                set_tdist(it1, it2, c);
        }
    }
    prev(1);
    INFO("END main cycle");
}






