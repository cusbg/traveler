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
public:
    virtual streampos print(
                const std::string& text);
    void init(
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
    virtual std::string get_line(
                point from,
                point to) const;

private:
    std::string get_header_element() const;
};


#endif /* !SVG_WRITER_HPP */

