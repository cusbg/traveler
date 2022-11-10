#include <document_writer.hpp>
#include "pseudoknots.hpp"
#include "convex_hull.hpp"

using namespace std;

vector<pseudoknot_segment> find_pseudoknot_segments(rna_tree::pre_post_order_iterator begin, rna_tree::pre_post_order_iterator end){

    vector<pair<rna_tree::pre_post_order_iterator, rna_tree::pre_post_order_iterator>> pn_pairs;

    int ix_begin = 0;
    while(begin != end)
    {
        for (int i = 0; i < begin->size(); ++i)
//        for(auto&& l: begin->labels)
        {
            auto l = (*begin)[i];
            if(!l.pseudoknot.empty())
            {
                auto rest = begin;
                auto ix_rest = ix_begin;
                ++rest; ++ix_rest;

                while(rest != end)
                {
                    for (int j = 0; j < rest->size(); ++j)
//                    for(auto&& ll: rest->labels)
                    {
                        auto ll = (*rest)[j];
                        if(ll.pseudoknot == l.pseudoknot)
                        {
                            pn_pairs.push_back(make_pair(begin, rest));
                        }
                    }
                    ++rest;
                }
            }
        }

        ++begin; ++ix_begin;
    }

    vector<pseudoknot_segment> segments;

    if (pn_pairs.size() > 0){
        pseudoknot_segment s = {make_pair(pn_pairs[0].first, pn_pairs[0].first), make_pair(pn_pairs[0].second, pn_pairs[0].second)};
        for (int i = 1; i < pn_pairs.size(); ++i){
            auto next1 = rna_tree::pre_post_order_iterator(s.interval1.second); next1++;
            auto next2 = rna_tree::pre_post_order_iterator(pn_pairs[i].second); next2++;
            if (next1 == pn_pairs[i].first && next2 == s.interval2.first){
                //if the first and second residue in the considered pseudoknot pair both directly extend the last pseudoknot segment, let's extend the segment
                s.interval1.second = pn_pairs[i].first;
                s.interval2.first = pn_pairs[i].second;
            } else {
                segments.push_back(s);
                s = {make_pair(pn_pairs[i].first, pn_pairs[i].first), make_pair(pn_pairs[i].second, pn_pairs[i].second)};
            }
        }
        segments.push_back(s);
    }

    return segments;
}



vector<line> get_lines_from_points(vector<point> &points){
    vector<line> lines;
    for (int i = 1; i < points.size(); i++) {
        lines.emplace_back(points[i-1], points[i]);
//        lines.push_back(make_pair(points[i-1], points[i]));
    }
    if (points.size() > 1) {
        lines.emplace_back(points[points.size()-1], points[0]);
//        lines.push_back(make_pair(points[points.size()-1], points[0]));
    }
    return lines;
}

pair<int, point> get_closest_hull_intersection(vector<line> hull_lines, point p) {
    auto direction_lines = {
            make_pair(p, p + point(1,0)),
            make_pair(p, p + point(0,1))
    };

    double min_dist = numeric_limits<double>::max();
    point intersection_point = point::bad_point();
    int min_ix = -1;
    for (line dl:direction_lines){
        for (int ix = 0; ix< hull_lines.size(); ix++){
            line hl = hull_lines[ix];
            point i = lines_intersection(dl.first, dl.second, hl.first, hl.second);
            if (!i.bad()){
                auto dist = distance(p, i);
                if (dist < min_dist) {
                    min_dist = dist;
                    min_ix = ix;
                    intersection_point = i;
                }
            }
        }
    }
    assert(min_ix >= 0);
    return make_pair(min_ix, intersection_point);


}

