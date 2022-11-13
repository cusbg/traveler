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

    std::string get(std::string key) const {
        for (const property p : props) {
            if (p.name == key) {
                return p.value;
            }
        }
        return NULL;
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
                                    rna_tree& rna,
                                    bool labels_template)
{
    document_writer::init(filename, SVG_FILENAME_EXTENSION, rna);

   /* rna_tree::iterator root = rna.begin();
    
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
//    auto bp_dist = rna.get_base_pair_distance();
//    scaling_ratio = 20 / bp_dist;
//    scale = point(scaling_ratio , scaling_ratio);
//    scale = point(8, 8);
    dimensions = (tr - bl) *//** get_scaling_ratio()*//* + margin;





    letter.x = dimensions.x;// / scale.x;
    letter.y = dimensions.y; // scale.y;

    //double font_size = rna.get_seq_distance_median();
    double font_size = rna.get_seq_distance_min() * 1.2;
    double bp_distance = rna.get_base_pair_distance();
    // If the image is small, the font can be too big which can be recognized by basically not seeing the base pairs
    // as all the space between bpaired residues is covered by the residue letters. If that is the case, the font
    // size is modified
    if ( abs(bp_distance - font_size) / bp_distance < 0.5){
        font_size = 0.25 * bp_distance;
    }
    set_font_size(font_size);*/

    print(get_header_element(rna));
    print(create_style_definitions(rna, labels_template));
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
                                                           double radius,
                                                           const shape_options opts) const
{
    properties out;
    
    out
    << get_point_formatted(centre, "c", "")
    << property("r", radius);
    if (opts.opacity >= 0){
        out << property("fill-opacity", msprintf("%f", opts.opacity));
    }
    if (!opts.color.empty()) {
        out << property("fill", msprintf("%s", opts.color));
    }
    if (!opts.clazz.empty()) {
        out << property("class", msprintf("%s", opts.clazz));
    }

    return create_element("circle", out);
}

/* virtual */ std::string svg_writer::get_label_formatted(
                                                          const rna_label& label,
                                                          const RGB& color,
                                                          const rna_pair_label::status_type status,
                                                          const label_info li,
                                                          const shape_options opts) const
{
    return get_label_formatted(label, color.get_name(), status, li, opts);
//    properties out;
//
//    out
//    << get_point_formatted(label.p, "", "")
////    << property("text-anchor", "middle")
////    << property("baseline-shift", "-50%")
//    << property("class", color.get_name());
//
//    return create_element("text", out, label.label, li);
}

/* virtual */ std::string svg_writer::get_label_formatted(
        const rna_label& label,
        const std::string& clazz,
        const rna_pair_label::status_type status,
        const label_info li,
        const shape_options opts) const
{
    properties out;
    
    out
    << get_point_formatted(label.p, "", "")
//    << property("text-anchor", "middle")
//    << property("baseline-shift", "-50%")
    //<< property("class", color.get_name());
            << property("class", clazz);
    
    return create_element("text", out, label.label, li);
}


/* virtual */ std::string svg_writer::get_line_formatted(
                                                         point from,
                                                         point to,
                                                         int ix_from,
                                                         int ix_to,
                                                         bool is_base_pair,
                                                         bool is_predicted,
                                                         const RGB& color,
                                                         const shape_options opts) const
{

    return get_line_formatted(from, to, ix_from, ix_to, is_base_pair, is_predicted, color.get_name(), opts);
}

/* virtual */ std::string svg_writer::get_line_formatted(
        point from,
        point to,
        int ix_from,
        int ix_to,
        bool is_base_pair,
        bool is_predicted,
        const std::string& clazz,
        const shape_options opts) const
{
    properties out;

    std::stringstream ssClazz;

    ssClazz << clazz;
    if (is_predicted) {
        ssClazz << " predicted";
    }

    out
            << get_point_formatted(from, "", "1")
            << get_point_formatted(to, "", "2")
            << property("class", ssClazz.str());

    if (opts.opacity >= 0){
        out << property("stroke-opacity", msprintf("%f", opts.opacity));
    }
    if (opts.width >= 0) {
        out << property("stroke-width", msprintf("%s", opts.width));
    };

    return create_element("line", out);
}

std::string svg_writer::get_polyline_formatted(
        std::vector<point> &points,
        const RGB& color,
        const shape_options opts) const {

    properties out;

    std::ostringstream oss_points;
    bool first = true;
    for (point p: points) {
        if (first) {
            first = false;
        } else {
            oss_points << ", ";
        }
        //shift_point(p);
        p = map_point(p);
        oss_points << p.x << " " << p.y;
    }

    out << property("points", oss_points.str());
    out << property("class", msprintf("%s %s", color.get_name(), opts.clazz));
    if (!opts.color.empty()) {
        out << property("stroke", msprintf("%s", opts.color));
    }
    if (opts.opacity >= 0){
        out << property("stroke-opacity", msprintf("%f", opts.opacity));
    }
    if (opts.width >= 0) {
        out << property("stroke-width", msprintf("%s", opts.width));
    }
    if (!opts.title.empty()) {
        out << property("title", msprintf("%s", opts.title));
    }
    if (!opts.id.empty()) {
        out << property("id", msprintf("%s", opts.id));
    }

    return create_element("polyline", out, "", {-1, ""});
}

