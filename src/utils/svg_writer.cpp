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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */


#include "svg_writer.hpp"

#define SVG_END_STRING          "</svg>\n"
#define SVG_END_LENGTH          (sizeof(SVG_END_STRING) - 1)
#define SVG_FILENAME_EXTENSION  ".svg"



using namespace std;

struct svg_writer::properties
{
public:
    struct property
    {
        const std::string name;
        const std::string value;
        
        template<typename value_type>
        property(
                 const std::string& name,
                 const value_type& value)
        : name(name), value(to_string(value))
        { }
        
        friend std::ostream& operator<< (
                                         std::ostream& out,
                                         const property& p)
        {
            out
            << p.name
            << "=\""
            << p.value
            << "\"";
            
            return out;
        }
    };
    
private:
    std::vector<property> props;
    
public:
    properties() = default;
    properties(const property& property)
    {
        *this << property;
    }
    properties& operator<<(
                           const property& property)
    {
        props.emplace_back(property);
        return *this;
    }
    properties& operator<<(
                           const properties& other)
    {
        for (const property p : other.props)
            *this << p;
        return *this;
    }
    
    friend std::ostream& operator<<(
                                    std::ostream& out,
                                    const properties& props)
    {
        for (const property& p: props.props)
        {
            out
            << p
            << " ";
        }
        return out;
    }
#define property svg_writer::properties::property
};

struct svg_writer::style
{
    const std::string name;
    const std::string value;
    
    template<typename value_type>
    style(
          const std::string& name,
          const value_type& value)
    : name(name), value(to_string(value))
    { }
    
    friend std::ostream& operator<< (
                                     std::ostream& out,
                                     const style& p)
    {
        out
        << p.name
        << ": "
        << p.value
        << "; ";
        
        return out;
    }
};

//double svg_writer::get_scaling_ratio() const{
//    return scaling_ratio;
//}

/* virtual */ void svg_writer::init(
                                    const std::string& filename,
                                    rna_tree& rna)
{
    document_writer::init(filename, SVG_FILENAME_EXTENSION);

    rna_tree::iterator root = rna.begin();
    
    tr = rna_tree::top_right_corner(root);
    bl = rna_tree::bottom_left_corner(root);
    
//    shift = -bl + MARGIN / 2;
////    shift = MARGIN;
//
//    scale = abs(tr) - abs(bl);
//    scale.x = (LETTER.x - MARGIN.x) / scale.x;
//    scale.y = (LETTER.y  - MARGIN.y) / scale.y;
//
//    letter.x = LETTER.x;// / scale.x;
//    letter.y = LETTER.y; // scale.y;

    margin = point(100,100);
//    auto bp_dist = rna.get_pair_base_distance();
//    scaling_ratio = 20 / bp_dist;
//    scale = point(scaling_ratio , scaling_ratio);
//    scale = point(8, 8);
    dimensions = (tr - bl) * get_scaling_ratio() + margin;





    letter.x = dimensions.x;// / scale.x;
    letter.y = dimensions.y; // scale.y;

    print(get_header_element(root));
    print(create_style_definitions());
}
//
//void svg_writer::scale_point(point &p) const {
////    p -= bl;
//    p.x *= scale.x;
//    p.y *= scale.y;
//
//}

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
    properties out;
    
    out
    << get_point_formatted(centre, "c", "")
    << property("r", radius);
    
    return create_element("circle", out);
}

/* virtual */ std::string svg_writer::get_label_formatted(
                                                          const rna_label& label,
                                                          const RGB& color,
                                                          const label_info li) const
{
    properties out;
    
    out
    << get_point_formatted(label.p, "", "")
    << property("text-anchor", "middle")
    << property("alignment-baseline", "middle")
    << property("class", color.get_name());
    
    return create_element("text", out, label.label, li);
}


