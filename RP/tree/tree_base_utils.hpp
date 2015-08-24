/*
 * File: tree_base_utils.hpp
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

#ifndef TREE_BASE_UTILS_HPP
#define TREE_BASE_UTILS_HPP

#ifndef TREE_BASE_HPP
#include "tree_base.hpp"
#endif


/* inner class postorder iterator */
template <typename node_type>
class tree_base<node_type>::_reverse_post_order_iterator : public tree_base<node_type>::tree_type::iterator_base
{
    typedef typename tree_base<node_type>::tree_type::iterator_base iterator_base;
public:
    _reverse_post_order_iterator(const iterator_base&);
    _reverse_post_order_iterator(tree_node* nodeptr = nullptr);
    
    bool operator==(const _reverse_post_order_iterator&) const;
    bool operator!=(const _reverse_post_order_iterator&) const;
    
    _reverse_post_order_iterator   operator++(int);
    _reverse_post_order_iterator&  operator++();
};

/* inner class prepostorder iterator */
/* visit every non-leaf node 2x, once when going in the subtree, once when going up */
template <typename node_type>
class tree_base<node_type>::_pre_post_order_iterator : public tree_base<node_type>::tree_type::iterator_base
{
    typedef typename tree_base<node_type>::tree_type::iterator_base iterator_base;
public:
    _pre_post_order_iterator(const iterator_base&, bool _preorder = true);
    _pre_post_order_iterator(tree_node* nodeptr = nullptr, bool _preorder = true);
    
    bool operator==(const _pre_post_order_iterator&) const;
    bool operator!=(const _pre_post_order_iterator&) const;
    
    _pre_post_order_iterator   operator++(int);
    _pre_post_order_iterator&  operator++();

    friend _pre_post_order_iterator tree_base<node_type>::begin_pre_post() const;
    friend _pre_post_order_iterator tree_base<node_type>::end_pre_post() const;

    bool is_preorder() const;

private:
    bool preorder;
};

template <typename node_type>
struct tree_base<node_type>::iterator_hash
{
    size_t operator()(iterator it) const;
};



// GLOBAL FUNCTIONS:

/* global function */
template <typename iterator>
std::string label(iterator it)
{
    if (it.node == NULL)
        return "<null>";
    // TODO: zmenit to, vraciam pointer do mazaneho objektu
    std::stringstream out;
    out << (*it);
    return out.str().c_str();
}

/* global function */
template <typename iterator>
size_t id(iterator it)
{
    if (it.node == NULL)
        throw std::invalid_argument("id(): NULL iterator");
    return it->get_id();
}


/* global function */
template <typename iter>
iter move_it_plus(iter it, size_t count)
{
    while(count--)
        ++it;
    return it;
}

/* global function */
template <typename iter>
iter move_it_minus(iter it, size_t count)
{
    while(count--)
        --it;
    return it;
}

/* global function */
template <typename iter, typename funct>
size_t count_children_if(iter it, funct f)
{
    size_t count = 0;
    auto sib = it.begin();
    while (sib != it.end())
    {
        if (f(sib))
            ++count;
        ++sib;
    }
    return count;
}

/* global function */
template <typename iter, typename funct>
inline size_t count_if(iter begin, iter end, funct f)
{
    size_t count = 0;
    while (begin != end)
    {
        if (f(begin))
            ++count;
        ++begin;
    }
    return count;
}



// ITERATORS:




/* _pre_post_order_iterator class functions: */
template <typename node_type>
tree_base<node_type>::_pre_post_order_iterator::_pre_post_order_iterator(tree_node* nodeptr, bool _preorder)
    : iterator_base(nodeptr), preorder(_preorder)
{}

template <typename node_type>
tree_base<node_type>::_pre_post_order_iterator::_pre_post_order_iterator(const iterator_base& it, bool _preorder)
    : iterator_base(it.node), preorder(_preorder)
{}

template <typename node_type>
bool tree_base<node_type>::_pre_post_order_iterator::operator!=(const _pre_post_order_iterator& it) const
{
    return !(it == *this);
}

