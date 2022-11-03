/*
 * File: document_writer.cpp
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


#include "document_writer.hpp"
#include "svg_writer.hpp"
#include "json_writer.hpp"
//#include "ps_writer.hpp"
#include "traveler_writer.hpp"

#include <iostream>

using namespace std;

#define COLOR_DELETE        RGB::GRAY
#define COLOR_INSERT        RGB::RED
#define COLOR_REINSERT      RGB::BLUE
#define COLOR_EDIT          RGB::GREEN
#define COLOR_ROTATE        RGB::BROWN
#define COLOR_DEFAULT       RGB::BLACK

// initialize RGB constants:
const RGB RGB::RED = RGB(1., 0., 1., "red"); //changed to magenta
const RGB RGB::GREEN = RGB(0., 1., 0., "green");
const RGB RGB::BLUE = RGB(0., 0., 1., "blue");
const RGB RGB::BLACK = RGB(0., 0., 0., "black");
const RGB RGB::GRAY = RGB(0.8, 0.8, 0.8, "gray");
const RGB RGB::BROWN = RGB(0.83, 0.41, 0.12, "brown");

RGB::RGB(
         double _red,
         double _green,
         double _blue,
         const std::string& _name)
: red(_red), green(_green), blue(_blue), name(_name)
{ }

bool RGB::operator==(
                     const RGB& other) const
{
    return name == other.name;
}

/* static */ image_writers document_writer::get_writers(
                                                        bool use_colors)
{
    image_writers vec;
    vec.emplace_back(new json_writer());
    vec.emplace_back(new svg_writer());
    // vec.emplace_back(new ps_writer());
//    if (use_colors) {
//        vec.emplace_back(new traveler_writer());
//    }
    
    for (const auto& writer : vec)
        writer->use_colors(use_colors);
    
    return vec;
}

/* static */ unique_ptr<document_writer> document_writer::get_traveler_writer()
{
    return unique_ptr<document_writer>(new traveler_writer());
}

std::string document_writer::get_edge_formatted(
                                                point from,
                                                point to,
                                                const int ix_from,
                                                const int ix_to,
                                                bool is_base_pair,
                                                bool is_predicted,
                                                const shape_options opts
                                                ) const
{
    if (from.bad() || to.bad())
    {
        WARN("Cannot draw line between bad points");
        return "";
    }

    if (is_base_pair)
    {
        point tmp = rna_tree::base_pair_edge_point(from, to, get_settings());
        to = rna_tree::base_pair_edge_point(to, from, get_settings());
        from = tmp;
    }
    
    return get_line_formatted(from, to, ix_from, ix_to, is_base_pair, is_predicted, RGB::BLACK);
}

bool rect_overlaps(const rectangle &r, const std::vector<point> &points ){
    for (point p: points) {
        if (r.has(p)) return true;
    }
    return false;
}

bool rect_overlaps(const rectangle &r, const std::vector<pair<point, point>> &lines ){
    for (auto l: lines) {
        if (r.intersects(l.first, l.second)) return true;
    }
    return false;
}

rectangle get_label_bb(point p, int number, float font_size){
    int cnt_digits = 0;
    while (number != 0) { number /= 10; cnt_digits++; }

    // The following is approximate *(font_size approximately corresponds to the height, and width is 0.6*height)
    point dim = point(cnt_digits * font_size * 0.6, font_size);

    return rectangle(p - dim / 2, p + dim /2);
}

float isLeft( point P0, point P1, point P2 )
{
    return ( (P1.x - P0.x) * (P2.y - P0.y) - (P2.x - P0.x) * (P1.y - P0.y) );
}

bool pointInRect(point rect[], point P)
{
    //taken from https://gamedev.stackexchange.com/questions/110229/how-do-i-efficiently-check-if-a-point-is-inside-a-rotated-rectangle

    return (isLeft(rect[0], rect[1], P) > 0 && isLeft(rect[1], rect[2], P) > 0 && isLeft(rect[2], rect[3], P) > 0 && isLeft(rect[3], rect[0], P) > 0);
}