vector<line> get_pseudoknot_curves(pseudoknot_segment pn, vector<point> hull, double font_size, bool use_hull = false){

    //TODO: use_hull is only very basic implementation

    vector<line> curves;

    point begin = pn.interval1.first->at(pn.interval1.first.label_index()).p;
    point end = pn.interval2.first->at(pn.interval2.first.label_index()).p;


    if (!use_hull) {
        point dir = normalize(end - begin);
        curves.push_back(make_pair(begin + dir * font_size / 2,end - dir * font_size / 2));
        return curves;
    }

    auto hull_lines = get_lines_from_points(hull);

    auto hull_intersection_begin = get_closest_hull_intersection(hull_lines, begin);
    auto hull_intersection_end = get_closest_hull_intersection(hull_lines, end);

    int ix_begin = hull_intersection_begin.first;
    point intersection_begin = hull_intersection_begin.second;
    curves.emplace_back(begin, intersection_begin);

    int ix_end = hull_intersection_end.first;
    point intersection_end = hull_intersection_end.second;

    if (ix_begin == ix_end){
        curves.emplace_back(intersection_begin, intersection_end);

    } else {
        // We need to iterate through all lines from first to second intersection and compute the accumulated distance
        // Then we check whether this "clockwise" distance is lower then the anticlockwise distance and based on
        // that we add the lines to the connecting_curve

        vector<line> aux_curve;
        aux_curve.emplace_back(intersection_begin, hull_lines[ix_begin].second);
        int ix = ix_begin + 1;
        while (ix != ix_end ){
            aux_curve.emplace_back(hull_lines[ix].first, hull_lines[ix].second);
            ix = (ix + 1) % hull_lines.size();

        }
        aux_curve.emplace_back(hull_lines[ix_end].first, intersection_end);

        double hull_perimeter = 0;
        for (line hl: hull_lines) {
            hull_perimeter += distance(hl.first, hl.second);
        }

        double dist = 0;
        for (line c: aux_curve) {
            dist += distance(c.first, c.second);
        }

        if (dist <= hull_perimeter / 2) {
            curves.insert(curves.end(), aux_curve.begin(), aux_curve.end());
        } else {
            curves.emplace_back(intersection_begin, hull_lines[ix_begin].first);
            ix = ix_begin - 1;
            if (ix == -1) ix = hull_lines.size() - 1;
            while (ix != ix_end ){
                curves.emplace_back(hull_lines[ix].second, hull_lines[ix].first);
                ix--;
                if (ix == -1) ix = hull_lines.size() - 1;
            }
            curves.emplace_back(hull_lines[ix_end].second, intersection_end);
        }
    }

    curves.emplace_back(intersection_end, end);

    return curves;
}


bool curves_share_point(vector<line> l1, vector<line> l2) {
    for (int i = 0; i < l1.size(); i++) {
        for (int j = 0; j < l2.size(); j++) {
            if (l1[i].first == l2[j].first || l1[i].second == l2[j].first)
                return true;
        }
    }
    return false;
}


pseudoknots::pseudoknots(rna_tree &rna, const document_settings &settings) {
    this->font_size = settings.font_size;
    this->segments = find_pseudoknot_segments(rna.begin_pre_post(), rna.end_pre_post());

    if (this->segments.size() == 0) return;

//    auto points = rna.get_points();
    vector<point> points;
    for (rectangle bo: rna.begin()->get_bounding_objects()) {
        points.push_back(bo.get_top_left());
        points.push_back(bo.get_top_right());
        points.push_back(bo.get_bottom_right());
        points.push_back(bo.get_bottom_left());
    }
    auto h = convex_hull(points);

    vector<vector<line>> curves;
    auto padding_step = rna.get_pairs_distance() / 2;
    add_padding(h, padding_step*3);
    for (int i = 0; i< this->segments.size(); ++i){

        int cnt_padding = 0;
        bool share = false;
        do {
            vector<line> curve = get_pseudoknot_curves(this->segments[i], h, this->font_size);

            share = false;

            for (int j = 0; j < i; ++j){
                if (curves_share_point(curve, this->segments[j].connecting_curve)) {
                    share = true;
                    break;
                }
            }

            if (i > 0 && share) {
                add_padding(h, padding_step);
                cnt_padding++;
            } else {
                if (cnt_padding > 0) {
                    //clear all added padding
                    add_padding(h, - cnt_padding * padding_step);
                }
                this->segments[i].connecting_curve = curve;
            }
        } while (share);

    }

}

std::string pseudoknot_segment::get_label() const{
    std::ostringstream oss;

    oss << "Pseudoknot " << interval1.first.seq_ix() << ":" << interval1.second.seq_ix() << "--" <<
            interval2.first.seq_ix() << ":" << interval2.second.seq_ix() << "(";

    for (auto interval: {interval1, interval2}){
        auto i = interval.first;
        while (i != interval.second) {
            oss << i->at(i.label_index()).label;
            i++;
        }

        oss << i->at(i.label_index()).label;

        if (interval == interval1) {
            oss << "--";
        }
    }

    oss << ")";


    return oss.str();
}

std::string pseudoknot_segment::get_id() const {
    return msprintf("pn-%s-%s-%s-%s", interval1.first.seq_ix(), interval1.second.seq_ix(), interval2.first.seq_ix(), interval2.second.seq_ix());
}



