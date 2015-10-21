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

//#undef NDEBUG

#include "gted.hpp"
#include "mapping.hpp"

//#undef DEBUG
//#define DEBUG(...)

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
    size_t i;
    i = 0;
    for (auto it = t1.begin_post(); it != t1.end_post(); ++it, ++i)
        assert(id(it) == i);
    i = 0;
    for (auto it = t2.begin_post(); it != t2.end_post(); ++it, ++i)
        assert(id(it) == i);
}

void gted::run()
{
    APP_DEBUG_FNAME;

    //LOGGER_PRIORITY_ON_FUNCTION(INFO);

    checks();

    tdist.resize(t1.size(), vector<size_t>(t2.size(), BAD));

    compute_distance_recursive(t1.begin(), t2.begin());

    compute_mapping();
}

void gted::compute_distance_recursive(
                iterator it1,
                iterator it2)
{
    // using keyroots

    APP_DEBUG_FNAME;

    t1.check_same_tree(it1);
    t2.check_same_tree(it2);

    DEBUG("recursion, its %s, %s",
            clabel(it1), clabel(it2));

    strategy str = STR[id(it1)][id(it2)];
    if (str.is_heavy())
        str = strategy(RTED_T2_RIGHT);

    INFO("str = %s", to_cstr(actual_str));

    if (str.is_T1())
    {
        DEBUG("decomponing T1 - keyroots");
        for (const auto& val :
                get_table(actual_str, t1.get_keyroots(it1)))
        {
            actual_str = str;
            compute_distance_recursive(val, it2);
        }
    }
    else
    {
        for (const auto& val :
                get_table(actual_str, t2.get_keyroots(it2)))
        {
            actual_str = str;
            compute_distance_recursive(it1, val);
        }
    }

    actual_str = str;
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
                iterator root1,
                iterator root2)
{
    APP_DEBUG_FNAME;

    tree_type *t1ptr = &t1;
    tree_type *t2ptr = &t2;

    if (actual_str.is_T2())
    {
        swap(t1ptr, t2ptr);
        swap(root1, root2);
    }

    t1ptr->check_same_tree(root1);
    t2ptr->check_same_tree(root2);

    if (actual_str.is_left())
    {
        auto leaf_funct = [](tree_type& t, iterator root) {
            return t.get_leafs(root).left;
        };
        compute_distance_LR<post_order_iterator>
                (root1, root2, *t1ptr, *t2ptr, leaf_funct);
    }
    else if (actual_str.is_right())
    {
        auto leaf_funct = [](const tree_type& t, const iterator& root) {
            return t.get_leafs(root).right;
        };
        compute_distance_LR<rev_post_order_iterator>
                (root1, root2, *t1ptr, *t2ptr, leaf_funct);
    }
}

