/*
 * File: tree_base_utils.hpp
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

#ifndef TREE_BASE_UTILS_HPP
#define TREE_BASE_UTILS_HPP

#include "tree_base.hpp"
#include "tree_base_iter.hpp"


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

    iterator it;
    size_t i = 0;
    
    assert(_brackets.size() == _labels.size());

    _tree.set_head(label_type("ROOT_" + std::to_string(_id)));
    _size = 1;  // ROOT
    it = begin();

    while(i < _brackets.size())
    {
        switch (_brackets[i])
        {
            assert(_tree.is_valid(it));
            case '(':
                it = _tree.append_child(it, _labels[i]);
                ++_size;
                break;
            case ')':
                assert(!is_root(it));
                *it = *it + _labels[i];
                it = parent(it);
                break;
            case '.':
                _tree.append_child(it, _labels[i]);
                ++_size;
                break;
            default:
                throw std::invalid_argument(
                        std::string() +
                        "Tree constructor: invalid bracket character '" +
                        _brackets[i] +
                        "' at index " +
                        std::to_string(i)
                        );
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
    std::stringstream stream;
    stream
        << "SUBTREE("
        << ::label(root)
        << ":"
        << ::id(root)
        << "): \t";

    std::function<void(const iterator& it, std::stringstream& out)>
        print_recursive = [&print_recursive](iterator it, std::stringstream& out)
        {
            assert(it.node != nullptr);

            out << label(it);

            if (!is_leaf(it))
            {
                out << "(";

                assert(it.begin() != it.end());

                for (sibling_iterator sib = it.begin(); sib != it.end(); ++sib)
                {
                    print_recursive(sib, out);

                    if (!is_last_child(sib))
                        out << ", ";
                }
                out << ")";
            }
        };

    print_recursive(root, stream);
    if (debug_out)
        logger.debugStream() << stream.str();
    return stream.str();
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

