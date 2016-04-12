/*
 * File: utils_ps_writer.cpp
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

#include <iomanip>

#include "ps_writer.hpp"
#include "utils.hpp"

#define PS_COLUMNS_WIDTH    15
#define LETTER              {612, 792}
#define PS_END_STRING       "showpage\n"
#define PS_END_LENGTH       (sizeof(PS_END_STRING) - 1)


using namespace std;

/* virtual */ streampos ps_writer::print(
                const std::string& text)
{
    streampos pos = get_pos();

    fill();

    print_to_stream(text + PS_END_STRING);
    seek_from_current_pos(-PS_END_LENGTH);

    return pos;
}

/* virtual */ std::string ps_writer::get_circle_formatted(
                point centre,
                double radius) const
{
    ostringstream out;

    out
        << "360 0 "
        << radius
        << " 1.0 1.0 "
        << centre
        << " lwarc"
        << endl;

    return out.str();
}

/* virtual */ std::string ps_writer::get_line_formatted(
                point from,
                point to) const
{
    ostringstream out;

    if (from.bad() || to.bad())
        return "";

    for (double coordinates : {from.x, from.y, to.x, to.y})
    {
        out
            << std::left
            << std::setw(PS_COLUMNS_WIDTH)
            << coordinates;
    }
    out
        << " lwline"
        << endl;

    return out.str();
}

/* virtual */ std::string ps_writer::get_text_formatted(
                point p,
                const std::string& text) const
{
    ostringstream out;

    out
        << std::left
        << std::setw(PS_COLUMNS_WIDTH)
        << ("(" + text + ")")   // full text should have aligned width
        << " "
        << std::setw(PS_COLUMNS_WIDTH)
        << p.x
        << std::setw(PS_COLUMNS_WIDTH)
        << p.y
        << std::setw(PS_COLUMNS_WIDTH)
        << " lwstring"
        << endl;

    return out.str();
}

/* virtual */ std::string ps_writer::get_label_formatted(
                const rna_label& label,
                const RGB& color) const
{
    ostringstream out;

    if (color == RGB::BLACK)
    {
        out
            << get_text_formatted(label.p, label.label);
    }
    else
    {
        out
            << get_color_formatted(color)
            << get_text_formatted(label.p, label.label)
            << get_color_formatted(RGB::BLACK);
    }

    return out.str();
}

std::string ps_writer::get_color_formatted(
                const RGB& color) const
{
    ostringstream out;

    for (double fragment : {color.get_red(), color.get_green(), color.get_blue()})
    {
        out
            << std::left
            << std::setw(PS_COLUMNS_WIDTH)
            << fragment;
    }
    out
        << " setrgbcolor"
        << endl;

    return out.str();
}


void ps_writer::init(
                const std::string& filename,
                rna_tree::iterator root)
{
    APP_DEBUG_FNAME;

    document_writer::init(filename + ".ps");

    print(get_default_prologue(root));
}

std::string ps_writer::get_default_prologue(
                rna_tree::pre_post_order_iterator root) const
{
    APP_DEBUG_FNAME;

    ostringstream str;
    point tr, bl, letter, scale;

    scale = {0.54, 0.54};
    letter = LETTER;

    letter.x /= scale.x;
    letter.y /= scale.y;

    tr = top_right_corner(root);
    bl = bottom_left_corner(root);

    if (size(letter) > distance(tr, bl))
        WARN("rna probably wont fit document");

    tr = -tr;
    bl = -bl;

    bl.x += 50;
    bl.y = letter.y + tr.y - 50;

    str
        << get_default_prologue()
        << scale
            << " scale"
            << endl
        << bl
            << " translate"
            << endl;

    return str.str();
}

std::string ps_writer::get_default_prologue() const
{
    return
        "%!\n"
        "/lwline {newpath moveto lineto stroke} def\n"
        "/lwstring {moveto show} def\n"
        "/lwarc {newpath gsave translate scale /rad exch def /ang1 exch def"
            " /ang2 exch def 0.0 0.0 rad ang1 ang2 arc stroke grestore} def\n"
        "/Helvetica findfont 8.00 scalefont setfont\n"
        ;
}


