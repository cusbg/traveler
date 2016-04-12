/*
 * File: document_writer.cpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard@ba30.eu>
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

#include "document_writer.hpp"
#include "svg_writer.hpp"
#include "ps_writer.hpp"

using namespace std;

#define COLOR_DELETE        RGB::GRAY
#define COLOR_INSERT        RGB::RED
#define COLOR_REINSERT      RGB::BLUE
#define COLOR_EDIT          RGB::GREEN
#define COLOR_ROTATE        RGB::BROWN
#define COLOR_DEFAULT       RGB::BLACK

// initialize RGB constants:
const RGB RGB::RED = RGB(1., 0., 0.);
const RGB RGB::GREEN = RGB(0., 1., 0.);
const RGB RGB::BLUE = RGB(0., 0., 1.);
const RGB RGB::BLACK = RGB(0., 0., 0.);
const RGB RGB::GRAY = RGB(0.8, 0.8, 0.8);
const RGB RGB::BROWN = RGB::for_255(210, 105, 30);

RGB::RGB(
                double _red,
                double _green,
                double _blue)
{
    red = _red;
    green = _green;
    blue = _blue;
}

/* static */ RGB RGB::for_255(
                size_t _red,
                size_t _green,
                size_t _blue)
{
    return RGB(_red / 255., _green / 255., _blue / 255.);
}

bool RGB::operator==(
                const RGB& other) const
{
    return red == other.red &&
        green == other.green &&
        blue == other.blue;
}



/* static */ std::vector<std::unique_ptr<document_writer>> document_writer::get_writers()
{
    decltype(get_writers()) vec;
    vec.emplace_back(new svg_writer());
    vec.emplace_back(new ps_writer());

    return vec;
}

std::string document_writer::get_edge_formatted(
                point from,
                point to,
                bool is_base_pair) const
{
    if (from.bad() || to.bad())
    {
        WARN("cannot draw line between bad points");
        return "";
    }
    if (is_base_pair)
    {
        point tmp = rna_tree::base_pair_edge_point(from, to);
        to = rna_tree::base_pair_edge_point(to, from);
        from = tmp;
    }

    return get_line_formatted(from, to);
}

std::string document_writer::get_label_formatted(
                rna_tree::pre_post_order_iterator it) const
{
    if (!it->inited_points())
        return "";

    ostringstream out;

    out
        << get_label_formatted(it->at(it.label_index()), get_default_color(it->status));

    if (it->paired() &&
            it.preorder() &&
            it->inited_points() &&
            !rna_tree::is_root(it))
    {
        out
            << get_edge_formatted(it->at(0).p, it->at(1).p, true);
    }

    return out.str();
}

const RGB& document_writer::get_default_color(
                rna_pair_label::status_type status) const
{
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
    assert_err(!out.fail(), "Error in document stream");
}

std::string document_writer::get_rna_subtree_formatted(
                rna_tree::iterator root) const
{
    ostringstream out;

    auto print =
        [&out, this](rna_tree::pre_post_order_iterator it)
        {
            out << get_label_formatted(it);
        };

    rna_tree::for_each_in_subtree(root, print);

    return out.str();
}

std::string document_writer::get_rna_subtree_formatted_colored(
                rna_tree::iterator root) const
{
    APP_DEBUG_FNAME;

    ostringstream out;

    struct
    {
        size_t from, to;
    } ids;

    for (rna_tree::iterator it = rna_tree::first_child(root);;)
    {
        if (rna_tree::is_leaf(it))
        {
            ids.from = id(it);
            break;
        }
        else 
            it = rna_tree::first_child(it);
    }
    ids.to = id(root);

    rna_tree::print_subtree(root);
    auto print =
        [&out, this, ids](rna_tree::pre_post_order_iterator it)
        {
            if (!it->inited_points())
                return;

            rna_label lbl = it->at(it.label_index());
            if (ids.from <= id(it) && id(it) <= ids.to)
                out << get_label_formatted(lbl, RGB::RED);
            else
                out << get_label_formatted(lbl, RGB::BLACK);
        };

    rna_tree::for_each_in_subtree(rna_tree::parent(root), print);

    return out.str();
}

std::string document_writer::get_rna_formatted(
                rna_tree rna) const
{
    return get_rna_subtree_formatted(rna.begin());
}

void document_writer::init(
                const std::string& filename)
{
    APP_DEBUG_FNAME;
    DEBUG("init(%s)", to_cstr(filename));
    assert(!filename.empty());

    out.close();

    // create file & truncate
    out.open(filename, ios::out);
    out.close();

    // open in normal mode
    out.open(filename, ios::out | ios::in);
    out
        << std::unitbuf
        << std::scientific;

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