point sample_relevant_space(rectangle &r, point &p_start, point &dir, float grid_density,
        const vector<point> &resiue_points,
        const vector<pair<point, point>> &lines,
        const point &p_label){

    point dir_ortho = orthogonal(dir);
    vector<point> grid_points;

    const double iMax = 10;
    const double iStep = 0.5;
    const double jMax = 5;
    const double jStep = 0.5;

    // Generate points in a rectangle of which the p_start is in the middle of the "bottom" line
    // The grid points are stored in the vector in the preferred order, so that later, the first grid point
    // which passes conditions will be returned.
    for (double i = 0; i < iMax; i += iStep) {
        point p = p_start + dir * i * grid_density;
        double j = 0;
        while (j <= jMax) {
            grid_points.push_back(p + dir_ortho * j * grid_density);
            if (j > 0) grid_points.push_back(p - dir_ortho * j * grid_density);
            j += jStep;
        }
    }

    // Get the rectangle of the grid, in order to determin residue points and lines which are part of the
    // grid (plus som padding). This is done to speed up later intersection checking so that it is not done
    // for every grid point and _every_ residue poin/line
    point grid_rect[] = {point(p_start - dir * grid_density - (jMax + 1) * dir_ortho * grid_density),
                         point(p_start - dir * grid_density + (jMax + 1) * dir_ortho * grid_density),
                         point(p_start + dir * (iMax + 1) * grid_density + (jMax + 1) * dir_ortho * grid_density),
                         point(p_start + dir * (iMax + 1) * grid_density - (jMax + 1) * dir_ortho * grid_density)};

    vector<point> residue_points_in_grid;
    for (auto rp: resiue_points){
        if ( pointInRect(grid_rect, rp)){
            residue_points_in_grid.push_back(rp);
        }
    }

    vector<pair<point, point>> lines_in_grid;
    for (auto l: lines){
        if ( pointInRect(grid_rect, l.first) || pointInRect(grid_rect, l.second)){
            lines_in_grid.push_back(l);
        }
    }

    for (point p: grid_points) {
        bool cont = true;
        //do not consider grid points which are close to any residue point
        for (auto rp: residue_points_in_grid) {
            if (distance(rp, p) < grid_density){
                cont = false;
            };
        }
        if (!cont) continue;

        cont = true;
        //do not consider lines which intersect with line connecting the grid point and the label point
        for (auto l: lines_in_grid) {
            if (lines_intersect(p_label, p_start, l.first, l.second)) {
                cont = false;
                break;
            }
        }

        if (!cont) continue;

        //return the first grid point which passes
        return p;
    }

    return p_start;
}

point get_loop_center(rna_tree::iterator it) {
    vector<point> points;

    point center = point(0,0);

    //since it is a start of a loop, it needs to be a base-paired node
    assert(it->paired());
    center += it->at(0).p;
    center += it->at(1).p;

    int cnt  = 2;
    for (rna_tree::sibling_iterator si = it.begin(); si != it.end(); ++si){
        center += si->at(0).p;
        cnt++;
        if (si->paired()){
            center += si->at(1).p;
            cnt++;
        }
    }

    return center / cnt;
}

