/*
 * File: rna_tree_label.hpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard.elias@matfyz.cz>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */

#ifndef RNA_TREE_LABEL_HPP
#define RNA_TREE_LABEL_HPP

#include <vector>

#include "tree_base_node.hpp"
#include "point.hpp"
#include "rectangle.hpp"

/**
 * object representing one base
 */
struct rna_label
{
    bool operator==(
                    const rna_label& other) const;

    
    std::string label;
    std::string tmp_label; //label used in the template (can be used to store information about the mapped nodes label in the template)
    int tmp_ix; //position in template
    std::string tmp_numbering_label; //is of type string because index can be 21a when provided by the user
    point p;
    int seq_ix; //sequential index of the label. Corresponds to the position of the label as assigned by the pre_post_order iterator
};

/**
 * object representing one node in tree - base or base pair
 */

class rna_pair_label : public node_base
{
public:
    enum status_type
    {
        untouched,
        touched,
        edited,
        deleted,
        inserted,
        reinserted,
        rotated,
        pair_changed,
    };
    
public:
    rna_pair_label() = default;
    rna_pair_label(
                   const std::string& s);
    rna_pair_label(
            const std::string& s, const bool de_novo);
    bool operator==(
                    const rna_pair_label& other) const;
    rna_pair_label operator+(
                             const rna_pair_label& other) const;
    
    const rna_label& operator[](
                                size_t index) const;
    rna_label& operator[](
                          size_t index);
    const rna_label& at(
                        size_t index) const;
    rna_label& at(
                  size_t index);
    
    friend std::ostream& operator<<(
                                    std::ostream& out,
                                    rna_pair_label lbl);
    friend std::ostream& operator<<(
                                    std::ostream& out,
                                    status_type status);
    
    size_t size() const;
    /**
     * returns if this node is base pair
     */
    bool paired() const;
    /**
     * returns if all points are inited (in pair-bases or single base)
     */
    bool initiated_points() const;
    /**
     * returns point between bases in pair. If signle pase is present, returns its position
     */
    point center() const;

    inline bool is_de_novo_predicted() const{
        return de_novo_predicted;
    }
    
    void clear_points();
    /**
     * set bases
     */
    void set_label_strings(
                           const rna_pair_label& other,
                           const int cnt_children,
                           const int other_cnt_children);
    
    void set_parent_center(
                           const point& center);
    
    const point & get_parent_center();

    void set_p(const point _p, const size_t index);

    void set_bounding_objects(std::vector<rectangle> bo) {
        bounding_objects = bo;
    }

    void set_bounding_objects(rectangle bo) {
        bounding_objects.clear();
        add_bounding_objects(bo);
    }

    void add_bounding_objects(rectangle bo) {
        bounding_objects.push_back(bo);
    }

    void add_bounding_objects(std::vector<rectangle> bos) {
        bounding_objects.insert(bounding_objects.end(), bos.begin(), bos.end());
    }

    std::vector<rectangle> get_bounding_objects(){
        return bounding_objects;
    }

    int get_node_ix_in_source(){
        return source_ix;
    }

    void set_node_ix_in_source(int source_ix){
        this->source_ix = source_ix;
    }


    
public:
    status_type status = untouched;
    std::vector<size_t> remake_ids;
    
private:
    std::vector<rna_label> labels;
    bool de_novo_predicted; //information about whether a base-pair in the target was predicted de novo or copied over from template
    point parent_center;
    std::vector<rectangle> bounding_objects;
    int source_ix = 0; //node index in the source tree (template)

    
};

std::string get_status_name(const rna_pair_label::status_type type);

#endif /* !RNA_TREE_LABEL_HPP */
