/*
 * File: rna_tree.cpp
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

#include "rna_tree.hpp"
#include "rna_tree_labels.hpp"
#include "util.hpp"
#include "ps.hpp"

#include <unordered_map>

using namespace std;

#define is(_iter, _status) \
    (_iter->get_label().status == rna_pair_label::_status)

inline std::vector<rna_node_type> convert(const std::string& labels);

rna_tree::rna_tree(
                const std::string& brackets,
                const std::string& labels,
                const std::string& _name)
    : tree_base<rna_node_type>(brackets, convert(labels)), name(_name)
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    std::stringstream stream;
    stream
        << "TREE '"
        << name
        << "' WAS CONSTRUCTED, size = "
        << size()
        << std::endl
        << print_tree(false);
    logger.debugStream() << stream.str();
}

std::vector<rna_node_type> convert(
                const std::string& labels)
{
    std::vector<rna_node_type> vec;
    vec.reserve(labels.size());
    for (size_t i = 0; i < labels.size(); ++i)
        vec.emplace_back(labels.substr(i, 1));
    return vec;
}

bool rna_tree::operator==(
                rna_tree& other)
{
    return _tree.equal_subtree(begin(), other.begin());
}

rna_tree::iterator rna_tree::insert(sibling_iterator it, rna_node_type node, size_t steal)
{
    APP_DEBUG_FNAME;

    DEBUG("insert(%s, %lu) <- %s", label(it), steal, label_str(node));

    sibling_iterator in, next;
    rna_node_type n(node.get_label());
    
    in = _tree.insert(it, n);
    ++_size;

    while (steal-- != 0)
    {
        next = in;
        ++next;
        assert(next != it.end());

        _tree.reparent(in, next, ++sibling_iterator(next));
    }

    return in;
}

void rna_tree::mark(std::vector<size_t> node_ids, rna_pair_label::label_status_type status)
{
    APP_DEBUG_FNAME;
    post_order_iterator it = begin_post();
    size_t i = 0;
    for (size_t index : node_ids)
    {
        --index;    // indexy cislovane od 1
        size_t to_move = index - i;
        it = move_it_plus(it, to_move);
        it->get_label().status = status;
        i = index;
    }
}

void rna_tree::modify(const rna_tree& other)
{
    APP_DEBUG_FNAME;

    iterator it1, it2;
    it1 = begin();
    it2 = other.begin();

    while (it1 != end() && it2 != other.end())
    {
        if (is(it1, inserted) || is(it1, deleted))
        {
            ++it1;
            continue;
        }
        if (is(it2, inserted) || is(it2, deleted))
        {
            ++it2;
            continue;
        }

        it1->get_label().set_label_strings(it2->get_label());

        ++it1;
        ++it2;
    }
    assert(it1 == end() && it2 == other.end());
}



struct iter_hash
{
    typedef unordered_map<
                        rna_tree::iterator,
                        size_t,
                        iter_hash> iterator_hash_map;

    inline size_t operator()(
                rna_tree::iterator it) const;

    template <typename funct>
    static iterator_hash_map compute_sizes(
                const rna_tree& rna, funct f);
};


void rna_tree::merge(rna_tree other, const mapping& m)
{   // this <- by mala byt template rna, s inicializovanymi bodmi..
    APP_DEBUG_FNAME;

    iterator it1, it2;
    iterator end1, end2;
    sibling_iterator sib1, sib2;
    auto has_ins = [](rna_tree::iterator iter)
    {
        return count_children_if(iter,
                [](rna_tree::iterator _iter)
                {
                    return is(_iter, inserted);
                }) != 0;
    };
    auto m_del = iter_hash::compute_sizes(*this, [](sibling_iterator sib) { return !is(sib, deleted); });
    auto m_ins = iter_hash::compute_sizes(other, [](sibling_iterator sib) { return !is(sib, inserted); });

    mark      (m.get_to_remove(), rna_pair_label::deleted);
    other.mark(m.get_to_insert(), rna_pair_label::inserted);
    modify(other);

    it1 = begin();
    it2 = other.begin();
    end1 = end();
    end2 = other.end();

    while(it2 != end2)
    {
        if (has_ins(it2))
        {
            sib1 = it1.begin();
            sib2 = it2.begin();

            DEBUG("BEGIN");
            print_subtree(it1);
            print_subtree(it2);

            while (sib1 != it1.end())
            {
                if (is(sib2, inserted))
                {
                    size_t n;

                    if (!sib2->get_label().is_paired())
                        n = 0;
                    else
                    {
                        print_subtree(it1);
                        print_subtree(it2);
                        size_t needed_size = m_ins.at(sib2);
                        size_t actual_size = m_del.at(sib1);
                        n = 1;

                        while (needed_size > actual_size)
                        {
                            assert(sib1 != it1.end());
                            actual_size += m_del.at(sib1);
                            ++sib1;
                            ++n;
                        }
                        assert(needed_size == actual_size);
                        --sib1;
                    }
                    sib1 = insert(sib1, *sib2, n);
                }

                ++sib1;
                ++sib2;
            }
            //print_subtree(it1);
            //print_subtree(it2);

            while (sib2 != it2.end())
            {   // insert all right brothers of sib2
                if (is(sib2, inserted))
                {
                    assert(!sib2->get_label().is_paired());
                    sib1 = insert(sib1, *sib2, 0);
                    ++sib1;
                }
                ++sib2;
            }

            DEBUG("END");
            print_subtree(it1);
            print_subtree(it2);
        }
        if (is(it1, deleted))
        {
            ++it1;
            continue;
        }
        ++it1;
        ++it2;
    }

    print_tree();
    psout.save(*this);
    assert(it1 == end1 && it2 == end2);
}






size_t iter_hash::operator()(rna_tree::iterator it) const
{
    assert(it.node != nullptr);
    return it->get_id();
}

/* static */
template <typename funct>
iter_hash::iterator_hash_map iter_hash::compute_sizes(const rna_tree& rna, funct f)
{
    iterator_hash_map m;
    rna_tree::sibling_iterator sib;
    rna_tree::post_order_iterator it;

    m.reserve(rna.size());

    for (it = rna.begin_post(); it != rna.end_post(); ++it)
    {
        //rna.print_subtree(it);

        m[it] = f(it);
        for (sib = it.begin(); sib != it.end(); ++sib)
            m[it] += m.at(sib);
        //DEBUG("m[%s] = %lu", label(it), m[it]);
    }

    DEBUG("size=%lu", m.size());

    return m;
}



/* global function */
size_t get_label_index(
                rna_tree::pre_post_order_iterator iter)
{
    if (iter.is_preorder() || !iter->get_label().is_paired())
        return 0;
    else
        return 1;
}

/* global function */
size_t get_label_index(
                rna_tree::base_iterator iter)
{
    ERR("need explicitly pre_post_order_iterator !!!");
    abort();
}





