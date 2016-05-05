/*
 * File: utils_document_writer.hpp
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

#ifndef UTILS_DOCUMENT_WRITER_HPP
#define UTILS_DOCUMENT_WRITER_HPP

#include <fstream>
#include "rna_tree.hpp"

#define LETTER              point({612, 792})

struct RGB;

class document_writer
{
public:
    typedef std::fstream::streampos             streampos;
    typedef std::fstream::off_type              off_type;

protected:
    document_writer() = default;

public:
    static std::vector<std::unique_ptr<document_writer>> get_writers(
                bool use_colors);

    /**
     * print `text` to document
     */
    virtual streampos print(
                const std::string& text) = 0;

    void use_colors(
                bool colored);

public: // formatters
    virtual std::string get_circle_formatted(
                point centre,
                double radius) const = 0;
    std::string get_edge_formatted(
                point from,
                point to,
                bool is_base_pair = true) const;
    virtual std::string get_text_formatted(
                point p,
                const std::string& text) const = 0;
    std::string get_label_formatted(
                rna_tree::pre_post_order_iterator it) const;
    virtual std::string get_label_formatted(
                const rna_label& label,
                const RGB& color) const = 0;

public:
    std::string get_rna_formatted(
                rna_tree rna) const;
    std::string get_rna_subtree_formatted(
                rna_tree::iterator root) const;
    std::string get_rna_subtree_formatted_colored(
                rna_tree::iterator root) const;

public:
    /**
     * initialize new document_writer on document `filename`
     */
    void init(
                const std::string& filename);
    virtual void init(
                const std::string& filename,
                rna_tree::iterator root) = 0;

    /**
     * fill document from actual position to end of file with `ch`-chars
     * and seek to actual position
     */
    size_t fill(
                char ch = ' ');

public:
    /**
     * seek to `pos`
     */
    void seek(
                streampos pos);

    /**
     * seeks to the end of file
     */
    void seek_end();

    /**
     * return actual position in stream
     */
    streampos get_pos();

protected:
    virtual std::string get_line_formatted(
                point from,
                point to) const = 0;
    void print_to_stream(
                const std::string& text);
    void seek_from_current_pos(
                off_type offset);

    const RGB& get_default_color(
                rna_pair_label::status_type status) const;

private:
    void validate_stream() const;

private:
    std::ofstream out;
    bool colored = false;
};


struct RGB
{
public: /* constants: */
    static const RGB RED;
    static const RGB GREEN;
    static const RGB BLUE;
    static const RGB BLACK;
    static const RGB GRAY;
    static const RGB BROWN;

private:
    RGB(
                double _red,
                double _green,
                double _blue);

    static RGB for_255(
                size_t _red,
                size_t _green,
                size_t _blue);

public:
    bool operator==(
                const RGB& other) const;
    inline double get_red() const
    {
        return red;
    }
    inline double get_green() const
    {
        return green;
    }
    inline double get_blue() const
    {
        return blue;
    }

private:
    double red;
    double green;
    double blue;
};


#endif /* !UTILS_DOCUMENT_WRITER_HPP */

