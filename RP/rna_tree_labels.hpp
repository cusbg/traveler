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
#include "point.hpp"


#define label_str(node) \
    (node).get_label().to_string().c_str()

struct rna_label
{
    bool operator==(const rna_label& other) const;

    std::string to_string() const;

    Point point = Point::bad_point();
    std::string label;
};



class rna_pair_label
{
public:
    rna_pair_label() = default;
    rna_pair_label(
                const std::string& s);

    bool operator==(
                const rna_pair_label& other) const;
    rna_pair_label operator+(
                const rna_pair_label& other) const;
    friend std::ostream& operator<<(
                std::ostream& out,
                const rna_pair_label& label);

    void set_label_strings(
                const rna_pair_label& from);

    void set_points_nearby(
                const rna_pair_label& from);
    void set_points_exact(
                const rna_pair_label& from);
    void set_points_nearby(
                Point p,
                size_t which);
    void set_points_exact(
                Point p,
                size_t which);

    rna_label& lbl(size_t index);
    const rna_label& lbl(size_t index) const;
    size_t size() const;

    Point get_centre() const;

    std::string to_string() const;
    std::string get_points_string() const;
    bool is_paired() const;
    bool inited_points() const;

public:
    enum label_status_type
    {
        untouched,
        touched,
        edited,
        deleted,
        inserted,
        reinserted,
        pair_changed

    } status = untouched;

private:
    std::vector<rna_label> labels;
};



#endif /* !RNA_TREE_LABELS_HPP */

