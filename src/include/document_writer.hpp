/*
 * File: document_writer.hpp
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

#ifndef DOCUMENT_WRITER_HPP
#define DOCUMENT_WRITER_HPP

#include <fstream>
#include "rna_tree.hpp"

// US letter
#define LETTER              point({2*612, 2*792})
// minimum margin: left + right; top + bottom
#define MARGIN              point({0, 0})

struct RGB;
class document_writer;

typedef std::vector<std::unique_ptr<document_writer>> image_writers;

/**
 * Additional information about the label to be used by a writer.
 */
struct label_info
{
    int ix;
    std::string tmp_label; //label used in the template (can be used to store information about the mapped nodes label in the template)
};

/**
 * class for printing visualization
 */
class document_writer
{
public:
    typedef std::fstream::streampos             streampos;
    typedef std::fstream::off_type              off_type;
    
protected:
    document_writer() = default;
    
public:
    /**
     * initialize, and return all known writers
     */
    static image_writers get_writers(
                                     bool use_colors);
    static std::unique_ptr<document_writer> get_traveler_writer();
    
public:
    
    /**
     * print `text` to document
     */
    virtual streampos print(
                            const std::string& text) = 0;
    
    /**
     * set, if writer should use colors in output
     */
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
    std::string get_label_formatted(
                                    rna_tree::pre_post_order_iterator it,
                                    const label_info li) const;
    virtual std::string get_label_formatted(
                                            const rna_label& label,
                                            const RGB& color,
                                            const label_info li) const = 0;
    
public:
    /**
     * returns rna backbone visualization
     */
    std::string get_rna_background_formatted(
                                             rna_tree::pre_post_order_iterator begin,
                                             rna_tree::pre_post_order_iterator end) const;
    std::string get_rna_formatted(
                                  rna_tree rna) const;
    std::string get_rna_subtree_formatted(
                                          rna_tree::iterator root) const;
    
    std::string find_pseudoknots(
                                 rna_tree::pre_post_order_iterator begin,
                                 rna_tree::pre_post_order_iterator end) const;
    
public:
    /**
     * initialize new document_writer on document `filename`.`suffix`
     */
    void init(
              const std::string& filename,
              const std::string& suffix);
    /**
     * initialize writer, set basic properties to document (scale/..)
     */
    virtual void init(
                      const std::string& filename,
                      rna_tree& rna) = 0;
    
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

    virtual void set_scaling_ratio(rna_tree& rna);
    
protected:
    virtual std::string get_line_formatted(
                                           point from,
                                           point to,
                                           const RGB& color) const = 0;
    /**
     * flush `text` to output
     */
    void print_to_stream(
                         const std::string& text);
    const RGB& get_default_color(
                                 rna_pair_label::status_type status) const;
    
    /**
     * seek with offset
     */
    void seek_from_current_pos(
                               off_type offset);

    virtual double get_scaling_ratio() const;

private:
    void validate_stream() const;
    
private:
    std::ofstream out;
    bool colored = false;

    double scaling_ratio = 1;
};


/**
 * RGB colors
 */
struct RGB
{
public: /* constants: */
    static const RGB RED;
    static const RGB GREEN;
    static const RGB BLUE;
    static const RGB BLACK;
    static const RGB GRAY;
    static const RGB BROWN;
    
public:
    static std::vector<RGB> get_all()
    {
        return {RED, GREEN, BLUE, BLACK, GRAY, BROWN};
    }
    
private:
    RGB(
        double _red,
        double _green,
        double _blue,
        const std::string& _name);
    
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
    inline std::string get_name() const
    {
        return name;
    }
    
private:
    const double red;
    const double green;
    const double blue;
    const std::string name;

};

#endif /* !DOCUMENT_WRITER_HPP */