template <typename node_type>
bool tree_base<node_type>::_pre_post_order_iterator::operator==(const _pre_post_order_iterator& it) const
{
    return it.node == this->node && it.preorder == this->preorder;
}

template <typename node_type>
typename tree_base<node_type>::_pre_post_order_iterator tree_base<node_type>::_pre_post_order_iterator::operator++(int)
{
    _pre_post_order_iterator other = *this;
    ++(*this);
    return other;
}

template <typename node_type>
typename tree_base<node_type>::_pre_post_order_iterator& tree_base<node_type>::_pre_post_order_iterator::operator++()
{
    // if preorder, go unltil i am leaf, then become postorder
    // if postorder and have next sibling, became him, and become preorder

    LOGGER_PRIORITY_ON_FUNCTION(INFO);
    DEBUG("%s", clabel(*this));
    if (preorder)
    {
        if (is_leaf(*this))
        {
            DEBUG("is preorder leaf, go postorder");
            preorder = false;
            ++*this;
        }
        else
        {
            DEBUG("is preorder, go preorder");
            iterator i = *this;
            ++i;
            *this = i;
            preorder = true;
        }
    }
    else
    {
        if (this->node->next_sibling != nullptr)
        {
            DEBUG("is postorder with sibling, go to sibling");
            *this = base_iterator(this->node->next_sibling);
            if (!is_leaf(*this))
                preorder = true;
            else
                preorder = false;
        }
        else
        {
            DEBUG("is postorder, go postorder");
            post_order_iterator i = *this;
            ++i;
            *this = i;
            preorder = false;
        }
    }
    DEBUG("%s", clabel(*this));

    return *this;
}

template <typename node_type>
bool tree_base<node_type>::_pre_post_order_iterator::is_preorder() const
{
    return preorder;
}



/* _reverse_post_order_iterator class functions: */
template <typename node_type>
tree_base<node_type>::_reverse_post_order_iterator::_reverse_post_order_iterator(tree_node* nodeptr)
    : iterator_base(nodeptr)
{}

template <typename node_type>
tree_base<node_type>::_reverse_post_order_iterator::_reverse_post_order_iterator(const iterator_base& it)
    : iterator_base(it.node)
{}

template <typename node_type>
bool tree_base<node_type>::_reverse_post_order_iterator::operator!=(const _reverse_post_order_iterator& it) const
{
    return it.node != this->node;
}

template <typename node_type>
bool tree_base<node_type>::_reverse_post_order_iterator::operator==(const _reverse_post_order_iterator& it) const
{
    return it.node == this->node;
}

template <typename node_type>
typename tree_base<node_type>::_reverse_post_order_iterator tree_base<node_type>::_reverse_post_order_iterator::operator++(int)
{
    _reverse_post_order_iterator other = *this;
    ++(*this);
    return other;
}

template <typename node_type>
typename tree_base<node_type>::_reverse_post_order_iterator& tree_base<node_type>::_reverse_post_order_iterator::operator++()
{
    //// z _tree<>::post_order_iterator::operator++():
    //
	//assert(this->node!=0);
	//if(this->node->next_sibling==0) {
		//this->node=this->node->parent;
		//this->skip_current_children_=false;
		//}
	//else {
		//this->node=this->node->next_sibling;
		//if(this->skip_current_children_) {
			//this->skip_current_children_=false;
			//}
		//else {
			//while(this->node->first_child)
				//this->node=this->node->first_child;
			//}
		//}
	//return *this;

    assert(this->node != 0);
    if (this->node->prev_sibling == 0)
    {
        this->node = this->node->parent;
        this->skip_current_children_ = false;
    }
    else
    {
        this->node = this->node->prev_sibling;
        if (this->skip_current_children_)
            this->skip_current_children_ = false;
        else
        {
            while(this->node->last_child)
                this->node = this->node->last_child;
        }
    }
    return *this;
}



