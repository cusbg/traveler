/*
 * File: trees.cpp
 * Created: 2014-09-01
 * By: eliasr <eliasr@u-pl22>
 */

#include "trees.hpp"

using namespace std;

size_t Node::ID = 0;

size_t Node::createTree(const std::string& s, size_t from)
{
    while(s.size() > from)
    {
        switch(s[from])
        {
            case '.':
                ch.push_back(Node("."));
                ch.back().type = base;
                break;
            case '(':
                ch.push_back(Node("()"));
                from = ch.back().createTree(s,from + 1);
                ch.back().type = pair;
                break;
            case ')':
                return from;
        }
        ++from;
    }
}

size_t Node::createTreeLabels(const string& s, size_t from)
{
    assert(from < s.size());
    l = s.substr(from, 1);
    ++from;
    
    for (auto& child : ch)
        from = child.createTreeLabels(s,from);
    if (type == pair)
        l += s.substr(from++,1);

    return from;
}

Node::Node(const label& _l)
    :l(_l)
{}

Node::Node() {}

void Node::push_back(const Node& _ch)
{
    ch.push_back(_ch);
}

string Node::getTreeLabels() const
{
    string out = l.substr(0,1);
    for (const auto& child : ch)
        out += child.getTreeLabels();
    if (type == pair)
        out += l.substr(1);
    return out;
}

string Node::toString() const
{
    string output;
    if (type == pair)
        output = "(";
    else if (type == base)
        output = ".";
    for (auto& child : ch)
        output += child.toString();
    if (type == pair)
        output += ")";
    return output;
}

void Node::makeRoot()
{
   isroot = true; 
}

void Node::getIDs() const
{
    std::cout << id << std::endl;
    for (const auto& child : ch)
        child.getIDs();
}
