/*
 * File: trees.cpp
 * Created: 2014-09-01
 * By: eliasr <eliasr@u-pl22>
 */

#include "trees.hpp"

using namespace std;

/* static */size_t Node::ID = 0;

/* virtual */ Node::~Node() {}

Node::Node(const labelType& _label)
    : label(_label)
{}

size_t Node::createTree(const std::string& labels, const std::string& brackets, size_t from)
{
    assert(labels.size() == brackets.size());
    while(brackets.size() > from)
    {
        switch (brackets[from])
        {
            case '.':
                children.push_back(Node());
                children.back().label += labels.substr(from,1);
                break;
            case '(':
                children.push_back(Node());
                children.back().label += labels.substr(from,1);
                from = children.back().createTree(labels, brackets,from + 1);
                children.back().label += labels.substr(from,1);
                break;
            case ')':
                return from;
            default:
                assert(false && "ERROR: should not happen");
        }
        ++from;
    }
    return from;
}

const Node::childrenType& Node::getChildren() const
{
    return children;
}

const Node::labelType& Node::getLabel() const
{
    return label;
}

size_t Node::getId() const
{
    return id;
}