labels_lines_def document_writer::create_numbering_formatted(
        rna_tree::pre_post_order_iterator it,
        const int ix,
        const float residue_distance,
        std::vector<point> pos_residues,
        const std::vector<std::pair<point, point>> lines,
        const numbering_def& numbering) const
{
    /*
     * The position of the label differs based on whether the residue is paired or not.
     * If it is paired, the idea is to position it in the direction of the base pair facing away
     * from the paired residue.
     * If the residue is not paired, it is expected to be a part of a loop and then it should lay
     * on a line connecting the residue and the center of the loop.
     * In each case, the position is perpendicular to a line connecting the previous and the next residue.
     */

    vector<label_def> label_defs;
    vector<line_def> line_defs;

    auto  found = std::find (numbering.positions.begin(), numbering.positions.end(), ix);
    if (!(found != numbering.positions.end() || (ix > 0 && ix % numbering.interval == 0))){
        return {label_defs, line_defs};
    }



    rna_tree::pre_post_order_iterator it_prev = --rna_tree::pre_post_order_iterator(it);
    rna_tree::pre_post_order_iterator it_next = ++rna_tree::pre_post_order_iterator(it);
    point p_it = it->at(it.label_index()).p;

    if (rna_tree::is_root(it_next)){
        //the label would be for the 3' "residue"
        return {label_defs, line_defs};
    }
    point p_center;
    point v_perp;

    point p_prev = it_prev->at(it_prev.label_index()).p;
//    if (rna_tree::is_root(it_next)){
//        p_center = p_it;
//        v_perp = normalize(orthogonal(p_prev - p_center));
//    } else {
//        point p_next = it_next->at(it_next.label_index()).p;
//        p_center = center(p_prev, p_next);
//        v_perp = normalize(orthogonal(p_prev - p_next));
//    }
    point p_next = it_next->at(it_next.label_index()).p;
    p_center = center(p_prev, p_next);
    v_perp = normalize(orthogonal(p_prev - p_next));



    if (it->paired()){
        point p_other = it->at(1-it.label_index()).p;
        if (distance(p_center + v_perp, p_other) < distance(p_center - v_perp, p_other)) {
            v_perp = -v_perp;
        }
    } else {
        auto it_parent = rna_tree::parent(rna_tree::iterator(it));
        point par_center = get_loop_center(it_parent);
        if (distance(p_center + v_perp, par_center) < distance(p_center - v_perp, par_center)) {
            v_perp = -v_perp;
        }
    }


    float grid_density = 1.5 * get_font_size();
    auto p = p_it + v_perp * grid_density * 1.5;
    rectangle bb = get_label_bb(p, ix, get_font_size());
    if (rect_overlaps(bb, pos_residues) or rect_overlaps(bb, lines)) {
//            p += normalize(v) * residue_distance * 3;
        p = sample_relevant_space(bb, p, v_perp, grid_density, pos_residues, lines, p_it);
        bb = get_label_bb(p, ix, get_font_size());
    }



    rna_label l;
    for (int i = 0; i < 2; ++i) {
        string label, label_class, line_class;
        if (i == 0) {
            label = msprintf("%s", ix);
            label_class = string("numbering-label sequential");
            line_class = string("numbering-line sequential");
        } else {
            label = msprintf("%s", it->at(it.label_index()).tmp_numbering_label);
            if (label.empty()) {
                //numbering label at an inserted position
                continue;
            }
            label_class = string("numbering-label template");
            line_class = string("numbering-line template");
        }

        l.label = label;
        l.p = p;

        label_info li = label_info();
        li.ix = ix;
        li.is_nt = false;
        label_defs.push_back({l, label_class, it->status, li});
        //out << get_label_formatted(l, label_class, it->status, li);

        point isec = bb.intersection(p, p_it);
        line_defs.push_back({p_it + v_perp * get_font_size() * 0.6 , isec, -1, ix,  false, false, line_class});
        //out << get_line_formatted(p_it + v_perp * get_font_size() * 0.6 , isec, -1, ix,  false, false, line_class);
    }

    return {label_defs, line_defs};

}

std::string document_writer::get_numbering_formatted(
        rna_tree::pre_post_order_iterator it,
        const int ix,
        const float residue_distance,
        std::vector<point> pos_residues,
        const std::vector<std::pair<point, point>> lines,
        const numbering_def& numbering) const
{
    ostringstream out;

    labels_lines_def lld = create_numbering_formatted(it, ix, residue_distance, pos_residues, lines, numbering);

   for(label_def const& ld: lld.label_defs) {
        out << get_label_formatted(ld.label, ld.clazz, ld.status, ld.li);
    }

    for(line_def const& ld: lld.line_defs) {
        out << get_line_formatted(ld.from , ld.to, ld.ix_from, ld.ix_to, ld.is_base_pair, ld.is_predicted, ld.clazz);
    }

    return out.str();

}

