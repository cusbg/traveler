/*
 * File: svg_writer.hpp
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

#ifndef SVG_WRITER_HPP
#define SVG_WRITER_HPP

#include "document_writer.hpp"

class svg_writer : public document_writer
{
private:
    struct properties;

protected:
    struct style;
    
public:
    virtual ~svg_writer() = default;
    
public:
    virtual void init(
                      const std::string& filename,
                      rna_tree& rna,
                      bool labels_template);
    virtual streampos print(
                            const std::string& text);
    
public: // formatters
    virtual std::string get_circle_formatted(
                                             point centre,
                                             double radius,
                                             shape_options opts = shape_options()) const;


    virtual std::string get_label_formatted(
                                            const rna_label& label,
                                            const RGB& color,
                                            rna_pair_label::status_type status,
                                            label_info li,
                                            shape_options opts = shape_options()) const;
    virtual std::string get_label_formatted(
            const rna_label& label,
            const std::string& clazz,
            rna_pair_label::status_type status,
            label_info li,
            shape_options opts = shape_options()) const;



    
protected:
    virtual std::string get_line_formatted(
                                           point from,
                                           point to,
                                           int ix_from,
                                           int ix_to,
                                           bool is_base_pair,
                                           bool is_predicted,
                                           const RGB& color,
                                           shape_options opts = shape_options()) const;
    virtual std::string get_line_formatted(
            point from,
            point to,
            int ix_from,
            int ix_to,
            bool is_base_pair,
            bool is_predicted,
            const std::string& clazz,
            shape_options opts = shape_options()) const;

    virtual std::string get_polyline_formatted(
            std::vector<point> &points,
            const RGB& color,
            shape_options opts = shape_options()) const;

//    double get_scaling_ratio() const;
    
private:
    std::string get_header_element(rna_tree& rna);
    document_writer::style get_color_style(
            const std::string& feature,
            const RGB& color) const;
//    properties get_styles(const document_writer::styles&) const;
    std::string create_element(
                               const std::string& name,
                               const properties& properties,
                               const std::string& value = "",
                               label_info li = label_info(-1, "", -1, "", true)
                               // const std::string& title = "",
                                // const std::string& g_clazz = ""
                                       ) const;
//    std::string create_element(
//            const std::string& name,
//            const properties& properties,
//            const int ix) const;
    std::string create_style_definitions(rna_tree& rna, bool labels_template) const;
    
    properties get_point_formatted(
                                   point p,
                                   const std::string& prefix,
                                   const std::string& postfix,
                                   bool should_shift_p = true,
                                   shape_options opts = shape_options()) const;

    //void scale_point(point &p) const;

    //point shift_point(point &p) const;



private:
    point dimensions;
    point margin;

//protected:
//    struct style : document_writer::style {
//        std::stringstream serialize(const document_writer::style& s) override {
//            return std::stringstream ("");
//        }
//    };

};

#endif /* !SVG_WRITER_HPP */
