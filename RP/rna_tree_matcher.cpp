/*
 * File: rna_tree_matcher.cpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard@ba30.eu>
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

#include <unordered_map>

#include "rna_tree_matcher.hpp"
#include "util.hpp"

using namespace std;

#define remake(iter) \
    { \
        rna_tree::parent(iter)->get_label().remake.push_back(child_index(iter)); \
        /*DEBUG("remake('%s:%lu:%lu)", iter->get_label().to_string().c_str(), id(iter), child_index(iter)); \*/ \
    }
#define print_parent_subtree(iter) \
    if (iter.node != nullptr && iter.node->parent != nullptr) \
        rna_tree::print_subtree(rna_tree::parent(iter)); \
    else \
        WARN("print_parent_subtree on NULL");

typedef unordered_map<rna_tree::iterator, size_t, rna_tree::iterator_hash>
                iterator_hash_map;

template <typename funct>
iterator_hash_map compute_sizes(const rna_tree& rna, funct f);

size_t matcher::child_index(rna_tree::sibling_iterator sib)
{
    size_t n = 0;
    rna_tree::sibling_iterator other;

    other = rna_tree::first_child(rna_tree::parent(sib));   // failne v root-e
    while (other != sib)
    {
        ++other;
        ++n;
    }
    return n;
}

template <typename funct>
iterator_hash_map compute_sizes(const rna_tree& rna, funct f)
{
    APP_DEBUG_FNAME;

    iterator_hash_map m;
    rna_tree::sibling_iterator sib;
    rna_tree::post_order_iterator it;

    m.reserve(rna.size());

    for (it = rna.begin_post(); it != rna.end_post(); ++it)
    {
        m[it] = f(it);
        for (sib = it.begin(); sib != it.end(); ++sib)
            m.at(it) += m.at(sib);
        //DEBUG("m[%s] = %lu", clabel(it), m[it]);
    }

    DEBUG("rna_size=%lu, root_subtree=%lu, map_size=%lu", rna.size(), m[rna.begin()], m.size());

    return m;
}



void matcher::mark(
                rna_tree& rna,
                const indexes_type& postorder_indexes,
                rna_pair_label::label_status_type status)
{
    APP_DEBUG_FNAME;

    post_order_iterator it = rna.begin_post();
    size_t i = 0;

    for (size_t index : postorder_indexes)
    {
        --index;    // indexy cislovane od 1
        size_t to_move = index - i;
        it = move_it_plus(it, to_move);
        //remake(it);
        it->get_label().status = status;
        DEBUG("mark: %s", it->get_label().to_string().c_str());
        i = index;
    }
}

void matcher::run(
                rna_tree& templated,
                rna_tree other,
                const mapping& m)
{
    APP_DEBUG_FNAME;

    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    mark(templated, m.get_to_remove(), rna_pair_label::deleted);
    mark(other, m.get_to_insert(), rna_pair_label::inserted);
    merge(templated, other);

    DEBUG("END");
}


void matcher::merge(
                rna_tree& templated,
                const rna_tree& other)
{
    APP_DEBUG_FNAME;

    iterator it1, it2;
    sibling_iterator sib1, sib2;
    iterator_hash_map m_del, m_ins;

    m_del = compute_sizes(templated, [](iterator sib)
            { return !is(sib, rna_pair_label::deleted); });
    m_ins = compute_sizes(other, [](iterator sib)
            { return !is(sib, rna_pair_label::inserted); });

    it1 = templated.begin();
    it2 = other.begin();

    assert(m_del.at(it1) == m_ins.at(it2));
    it1->set_label(it2->get_label());

    while (it1 != templated.end() && it2 != other.end())
    {
        assert(!is(it1, rna_pair_label::deleted));

        sib1 = it1.begin();
        sib2 = it2.begin();

        while (sib2 != it2.end())
        {
            if (is(sib1, rna_pair_label::deleted))
            {
                remake(sib1);
                sib1 = templated.erase(sib1);
                continue;
            }

            if (is(sib2, rna_pair_label::inserted))
            {
                size_t steal = 0;
                sibling_iterator ins = sib1;

                if (sib2->get_label().is_paired())
                {
                    size_t actual, need;

                    actual = 0;
                    need = m_ins.at(sib2);

                    while (actual < need)
                    {
                        if (is(sib1, rna_pair_label::deleted))
                        {
                            sib1 = templated.erase(sib1);
                            continue;
                        }

                        actual += m_del.at(sib1);
                        ++steal;
                        ++sib1;
                    }
                }

                //print_parent_subtree(sib1);
                //print_parent_subtree(sib2);
                sib1 = templated.insert(ins, sib2->get_label(), steal);
                remake(sib1);
            }
            else
            {
                sib1->get_label().set_label_strings(sib2->get_label());
            }

            ++sib1;
            ++sib2;
        }
        while (sib1 != it1.end())
        {
            assert(is(sib1, rna_pair_label::deleted));
            remake(sib1);
            sib1 = templated.erase(sib1);
        }

        assert(sib1 == it1.end() && sib2 == it2.end());
        unique_indexes(it1);

        ++it1;
        ++it2;
    }

    assert(it1 == templated.end() && it2 == other.end());
    assert(templated == other);
    templated.test_branches();
}


void matcher::unique_indexes(
                iterator it)
{
    auto& vec = it->get_label().remake;
    sort(vec.begin(), vec.end());
    auto end = unique(vec.begin(), vec.end());
    vec.erase(end, vec.end());
}






















