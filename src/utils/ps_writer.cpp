/*
 * File: ps_writer.cpp
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


#include <iomanip>

#include "ps_writer.hpp"

#define PS_COLUMNS_WIDTH        15
#define PS_END_STRING           "showpage\n"
#define PS_END_LENGTH           (sizeof(PS_END_STRING) - 1)
#define PS_FILENAME_EXTENSION   ".ps"

using namespace std;

void ps_writer::init(
                     const std::string& filename,
                     rna_tree& rna)
{
    APP_DEBUG_FNAME;
    
    document_writer::init(filename, PS_FILENAME_EXTENSION);

    print(get_default_prologue(rna.begin()));
}

std::string ps_writer::get_default_prologue(
                                            rna_tree::pre_post_order_iterator root) const
{
    APP_DEBUG_FNAME;
    
    ostringstream str;
    point tr, bl, scale;
    
    tr = rna_tree::top_right_corner(root);
    bl = rna_tree::bottom_left_corner(root) - MARGIN;
    
    scale = abs(tr) + abs(bl);
    scale.x = LETTER.x / scale.x;
    scale.y = LETTER.y / scale.y;
    
    bl = -bl - MARGIN / 2;
    
    str
    << get_default_prologue()
    << scale
    << " scale"
    << endl
    << bl
    << " translate"
    << endl
    << endl;
    
    return str.str();
}

std::string ps_writer::get_default_prologue() const
{
    auto define_color = [](const RGB& rgb) {
        return msprintf("/lw%s {%i %i %i setrgbcolor} def\n",
                        rgb.get_name(), rgb.get_red(), rgb.get_green(), rgb.get_blue());
    };
    ostringstream out;
    
    out
    << "%!\n"
    << "/lwline {newpath moveto lineto stroke} def\n"
    << "/lwstring {moveto show} def\n"
    << "/lwarc {newpath gsave translate scale /rad exch def /ang1 exch def\n\t"
    <<     " /ang2 exch def 0.0 0.0 rad ang1 ang2 arc stroke grestore} def\n"
    << "/lwcircle {/rad exch def /pos2 exch def /pos1 exch def \n\t"
    <<     " 360 0 rad 1.0 1.0 pos1 pos2 lwarc } def\n";
    
    for (const RGB& rgb : RGB::get_all())
        out << define_color(rgb);
    
    out
    << endl
    << "/Helvetica findfont 8.00 scalefont setfont\n";
    
    return out.str();
}


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
    return get_color_formatted(RGB::BLACK) + msprintf("%s %s lwcircle\n", centre, radius);
}

/* virtual */ std::string ps_writer::get_line_formatted(
                                                        point from,
                                                        point to,
                                                        const RGB& color) const
{

    return get_line_formatted(from, to, color.get_name());
}

/* virtual */ std::string ps_writer::get_line_formatted(
        point from,
        point to,
        const std::string& clazz) const
{
    ostringstream out;

    if (from.bad() || to.bad())
        return "";
    from = from * get_scaling_ratio();
    to = to * get_scaling_ratio();

    string color_beg, color_end;

    out << clazz;


    for (double coordinates : {from.x , from.y, to.x, to.y})
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

/* virtual */ std::string ps_writer::get_label_formatted(
                                                         const rna_label& label,
                                                         const RGB& color,
                                                         const label_info li) const
{
    ostringstream out;
    
    out
    << get_color_formatted(color)
    << get_text_formatted(label.p * get_scaling_ratio(), label.label);
    
    return out.str();
}

/* virtual */ std::string ps_writer::get_label_formatted(
        const rna_label& label,
        const std::string& clazz,
        const label_info li) const
{
    ostringstream out;

    // TODO: usage of class in PS is not valid and the whole serialization needs to be rewritten to support SVG only,
    // and this is here only so that the code compiles
    out
            << clazz
            << get_text_formatted(label.p * get_scaling_ratio(), label.label);

    return out.str();
}

std::string ps_writer::get_text_formatted(
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

std::string ps_writer::get_color_formatted(
                                           const RGB& color) const
{
    if (color == *last_used)
        return "";
    else
    {
        last_used = &color;
        return msprintf("lw%s\n", color.get_name());
    }
}