/* normal iterators + post_order iterators returns tree_ptr->FUNCT_NAME()..  */
template <typename node_type>
typename tree_base<node_type>::iterator tree_base<node_type>::begin() const
{
    return _tree.begin();
}

template <typename node_type>
typename tree_base<node_type>::iterator tree_base<node_type>::end() const
{
    return _tree.end();
}

template <typename node_type>
typename tree_base<node_type>::post_order_iterator tree_base<node_type>::begin_post() const
{
    return _tree.begin_post();
}

template <typename node_type>
typename tree_base<node_type>::post_order_iterator tree_base<node_type>::end_post() const
{
    return _tree.end_post();
}

/* reverse iterator .begin() and .end(): */
template <typename node_type>
typename tree_base<node_type>::reverse_post_order_iterator tree_base<node_type>::begin_post_reverse() const
{
    //// z _tree<>::begin_post():
    //
	//tree_node *tmp=head->next_sibling;
	//if(tmp!=feet) {
		//while(tmp->first_child)
			//tmp=tmp->first_child;
		//}
	//return post_order_iterator(tmp);
    
    tree_node* tmp = _tree.head->next_sibling;
    if (tmp != _tree.feet)
    {
        while(tmp->last_child)
            tmp = tmp->last_child;
    }
    return reverse_post_order_iterator(base_iterator(tmp));
}

template <typename node_type>
typename tree_base<node_type>::reverse_post_order_iterator tree_base<node_type>::end_post_reverse() const
{
    //// z _tree<>::end_post():
    //
	//return post_order_iterator(feet);
    //
    //// Ja idem vzdy do .prev_sibling v ++ => nakoniec dojdem do .head
    
    return reverse_post_order_iterator(base_iterator(_tree.head));
}

/* pre_post iterator .begin() and .end(): */
template <typename node_type>
typename tree_base<node_type>::pre_post_order_iterator tree_base<node_type>::begin_pre_post() const
{
    pre_post_order_iterator it = begin();
    it.preorder = true;
    return it;
}

template <typename node_type>
typename tree_base<node_type>::pre_post_order_iterator tree_base<node_type>::end_pre_post() const
{
    pre_post_order_iterator it = begin();
    it.preorder = false;
    ++it;
    return it;
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
    assert(it.node->parent != nullptr);
    return iter(it.node->parent);
}

/* static */
template <typename node_type>
typename tree_base<node_type>::sibling_iterator tree_base<node_type>::child(const base_iterator& it, size_t n)
{
    assert(it.number_of_children() > n);

    return tree_type::child(it, n);
}


/* static */
template <typename node_type>
template <typename iter>
iter tree_base<node_type>::first_child(const iter& it)
{
    assert(!is_leaf(it));
    return iter(it.node->first_child);
}

/* static */
template <typename node_type>
template <typename iter>
iter tree_base<node_type>::last_child(const iter& it)
{
    assert(!is_leaf(it));
    return iter(it.node->last_child);
}

/* static */
template <typename node_type>
bool tree_base<node_type>::is_first_child(const base_iterator& it)
{
    assert(it.node != nullptr);
    return it.node->prev_sibling == nullptr;
}

/* static */
template <typename node_type>
bool tree_base<node_type>::is_last_child(const base_iterator& it)
{
    assert(it.node != nullptr);
    return it.node->next_sibling == nullptr;
}

/* static */
template <typename node_type>
bool tree_base<node_type>::is_leaf(const base_iterator& it)
{
    assert(it.node != nullptr);
    return it.node->last_child == nullptr;
}

/* static */
template <typename node_type>
bool tree_base<node_type>::is_only_child(const base_iterator& it)
{
    assert(it.node != nullptr);
    return (it.node->parent == nullptr ||
            it.node->parent->first_child == it.node->parent->last_child);
}



template <typename node_type>
size_t tree_base<node_type>::iterator_hash::operator()(iterator it) const
{
    assert(it.node != nullptr);
    return ::id(it);
}


#endif /* !TREE_BASE_UTILS_HPP */

