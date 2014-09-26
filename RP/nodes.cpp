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

void Node::addToLabel(const string& _label)
{
    label+= _label;
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


/* virtual */ bool TreeNode::hasParent() const
{
    return parent != 0xBAADF00D; //TODO: mozno doplnit radsej nejaky bool kontrolujuci inicializaciu parent-a
}

size_t TreeNode::getParent() const
{
    assert(hasParent());
    return parent;
}

TreeNode::TreeNode(size_t _parent, Tree* _tree)
    : parent(_parent), tree(_tree)
{}

TreeNode::TreeNode()
{}

/* virtual */ TreeNode::~TreeNode()
{}

void TreeNode::push_back(size_t child)
{
    children.push_back(child);
}

void TreeNode::setTree(Tree* _tree)
{
    tree = _tree;
}

/* virtual */ bool TreeNode::isCompressed() const
{
    return false;
}


/* ROOTNODE */

RootNode::RootNode()
{
    label+=" - ROOTNODE";
}

/* virtual */  RootNode::~RootNode()
{}

/* virtual */ bool RootNode::hasParent() const
{
    return false;
}



/* COMPRESSEDNODE */


/* virtual */ CompressedNode::~CompressedNode()
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






