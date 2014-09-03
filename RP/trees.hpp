/*
 * trees.hpp
 * Copyright (C) 2014 eliasr <eliasr@u-pl22>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TREES_HPP
#define TREES_HPP

#include <iostream>
#include "types.hpp"

class Node
{
private:
    static size_t ID; // ID je inicializovane na 0
public:
    typedef std::string labelType;
    typedef std::vector<Node> childrenType;
protected:
    labelType label;
    childrenType children;
    size_t id = ID++;
private:
public:
    virtual ~Node();
    Node(const labelType& _label = std::to_string(ID));

    size_t createTree(const std::string& labels, const std::string& brackets, size_t from);
        // vytvori cely strom podla zatvoriek a da im labely
        //      podla toho ci to je par (i,j) -> labels[i]labels[j]
        //      alebo je to len samotna baza i -> labels[i]
    const childrenType& getChildren() const;
    const labelType& getLabel() const;
    size_t getId() const;
};





#endif /* !TREES_HPP */