std::string document_writer::get_label_formatted(
                                                 rna_tree::pre_post_order_iterator it, label_info li, const shape_options opts) const
{
    if (!it->initiated_points())
        return "";
    
    ostringstream out;

    //TODO: refactor -  superfluous passing of color and status at the same time
    out
    << get_label_formatted(it->at(it.label_index()), get_default_color(it->status), it->status, li);

    if (it->paired() &&
        it.preorder() &&
        it->initiated_points() &&
        !rna_tree::is_root(it))
    {
        out
        << get_edge_formatted(it->at(0).p, it->at(1).p, it->at(0).seq_ix, it->at(1).seq_ix,
                              true, it->is_de_novo_predicted());
    }

    auto x = out.str();
    return out.str();
}

const RGB& document_writer::get_default_color(
                                              rna_pair_label::status_type status) const
{
    if (!colored)
        return RGB::BLACK;
    
    switch (status)
    {
#define switchcase(status, rgb) \
case rna_pair_label::status: \
return COLOR_ ## rgb;
            
            switchcase(deleted, DELETE);
            switchcase(edited, EDIT);
            switchcase(inserted, INSERT);
            switchcase(reinserted, REINSERT);
            switchcase(rotated, ROTATE);
            switchcase(touched, DEFAULT);
            switchcase(untouched, DEFAULT);
        default:
            abort();
#undef switchcase
    }
}

void document_writer::print_to_stream(
                                      const std::string& text)
{
    out << text;
    validate_stream();
}

void document_writer::seek_from_current_pos(
                                            off_type offset)
{
    out.seekp(offset, fstream::cur);
    validate_stream();
}

void document_writer::validate_stream() const
{
    if (out.fail())
        throw io_exception("Writing document failed");
}

vector<point> document_writer::get_residues_positions(rna_tree &rna) const{

    vector<point> points;
    auto extract_point =
            [&points](rna_tree::pre_post_order_iterator it)
            {
                points.push_back(it->at(it.label_index()).p);
            };

    rna_tree::for_each_in_subtree(rna.begin_pre_post(), extract_point);
    return points;
}

vector<pair<point, point>> document_writer::get_lines(rna_tree &rna) const{

    vector<pair<point, point>> lines;
    auto extract_line =
            [&lines](rna_tree::pre_post_order_iterator it)
            {
                if (it->paired()) {
                    lines.push_back(make_pair(it->at(0).p, it->at(1).p));
                }

            };

    rna_tree::for_each_in_subtree(rna.begin(), extract_line);
    return lines;
}

std::string document_writer::get_rna_subtree_formatted(
                                                       rna_tree &rna,
                                                       const numbering_def& numbering) const
{
    ostringstream out;
    vector<point> residues_positions = get_residues_positions(rna);
    vector<pair<point, point>> lines = get_lines(rna);
    int seq_ix = 0;
    auto print =
    [&rna, &out, &seq_ix, &residues_positions, &lines, &numbering, this](rna_tree::pre_post_order_iterator it)
    {
        out << get_label_formatted(it, {seq_ix,
                                        it->at(it.label_index()).tmp_label,
                                        it->at(it.label_index()).tmp_ix,
                                        it->at(it.label_index()).tmp_numbering_label});
        out << get_numbering_formatted(it, seq_ix, rna.get_seq_distance_median(), residues_positions, lines, numbering);
        seq_ix++;
    };
    
    rna_tree::for_each_in_subtree(rna.begin_pre_post(), print);
    
    return out.str();
}

double document_writer::get_scaling_ratio() const{
    return this->settings.scaling_ratio;
}

void document_writer::set_scaling_ratio(rna_tree& rna){
//    auto bp_dist = rna.get_base_pair_distance();
//    scaling_ratio = 20 / bp_dist;
        this->settings.scaling_ratio = 1;
};

void document_writer::set_font_size(double size){
//    auto bp_dist = rna.get_base_pair_distance();
//    scaling_ratio = 20 / bp_dist;

    this->settings.font_size = size;
};

