/*
 * File: rna_tree_labels.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard@ba30.eu>
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

#ifndef RNA_TREE_LABELS_HPP
#define RNA_TREE_LABELS_HPP

#include "types.hpp"

struct rna_label
{
#define BAD_POINT     Point({0xBADF00D, 0xBADF00D})
    Point point = BAD_POINT;
    std::string label;

    std::string point_to_string() const;
};

class rna_pair_label
{
public:
    rna_pair_label operator+(const rna_pair_label& other) const;
    friend std::ostream& operator<<(std::ostream& out, const rna_pair_label& label);
    std::string to_string() const;
    std::string get_points_string() const;
    void set_label_strings(const rna_pair_label& from);
    void set_points(const rna_pair_label& from);
    bool is_paired() const;

    rna_pair_label(const std::string& s);
    rna_pair_label() = default;

public:
    enum label_status
    {
        untouched,
        touched,
        edited,
        deleted,
        inserted,
        pair_changed

    } status = untouched;

    std::vector<rna_label> labels;
};



#endif /* !RNA_TREE_LABELS_HPP */

