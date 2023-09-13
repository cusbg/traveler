#ifndef TRAVELER_PSEUDOKNOTS_HPP
#define TRAVELER_PSEUDOKNOTS_HPP

#include <vector>
#include "rna_tree.hpp"
#include "geometry.hpp"

typedef std::pair<point, point> line;
typedef std::vector<line> curve;


struct pseudoknot_segment{
    std::pair<rna_tree::pre_post_order_iterator, rna_tree::pre_post_order_iterator> interval1;
    std::pair<rna_tree::pre_post_order_iterator, rna_tree::pre_post_order_iterator> interval2;

    curve connecting_curve;

    std::string get_label() const;
    std::string get_id() const;

};

typedef std::pair<rna_tree::pre_post_order_iterator, rna_tree::pre_post_order_iterator> pseudoknot_pair;

class pseudoknots {

private:

    std::vector<pseudoknot_pair> pairs;
    std::vector<pseudoknot_segment> segments;
    double font_size;

    void find_pseudoknot_segments(rna_tree::pre_post_order_iterator begin, rna_tree::pre_post_order_iterator end);

public:

    pseudoknots(rna_tree &rna, const document_settings &settings);
    const std::vector<pseudoknot_segment> get_segments() const {return this->segments; }
    const std::vector<pseudoknot_pair> get_pairs() const {return this->pairs; }
    double get_font_size() const {return this->font_size; }




};

#endif //TRAVELER_PSEUDOKNOTS_HPP
