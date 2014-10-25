/*
 * node_base.hpp
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef NODE_BASE_HPP
#define NODE_BASE_HPP

#include "types.hpp"


template <typename label_type>
class node_base
{
private:
    static size_t ID;
    size_t id = ID++;
    label_type label;
    bool isroot = false;
public:
    virtual ~node_base();
    node_base() = default;
    node_base(const label_type& _label, bool _isroot = false);
public:
    size_t get_id() const;
    const label_type& get_label() const;
    void set_label(const label_type& _label);
    friend std::ostream& operator<<(std::ostream& out, const node_base<label_type>& n)
    {
        out << n.label;
        return out;
    }
    virtual bool is_root() const;
    bool operator==(const node_base<label_type>& other) const;
};



template <typename label_type>
size_t node_base<label_type>::ID = 0;

template <typename label_type>
node_base<label_type>::~node_base()
{}

template<typename label_type>
node_base<label_type>::node_base(const label_type& _label, bool _isroot)
    : label(_label), isroot(_isroot)
{}

template <typename label_type>
size_t node_base<label_type>::get_id() const
{
    return id;
}

template <typename label_type>
const label_type& node_base<label_type>::get_label() const
{
    return label;
}

template <typename label_type>
void node_base<label_type>::set_label(const label_type& _label)
{
    label = _label;
}

template <typename label_type>
bool node_base<label_type>::is_root() const
{
    return isroot;
}

template <typename label_type>
bool node_base<label_type>::operator==(const node_base<label_type>& other) const
{
    return label == other.label;
}

#endif /* !NODE_BASE_HPP */

