/*
 * node.hpp
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef NODE_HPP
#define NODE_HPP

#include "node_base.hpp"


template <typename label_type>
class node : public node_base <label_type>
{
private:
    size_t depth = 0xBADF00D;
    bool depth_was_set = false;
public:
    node();
    node(size_t _depth);
    virtual ~node();
    void set_depth(size_t _depth);
};



template <typename label_type>
node<label_type>::node()
{}

template <typename label_type>
node<label_type>::node(size_t _depth)
    : depth(_depth), depth_was_set(true)
{}

template <typename label_type>
node<label_type>::~node()
{}

template <typename label_type>
void node<label_type>::set_depth(size_t _depth)
{
    depth = _depth;
}


#endif /* !NODE_HPP */

