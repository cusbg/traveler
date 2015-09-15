/*
 * File: rna_tree_label.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard.elias@matfyz.cz>
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

#ifndef RNA_TREE_LABEL_HPP
#define RNA_TREE_LABEL_HPP

#include "tree_base_node.hpp"

struct rna_label
{
    bool operator==(
                const rna_label& other) const;

    std::string label;
};


class rna_pair_label
    : public node_base
{
public:
    rna_pair_label() = default;
    rna_pair_label(
                const std::string& s);

    const rna_label& operator[](
                size_t index) const;
    rna_label& operator[](
                size_t index);

    friend std::ostream& operator<<(
                std::ostream& out,
                rna_pair_label lbl);

    bool paired() const;
    bool operator==(
                const rna_pair_label& other) const;
    rna_pair_label operator+(
                const rna_pair_label& other) const;

private:
    std::vector<rna_label> labels;
};


#endif /* !RNA_TREE_LABEL_HPP */

