/*
 * nodes.cpp
 * Copyright (C) 2014 eliasr <eliasr@u-pl30>
 *
 * Distributed under terms of the MIT license.
 */

#include "nodes.hpp"

using namespace std;

/* NODE */

/* static */ size_t Node::ID = 0;

/* virtual */ Node::~Node() {}

Node::Node(const std::string& _label)
    : label(_label)
{}

void Node::setLabel(const string& _label)
{
    label = _label;
}

const string& Node::getLabel() const
{
    return label;
}

size_t Node::getId() const
{
    return id;
}


/* TREENODE */


TreeNode::TreeNode()
{}

TreeNode::TreeNode(const string& _label)
    : Node(_label)
{}

/* virtual */ TreeNode::~TreeNode()
{}

/* virtual */ bool TreeNode::isCompressed() const
{
    return false;
}













/*
[> ROOTNODE <]

RootNode::RootNode()
{
    label+=" - ROOTNODE";
}

[> virtual <]  RootNode::~RootNode()
{}

[> virtual <] bool RootNode::hasParent() const
{
    return false;
}



[> COMPRESSEDNODE <]


[> virtual <] CompressedNode::~CompressedNode()
{}

CompressedNode::CompressedNode(compressionType _t, std::vector<size_t> c)
    : type(_t), compressedIds(c)
{
    label+="COMPRESSED_NODE";    
}

const CompressedNode::vecIds& CompressedNode::decompress() const
{
    return compressedIds;
}

CompressedNode::compressionType CompressedNode::getType() const
{
    return type;
}



*/


