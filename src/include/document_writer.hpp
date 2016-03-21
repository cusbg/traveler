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

struct rna_label;
struct point;
class rna_tree;


class RGB
{
public: /* constants: */
    static const RGB RED;
    static const RGB GREEN;
    static const RGB BLUE;
    static const RGB BLACK;
    static const RGB GRAY;

private:
    RGB(
                double _red,
                double _green,
                double _blue);

public:
    bool operator==(
                const RGB& other) const;
    double get_red() const
    {
        return red;
    }
    double get_green() const
    {
        return green;
    }
    double get_blue() const
    {
        return blue;
    }

private:
    double red;
    double green;
    double blue;
};


class document_writer
{
public:
    typedef std::fstream::streampos             streampos;

protected:
    document_writer() = default;

public:
    /**
     * print `text` to document
     */
    virtual streampos print(
                const std::string& text) = 0;

public: // formatters
    virtual std::string get_circle_formatted(
                point centre,
                double radius) const = 0;
    virtual std::string get_edge_formatted(
                point from,
                point to,
                bool is_base_pair = true) const = 0;
    virtual std::string get_text_formatted(
                point p,
                const std::string& text) const = 0;
    virtual std::string get_label_formatted(
                const rna_label& label,
                const RGB& color = RGB::BLACK) const = 0;

public:
    std::string get_rna_formatted(
                rna_tree rna) const;
    std::string get_rna_subtree_formatted(
                rna_tree::iterator root) const;

public:
    /**
     * initialize new document_writer on document `filename`
     */
    void init(
                const std::string& filename);

    /**
     * fill document from actual position to end of file with `ch`-chars
     * and seek to actual position
     */
    size_t fill(char ch = ' ');

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
    void validate_stream() const;

protected:
    std::ofstream out;
};


#endif /* !UTILS_DOCUMENT_WRITER_HPP */

