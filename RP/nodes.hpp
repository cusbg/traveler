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
    void addToLabel(const std::string& _label);
};


class TreeNode : public Node
{
public:
    typedef std::vector<size_t> vecIds;

    TreeNode(size_t _parent, Tree* _tree);
    TreeNode();
    virtual ~TreeNode();
    virtual bool hasParent() const;
    void push_back(size_t child);
    size_t getParent() const;
    const vecIds getChildren() const { return children;}
    virtual bool isCompressed() const;
protected:
private:
};



class RootNode : public TreeNode
{
public:
    virtual bool hasParent() const;
    virtual ~RootNode();
    RootNode();
};



class CompressedNode : public TreeNode
{
public:
    enum compressionType {path, sequence};
private:
    compressionType type;
    /*std::vector<size_t>*/ vecIds compressedIds;
public:
    CompressedNode(compressionType _t, std::vector<size_t> c);
    virtual ~CompressedNode();
    /*std::vector<size_t>*/ const vecIds& decompress() const;
    compressionType getType() const;
};




#endif /* !NODES_HPP */

