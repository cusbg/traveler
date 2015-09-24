/*
 * File: tree_matcher.cpp
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

#include "tree_matcher.hpp"
#include "mapping.hpp"

using namespace std;

#define set_remake(iter) \
    rna_tree::parent(iter)->remake_ids.push_back(child_index(iter));


matcher::matcher(
                const rna_tree& templated,
                const rna_tree& other)
    : t1(templated), t2(other)
{ }

/* inline */
void matcher::make_unique(
                iterator it)
{
    auto& vec = it->remake_ids;
    sort(vec.begin(), vec.end());
    auto end = unique(vec.begin(), vec.end());
    vec.erase(end, vec.end());
}

/* inline */
size_t matcher::child_index(rna_tree::sibling_iterator sib)
{
    size_t n = 1;
    while (!rna_tree::is_first_child(sib))
    {
        ++n;
        --sib;
    }
    return n;
}

void matcher::compute_sizes()
{
    APP_DEBUG_FNAME;

    auto comp_f =
        [](rna_tree& rna, vector<size_t> sizes) {
            post_order_iterator it;
            sibling_iterator ch;

            sizes.resize(rna.size());

            for (it = rna.begin_post(); it != rna.end_post(); ++it)
            {
                sizes[id(it)] = 1;

                if (!rna_tree::is_leaf(it))
                    for (ch = it.begin(); ch != it.end(); ++ch)
                        sizes[id(it)] += sizes[id(ch)];
            }
        };

    comp_f(t1, s1);
    comp_f(t2, s2);
}

void matcher::mark(
                rna_tree& rna,
                const indexes_type& postorder_indexes,
                rna_pair_label::status_type status)
{
    APP_DEBUG_FNAME;

    post_order_iterator it = rna.begin_post();
    size_t i = 0;

    for (size_t index : postorder_indexes)
    {
        --index;    // indexy cislovane od 1
        size_t to_move = index - i;
        it = plusplus(it, to_move);
        it->status = status;
        DEBUG("mark(%s:%s)",
                clabel(it), to_cstr(status));
        i = index;
    }
}

void matcher::run(
                const mapping& map)
{
    APP_DEBUG_FNAME;

    mark(t1, map.get_to_remove(), rna_pair_label::deleted);
    mark(t2, map.get_to_insert(), rna_pair_label::inserted);
    erase();
    merge();
}

void matcher::erase()
{
    APP_DEBUG_FNAME;

    iterator it;
    sibling_iterator ch;

    for (it = t1.begin(); it != t1.end(); ++it)
    {
        for (ch = it.begin(); ch != it.end(); ++ch)
        {
            if (is(ch, rna_pair_label::deleted))
            {
                set_remake(ch);
                ch = t1.erase(ch);
            }
        }
    }
}

void matcher::merge()
{
    APP_DEBUG_FNAME;

    iterator it1, it2;
    sibling_iterator ch1, ch2, ins;
    size_t actual, needed, steal;
    
    it1 = t1.begin();
    it2 = t2.begin();

    while (it1 != t1.end() && it2 != t2.end())
    {
        ch1 = it1.begin();
        ch2 = it2.begin();

        while (ch2 != it2.end())
        {
            if (is(ch2, rna_pair_label::inserted))
            {
                steal = 0;
                ins = ch1;

                if (ch2->paired())
                {
                    actual = 0;
                    needed = s2.at(id(ch2));

                    while (actual < needed)
                    {
                        actual += s1.at(id(ch1));
                        ++steal;
                        ++ch1;
                    }
                }

                ch1 = t1.insert(ins, *ch2, steal);
                set_remake(ch1);
            }
            else
                ch1->set_label_strings(*ch2);

            ++ch2;
            ++ch1;
        }

        make_unique(it1);

        ++it1;
        ++it2;
    }

    assert(it1 == t1.end() && it2 == t2.end());
    assert(t1 == t2);
}







