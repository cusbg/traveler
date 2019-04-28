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
#include "ps_writer.hpp"
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
    vec.emplace_back(new ps_writer());
    vec.emplace_back(new traveler_writer());
    
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
                                                bool is_base_pair) const
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
    
    return get_line_formatted(from, to, RGB::BLACK);
}

std::string document_writer::get_label_formatted(
                                                 rna_tree::pre_post_order_iterator it, label_info li) const
{
    if (!it->initiated_points())
        return "";
    
    ostringstream out;
    
    out
    << get_label_formatted(it->at(it.label_index()), get_default_color(it->status), li);
    
    if (it->paired() &&
        it.preorder() &&
        it->initiated_points() &&
        !rna_tree::is_root(it))
    {
        out
        << get_edge_formatted(it->at(0).p, it->at(1).p, true);
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

std::string document_writer::get_rna_subtree_formatted(
                                                       rna_tree::iterator root) const
{
    ostringstream out;

    int seq_ix = 0;
    auto print =
    [&out, &seq_ix, this](rna_tree::pre_post_order_iterator it)
    {
        out << get_label_formatted(it, {seq_ix, it->at(it.label_index()).tmp_label});
        seq_ix++;
    };
    
    rna_tree::for_each_in_subtree(root, print);
    
    return out.str();
}

double document_writer::get_scaling_ratio() const{
    return scaling_ratio;
}

void document_writer::set_scaling_ratio(rna_tree& rna){
    auto bp_dist = rna.get_pair_base_distance();
    scaling_ratio = 20 / bp_dist;
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

        //If the edge points cross, then the line should not be drawn at all
        if (diff.x > 0 && diff.y > 0) out << get_line_formatted(p1, p2, RGB::GRAY);
    }
    
    return out.str();
}

std::string document_writer::find_pseudoknots(rna_tree::pre_post_order_iterator begin, rna_tree::pre_post_order_iterator end) const
{
    ostringstream out;
    
    while(begin != end)
    {
        for(auto&& l: begin->labels)
        {
            if(l.pseudoknot != 0)
            {
                auto rest = begin;
                ++rest;
                
                while(rest != end)
                {
                    for(auto&& ll: rest->labels)
                    {
                        if(ll.pseudoknot == l.pseudoknot)
                        {
                            out << get_line_formatted(l.p, ll.p, RGB::RED);
                        }
                    }
                    ++rest;
                }
            }
        }
        
        ++begin;
    }
    
    return out.str();
}

std::string document_writer::get_rna_formatted(
                                               rna_tree rna) const
{
    return get_rna_subtree_formatted(rna.begin())
    + get_rna_background_formatted(rna.begin_pre_post(), rna.end_pre_post())
    + find_pseudoknots(rna.begin_pre_post(), rna.end_pre_post());
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