svg_writer::properties svg_writer::get_point_formatted(
                                                       point p,
                                                       const std::string& prefix,
                                                       const std::string& postfix,
                                                       bool should_shift_p,
                                                       const shape_options opts) const
{
    properties out;
    
    if (should_shift_p)
    {
        //cale_point(p);
        // p = (p - bl) * get_scaling_ratio() + margin/2;
        // p.y = letter.y - p.y;
        p = map_point(p);
//        p += shift;
    }

    out
    << property(msprintf("%sx%s", prefix, postfix), msprintf("%f", p.x))
    << property(msprintf("%sy%s", prefix, postfix), msprintf("%f", p.y));

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
    //if (ix >= 0) ss << "<title>" << ix << " (label in template: " <<  << "</title>";
    if (li.ix >= 0 && li.is_nt) {
        ss << "<title>" << li.ix;
        if (li.tmp_ix >= 0) {

            ss << " (position.label in template: ";
            if (!li.tmp_numbering_label.empty())
                ss << li.tmp_numbering_label;
            else
                ss << li.tmp_ix;
            ss << "." << li.tmp_label.c_str() << ")";

        } else {
            ss << " (inserted)";

        }
        ss << "</title>";

        // ss << "<rect x=\"" <<  std::stof(properties.get("x")) - 8 << "\" y=\"" << std::stof(properties.get("y"))
        // << "\" width=\"16\" height=\"16\" style=\"fill:red\"/>";
    }
    if (value.empty())
        return msprintf("<g>%s<%s %s/></g>\n", ss.str(), name, properties);
    else
        return msprintf("<g>%s<%s %s>%s</%s></g>\n", ss.str(), name, properties, value, name);
}

svg_writer::style svg_writer::get_color_style(
        const std::string& feature,
        const RGB& color) const
{
#define rgb_value(value)    (255.0 * value)
    string text = msprintf("rgb(%s, %s, %s)", rgb_value(color.get_red()), rgb_value(color.get_green()), rgb_value(color.get_blue()));
    return {feature, text};
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

std::string svg_writer::create_style_definitions(rna_tree& rna, bool labels_template) const
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

    double line_stroke_width = get_font_size() / 8;

    vector<element> elements;
    elements.push_back({"text", {}});
    elements.push_back({"circle", {{"fill", "none"}}});
    elements.push_back({"line", {{"stroke-width", line_stroke_width}}});
    
    for (const auto& element : elements)
    {
        string feature_name;
        if (element.name == "text") {
            feature_name = "fill";
        } else {
            feature_name = "stroke";
        }

        for (const RGB& rgb : RGB::get_all())
        {
            out
            << endl
            << element.name
            << "."
            << rgb.get_name()
            << " {"
            << get_color_style(feature_name, rgb);
            
            for (const style& s : element.styles)
            {
                out
                << s;
            }
            out
            << "}";
        }
        
        // by default when no color is specified, black is used
        if (element.name != "circle") {
            out
                    << endl
                    << element.name
                    << " {"
                    << get_color_style(feature_name, RGB::BLACK);
//        << style("fill", "none");
        }

        if (element.name == "text"){
            out << style("text-anchor", "middle");
//            out << style("baseline-shift", "sub");
            out << style("alignment-baseline", "middle");

        }
        if (element.name != "circle") {
            out << "}";
        }
    }
    out << endl;

    out << "text.numbering-label {fill: rgb(204, 204, 204);}" << endl;
    out << "line.numbering-line {stroke: rgb(204, 204, 204); stroke-width: " << line_stroke_width / 2 << ";}" << endl;
    out << "line.predicted {stroke: rgb(0, 0, 0); stroke-dasharray: 2}" << endl;
    //out << "text.background {fill: rgb(255, 255, 255);" << endl; //for some reason, when this is present, template visibility hidden does not apply (at least in Google Chrome v. 104)
    out << "." << (labels_template ? "sequential" : "template") << " {visibility:hidden}"; //either show position labels based on sequence position in target, or based on the positions in template (provided by the user in the Traveler XML format)

    out << endl << "polyline {fill:none; stroke-linejoin:round; }";
    out << endl << ".pseudoknot_segment1 {stroke-linecap:round; stroke-opacity: 0.4; stroke-width:" << to_string(get_font_size()) + "px" << "}";
    out << endl << ".pseudoknot_segment2 {stroke-linecap:round; stroke-opacity: 0.4; stroke-width:" << to_string(get_font_size()) + "px" << "}";
    out << endl << ".pseudoknot_connection {stroke-linecap:round; stroke-opacity: 0.2; stroke-width:" << 2   << "}";
    
    out << endl << "]]>" << endl;
    
    return create_element("style", properties(property("type", "text/css")), out.str());
}

std::string svg_writer::get_header_element(rna_tree& rna)
{
    ostringstream out;

//    bl = -bl - MARGIN / 2;

    TRACE("scale %s, bl %s, tr %s", get_scaling_ratio(), bl, tr);
    
    out
    << "<svg"
    << "\n\t" << property("xmlns", "http://www.w3.org/2000/svg")
    << "\n\t" << property("width", msprintf("%f", letter.x))
    << "\n\t" << property("height", msprintf("%f", letter.y))
//    << "\n\t" << get_styles({{"font-size",  "7px"}, {"stroke", "none"}, {"font-family", "Helvetica"}})
    << "\n\t" << get_styles({{"font-size",  to_string(get_font_size()) + "px"}, {"font-weight", "bold"}, {"font-family", "Helvetica"}})
    << ">"
    << endl;
    
    return out.str();
}