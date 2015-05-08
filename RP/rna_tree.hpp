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

#include "tree/tree_base.hpp"
#include "tree/node_base.hpp"

struct rna_label;
class rna_pair_label;
class rna_tree;
typedef node_base<rna_pair_label> rna_node_type;


class rna_tree : public tree_base<rna_node_type>
{
public:
    virtual ~rna_tree() = default;
    rna_tree() = default;
    rna_tree(const std::string& brackets, const std::string& labels, const std::string& _name = "");

    iterator modify(iterator it, rna_node_type node);
    iterator remove(iterator it);
    iterator insert(iterator it, rna_node_type node);
private:
    inline std::vector<rna_node_type> convert(const std::string& labels);

public:
    std::string name;
};




struct rna_label
{
#define BAD_POINT     Point({0xBADF00D, 0xBADF00D})
    Point point = BAD_POINT;
    std::string label;

#define IS_BAD_POINT(p) (p.x == 0xBADF00D && p.y == 0xBADF00D)
    friend std::ostream& operator<< (std::ostream& out, rna_label l)
    {
        //if (IS_BAD_POINT(l.point))
            //logger.warn("BAD_POINT");
        out << l.label << ": x=" << l.point.x << "; y=" << l.point.y;
        return out;
    }
};

class rna_pair_label
{
public:
    friend std::ostream& operator<< (std::ostream& out, const rna_pair_label& l);
    rna_pair_label operator+(const rna_pair_label& other) const;
    std::string to_string() const
    {
        std::stringstream str;
        str << *this;
        return str.str();
    }
    std::string get_points() const
    {
        std::stringstream str;
        for (const auto& val : labels)
            str << val << "|";
        return str.str();
    }
    void change_label_string(const std::string& str, size_t index)
    {
        assert(index < labels.size());
        labels.at(index).label = str;
    }
    void change_point(Point p, size_t index)
    {
        assert(index < labels.size());
        labels.at(index).point = p;
    }

    rna_pair_label(const std::string& s);
    rna_pair_label() = default;


    std::vector<rna_label> labels;
};


#endif /* !RNA_TREE_HPP */

