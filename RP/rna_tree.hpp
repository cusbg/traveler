/*
 * File: rna_tree.hpp
 *
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#ifndef RNA_TREE_HPP
#define RNA_TREE_HPP

#include "tree_base.hpp"
#include "node_base.hpp"
#include <unordered_map>


typedef tree_base<node_base<std::string> > RNA_tree_type;


class rna_tree : public RNA_tree_type
{
public:
    virtual ~rna_tree() = default;
    rna_tree() = default;
    rna_tree(const std::string& brackets, const std::string& labels)
        : tree_base<node_base<std::string> >(brackets, convert(labels))
    {}
        
private:
    inline std::vector<node_base<std::string> > convert(const std::string& labels)
    {
        std::vector<node_base<std::string> > vec;
        for (size_t i = 0; i < labels.size(); ++i)
            vec.emplace_back(labels.substr(i, 1));
        return vec;
    }
};


#endif /* !RNA_TREE_HPP */

