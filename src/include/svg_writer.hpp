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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#ifndef SVG_WRITER_HPP
#define SVG_WRITER_HPP

#include "document_writer.hpp"

class svg_writer : public document_writer
{
private:
    struct style;
    struct property;

public:
    svg_writer();
    virtual streampos print(
                const std::string& text);
    virtual void init(
                const std::string& filename,
                rna_tree::iterator root);

public: // formatters
    virtual std::string get_circle_formatted(
                point centre,
                double radius) const;
    virtual std::string get_text_formatted(
                point p,
                const std::string& text) const;
    virtual std::string get_label_formatted(
                const rna_label& label,
                const RGB& color) const;

protected:
    virtual std::string get_line_formatted(
                point from,
                point to) const;

private:
    std::string get_header_element(
                rna_tree::iterator root);
    style get_svg_color_style(
                const RGB& color) const;
    template <typename value_type>
    std::string create_property(
                const std::string& name,
                const value_type& value) const;
    std::string create_style(
                const std::vector<style> styles) const;
    std::string create_element(
                const std::string& name,
                const std::string& properties,
                const std::string& value = "") const;
    std::string create_white_background() const;

    std::string get_point_formatted(
                point p,
                const std::string& prefix,
                const std::string& postfix,
                bool change_y_direction = true) const;

private:
    point shift;
    point letter;
};


#endif /* !SVG_WRITER_HPP */

