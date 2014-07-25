/*
 * tree.hpp
 * Copyright (C) 2014 eliasr <eliasr@u-pl22>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TREE_HPP
#define TREE_HPP

#include <iostream>
#include "types.hpp"

class Tree
{
private:
    std::vector<Tree> SubTrees;
public:
    Tree(std::vector<Base> b, std::vector<BasePair> p);
	
};


#endif /* !TREE_HPP */