template <typename iterator_type, typename funct_get_begin>
std::vector<std::vector<size_t>> gted::compute_distance_LR(
                iterator root1,
                iterator root2,
                tree_type& t1,
                tree_type& t2,
                funct_get_begin get_begin_leaf)
{
    // subtree has id-s (id(leafs.left) ... id(root1))

    //LOGGER_PRIORITY_ON_FUNCTION(DEBUG);
    APP_DEBUG_FNAME;

    DEBUG("root1 %s:%lu, root2 %s:%lu",
            lblid(root1), lblid(root2));
    t1.print_subtree(root1);
    t2.print_subtree(root2);

    vector<vector<size_t>> fdist(
                t1.get_size(root1) + 1,
                    vector<size_t>(t2.get_size(root2) + 1, BAD));
    vector<size_t> vec(3);

    iterator_type it1, it2;
    const iterator_type empty;
    const iterator_type beg1 = get_begin_leaf(t1, root1);
    const iterator_type beg2 = get_begin_leaf(t2, root2);
    const iterator_type end1 = ++iterator_type(root1);
    const iterator_type end2 = ++iterator_type(root2);
    const size_t id1 = id(t1.get_leafs(root1).left);
    const size_t id2 = id(t2.get_leafs(root2).left);

    auto get_fdist =
        [&fdist, &id1, &id2](const iterator_type& iter1,
                const iterator_type& iter2) {
            size_t i1, i2, out;

            i1 = valid(iter1) ? id(iter1) - id1 + 1 : 0;
            i2 = valid(iter2) ? id(iter2) - id2 + 1 : 0;

            assert((int)i1 >= 0 && (int)i2 >= 0);
            assert(i1 < fdist.size() && i2 < fdist[i1].size());

            DEBUG("\tget FDist[%s:%lu:%lu][%s:%lu:%lu] -> %lu",
                    clabel(iter1), valid(iter1) ? id(iter1) : 0, i1,
                    clabel(iter2), valid(iter2) ? id(iter2) : 0, i2,
                    fdist[i1][i2]);

            out = fdist[i1][i2];

            assert(out != BAD);
            return out;
        };
    auto set_fdist =
        [&fdist, &id1, &id2](const iterator_type& iter1,
                const iterator_type& iter2, size_t value) {
            size_t i1, i2;

            i1 = valid(iter1) ? id(iter1) - id1 + 1 : 0;
            i2 = valid(iter2) ? id(iter2) - id2 + 1 : 0;

            assert((int)i1 >= 0 && (int)i2 >= 0);
            assert(i1 < fdist.size() && i2 < fdist[i1].size());

            DEBUG("set FDist[%s:%lu:%lu][%s:%lu:%lu] = %lu",
                    clabel(iter1), valid(iter1) ? id(iter1) : 0, i1,
                    clabel(iter2), valid(iter2) ? id(iter2) : 0, i2,
                    value);

            fdist[i1][i2] = value;
        };

#define prev(num) \
    ((it ##num == beg ##num) ? empty : --iterator_type(it ##num))

    DEBUG("BEG init");

    set_fdist(empty, empty, 0);
    for (it1 = beg1; it1 != end1; ++it1)
        set_fdist(it1, empty, get_fdist(prev(1), empty) + GTED_COST_DELETE);
    for (it2 = beg2; it2 != end2; ++it2)
        set_fdist(empty, it2, get_fdist(empty, prev(2)) + GTED_COST_DELETE);

    DEBUG("END init");
    DEBUG("BEG main cycle");

    for (it1 = beg1; it1 != end1; ++it1)
    {
        for (it2 = beg2; it2 != end2; ++it2)
        {
            DEBUG("its: %s - %s", clabel(it1), clabel(it2));

            size_t min;
            bool b = get_begin_leaf(t1, it1) == beg1 &&
                    get_begin_leaf(t2, it2) == beg2;
            
            vec[0] = get_fdist(prev(1), it2) + GTED_COST_DELETE; // delete `it1`
            vec[1] = get_fdist(it1, prev(2)) + GTED_COST_DELETE; // delete `it2`

            // modify iff both it-s are subtree roots
            if (b)
                vec[2] = get_fdist(prev(1), prev(2)) + GTED_COST_MODIFY; // modify `it1` ~> `it2`
            else
            {
                iterator_type prev_root1 = get_begin_leaf(t1, it1);
                iterator_type prev_root2 = get_begin_leaf(t2, it2);

                if (prev_root1 != beg1)
                    --prev_root1;
                else
                    prev_root1 = empty;
                if (prev_root2 != beg2)
                    --prev_root2;
                else
                    prev_root2 = empty;

                vec[2] = get_tdist(it1, it2) + get_fdist(prev_root1, prev_root2);
                // ^^ if (prev_root != empty) =>
                // prev_root is in sibling branch of it
                // and we computed this subtree yet
            }

            min = *min_element(vec.begin(), vec.end());

            set_fdist(it1, it2, min);
            if (b) // i am at subtree roots
                set_tdist(it1, it2, min);
        }
    }

    DEBUG("END main cycle");

    return fdist;
}




size_t gted::get_tdist(
                iterator it1,
                iterator it2)
{
    assert(valid(it1) && valid(it2));

    size_t i1, i2, out;

    if (actual_str.is_T2())
        swap(it1, it2);
    t1.check_same_tree(it1);
    t2.check_same_tree(it2);

    i1 = id(it1);
    i2 = id(it2);

    assert(i1 < tdist.size() && i2 < tdist[i1].size());

    DEBUG("\tget TDist[%s:%lu][%s:%lu] -> %lu",
            clabel(it1), i1,
            clabel(it2), i2,
            tdist[i1][i2]);

    out = tdist[i1][i2];

    assert(out != BAD);
    return out;
}

void gted::set_tdist(
                iterator it1,
                iterator it2,
                size_t value)
{
    assert(valid(it1) && valid(it2));

    size_t i1, i2;

    if (actual_str.is_T2())
        swap(it1, it2);
    t1.check_same_tree(it1);
    t2.check_same_tree(it2);

    i1 = id(it1);
    i2 = id(it2);

    assert(i1 < tdist.size() && i2 < tdist[i1].size());
    assert(value != BAD);

    DEBUG("set TDist[%s:%lu][%s:%lu] = %lu",
            clabel(it1), i1,
            clabel(it2), i2,
            value);

    tdist[i1][i2] = value;
}

size_t gted::get_fdist(
                const forest_distance_table_type& fdist,
                const iterator& it1,
                const iterator& it2,
                size_t idleft1,
                size_t idleft2)
{
    size_t i1, i2, out;

    i1 = valid(it1) ? id(it1) - idleft1 + 1 : 0;
    i2 = valid(it2) ? id(it2) - idleft2 + 1 : 0;

    assert((int)i1 >= 0 && (int)i2 >= 0);
    assert(i1 < fdist.size() && i2 < fdist[i1].size());

    DEBUG("\tget FDist[%s:%lu:%lu][%s:%lu:%lu] -> %lu",
            clabel(it1), valid(it1) ? id(it1) : 0, i1,
            clabel(it2), valid(it2) ? id(it2) : 0, i2,
            fdist[i1][i2]);

    out = fdist[i1][i2];

    assert(out != BAD);
    return out;
}

void gted::set_fdist(
                const forest_distance_table_type& fdist,
                const iterator& it1,
                const iterator& it2,
                size_t value,
                size_t id1,
                size_t id2)
{
}




mapping gted::compute_mapping()
{
    mapping m;
    vector<pair<iterator, iterator>> to_be_matched;
    iterator root1, root2;
    iterator it1, it2;
    iterator beg1, beg2;
    vector<vector<size_t>> fdist;

    auto compute_fdist_table =
        [this](iterator iter1, iterator iter2) {
            actual_str = strategy(RTED_T1_LEFT);

            auto leaf_funct =
                [](tree_type& t, iterator root) {
                    return t.get_leafs(root).left;
                };
            return compute_distance_LR<post_order_iterator>
                    (iter1, iter2, t1, t2, leaf_funct);
        };

    to_be_matched.push_back(make_pair(t1.begin(), t2.begin()));

    while (!to_be_matched.empty())
    {
        root1 = to_be_matched.back().first;
        root2 = to_be_matched.back().second;
        to_be_matched.pop_back();

        fdist = compute_fdist_table(root1, root2);

        beg1 = t1.get_leafs(root1).left;
        beg2 = t2.get_leafs(root2).left;
        it1 = root1;
        it2 = root2;

        while (it1 != beg1 || it2 != beg2)
        {
            assert(id(beg1) <= id(it1) && id(beg2) <= id(it2));

            if (it1 != beg1)
            {
            }
        }

    }

    return m;
}