inline double document_writer::get_font_size() const{
    return this->settings.font_size;
};

std::vector<line_def_rgb> document_writer::create_rna_background_formatted(
        rna_tree::pre_post_order_iterator begin,
        rna_tree::pre_post_order_iterator end) const
{
    rna_tree::pre_post_order_iterator prev;

    std::vector<line_def_rgb> ld;

    while (++rna_tree::pre_post_order_iterator(begin) != end)
    {
        prev = begin++;

        point p1 = prev->at(prev.label_index()).p;
        point p2 = begin->at(begin.label_index()).p;


        if (p1.bad() || p2.bad())
            continue;

        point diff_orig = p2 - p1;

        point tmp = rna_tree::base_pair_edge_point(p1, p2, get_settings());
        p2 = rna_tree::base_pair_edge_point(p2, p1, get_settings());
        p1 = tmp;

        point diff_edge = p2 - p1;

        point diff = diff_orig * diff_edge;

        int ix1 = prev->at(prev.label_index()).seq_ix;
        int ix2 = begin->at(begin.label_index()).seq_ix;

        //If the edge points cross, then the line should not be drawn at all
        if (diff.x >= 0 && diff.y >= 0) ld.push_back({p1, p2, ix1, ix2, false, false, RGB::GRAY});
        //if (diff.x >= 0 && diff.y >= 0) out << get_line_formatted(p1, p2, ix1, ix2, false, false, RGB::GRAY);
    }

    //return out.str();
    return ld;
}

std::string document_writer::get_rna_background_formatted(
                                                          rna_tree::pre_post_order_iterator begin,
                                                          rna_tree::pre_post_order_iterator end) const
{

    ostringstream out;

    vector<line_def_rgb> lds = document_writer::create_rna_background_formatted(begin, end);

    for(line_def_rgb const& ld: lds) {
        out << get_line_formatted(ld.from , ld.to, ld.ix_from, ld.ix_to, ld.is_base_pair, ld.is_predicted, ld.color);
    }

    return out.str();
}


std::string document_writer::render_pseudoknots(pseudoknots &pn) const
{
    ostringstream oss;

//    for (auto s:pn.segments){
//
//        auto l = s.interval1.first->at(s.interval1.first.label_index());
//        auto ll = s.interval2.first->at(s.interval2.first.label_index());
//
//        oss << get_line_formatted(l.p, ll.p, RGB::RED);
//    }

    shape_options opts_connection, opts_segment[2];
    opts_segment[0].color = "gray";
    opts_segment[1].color = "gray";
    opts_connection.color = "gray";


    //TODO: the shift is SVG-specific and should be somehow normalized
//    point shift = -point(0, FONT_HEIGHT/2);
    point shift = -point(0, 0);

    for (auto s:pn.get_segments()) {

        opts_segment[0].title = s.get_label();
        opts_segment[1].title = s.get_label();

        opts_segment[0].clazz = string("pseudoknot_segment1");
        opts_segment[0].g_clazz = s.get_id();
        opts_segment[1].clazz = string("pseudoknot_segment2");
        opts_segment[1].g_clazz = s.get_id();

        opts_connection.title = s.get_label();

        int ix_int = 0;
        for (auto interval: {s.interval1, s.interval2}) {

//            oss << get_circle_formatted(s1->at(s1.label_index()).p + shift, FONT_HEIGHT/5*4, opts_segment);
//            if (interval.second != interval.first) {
//                oss << get_circle_formatted(interval.second->at(interval.second.label_index()).p, 4, opts_segment);
//
//            }
            vector<point> points;
            auto it = interval.first;
            if (interval.first != interval.second) {
                while (it != interval.second) {
                    points.push_back(it->at(it.label_index()).p + shift);
    //                oss << get_line_formatted(s1->at(s1.label_index()).p + shift, s2->at(s2.label_index()).p + shift, RGB::GRAY, opts_segment);
                    it++;
                }

                points.push_back(it->at(it.label_index()).p + shift);
                oss << get_polyline_formatted(points, RGB::GRAY, opts_segment[ix_int]);
            } else {
                oss << get_circle_formatted(it->at(it.label_index()).p + shift, pn.get_font_size()/2, opts_segment[ix_int]);

            }


            ix_int++;
        }

        vector<point> points;
        for (line l:s.connecting_curve) {
            points.push_back(l.first+ shift);
//                oss << get_line_formatted(l.first, l.second, RGB::RED, opts_connection);
        }
        points.push_back(s.connecting_curve.back().second + shift);
        opts_connection.clazz = string("pseudoknot_connection");
        opts_connection.g_clazz = s.get_id();
        oss << get_polyline_formatted(points, RGB::GRAY, opts_connection);



    }

    return oss.str();
}

