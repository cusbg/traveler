/*
 * File: tree_base.hpp
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

#ifndef TREE_BASE_HPP
#define TREE_BASE_HPP

#include "tree.hh"
#undef assert
#include "types.hpp"

//
// only declarations of classes/functions
//

template <typename label_type>
class tree_base
{
private:
    class                                           _pre_post_order_iterator;
    class                                           _reverse_post_order_iterator;
    
protected:
    typedef tree<label_type>                        tree_type;
    typedef tree_node_<label_type>                  tree_node_type;
    
public:
    typedef typename tree_type::iterator_base       base_iterator;
    typedef typename tree_type::iterator            iterator;
    typedef typename tree_type::sibling_iterator    sibling_iterator;
    typedef typename tree_type::post_order_iterator post_order_iterator;
    typedef _pre_post_order_iterator                pre_post_order_iterator;
    typedef _reverse_post_order_iterator            reverse_post_order_iterator;
    
protected:
    tree_base() = default;
    
public:
    virtual ~tree_base() = default;
    template <typename labels_array>
    tree_base(
              const std::string& brackets,
              const labels_array& labels);
    inline bool operator==(
                           const tree_base<label_type>& other) const;
    
public:
    inline size_t id() const;
    inline size_t size() const;
    
public:
    template <typename iterator_type, typename funct>
    static funct for_each_in_subtree(
                                     iterator_type root,
                                     funct function);
    
protected:
    template <typename iterator_type, typename funct>
    static funct for_each(
                          iterator_type begin,
                          iterator_type end,
                          funct function);
    
public:
    static std::string print_subtree(
                                     const iterator& root,
                                     bool debug_output = true);
    inline std::string print_tree(
                                  bool debug_output = true) const;
    
public:
    void set_postorder_ids();
    bool is_ordered_postorder() const;
    
public:
    /* .begin(), .end() functions from tree<> */
    inline iterator begin();
    inline iterator end();
    inline post_order_iterator begin_post();
    inline post_order_iterator end_post();
    /* + pre_post_order iterators && reverse_post_order_iterators */
    inline pre_post_order_iterator begin_pre_post();
    inline pre_post_order_iterator end_pre_post();
    inline reverse_post_order_iterator begin_rev_post();
    inline reverse_post_order_iterator end_rev_post();
    
protected:
    /* constant functions */
    inline iterator begin() const;
    inline iterator end() const;
    inline post_order_iterator begin_post() const;
    inline post_order_iterator end_post() const;
    
public:
    /* STATIC functions: */
    /* !!! RETURN same type as tree<> returns, or as parameter */
    
    template <typename iter>
    static iter parent(
                       const iter& it);
    template <typename iter>
    static iter first_child(
                            const iter& it);
    template <typename iter>
    static iter last_child(
                           const iter& it);
    
    static bool is_first_child(
                               const base_iterator& it);
    static bool is_last_child(
                              const base_iterator& it);
    static bool is_leaf(
                        const base_iterator& it);
    static bool is_only_child(
                              const base_iterator& it);
    static bool is_root(
                        const base_iterator& it);
    
    static bool is_valid(
                         const base_iterator& it);
    
    int depth(
              const base_iterator& it);
    
    base_iterator previous_sibling(const base_iterator& it)
    {
        return _tree.previous_sibling(it);
    }
    
    base_iterator next_sibling(const base_iterator& it)
    {
        return _tree.next_sibling(it);
    }
    
private:
    static size_t ID;
    
protected:
    size_t _id = ID++;
    tree_type _tree;
    size_t _size;
};


/* global, declaration */
template <typename iter>
inline size_t id(
                 const iter& it);

/* global, declaration */
template <typename iter>
inline std::string label(
                         const iter& it);

#define clabel(iter) ((label(iter)).c_str())

/* global, declaration */
template <typename iter>
inline iter plusplus(
                     iter it,
                     size_t n);



#include "tree_base_iter.hpp"
#include "tree_base_utils.hpp"

#endif /* !TREE_BASE_HPP */
