/*
 * File: utils_ps_writer.hpp
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

#ifndef UTILS_PS_WRITER_HPP
#define UTILS_PS_WRITER_HPP


#include "document_writer.hpp"


struct point;


class ps_writer : public document_writer
{
public:
    virtual ~ps_writer() = default;
public:
    virtual streampos print(
                const std::string& text);
    virtual void init(
                const std::string& filename,
                rna_tree::iterator root);

public:
    virtual std::string get_circle_formatted(
                point centre,
                double radius) const;
    virtual std::string get_label_formatted(
                const rna_label& label,
                const RGB& color) const;

protected:
    virtual std::string get_line_formatted(
                point from,
                point to,
                const RGB& color) const;

private:
    std::string get_text_formatted(
                point p,
                const std::string& text) const;

private:
    std::string get_color_formatted(
                const RGB& color) const;
    std::string get_default_prologue() const;
    std::string get_default_prologue(
                rna_tree::pre_post_order_iterator root) const;

private:
    mutable const RGB* last_used = &RGB::BLACK;
};



#endif /* !UTILS_PS_WRITER_HPP */

