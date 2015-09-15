/*
 * File: rna_tree_label.cpp
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

#include "rna_tree_label.hpp"

using namespace std;


bool rna_label::operator==(
            const rna_label& other) const
{
    return label == other.label;
}




rna_pair_label::rna_pair_label(
                const std::string& s)
    : labels(1, {s})
{ }

const rna_label& rna_pair_label::operator[](
                size_t index) const
{
    assert(index == 0 || index == 1);

    return labels.at(index);
}

rna_label& rna_pair_label::operator[](
                size_t index)
{
    assert(index == 0 || index == 1);

    return labels.at(index);
}

bool rna_pair_label::operator==(
                const rna_pair_label& other) const
{
    if (labels.size() != other.labels.size())
        return false;

    for (size_t i = 0; i < labels.size(); ++i)
        if (labels[i] != other.labels[i])
            return false;

    return true;
}

rna_pair_label rna_pair_label::operator+(
                const rna_pair_label& other) const
{
    assert(!paired() && !other.paired());

    rna_pair_label out;
    out.labels.push_back(labels.back());
    out.labels.push_back(other.labels.back());

    return out;
}

bool rna_pair_label::paired() const
{
    assert(labels.size() == 1 || labels.size() == 2);

    return labels.size() == 2;
}

std::ostream& operator<<(
                std::ostream& out,
                rna_pair_label lbl)
{
    for (const auto& val : lbl.labels)
        out << val.label;

    return out;
}




