/*
 * File: svg_writer.cpp
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include "svg_writer.hpp"

#define SVG_END_STRING      "</svg>\n"
#define SVG_END_LENGTH      (sizeof(SVG_END_STRING) - 1)

#define quoted(text)        (string() + "\"" + to_string(text) + "\"")

using namespace std;


static std::string get_point_formatted(
                point p,
                const string& prefix,
                const string& postfix)
{
    ostringstream out;

    out
        << prefix
        << "x"
        << postfix
        << "="
        << quoted(p.x)
        << " "
        << prefix
        << "y"
        << postfix
        << "="
        << quoted(p.y);

    return out.str();
}

static std::string get_svg_color_style(
                const RGB& color)
{
    ostringstream out;

#define rgb_value(value)    (255 * value)

    out
        << "stroke: rgb("
        << rgb_value(color.get_red())
        << ", "
        << rgb_value(color.get_green())
        << ", "
        << rgb_value(color.get_blue())
        << ")";

#undef rgb_value

    return out.str();
}

/* virtual */ streampos svg_writer::print(
                const std::string& text)
{
    streampos pos = get_pos();

    fill();

    print_to_stream(text);
    print_to_stream(SVG_END_STRING);

    seek_from_current_pos(-SVG_END_LENGTH);

    return pos;
}

/* virtual */ std::string svg_writer::get_circle_formatted(
                point centre,
                double radius) const
{
    ostringstream out;

    out <<
        "<circle "
        << get_point_formatted(centre, "c", "")
        << " r="
        << quoted(radius)
        << " style="
        << quoted(get_svg_color_style(RGB::BLACK) + "; fill:none;")
        << " />"
        << endl;

    return out.str();
}

/* virtual */ std::string svg_writer::get_text_formatted(
                point p,
                const std::string& text) const
{
    ostringstream out;

    out
        << "<text "
        << get_point_formatted(p, "", "")
        << ">"
        << text
        << "</text>"
        << endl;

    return out.str();
}

/* virtual */ std::string svg_writer::get_label_formatted(
                const rna_label& label,
                const RGB& color) const
{
    ostringstream out;

    out
        << "<text "
        << get_point_formatted(label.p, "", "")
        << " style="
        << quoted(get_svg_color_style(color))
        << ">"
        << label.label
        << "</text>"
        << endl;

    return out.str();
}

/* virtual */ std::string svg_writer::get_line(
                point from,
                point to) const
{
    ostringstream out;

    out
        << "<line "
        << get_point_formatted(from, "", "1")
        << " "
        << get_point_formatted(to, "", "2")
        << " style="
        << quoted(get_svg_color_style(RGB::BLACK) + "; stroke-width: 2;")
        << " />"
        << endl;

    return out.str();
}

void svg_writer::init(
                const std::string& filename,
                rna_tree::iterator root)
{
    APP_DEBUG_FNAME;

    document_writer::init(filename);

    print(get_header_element());
}

std::string svg_writer::get_header_element() const
{
    APP_DEBUG_FNAME;

    ostringstream out;

    out
        << "<svg xmlns="
        << quoted("http://www.w3.org/2000/svg")
        << " xmlns:xlink="
        << quoted("http://www.w3.org/1999/xlink")
        << ">"
        << endl;

    return out.str();
}