/* virtual */ std::string svg_writer::get_line_formatted(
                                                         point from,
                                                         point to,
                                                         const RGB& color) const
{
    properties out;
    
    out
    << get_point_formatted(from, "", "1")
    << get_point_formatted(to, "", "2")
    << property("class", color.get_name());
    
    return create_element("line", out);
}

svg_writer::properties svg_writer::get_point_formatted(
                                                       point p,
                                                       const std::string& prefix,
                                                       const std::string& postfix,
                                                       bool should_shift_p) const
{
    properties out;
    
    if (should_shift_p)
    {
        //cale_point(p);
        p = (p - bl) * get_scaling_ratio() + margin/2;
        p.y = letter.y - p.y;
//        p += shift;
    }
    
    out
    << property(msprintf("%sx%s", prefix, postfix), p.x)
    << property(msprintf("%sy%s", prefix, postfix), p.y);
    
    return out;
}

//std::string svg_writer::create_element(
//        const std::string& name,
//        const properties& properties,
//        const int ix) const
//{
//    return create_element(name, properties, "", ix);
//
//}
std::string svg_writer::create_element(
                                       const std::string& name,
                                       const properties& properties,
                                       const std::string& value,
                                       const label_info li) const
{

    stringstream ss;
    //if (ix >= 0) ss << "<title>" << ix << " (lable in template: " <<  << "</title>";
    if (li.ix >= 0) {
        ss << "<title>" << li.ix << "(label in template: " << li.tmp_label.c_str() << ") </title>";
    }
    if (value.empty())
        return msprintf("<g>%s<%s %s/></g>\n", ss.str(), name, properties);
    else
        return msprintf("<g>%s<%s %s>%s</%s></g>\n", ss.str(), name, properties, value, name);
}

svg_writer::style svg_writer::get_color_style(
                                              const RGB& color) const
{
#define rgb_value(value)    (255.0 * value)
    string text = msprintf("rgb(%s, %s, %s)", rgb_value(color.get_red()), rgb_value(color.get_green()), rgb_value(color.get_blue()));
    return {"stroke", text};
#undef rgb_value
}

svg_writer::properties svg_writer::get_styles(
                                              const std::vector<style>& styles) const
{
    ostringstream out;
    
    for (const style& s : styles)
    {
        out
        << s;
    }
    
    return properties(property("style", out.str()));
}

std::string svg_writer::create_style_definitions() const
{
    ostringstream out;
    
    out
    << endl
    << "<!-- create color definitions -->"
    << endl
    << "<![CDATA[";
    
    struct element
    {
        string name;
        vector<style> styles;
    };
    
    vector<element> elements;
    elements.push_back({"text", {}});
    elements.push_back({"circle", {{"fill", "none"}}});
    elements.push_back({"line", {}});
    
    for (const auto& element : elements)
    {
        for (const RGB& rgb : RGB::get_all())
        {
            out
            << endl
            << element.name
            << "."
            << rgb.get_name()
            << " {"
            << get_color_style(rgb);
            
            for (const style& s : element.styles)
            {
                out
                << s;
            }
            out
            << "}";
        }
        
        // by default when no color is specified, black is used
        out
        << endl
        << element.name
        << " {"
        << get_color_style(RGB::BLACK)
        << style("fill", "none")
        << "}";
    }
    
    out
    << "]]>"
    << endl;
    
    return create_element("style", properties(property("type", "text/css")), out.str());
}

std::string svg_writer::get_header_element(
                                           rna_tree::iterator root)
{
    ostringstream out;

//    bl = -bl - MARGIN / 2;

    TRACE("scale %s, bl %s, tr %s", get_scaling_ratio(), bl, tr);
    
    out
    << "<svg"
    << "\n\t" << property("xmlns", "http://www.w3.org/2000/svg")
    << "\n\t" << property("width", letter.x)
    << "\n\t" << property("height", letter.y)
    << "\n\t" << get_styles({{"font-size",  "8px"}, {"stroke", "none"}, {"font-family", "Helvetica"}})
    << ">"
    << endl;
    
    return out.str();
}
