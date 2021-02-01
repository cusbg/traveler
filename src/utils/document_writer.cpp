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
//#include "ps_writer.hpp"
#include "traveler_writer.hpp"

using namespace std;

#define COLOR_DELETE        RGB::GRAY
#define COLOR_INSERT        RGB::RED
#define COLOR_REINSERT      RGB::BLUE
#define COLOR_EDIT          RGB::GREEN
#define COLOR_ROTATE        RGB::BROWN
#define COLOR_DEFAULT       RGB::BLACK

// initialize RGB constants:
const RGB RGB::RED = RGB(1., 0., 0., "red");
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
    vec.emplace_back(new svg_writer());
    // vec.emplace_back(new ps_writer());
    if (use_colors) {
        vec.emplace_back(new traveler_writer());
    }
    
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
                                                bool is_base_pair
                                                ) const
{
    if (from.bad() || to.bad())
    {
        WARN("Cannot draw line between bad points");
        return "";
    }
    if (is_base_pair)
    {
        point tmp = rna_tree::base_pair_edge_point(from, to);
        to = rna_tree::base_pair_edge_point(to, from);
        from = tmp;
    }
    
    return get_line_formatted(from, to, ix_from, ix_to, is_base_pair, RGB::BLACK);
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

rectangle get_label_bb(point p, int number, float residue_distance){
    int cnt_digits = 0;
    while (number != 0) { number /= 10; cnt_digits++; }

    // THe following is approximate
    point dim = point(cnt_digits * residue_distance * 0.6,  residue_distance * 1.5);

    return rectangle(p - dim / 2, p + dim /2);
}

point sample_relevant_space(rectangle &r, point &p_start, point &dir, float grid_density,
        const vector<point> &resiue_points,
        const vector<pair<point, point>> &lines){

    point p_min = point(p_start.x, p_start.y), p_max = point(p_start.x, p_start.y);
    point dir_ortho = orthogonal(dir);
    vector<point> grid_points;

    auto add_grid_point =
            [&p_min, &p_max, &grid_points](point p)
            {
                p_min.x = min(p_min.x, p.x);
                p_min.y = min(p_min.y, p.y);
                p_max.x = max(p_max.x, p.x);
                p_max.y = max(p_max.y, p.y);

                grid_points.push_back(p);
            };

    for (int i = 0; i < 10; i++) {
        point center = p_start + dir * i * grid_density ;
        add_grid_point(center);
        for (int j = 1; j <= 10; ++j) {
            add_grid_point(center + dir_ortho * j * grid_density);
            add_grid_point(center - dir_ortho * j * grid_density);
        } 
    }

    point r_dim = abs(r.get_bottom_right() - r.get_top_left());
    vector<point> residue_points_in_grid;
    rectangle grid_rect = rectangle(p_min - r_dim, p_max + r_dim);
    for (point p: resiue_points){
        if (grid_rect.has(p)){
            residue_points_in_grid.push_back(p);
        }
    }

    vector<pair<point, point>> lines_in_grid;
    for (auto l: lines){
        if (grid_rect.has(l.first) || grid_rect.has(l.second)){
            lines_in_grid.push_back(l);
        }
    }

    for (point p: grid_points) {
        rectangle r_candidate = rectangle(p - r_dim/2, p + r_dim/2);
        if (!rect_overlaps(r_candidate, residue_points_in_grid) && !rect_overlaps(r_candidate, lines_in_grid)) {
            return p;
        }
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

std::string document_writer::get_numbering_formatted(
        rna_tree::pre_post_order_iterator it,
        const int ix,
        const float residue_distance,
        std::vector<point> pos_residues,
        const std::vector<std::pair<point, point>> lines,
        const numbering_def& numbering) const
{
    auto  found = std::find (numbering.positions.begin(), numbering.positions.end(), ix);
    if (!(found != numbering.positions.end() || (ix > 0 && ix % numbering.interval == 0))){
        return "";
    }

    ostringstream out;

    point v, p1;
    if (it->paired()){
        p1 = it->at(it.label_index()).p;
        point p2 = it->at(1 - it.label_index()).p;
        v = normalize(p1 - p2);
    } else {
        auto it_parent = rna_tree::parent(rna_tree::iterator(it));
        point center = get_loop_center(it_parent);
        p1 = it->at(0).p;
        v = normalize(p1 - center);
    }

    auto p = p1 + v * residue_distance * 3;
    rectangle bb = get_label_bb(p, ix, residue_distance);
    if (rect_overlaps(bb, pos_residues) or rect_overlaps(bb, lines)) {
//            p += normalize(v) * residue_distance * 3;
        p = sample_relevant_space(bb, p, v, residue_distance, pos_residues, lines);
        bb = get_label_bb(p, ix, residue_distance);
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
        out << get_label_formatted(l, label_class, it->status, li);

        point p1_p = normalize(p - p1) ;
        point isec = bb.intersection(p1, p);
        out << get_line_formatted(p1 + p1_p * residue_distance/2, isec, -1, ix,  false, line_class);
    }

    return out.str();
}

std::string document_writer::get_label_formatted(
                                                 rna_tree::pre_post_order_iterator it, label_info li) const
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
        << get_edge_formatted(it->at(0).p, it->at(1).p, it->at(0).seq_ix, it->at(1).seq_ix, true);
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

vector<point> get_residues_positions(rna_tree &rna){

    vector<point> points;
    auto extract_point =
            [&points](rna_tree::pre_post_order_iterator it)
            {
                points.push_back(it->at(it.label_index()).p);
            };

    rna_tree::for_each_in_subtree(rna.begin_pre_post(), extract_point);
    return points;
}

vector<pair<point, point>> get_lines(rna_tree &rna){

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
    vector<point> residues_positions = vector<point>();//get_residues_positions(rna);
    vector<pair<point, point>> lines = get_lines(rna);
    int seq_ix = 0;
    auto print =
    [&rna, &out, &seq_ix, &residues_positions, &lines, &numbering, this](rna_tree::pre_post_order_iterator it)
    {
        out << get_label_formatted(it, {seq_ix,
                                        it->at(it.label_index()).tmp_label,
                                        it->at(it.label_index()).tmp_ix,
                                        it->at(it.label_index()).tmp_numbering_label});
        out << get_numbering_formatted(it, seq_ix, rna.get_pairs_distance(), residues_positions, lines, numbering);
        seq_ix++;
    };
    
    rna_tree::for_each_in_subtree(rna.begin_pre_post(), print);
    
    return out.str();
}

double document_writer::get_scaling_ratio() const{
    return scaling_ratio;
}

void document_writer::set_scaling_ratio(rna_tree& rna){
//    auto bp_dist = rna.get_pair_base_distance();
//    scaling_ratio = 20 / bp_dist;
        scaling_ratio = 1;
};

std::string document_writer::get_rna_background_formatted(
                                                          rna_tree::pre_post_order_iterator begin,
                                                          rna_tree::pre_post_order_iterator end) const
{
    rna_tree::pre_post_order_iterator prev;
    ostringstream out;
    
    while (++rna_tree::pre_post_order_iterator(begin) != end)
    {
        prev = begin++;
        
        point p1 = prev->at(prev.label_index()).p;
        point p2 = begin->at(begin.label_index()).p;


        if (p1.bad() || p2.bad())
            continue;

        point diff_orig = p2 - p1;

        point tmp = rna_tree::base_pair_edge_point(p1, p2, get_scaling_ratio());
        p2 = rna_tree::base_pair_edge_point(p2, p1, get_scaling_ratio());
        p1 = tmp;

        point diff_edge = p2 - p1;

        point diff = diff_orig * diff_edge;

        int ix1 = prev->at(prev.label_index()).seq_ix;
        int ix2 = prev->at(prev.label_index()).seq_ix;

        //If the edge points cross, then the line should not be drawn at all
        if (diff.x > 0 && diff.y > 0) out << get_line_formatted(p1, p2, ix1, ix2, false, RGB::GRAY);
    }
    
    return out.str();
}

std::string document_writer::get_rna_formatted(
                                               rna_tree rna,
                                               const numbering_def& numbering) const
{
    rna.update_labels_seq_ix(); //set indexes for the individual labels which is needed for outputing base pair indexes (at least in the traveler writer)
    return get_rna_subtree_formatted(rna, numbering)
    + get_rna_background_formatted(rna.begin_pre_post(), rna.end_pre_post());
}

void document_writer::init(
                           const std::string& filename,
                           const std::string& suffix)
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
