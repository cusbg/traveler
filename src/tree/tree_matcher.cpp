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

rna_tree& matcher::run(
                const mapping& map)
{
    APP_DEBUG_FNAME;

    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    mark(t1, map.get_to_remove(), rna_pair_label::deleted);
    mark(t2, map.get_to_insert(), rna_pair_label::inserted);

    assert((t1.size() - map.get_to_remove().size()) ==
            (t2.size() - map.get_to_insert().size()));

    erase();

    t1.set_postorder_ids();
    t2.set_postorder_ids();

    compute_sizes();

    merge();

    logger.debug_stream() << "MATCH OUT: " << t1.print_tree(false);

    assert_err(t1.correct_pairing(), "uncorrect tree pairing");
    assert_err(t2.correct_pairing(), "uncorrect tree pairing");

    update_ends_in_rna(t1);
    t1.set_postorder_ids();

    return t1;
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

        assert(!rna_tree::is_root(it));
        it->status = status;
        i = index;

        DEBUG("node '%s' marked", clabel(it));
    }

    rna.print_tree();
}

void matcher::erase()
{
    APP_DEBUG_FNAME;

    iterator it;
    sibling_iterator ch;

    for (it = t1.begin(); it != t1.end(); ++it)
    {
        for (ch = it.begin(); ch != it.end();)
        {
            if (is(ch, rna_pair_label::deleted))
            {
                set_remake(ch);
                ch = t1.erase(ch);

                continue;
            }
            ++ch;
        }
    }

    assert_err(t1.correct_pairing(), "uncorrect tree pairing");
}

void matcher::merge()
{
    APP_DEBUG_FNAME;

    iterator it1, it2;
    sibling_iterator ch1, ch2, ins;
    size_t actual, needed, steal;
    
    it1 = t1.begin();
    it2 = t2.begin();

    it1->set_label_strings(*it2);

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
                    assert(actual == needed);
                }
                ch1 = t1.insert(ins, *ch2, steal);
                ch1->clear_points();
                set_remake(ch1);
            }
            else
            {
                DEBUG("modify %s -> %s", clabel(ch1), clabel(ch2));
                ch1->set_label_strings(*ch2);
            }

            ++ch2;
            ++ch1;
        }

        assert(ch1 == it1.end() && ch2 == it2.end());

        make_unique(it1);

        ++it1;
        ++it2;
    }

    assert(it1 == t1.end() && it2 == t2.end());
    assert(t1 == t2);
}



void matcher::compute_sizes()
{
    APP_DEBUG_FNAME;

    auto comp_f =
        [](rna_tree& rna, vector<size_t>& sizes) {
            post_order_iterator it;
            sibling_iterator ch;

            sizes.resize(rna.size());

            for (auto it = rna.begin(); it != rna.end(); ++it)
                assert(id(it) < sizes.size());

            for (it = rna.begin_post(); it != rna.end_post(); ++it)
            {
                sizes[id(it)] =
                    (is(it, rna_pair_label::inserted) ||
                     is(it, rna_pair_label::deleted)) ? 0 : 1;

                if (!rna_tree::is_leaf(it))
                    for (ch = it.begin(); ch != it.end(); ++ch)
                        sizes[id(it)] += sizes[id(ch)];
            }
        };

    comp_f(t1, s1);
    comp_f(t2, s2);

    assert(s1.at(id(t1.begin())) == s2.at(id(t2.begin())));

}

/* inline */
void matcher::make_unique(
                iterator it)
{
    auto& vec = it->remake_ids;
    sort(vec.begin(), vec.end());
    auto end = unique(vec.begin(), vec.end());
    vec.erase(end, vec.end());
}


