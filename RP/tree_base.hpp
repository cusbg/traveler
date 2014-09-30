/*
 * tree_base.hpp
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TREE_BASE_HPP
#define TREE_BASE_HPP

#include "types.hpp"
#include "tree_hh/tree.hh"
#include "tree_hh/tree_util.hh"
#include <memory>



template <typename node_type>
class tree_base
{
public:
    typedef tree<node_type> tree_type;
    using typename tree_type::sibling_iterator;
    using typename tree_type::post_order_iterator;
    typedef typename tree_type::iterator pre_order_iterator;
    typedef std::vector<sibling_iterator> path_type;

protected:
    std::shared_ptr<tree_type> tree_ptr;
    sibling_iterator tree_head;

public:
    virtual size_t size() const = 0;
protected:
    virtual ~tree_base() = default;
    tree_base() = default;
    tree_base(std::shared_ptr<tree_type> _tree_ptr, sibling_iterator _tree_head);

    virtual path_type get_path(path_enum e) const = 0;
    std::vector<tree_base<node_type> > divide_with_path(const path_type& path) const;
};




template <typename node_type>
tree_base<node_type>::tree_base(std::shared_ptr<tree_type> _tree_ptr, sibling_iterator _tree_head)
    : tree_ptr(_tree_ptr), tree_head(_tree_head)
{}

template <typename node_type>
std::vector<tree_base<node_type> > tree_base<node_type>::divide_with_path(const typename tree_base<node_type>::path_type& _path) const
{
    std::vector<tree_base<node_type> > out;

    for (auto itR : _path)
    {
        assert(tree_ptr->is_valid(itR));

        auto itL = itR;
        while(tree_ptr->is_valid(--itL))
        {
            tree_base<node_type> b(tree_ptr, itL);
            out.push_bask(b);
        }
        while(tree_ptr->is_valid(++itR))
        {
            tree_base<node_type> b(tree_ptr, itR);
            out.push_bask(b);
        }
    }
    return out;
}




#endif /* !TREE_BASE_HPP */

