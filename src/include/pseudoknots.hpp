#ifndef TRAVELER_PSEUDOKNOTS_HPP

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

class pseudoknots {

private:

    std::vector<pseudoknot_segment> segments;
    double font_size;

public:

    pseudoknots(rna_tree &rna, const document_settings &settings);
    const std::vector<pseudoknot_segment> get_segments() const {return this->segments; }
    double get_font_size() const {return this->font_size; }




};


#define TRAVELER_PSEUDOKNOTS_HPP

#endif //TRAVELER_PSEUDOKNOTS_HPP
