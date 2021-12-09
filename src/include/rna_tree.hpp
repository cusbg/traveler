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

struct document_settings {
    /*
     * This should go to document_writer.hpp, but needs some refactoring as that would cause cross-linking.
     */
    double scaling_ratio = 1;
    double font_size = 0;
};

class rna_tree : public tree_base<rna_pair_label>
{
public:
    virtual ~rna_tree() = default;
    rna_tree() = default;
    rna_tree(
             const std::string& _brackets,
             const std::string& _constraints,
             const std::string& _labels,
             const std::string& _name = "");
    rna_tree(
             const std::string& _brackets,
             const std::string& _constraints,
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
                                      document_settings doc_settings);
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
    double get_base_pair_distance() const
    {
        return distances.base_pair_distance;
    }
    double get_loops_bases_distance() const
    {
        return distances.loops_bases_distance;
    }

    double get_seq_distance_avg() const
    {
        return distances.seq_distance_avg;
    }
    double get_seq_distance_median() const
    {
        return distances.seq_distance_median;
    }
    double get_seq_distance_min() const
    {
        return distances.seq_distance_min;
    }
    double get_stem_seq_distance_median() const
    {
        return distances.stem_seq_distance_median;
    }

    void update_bounding_boxes(bool leafs_have_size = false);

    rna_pair_label get_node_by_id(const int id);

    void update_numbering_labels(const std::vector<std::string> &numbering_labels);

    void update_labels_seq_ix();

    inline bool has_folding_info() { return folding_info; }

    /**
     * Compute distances between pairs and distances between unpaired bases in loops
     * as average distance from rna
     */
    void compute_distances();

    
private:
    std::string _name;
    bool folding_info = false; //whether the user provided line in the FASTA telling which bps were predicted de-novo and which were carried over from template
    struct
    {
        /**
         * average distance between paired bases - CG <-> CG
         */
        double pairs_distance;
        /**
         * average distance between bases in a  basepair - C <-> G
         */
        double base_pair_distance;
        /**
         * average distance between unpaired bases in loops
         */
        double loops_bases_distance;
        /**
         * average distance between sequence-neighboring reisudes
         */
        double seq_distance_avg;
        /**
         * median distance between sequence-neighboring reisudes
         */
        double seq_distance_median;
        /**
         * minimum distance between sequence-neighboring reisudes
         */
        double seq_distance_min;

        /**
         * median distance between sequence-neighboring reisudes in stems (these tend not to be influenced by possibly long distances in unpaired regions)
         */
        double stem_seq_distance_median;

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

rna_tree::sibling_iterator get_first_initiated_pair_descendant(
        rna_tree::sibling_iterator it);

#endif /* !RNA_TREE_HPP */
