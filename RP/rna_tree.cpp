/*
 * rna_tree.cpp
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
 *
 * Distributed under terms of the MIT license.
 */

#include "rna_tree.hpp"

using namespace std;


rna_tree::~rna_tree()
{}

rna_tree::rna_tree(const std::string& brackets, const std::string& labels)
    : tree_base<node_base<std::string> >(brackets, convert(labels))
{}

std::vector<node_base<std::string> > rna_tree::convert(const std::string& labels)
{
    std::vector<node_base<std::string> > vec;
    for (size_t i = 0; i < labels.size(); ++i)
        vec.emplace_back(labels.substr(i, 1));
    return vec;
}



