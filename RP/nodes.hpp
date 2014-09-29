/*
 * nodes.hpp
 * Copyright (C) 2014 eliasr <eliasr@u-pl30>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef NODES_HPP
#define NODES_HPP

#include <iostream>
#include "types.hpp"

class Tree;

class Node
{
private:
    static size_t ID;
protected:
    size_t id = ID++;
    std::string label;
public:
    Node(const std::string& _label = std::to_string(ID));
    virtual ~Node();
    size_t getId() const;
    const std::string& getLabel() const;
    void setLabel(const std::string& _label);
    friend std::ostream& operator<<(std::ostream & out, Node& n)
    {
        out << n.label;
        return out;
    }
};


class TreeNode : public Node
{
public:
    TreeNode();
    TreeNode(const std::string& _label);
    virtual ~TreeNode();
    bool isCompressed() const;
};



/*class RootNode : public TreeNode
{
public:
    virtual bool hasParent() const;
    virtual ~RootNode();
    RootNode();
};*/



/*class CompressedNode : public TreeNode
{
public:
    enum compressionType {path, sequence};
private:
    compressionType type;
    [>std::vector<size_t><] vecIds compressedIds;
public:
    CompressedNode(compressionType _t, std::vector<size_t> c);
    virtual ~CompressedNode();
    [>std::vector<size_t><] const vecIds& decompress() const;
    compressionType getType() const;
};*/




#endif /* !NODES_HPP */

