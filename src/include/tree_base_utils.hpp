/*
 * File: tree_base_utils.hpp
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */

#ifndef TREE_BASE_UTILS_HPP
#define TREE_BASE_UTILS_HPP

#include "tree_base.hpp"

#define PSEUDOKNOT_CHARACTERS "[{}]"

//
// tree<> functions:
//

/* static */
template <typename label_type>
size_t tree_base<label_type>::ID = 0;


template <typename label_type>
template <typename labels_array>
tree_base<label_type>::tree_base(
                                 const std::string& _brackets,
                                 const labels_array& _labels)
{
    APP_DEBUG_FNAME;
    

    if (_brackets.size() != _labels.size()) {
        std::ostringstream out;
        for (int i = 0; i < _labels.size(); ++i) out << _labels[i].at(0).label;
        throw wrong_argument_exception(
                std::string("\nNumber of brackets != Number of labels\nBrackets: " + _brackets + "\nLabels:   "+out.str()).c_str());
    }

    iterator it;
    size_t i = 0;


    label_type root = label_type(""
                                 "ROOT_" + std::to_string(_id)) + label_type("");
    _tree.set_head(root);
    _size = 1;  // ROOT
    it = begin();
    
    while(i < _brackets.size())
    {
        char ch = _brackets[i];
        if (ch == '(')
        {
            it = _tree.append_child(it, _labels[i]);
            ++_size;
        }
        else if(ch == ')')
        {
            assert(!is_root(it));
            *it = *it + _labels[i];
            it = parent(it);
        }
        else if (ch == '.' || contains(PSEUDOKNOT_CHARACTERS, ch))
        {
            _tree.append_child(it, _labels[i]);
            ++_size;
        }
        else
        {
            throw wrong_argument_exception("Invalid dot-bracket character '%s' at index %s", ch, i);
        }
        ++i;
    }
    assert(_tree.size() == size());
}

template <typename label_type>
bool tree_base<label_type>::operator==(
                                       const tree_base<label_type>& other) const
{
    return _tree.equal_subtree(_tree.begin(), other._tree.begin());
}

/* inline */
template <typename label_type>
size_t tree_base<label_type>::id() const
{
    return _id;
}

/* inline */
template <typename label_type>
size_t tree_base<label_type>::size() const
{
    return _size;
}

/* inline */
template <typename label_type>
std::string tree_base<label_type>::print_tree(
                                              bool debug_out) const
{
    return print_subtree(_tree.begin(), debug_out);
}

/* static */
template <typename label_type>
std::string tree_base<label_type>::print_subtree(
                                                 const iterator& root,
                                                 bool debug_out)
{
    std::ostringstream out;
    out
    << "SUBTREE("
    << ::label(root)
    << ":"
    << ::id(root)
    << "): \t";
    
    auto f = [&out](const pre_post_order_iterator& iter)
    {
        if (is_leaf(iter))
        {
            out << label(iter);
            
            if (!is_last_child(iter))
                out << ", ";
        }
        else
        {
            if (iter.preorder())
                out << label(iter) << "(";
            else
            {
                out << ")";
                if (!is_last_child(iter) && !is_root(iter))
                    out << ", ";
            }
        }
    };
    
    for_each_in_subtree(root, f);
    
    if (debug_out)
        DEBUG("%s", out.str());
    
    return out.str();
}

template <typename label_type>
void tree_base<label_type>::set_postorder_ids()
{
    APP_DEBUG_FNAME;

    post_order_iterator it;

    label_type::reset_ID();

    for (it = begin_post(); it != end_post(); ++it)
        it->reset_id();

    assert(size() - 1 == ::id(begin()));
}

//template <typename label_type>
//void tree_base<label_type>::set_pre_postorder_ids(const mapping& mapping)
//{
//    APP_DEBUG_FNAME;
//
//    pre_post_order_iterator it;
//    int i = 0;
//    for (it = begin_pre_post(); it != end_pre_post(); ++it){
//        i++;
////        it->set_seq_id_mapped(i);
//    }
//
//}

template <typename label_type>
bool tree_base<label_type>::is_ordered_postorder() const
{
    size_t i = 0;
    for (post_order_iterator it = begin_post(); it != end_post(); ++it, ++i)
        if (i != ::id(it))
            return false;
    
    return true;
}

//
// global functions:
//

/* global */
/* inline */
template <typename iter>
size_t id(
          const iter& it)
{
    if (it.node == nullptr)
        throw std::invalid_argument("id(iter): NULL iterator");
    return it->id();
}

/* global */
/* inline */
template <typename iter>
std::string label(
                  const iter& it)
{
    if (it.node == nullptr)
        return "<null>";
    else
        return to_string(*it);
}

/* global */
/* inline */
template <typename iter>
inline iter plusplus(
                     iter it,
                     size_t n)
{
    while (n--)
        ++it;
    return it;
}

#endif /* !TREE_BASE_UTILS_HPP */
