/*
 * File: rna_tree.hpp
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

#ifndef RNA_TREE_HPP
#define RNA_TREE_HPP

#include "tree_base.hpp"
#include "rna_tree_label.hpp"

struct point;

class rna_tree : public tree_base<rna_pair_label>
{
public:
    virtual ~rna_tree() = default;
    rna_tree() = default;
    rna_tree(
             const std::string& _brackets,
             const std::string& _labels,
             const std::string& _name = "");
    rna_tree(
             const std::string& _brackets,
             const std::string& _labels,
             const std::vector<point>& _points,
             const std::string& _name = "");
    
    /**
     * update postorder points
     */
    void update_points(
                       const std::vector<point>& points);
    
    /**
     * insert `label` to tree before `it` and set
     * `steal` next siblings to be children of inserted node
     */
    sibling_iterator insert(
                            sibling_iterator it,
                            rna_pair_label label,
                            size_t steal = 0);
    /**
     * reparent `sib`-s children to be its next siblings,
     * remove `sib` from tree and return its next sibling
     */
    sibling_iterator erase(
                           sibling_iterator sib);
    
    std::string name() const;
    
    /**
     * sets name for RNA
     */
    void set_name(
                  const std::string& name);


    /**
     * returns RNA sequence
     */
    static std::string get_labels(
                                  const iterator& root);
    /**
     * returns RNA sequence
     */
    std::string get_labels() const;
    /**
     * returns dot-bracket secondary structure
     */
    static std::string get_brackets(
                                    const iterator& root);
    /**
     * returns dot-bracketed secondary structure
     */
    std::string get_brackets() const;
    
    /**
     * check if all inner node are paired and leafs are non-paired
     */
    bool correct_pairing() const;
    
    /**
     * Obtains a node and returns first initiated descendant in the most left branch
     */
    static iterator get_leftest_initiated_descendant(
                                                     const iterator& node);
    
    /**
     * Obtains a node and returns first initiated descendant in the most right branch
     */
    static iterator get_rightest_initiated_descendant(
                                                      const iterator& node);

    static point base_pair_edge_point(
                                      point from,
                                      point to,
                                      float scaling_ratio = 1);
    /**
     * returns top right corner of tree visualization
     */
    static point top_right_corner(
                                  rna_tree::iterator root);
    /**
     * returns bottom left corner of tree visualization
     */
    static point bottom_left_corner(
                                    rna_tree::iterator root);

    double get_pairs_distance() const
    {
        return distances.pairs_distance;
    }
    double get_pair_base_distance() const
    {
        return distances.pair_base_distance;
    }
    double get_loops_bases_distance() const
    {
        return distances.loops_bases_distance;
    }

    void update_bounding_boxes(bool leafs_have_size = false);

private:
    /**
     * Compute distances between pairs and distances between unpaired bases in loops
     * as average distance from rna
     */
    void compute_distances();
    
private:
    std::string _name;
    struct
    {
        /**
         * distance between paired bases - CG <-> CG
         */
        double pairs_distance;
        /**
         * distance between bases in pair - C <-> G
         */
        double pair_base_distance;
        /**
         * distance between unpaired bases in loops
         */
        double loops_bases_distance;
    } distances;
};

inline bool is(
               const rna_tree::base_iterator& iter,
               rna_pair_label::status_type s)
{
    return iter->status == s;
}

/*
 * highlights 5' and 3' end
 */
void set_53_labels(
        rna_tree &rna);

size_t child_index(
                   rna_tree::sibling_iterator sib);

rna_tree::iterator child_by_index(rna_tree::iterator parent, size_t index);

/**
 * return only-branch-node if exist, otherwise return non-valid iterator
 */
rna_tree::sibling_iterator get_onlyone_branch(
                                              rna_tree::sibling_iterator it);

#endif /* !RNA_TREE_HPP */
