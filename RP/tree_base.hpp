/*
 * File: tree_base.hpp
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


#ifndef TREE_BASE_HPP
#define TREE_BASE_HPP

#include "types.hpp"
#include "tree_hh/tree.hh"
#include "tree_hh/tree_util.hh"
#include <memory>
#include <sstream>


template <typename iterator>
const char* label(iterator it);

template <typename iterator>
size_t id(iterator it);


template <typename node_type>
class tree_base
{
private:
    typedef tree<node_type> tree_type;

public:
    typedef typename tree_type::sibling_iterator sibling_iterator;
    typedef typename tree_type::post_order_iterator post_order_iterator;
    typedef typename tree_type::iterator_base base_iterator;
    typedef typename tree_type::iterator iterator;
    typedef std::vector<sibling_iterator> path_type;

    std::shared_ptr<tree_type> tree_ptr;
    sibling_iterator tree_head;

protected:
    tree_base() = default;
    tree_base(std::shared_ptr<tree_type> _tree_ptr, sibling_iterator _tree_head);

public:
    virtual ~tree_base() = default;
    template<typename labels_array>
        tree_base(const std::string& brackets, const labels_array& l);
    /** for each node in postorder calls node.reset_id() => node_ids became post_ordered */
    void set_ids_postorder();

public: /* STATIC functions: */
    // POZOR! treba brat/vracat take iste typy ako povodne funkcie pre tree_type..

    /** return parent of it in tree */
    template <typename iter>
        static iter parent(iter it);
    /** return n-th child of it */
    static sibling_iterator child(const base_iterator& it, size_t n);

    /** return it.child(0) */
    template <typename iter>
        static iter first_child(iter it);
    /** return it.child(it.number_of_children - 1) */
    template <typename iter>
        static iter last_child(iter it);

    /**
     * return leftmost child in subtree rooted in it (=> it can be it itself)
     */
    static iterator leftmost_child(iterator it);
    /**
     * return rightmost child in subtree rooted in it (=> it can be it itself)
     */
    static iterator rightmost_child(iterator it);

    /** is first child of its parent */
    static bool is_first_child(const base_iterator& it);
    /** is last child of its parent */
    static bool is_last_child(const base_iterator& it);
    /** has no children */
    static bool is_leaf(const base_iterator& it);
};


template <typename node_type> 
template <typename labels_array>
tree_base<node_type>::tree_base(const std::string& brackets, const labels_array& _labels)
{
    assert(brackets.size() == _labels.size());
    
    tree_ptr = std::make_shared<tree_type>(node_type("ROOT", true));
    tree_head = tree_ptr->begin();
    auto iter = tree_ptr->begin();
    size_t i = 0;
    while(i < brackets.size())
    {
        switch (brackets[i])
        {
            assert(tree_ptr->is_valid(iter));
            case '(':
                iter = tree_ptr->append_child(iter, _labels[i]);
                break;
            case ')':
                assert(!iter->is_root());
                iter->set_label(iter->get_label() + _labels[i].get_label());
                    //TODO: nieco ako set_base_pair(b1, b2)
                iter = tree_ptr->parent(iter);
                break;
            case '.':
                tree_ptr->append_child(iter, _labels[i]);
                break;
            default:
                throw "EX";
        }
        ++i;
    }
    std::stringstream stream;
    kptree::print_tree_bracketed(*tree_ptr, stream);
    logger.info("tree was constructed:");
    logger.info(stream.str());
}

template <typename node_type>
tree_base<node_type>::tree_base(std::shared_ptr<tree_type> _tree_ptr, sibling_iterator _tree_head)
    : tree_ptr(_tree_ptr), tree_head(_tree_head)
{}

template <typename node_type>
void tree_base<node_type>::set_ids_postorder()
{
    for (auto it = tree_ptr->begin_post(); it != tree_ptr->end_post(); ++it)
        it->reset_id();
}


/* static */
template <typename node_type>
typename tree_base<node_type>::iterator tree_base<node_type>::leftmost_child(iterator it)
{
    while (!is_leaf(it))
        it = first_child(it);
    return it;
}

/* static */
template <typename node_type>
typename tree_base<node_type>::iterator tree_base<node_type>::rightmost_child(iterator it)
{
    while (!is_leaf(it))
        it = last_child(it);
    return it;
}


/* static */
template <typename node_type>
template <typename iter>
iter tree_base<node_type>::parent(iter it)
{
    return tree_type::parent(it);
}

/* static */
template <typename node_type>
typename tree_base<node_type>::sibling_iterator tree_base<node_type>::child(const base_iterator& it, size_t n)
{
    size_t ch_size = it.number_of_children();

    if(ch_size == 0 || n >= ch_size)
    {
        logger.fatal("node '%s:%lu' dont have so much children, %lu < %lu",
                label(it), it->get_id(), ch_size, n);
        exit(1);
    }

    return tree_type::child(it, n);
}


/* static */
template <typename node_type>
template <typename iter>
iter tree_base<node_type>::first_child(iter it)
{
    return iter(child(it, 0));
}

/* static */
template <typename node_type>
template <typename iter>
iter tree_base<node_type>::last_child(iter it)
{
    return iter(child(it, it.number_of_children() - 1));
}

/* static */
template <typename node_type>
bool tree_base<node_type>::is_first_child(const base_iterator& it)
{
    // it == it.begin() nefunguje!!!
    //return fist_child(parent(it)) == it;

    return first_child(parent(it)).node == it.node;

    //return tree_type::child(parent(it), 0) == it;
}

/* static */
template <typename node_type>
bool tree_base<node_type>::is_last_child(const base_iterator& it)
{
    // TODO: neist cez number_of_children, ale radsej cez tree::node..
    // .. nieje definovany operator==(base_it, base_it)..
    
    return last_child(parent(it)).node == it.node;
    
    //size_t n = parent(it).number_of_children();
    //return tree_type::child(parent(it), n - 1) == it;
}

/* static */
template <typename node_type>
bool tree_base<node_type>::is_leaf(const base_iterator& it)
{
    return it.number_of_children() == 0;
}



/* global function */
template <typename iterator>
const char* label(iterator it)
{
    return it->get_label().c_str();
}

/* global function */
template <typename iterator>
size_t id(iterator it)
{
    return it->get_id();
}

#endif /* !TREE_BASE_HPP */

