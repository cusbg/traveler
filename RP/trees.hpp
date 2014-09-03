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
    static size_t ID /* = 0 */;

public:
    typedef std::string label;
    typedef std::vector<Node> children;
    Node(const label& _l);
    Node();

    size_t findLongestPath() const
    {
        if (isroot)
            return 0;
        return (ch.size() != 1 ? 1 : 1 + ch[0].findLongestPath());
    }

    size_t createTree(const std::string& s, size_t from);
    size_t createTreeLabels(const std::string& s, size_t from);
    void push_back(const Node& _ch);
    const children& getChildren() const
    {
        return ch;
    }

    void makeRoot();
    bool isRoot()
    {
        return isroot;
    }
    bool empty() const
    {
        return ch.empty();
    }
    std::string getTreeLabels() const;
    std::string toString() const;
    void getIDs() const;
public:
    
private:
    children ch;
    label l;
    size_t id = ID++;

    enum { undefined, pair, base } type = undefined;
    bool isroot = false;
};



#endif /* !TREES_HPP */