std::string document_writer::get_rna_formatted(
                                               rna_tree rna,
                                               const numbering_def& numbering,
                                               pseudoknots pn) const
{
    rna.update_labels_seq_ix(); //set indexes for the individual labels which is needed for outputing base pair indexes (at least in the traveler writer)
//    return get_rna_subtree_formatted(rna, numbering)
//           + get_rna_background_formatted(rna.begin_pre_post(), rna.end_pre_post());
    return render_pseudoknots(pn)
        + get_rna_subtree_formatted(rna, numbering)
        + get_rna_background_formatted(rna.begin_pre_post(), rna.end_pre_post())
    ;
}

void document_writer::init(
                           const std::string& filename,
                           const std::string& suffix,
                           rna_tree& rna)
{
    APP_DEBUG_FNAME;
    assert(!filename.empty());
    
    string file = filename + suffix;
    INFO("Opening document %s for writing RNA", file);
    
    out.close();
    
    // create file & truncate
    out.open(file, ios::out);
    out.close();
    
    // open in normal mode
    out.open(file, ios::out | ios::in);
    out
    << std::unitbuf
    << std::scientific;
    
    if (!out.good())
        throw io_exception("Cannot open output file %s for writing.", filename);
    assert(out.good());

    rna_tree::iterator root = rna.begin();
    tr = rna_tree::top_right_corner(root);
    bl = rna_tree::bottom_left_corner(root);

    margin = point(100,100);
    dimensions = (tr - bl) /** get_scaling_ratio()*/ + margin;
    letter.x = dimensions.x;// / scale.x;
    letter.y = dimensions.y; // scale.y;
    double font_size = rna.get_seq_distance_min() * 1.2;
    double bp_distance = rna.get_base_pair_distance();
    // If the image is small, the font can be too big which can be recognized by basically not seeing the base pairs
    // as all the space between base-paired residues is covered by the residue letters. If that is the case, the font
    // size is modified
    if ( abs(bp_distance - font_size) / bp_distance < 0.5){
        font_size = 0.25 * bp_distance;
    }
    set_font_size(font_size);
}

void document_writer::seek(
                           streampos pos)
{
    out.seekp(pos);
    
    assert(out.good());
}

void document_writer::seek_end()
{
    out.seekp(0, out.end);
    
    assert(out.good());
}

streampos document_writer::get_pos()
{
    streampos pos = out.tellp();
    assert(pos != -1);
    
    return pos;
}

size_t document_writer::fill(
                             char ch)
{
    streampos pos, end;
    
    pos = get_pos();
    seek_end();
    end = get_pos();
    
    size_t n = end - pos;
    
    if (n != 0)
    {
        seek(pos);
        out
        << string(n - 1, ch)
        << endl;
    }
    seek(pos);
    return n;
}

void document_writer::use_colors(
                                 bool _colored)
{
    colored = _colored;
}

document_settings document_writer::get_settings() const{
    return this->settings;
}

point document_writer::map_point(const point& p, bool use_margin) const
{

    point p_new = (p - bl) * get_scaling_ratio() + (use_margin ? margin/2 : point(0,0));
    p_new.y = letter.y - p_new.y;

    return p_new;
}
