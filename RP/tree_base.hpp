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
#include <sstream>



template <typename node_type>
class tree_base
{
public:
    typedef tree<node_type> tree_type;
    typedef typename tree_type::sibling_iterator sibling_iterator;
    typedef typename tree_type::post_order_iterator post_order_iterator;
    typedef typename tree_type::iterator pre_order_iterator;
    typedef std::vector<sibling_iterator> path_type;

    std::shared_ptr<tree_type> tree_ptr;
    sibling_iterator tree_head;

protected:
    tree_base() = default;
    tree_base(std::shared_ptr<tree_type> _tree_ptr, sibling_iterator _tree_head);


    //virtual path_type get_path(path_strategy e) const = 0;
    //virtual std::vector<tree_base<node_type> > divide_with_path(const path_type& path) const = 0;
    post_order_iterator most_left() const;

public:
    virtual ~tree_base();
    template<typename labels_array>
        tree_base(const std::string& brackets, const labels_array& l);
};



template <typename node_type>
tree_base<node_type>::~tree_base()
{}

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
typename tree_base<node_type>::post_order_iterator tree_base<node_type>::most_left() const
{
    //TODO: skusit inac ako number_of_children, lebo toto iteruje vsetkych synov..
    auto it = tree_head;
    while(tree_ptr->number_of_children(it) != 0)
        it = tree_ptr->child(it, 0);
    return it;
}

/*template <typename node_type>
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
}*/



#endif /* !TREE_BASE_HPP */

