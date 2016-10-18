/*
 * File: tree_base_iter.hpp
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


#ifndef TREE_BASE_ITER_HPP
#define TREE_BASE_ITER_HPP

#include "tree_base.hpp"

/**
 * visit every non-leaf node 2x, once when going in the subtree, once going up
 */
template <typename label_type>
class tree_base<label_type>::_pre_post_order_iterator
    : public tree_base<label_type>::tree_type::iterator_base
{
public:
    typedef typename tree_base<
                        label_type>::tree_type::iterator_base iterator_base;

public:
    _pre_post_order_iterator(
                const iterator_base&,
                bool _preorder = true);
    _pre_post_order_iterator(
                tree_node_type* nodeptr = nullptr,
                bool _preorder = true);
    _pre_post_order_iterator(
                const _pre_post_order_iterator& other) = default;
    
    bool operator==(
                const _pre_post_order_iterator&) const;
    
    _pre_post_order_iterator    operator++(int);
    _pre_post_order_iterator&   operator++();

    inline bool preorder() const;
    inline size_t label_index() const;

private:
    bool _preorder;
};

/**
 * go post order from end of tree
 */
template <typename label_type>
class tree_base<label_type>::_reverse_post_order_iterator
    : public tree_base<label_type>::tree_type::iterator_base
{
public:
    typedef typename tree_base<
                        label_type>::tree_type::iterator_base iterator_base;

public:
    _reverse_post_order_iterator(const iterator_base&);
    _reverse_post_order_iterator(tree_node_type* nodeptr = nullptr);
    bool operator==(
                const iterator_base&) const;

public:
    _reverse_post_order_iterator& operator++();
    _reverse_post_order_iterator& operator--();
};

//
// _pre_post_order_iterator class functions:
//

template <typename label_type>
tree_base<label_type>::_pre_post_order_iterator::_pre_post_order_iterator(
                tree_node_type* nodeptr,
                bool preorder)
    : iterator_base(nodeptr),
    _preorder(preorder)
{}

template <typename label_type>
tree_base<label_type>::_pre_post_order_iterator::_pre_post_order_iterator(
                const iterator_base& it,
                bool preorder)
    : iterator_base(it.node),
    _preorder(preorder)
{}

template <typename label_type>
bool tree_base<label_type>::_pre_post_order_iterator::operator==(
                const _pre_post_order_iterator& it) const
{
    return it.node == this->node &&
        it.preorder() == this->preorder();
}

template <typename label_type>
typename tree_base<label_type>::_pre_post_order_iterator
tree_base<label_type>::_pre_post_order_iterator::operator++(int)
{
    _pre_post_order_iterator other = *this;
    ++(*this);
    return other;
}

template <typename label_type>
typename tree_base<label_type>::_pre_post_order_iterator&
tree_base<label_type>::_pre_post_order_iterator::operator++()
{
    // if preorder, go unltil i am leaf, then become postorder
    // if postorder and have next sibling, became him, and become preorder

    LOGGER_PRIORITY_ON_FUNCTION(INFO);
    DEBUG("BEG: %s", clabel(*this));

    if (_preorder)
    {
        if (is_leaf(*this))
        {
            _preorder = false;
            ++*this;
        }
        else
        {
            iterator i = *this;
            ++i;
            *this = i;
            _preorder = true;
        }
    }
    else
    {
        if (this->node->next_sibling != nullptr)
        {
            *this = base_iterator(this->node->next_sibling);
            if (!is_leaf(*this))
                _preorder = true;
            else
                _preorder = false;
        }
        else
        {
            post_order_iterator i = *this;
            ++i;
            *this = i;
            _preorder = false;
        }
    }
    DEBUG("END: %s", clabel(*this));

    return *this;
}

/* inline */
template <typename label_type>
bool tree_base<label_type>::_pre_post_order_iterator::preorder() const
{
    return _preorder;
}

/* inline */
template <typename label_type>
size_t tree_base<label_type>::_pre_post_order_iterator::label_index() const
{
    if (this->preorder() || !(*this)->paired())
        return 0;
    return 1;
}

//
// _reverse_post_order_iterator class functions:
//
template <typename label_type>
tree_base<label_type>::_reverse_post_order_iterator::_reverse_post_order_iterator(
                tree_node_type* nodeptr)
    : iterator_base(nodeptr)
{ }

template <typename label_type>
tree_base<label_type>::_reverse_post_order_iterator::_reverse_post_order_iterator(
                const iterator_base& it)
    : iterator_base(it.node)
{ }

template <typename label_type>
bool tree_base<label_type>::_reverse_post_order_iterator::operator==(
                const iterator_base& it) const
{
    return it.node == this->node;
}

template <typename label_type>
typename tree_base<label_type>::_reverse_post_order_iterator&
tree_base<label_type>::_reverse_post_order_iterator::operator++()
{
    *this = --iterator(*this);
    return *this;
}

