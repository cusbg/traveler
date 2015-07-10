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

#include <unordered_map>

#include "rna_tree.hpp"
#include "rna_tree_labels.hpp"
#include "util.hpp"

using namespace std;

/*
 * make vector of rna_nodes from string
 */
inline std::vector<rna_node_type> convert(
                const std::string& labels);

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
                const rna_tree& rna,
                funct f);
};


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
                const rna_tree& other) const
{
    return _tree.equal_subtree(begin(), other.begin());
}







void rna_tree::mark(
                std::vector<size_t> postorder_indexes,
                rna_pair_label::label_status_type status)
{
    APP_DEBUG_FNAME;
    post_order_iterator it = begin_post();
    size_t i = 0;
    for (size_t index : postorder_indexes)
    {
        --index;    // indexy cislovane od 1
        size_t to_move = index - i;
        it = move_it_plus(it, to_move);
        it->get_label().status = status;
        i = index;
    }
}

void rna_tree::modify(
                const rna_tree& other)
{
    APP_DEBUG_FNAME;

    iterator it1, it2;
    it1 = begin();
    it2 = other.begin();

#define is_ins_del(iter) \
    (is(iter, rna_pair_label::inserted) || \
     is(iter, rna_pair_label::deleted))

    while (it1 != end() && it2 != other.end())
    {
        if (is_ins_del(it1))
        {
            ++it1;
            continue;
        }
        if (is_ins_del(it2))
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

rna_tree::iterator rna_tree::insert(sibling_iterator it, rna_pair_label lbl, size_t steal)
{
    DEBUG("insert(%s, %lu) <- %s", label(it), steal, lbl.to_string().c_str());

    sibling_iterator in, next;
    rna_node_type node(lbl);
    
    in = _tree.insert(it, node);
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

template <typename iter>
    iter rna_tree::erase(iter it)
{
    DEBUG("erasing node %s", label_str(*it));

    iter del;

    it = _tree.flatten(it);
    assert(is_leaf(it));

    del = it++;
    _tree.erase(del);
    --_size;
    return it;
}

/* static */ void rna_tree::merge(
                rna_tree& templated,
                rna_tree other,
                const mapping& m)
{
    APP_DEBUG_FNAME;

    iterator it1, it2;
    iterator end1, end2;
    sibling_iterator sib1, sib2, ins;
    iter_hash::iterator_hash_map m_del, m_ins;

    templated.mark  (m.get_to_remove(), rna_pair_label::deleted);
    other.mark      (m.get_to_insert(), rna_pair_label::inserted);
    templated.modify(other);

    m_del = iter_hash::compute_sizes(templated, [](sibling_iterator sib)
            { return !is(sib, rna_pair_label::deleted); });
    m_ins = iter_hash::compute_sizes(other, [](sibling_iterator sib)
            { return !is(sib, rna_pair_label::inserted); });
    
    assert(m_del.at(templated.begin()) == m_ins.at(other.begin()));

    it1 = templated.begin();
    it2 = other.begin();
    end1 = templated.end();
    end2 = other.end();

    while(it2 != end2)
    {
        if (has_child(it2, rna_pair_label::inserted))
        {
            DEBUG("BEGIN");
            print_subtree(it1);
            print_subtree(it2);

            sib1 = it1.begin();
            sib2 = it2.begin();

            while (sib1 != it1.end())
            {
                if (sib1->get_label().is_paired() && is(sib1, rna_pair_label::deleted)
                        && !sib2->get_label().is_paired())
                {
                    m_del[sib1] = 0;
                    sib1 = templated.erase(sib1);
                    continue;
                }
                if (is(sib2, rna_pair_label::inserted))
                {
                    size_t n;
                    ins = sib1;

                    if (!sib2->get_label().is_paired())
                        n = 0;
                    else
                    {
                        size_t needed_size = m_ins.at(sib2);
                        size_t actual_size = m_del.at(sib1);
                        n = 1;

                        ++sib1;
                        while(true)
                        {
                            if (needed_size == actual_size)
                                break;
                            if (needed_size > actual_size)
                            {
                                assert(sib1 != it1.end());
                                actual_size += m_del.at(sib1);
                                ++sib1;
                                ++n;
                            }
                            else
                            {
                                assert(needed_size < actual_size);
                                assert(is(sib1, rna_pair_label::deleted));
                                actual_size -= m_del.at(sib1);
                                m_del[sib1] = 0;
                                sib1 = templated.erase(sib1);
                            }
                        }
                        --sib1;
                    }
                    sib1 = templated.insert(ins, sib2->get_label(), n);
                }

                ++sib1;
                ++sib2;
            }
            print_subtree(it1);
            print_subtree(it2);

            while (sib2 != it2.end())
            {   // insert all right brothers of sib2
                if (is(sib2, rna_pair_label::inserted))
                {
                    if (sib2->get_label().is_paired())
                    {
                        print_subtree(it1);
                        print_subtree(it2);
                        abort();
                    }
                    assert(!sib2->get_label().is_paired());
                    sib1 = templated.insert(sib1, sib2->get_label(), 0);
                    ++sib1;
                }
                ++sib2;
            }

            DEBUG("END");
            print_subtree(it1);
            print_subtree(it2);
        }
        if (is(it1, rna_pair_label::deleted))
        {
            ++it1;
            continue;
        }
        ++it1;
        ++it2;
    }
    assert(it1 == end1 && it2 == end2);
}

/*
void rna_tree::merge(rna_tree other, const mapping& m)
{   // this <- by mala byt template rna, s inicializovanymi bodmi..
    APP_DEBUG_FNAME;
    //LOGGER_PRIORITY_ON_FUNCTION(INFO);

    iterator it1, it2;
    iterator end1, end2;
    sibling_iterator sib1, sib2, ins;
    iter_hash::iterator_hash_map m_del, m_ins;

    mark      (m.get_to_remove(), rna_pair_label::deleted);
    other.mark(m.get_to_insert(), rna_pair_label::inserted);
    modify(other);

    m_del = iter_hash::compute_sizes(*this, [](sibling_iterator sib)
            { return !is(sib, rna_pair_label::deleted); });
    m_ins = iter_hash::compute_sizes(other, [](sibling_iterator sib)
            { return !is(sib, rna_pair_label::inserted); });
    
    assert(m_del.at(begin()) == m_ins.at(other.begin()));

    it1 = begin();
    it2 = other.begin();
    end1 = end();
    end2 = other.end();

    while(it2 != end2)
    {
        if (has_child(it2, rna_pair_label::inserted))
        {
            DEBUG("BEGIN");
            print_subtree(it1);
            print_subtree(it2);

            sib1 = it1.begin();
            sib2 = it2.begin();

            while (sib1 != it1.end())
            {
                if (sib1->get_label().is_paired() && is(sib1, rna_pair_label::deleted)
                        && !sib2->get_label().is_paired())
                {
                    m_del[sib1] = 0;
                    sib1 = erase(sib1);
                    continue;
                }
                if (is(sib2, rna_pair_label::inserted))
                {
                    size_t n;
                    ins = sib1;

                    if (!sib2->get_label().is_paired())
                        n = 0;
                    else
                    {
                        size_t needed_size = m_ins.at(sib2);
                        size_t actual_size = m_del.at(sib1);
                        n = 1;

                        ++sib1;
                        while(true)
                        {
                            if (needed_size == actual_size)
                                break;
                            if (needed_size > actual_size)
                            {
                                assert(sib1 != it1.end());
                                actual_size += m_del.at(sib1);
                                ++sib1;
                                ++n;
                            }
                            else
                            {
                                assert(needed_size < actual_size);
                                assert(is(sib1, rna_pair_label::deleted));
                                actual_size -= m_del.at(sib1);
                                m_del[sib1] = 0;
                                sib1 = erase(sib1);
                            }
                        }
                        --sib1;
                    }
                    sib1 = insert(ins, sib2->get_label(), n);
                }

                ++sib1;
                ++sib2;
            }
            print_subtree(it1);
            print_subtree(it2);

            while (sib2 != it2.end())
            {   // insert all right brothers of sib2
                if (is(sib2, rna_pair_label::inserted))
                {
                    if (sib2->get_label().is_paired())
                    {
                        print_subtree(it1);
                        print_subtree(it2);
                        abort();
                    }
                    assert(!sib2->get_label().is_paired());
                    sib1 = insert(sib1, sib2->get_label(), 0);
                    ++sib1;
                }
                ++sib2;
            }

            DEBUG("END");
            print_subtree(it1);
            print_subtree(it2);
        }
        if (is(it1, rna_pair_label::deleted))
        {
            ++it1;
            continue;
        }
        ++it1;
        ++it2;
    }
    assert(it1 == end1 && it2 == end2);
}
*/



size_t iter_hash::operator()(rna_tree::iterator it) const
{
    assert(it.node != nullptr);
    return it->get_id();
}

/* static */ template <typename funct>
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




/* global */ size_t get_label_index(
                rna_tree::pre_post_order_iterator iter)
{
    if (iter.is_preorder() || !iter->get_label().is_paired())
        return 0;
    else
        return 1;
}


/* global */ bool is(
                rna_tree::iterator it,
                rna_pair_label::label_status_type status)
{
    return it->get_label().status == status;
}

/* global */ bool has_child(
                rna_tree::iterator it,
                rna_pair_label::label_status_type status)
{
    return count_children_if(it,
            [status](rna_tree::iterator _iter) {return is(_iter, status);}) != 0;
}



