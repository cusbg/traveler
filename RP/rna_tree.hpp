/*
 * rna_tree.hpp
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef RNA_TREE_HPP
#define RNA_TREE_HPP

#include "tree_base.hpp"
#include "node_base.hpp"
#include <unordered_map>



class rna_tree : public tree_base<node_base<std::string> >
{
private:
    std::vector<node_base<std::string> > convert(const std::string& labels);

public:
    rna_tree() = default;
    rna_tree(const std::string& brackets, const std::string& labels);
    virtual ~rna_tree();
};

typedef tree<node_base<std::string>> TREE;

#endif /* !RNA_TREE_HPP */