template <typename label_type>
typename tree_base<label_type>::_reverse_post_order_iterator&
tree_base<label_type>::_reverse_post_order_iterator::operator--()
{
    *this = ++iterator(*this);
    return *this;
}



//
// TREE->ITERATOR functions:
//

template <typename label_type>
typename tree_base<label_type>::iterator
tree_base<label_type>::begin()
{
    return _tree.begin();
}

template <typename label_type>
typename tree_base<label_type>::iterator
tree_base<label_type>::end()
{
    return _tree.end();
}

template <typename label_type>
typename tree_base<label_type>::post_order_iterator
tree_base<label_type>::begin_post()
{
    return _tree.begin_post();
}

template <typename label_type>
typename tree_base<label_type>::post_order_iterator
tree_base<label_type>::end_post()
{
    return _tree.end_post();
}

template <typename label_type>
typename tree_base<label_type>::pre_post_order_iterator
tree_base<label_type>::begin_pre_post()
{
    return pre_post_order_iterator(begin(), true);
}

template <typename label_type>
typename tree_base<label_type>::pre_post_order_iterator
tree_base<label_type>::end_pre_post()
{
    return ++pre_post_order_iterator(begin(), false);
}

template <typename label_type>
typename tree_base<label_type>::reverse_post_order_iterator
tree_base<label_type>::begin_rev_post()
{
    return --end();
}

template <typename label_type>
typename tree_base<label_type>::reverse_post_order_iterator
tree_base<label_type>::end_rev_post()
{
    return --begin();
}



//
// protected TREE->ITERATOR functions:
//
template <typename label_type>
typename tree_base<label_type>::iterator
tree_base<label_type>::begin() const
{
    return _tree.begin();
}

template <typename label_type>
typename tree_base<label_type>::iterator
tree_base<label_type>::end() const
{
    return _tree.end();
}

template <typename label_type>
typename tree_base<label_type>::post_order_iterator
tree_base<label_type>::begin_post() const
{
    return _tree.begin_post();
}

template <typename label_type>
typename tree_base<label_type>::post_order_iterator
tree_base<label_type>::end_post() const
{
    return _tree.end_post();
}






/* static */
template <typename label_type>
template <typename iter>
iter tree_base<label_type>::parent(
                const iter& it)
{
    static_assert(!std::is_same<iter,
            tree_base<label_type>::pre_post_order_iterator>::value,
            "call parent(pre_post_order_iterator) is forbidden");

    assert(it.node->parent != nullptr);
    return iter(it.node->parent);
}

/* static */
template <typename label_type>
template <typename iter>
iter tree_base<label_type>::first_child(
                const iter& it)
{
    assert(!is_leaf(it));
    return iter(it.node->first_child);
}

/* static */
template <typename label_type>
template <typename iter>
iter tree_base<label_type>::last_child(
                const iter& it)
{
    assert(!is_leaf(it));
    return iter(it.node->last_child);
}

/* static */
template <typename label_type>
bool tree_base<label_type>::is_first_child(
                const base_iterator& it)
{
    assert(it.node != nullptr);
    return it.node->prev_sibling == nullptr;
}

/* static */
template <typename label_type>
bool tree_base<label_type>::is_last_child(
                const base_iterator& it)
{
    assert(it.node != nullptr);
    return it.node->next_sibling == nullptr;
}

/* static */
template <typename label_type>
bool tree_base<label_type>::is_leaf(
                const base_iterator& it)
{
    assert(it.node != nullptr);
    return it.begin() == it.end();
}

/* static */
template <typename label_type>
bool tree_base<label_type>::is_only_child(
                const base_iterator& it)
{
    assert(it.node != nullptr);
    return it.node->prev_sibling == nullptr &&
        it.node->next_sibling == nullptr;
}

/* static */
template <typename label_type>
bool tree_base<label_type>::is_root(
                const base_iterator& it)
{
    return it.node->parent == nullptr;
}

/* static */
template <typename label_type>
bool tree_base<label_type>::is_valid(
                const base_iterator& it)
{
    return it.node != nullptr;
}


/* static */
template <typename label_type>
template <typename iterator_type, typename funct>
funct tree_base<label_type>::for_each_in_subtree(
                iterator_type root,
                funct f)
{
    pre_post_order_iterator begin(root, true);
    pre_post_order_iterator end(root, false);
    ++end;

    return std::move(for_each(begin, end, f));
}

/* static */
template <typename label_type>
template <typename iterator_type, typename funct>
funct tree_base<label_type>::for_each(
                iterator_type begin,
                iterator_type end,
                funct f)
{
    while (begin != end)
    {
        f(begin);
        ++begin;
    }
    return std::move(f);
}


#endif /* !TREE_BASE_ITER_HPP */

