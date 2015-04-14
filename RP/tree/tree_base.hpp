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

#include "../types.hpp"
#include "tree_hh/tree.hh"
#include "tree_hh/tree_util.hh"
#include <memory>
#include <sstream>


template <typename iterator>
inline const char* label(iterator it);

template <typename iterator>
inline size_t id(iterator it);


template <typename node_type>
class tree_base
{
private:
    typedef tree<node_type> tree_type;
    class _reverse_post_order_iterator;

public:
    typedef typename tree_type::sibling_iterator sibling_iterator;
    typedef typename tree_type::post_order_iterator post_order_iterator;
    typedef _reverse_post_order_iterator reverse_post_order_iterator;
    typedef typename tree_type::iterator_base base_iterator;
    typedef typename tree_type::iterator iterator;
    typedef std::vector<sibling_iterator> path_type;
    typedef tree_node_<node_type> tree_node;

protected:
    tree_base() = default;

public:
    virtual ~tree_base() = default;
    template<typename labels_array>
        tree_base(const std::string& brackets, const labels_array& l);
    inline size_t size() const;
    /**
     * for each node in postorder calls node.reset_id() => node_ids became post_ordered
     */
    inline void set_ids_postorder();
    /*
     * find node with id/label
     */
    iterator find(size_t id) const;
    iterator find(node_type node) const;
    void print_subtree(iterator root) const;
    void print_tree() const;

public:
    // .begin() a .end() funkcie z tree<>
    inline iterator begin() const;
    inline iterator end() const;
    inline post_order_iterator begin_post() const;
    inline post_order_iterator end_post() const;
    // + reverzny post_order
    inline reverse_post_order_iterator begin_post_reverse() const;
    inline reverse_post_order_iterator end_post_reverse() const;

public: /* STATIC functions: */
    // POZOR! treba brat/vracat take iste typy ako povodne funkcie pre tree_type

    /** return parent of it in tree */
    template <typename iter>
        static iter parent(iter it);
    /** return n-th child of it */
    static sibling_iterator child(const base_iterator& it, size_t n);

    /** return it.child(0) */
    template <typename iter>
        static iter first_child(const iter& it);
    /** return it.child(it.number_of_children - 1) */
    template <typename iter>
        static iter last_child(const iter& it);

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
    /** parent has no other children */
    static bool is_only_child(const base_iterator& it);

private:
    tree_type _tree;
    static size_t ID;
    size_t _size = 1;   // vzdy je tam aspon ROOT_id
    size_t id = ID++;
    // do korena posadi vrchol ROOT_id
};


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



// STATIC member ID init
/* static */
template <typename node_type>
size_t tree_base<node_type>::ID = 0;



// CONSTRUCTORS: 

template <typename node_type> 
template <typename labels_array>
tree_base<node_type>::tree_base(const std::string& brackets, const labels_array& _labels)
{
    assert(brackets.size() == _labels.size());
    
    _tree.set_head(node_type("ROOT_" + std::to_string(id), true));
    //tree_ptr = std::make_shared<tree_type>(node_type("ROOT_" + std::to_string(id), true));
    auto iter = begin();
    size_t i = 0;
    while(i < brackets.size())
    {
        switch (brackets[i])
        {
            assert(_tree.is_valid(iter));
            //assert(tree_ptr->is_valid(iter));
            case '(':
                iter = _tree.append_child(iter, _labels[i]);
                //iter = tree_ptr->append_child(iter, _labels[i]);
                ++_size;
                break;
            case ')':
                assert(!iter->is_root());
                iter->set_label(iter->get_label() + _labels[i].get_label());
                    //TODO: nieco ako set_base_pair(b1, b2)
                iter = parent(iter);
                //iter = tree_ptr->parent(iter);
                break;
            case '.':
                _tree.append_child(iter, _labels[i]);
                //tree_ptr->append_child(iter, _labels[i]);
                ++_size;
                break;
            default:
                std::string s = std::string() + "Tree constructor: invalid bracket character '"
                    + brackets[i] + "' at index " + std::to_string(i);
                std::invalid_argument ex(s);
                throw ex;
        }
        ++i;
    }
    assert(_tree.size() == size());
    //assert(tree_ptr->size() == size());
    std::stringstream stream;
    stream << "TREE WAS CONSTRUCTED, size = " << size() << std::endl;
    kptree::print_tree_bracketed(_tree, stream);
    //kptree::print_tree_bracketed(*tree_ptr, stream);
    logger.info(stream.str());
}

// PUBLIC FUNCTIONS:

template <typename node_type>
size_t tree_base<node_type>::size() const
{
    return _size;
}

template <typename node_type>
void tree_base<node_type>::set_ids_postorder()
{
    for (auto it = _tree.begin_post(); it != _tree.end_post(); ++it)
        it->reset_id();
}

template <typename node_type>
typename tree_base<node_type>::iterator tree_base<node_type>::find(size_t _id) const
{
    for (auto it = begin(); it != end(); ++it)
        if (::id(it) == _id)
            return it;
    std::stringstream s;
    s << "id '" << _id << "' not found in tree";

    throw std::invalid_argument(s.str());
}

template <typename node_type>
typename tree_base<node_type>::iterator tree_base<node_type>::find(node_type node) const
{
    for (auto it = begin(); it != end(); ++it)
        if (::label(it) == node.get_label())
            return it;
    std::stringstream s;
    s << "label '" << node.get_label() << "' not found in tree";

    throw std::invalid_argument(s.str());
}

template <typename node_type>
void tree_base<node_type>::print_subtree(iterator root) const
{
    std::stringstream stream;
    kptree::print_subtree_bracketed(_tree, root, stream);
    logger.debug("%s", stream.str().c_str());
}

template <typename node_type>
void tree_base<node_type>::print_tree() const
{
    print_subtree(begin());
}



// ITERATORS:

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


// STATIC FUNCTIONS:

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
/*
[> static <]
template <typename node_type>
typename tree_base<node_type>::reverse_post_order_iterator tree_base<node_type>::parent(reverse_post_order_iterator it)
{
    //TODO
    return reverse_post_order_iterator(parent(iterator(it)));
}
*/
/* static */
template <typename node_type>
template <typename iter>
iter tree_base<node_type>::parent(iter it)
{
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
    return iter(it.node->first_child);
}

/* static */
template <typename node_type>
template <typename iter>
iter tree_base<node_type>::last_child(const iter& it)
{
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


// REVERSE POST ORDER ITERATOR FUNCTIONS:

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



// GLOBAL FUNCTIONS:

/* global function */
template <typename iterator>
const char* label(iterator it)
{
    if (it.node == NULL)
        return "<null>";
    return it->get_label().c_str();
}

/* global function */
template <typename iterator>
size_t id(iterator it)
{
    if (it.node == NULL)
        throw std::invalid_argument("id(): NULL iterator");
    return it->get_id();
}

#endif /* !TREE_BASE_HPP */

