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
#include "utils.hpp"

#define SVG_END_STRING      "</svg>\n"
#define SVG_END_LENGTH      (sizeof(SVG_END_STRING) - 1)

#define quoted(text)        (string() + "\"" + to_string(text) + "\"")

using namespace std;


static int indentation = 0;


static std::string indent()
{
    ostringstream out;

    out << endl;

    for (int i = 0; i <= indentation; ++i)
        out << "  ";

    return out.str();
}


struct svg_writer::style
{
    std::string name;
    std::string value;

    friend std::ostream& operator<< (
                std::ostream& out,
                const style& s)
    {
        ++indentation;
        out
            << indent()
            << s.name
            << ": "
            << s.value;
        --indentation;
        return out;
    }
};


svg_writer::svg_writer()
{
    assert(indentation == 0);
}

std::string svg_writer::get_header_element(
                rna_tree::iterator root)
{
    APP_DEBUG_FNAME;

    ostringstream out;

    letter = LETTER;

    point scale = {0.54, 0.54};
    letter.x /= scale.x;
    letter.y /= scale.y;

    shift = -bottom_left_corner(root) + point({50, 50});

    point bl = bottom_left_corner(root);
    point tr = top_right_corner(root);
    point size = abs(bl) + abs(tr) + point({100, 100});

    DEBUG("shift %s, lt %s, size %s", to_cstr(shift), to_cstr(letter), to_cstr(size));

    WAIT;

    out
        << "<svg"
        << create_property("xmlns", "http://www.w3.org/2000/svg")
        << create_property("xmlns:xlink", "http://www.w3.org/1999/xlink")
        << create_property("width", letter.x)
        << create_property("height", letter.y)
        << create_property("viewBox", "0 0 " + to_string(size.x) + "px " + to_string(size.y) + "px")
        << create_style({{"font-size",  "8px"}, {"stroke", "none"}, {"font-family", "Helvetica"}})
        << ">"
        << endl;

    ++indentation;

    return out.str();
}

void svg_writer::init(
                const std::string& filename,
                rna_tree::iterator root)
{
    APP_DEBUG_FNAME;

    document_writer::init(filename + ".svg");

    print(get_header_element(root) + create_white_background());
}


svg_writer::style svg_writer::get_svg_color_style(
                const RGB& color) const
{
    ostringstream out;

#define rgb_value(value)    (255 * value)

    out
        << "rgb("
        << rgb_value(color.get_red())
        << ", "
        << rgb_value(color.get_green())
        << ", "
        << rgb_value(color.get_blue())
        << ")";

#undef rgb_value

    return {"stroke", out.str()};
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

    out
        << get_point_formatted(centre, "c", "")
        << create_property("r", radius)
        << create_property("style", create_style({get_svg_color_style(RGB::BLACK), {"fill", "none"}}));

    return create_element("circle", out.str(), "");
}

/* virtual */ std::string svg_writer::get_text_formatted(
                point p,
                const std::string& text) const
{
    ostringstream out;

    out
        << get_point_formatted(p, "", "")
        << create_style({{string("font-size"), string("8px")}});

    return create_element("text", out.str(), text);
}

/* virtual */ std::string svg_writer::get_label_formatted(
                const rna_label& label,
                const RGB& color) const
{
    ostringstream out;

    out
        << get_point_formatted(label.p, "", "")
        << create_style({get_svg_color_style(color)});

    return create_element("text", out.str(), label.label);
}

/* virtual */ std::string svg_writer::get_line_formatted(
                point from,
                point to) const
{
    ostringstream out;

    out
        << get_point_formatted(from, "", "1")
        << " "
        << get_point_formatted(to, "", "2")
        << create_style({get_svg_color_style(RGB::BLACK), {"stroke-width", "2"}});

    return create_element("line", out.str());
}

std::string svg_writer::get_point_formatted(
                point p,
                const std::string& prefix,
                const std::string& postfix,
                bool change_y_direction) const
{
    ostringstream out;

    if (change_y_direction)
    {
        p += shift;
        p.y = letter.y - p.y;
    }

    out
        << create_property(prefix + "x" + postfix, p.x)
        << create_property(prefix + "y" + postfix, p.y);

    return out.str();
}

template <typename value_type>
std::string svg_writer::create_property(
                const std::string& name,
                const value_type& value) const
{
    return indent() + name + "=" + quoted(value);
}

std::string svg_writer::create_style(
                const std::vector<style> styles) const
{
    ostringstream out;

    for (const style& s : styles)
    {
        out
            << s
            << "; ";
    }
    
    return create_property("style", out.str());
}

std::string svg_writer::create_element(
                const std::string& name,
                const std::string& properties,
                const std::string& value) const
{
    ostringstream out;

    --indentation;

    out
        << indent()
        << "<"
        << name
        << " "
        << properties;

    if (value.empty())
    {
        out
            << "/>";
    }
    else
    {
        out
            << ">"
            << value
            << "</"
            << name
            << ">";
    }

    out
        << endl;

    ++indentation;

    return out.str();
}

std::string svg_writer::create_white_background() const
{
    ostringstream out;

    out
        << get_point_formatted({-500, -500}, "", "", false)
        << create_property("height", "150%")
        << create_property("width", "150%")
        << create_style({{"stroke", "none"}, {"fill", "#FFF"}});

    return create_element("rect", out.str(), "");
}


