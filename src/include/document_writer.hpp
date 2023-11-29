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
#include <sstream>

#include "rna_tree.hpp"
#include "pseudoknots.hpp"

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
    label_info(int ix = -1, std::string tmp_label = "", int tmp_ix = -1, std::string tmp_numbering_label = "", bool is_nt = true){
        this->ix = ix;
        this->tmp_label = tmp_label;
        this->tmp_ix = tmp_ix;
        this->tmp_numbering_label = tmp_numbering_label;
        this->is_nt = is_nt;
    }
    int ix;
    std::string tmp_label; //label used in the template (can be used to store information about the mapped nodes label in the template)
    int tmp_ix = -1; //position of the nucleotide in the template
    std::string tmp_numbering_label; // label of position in template (can be, e.g. 21A)
    bool is_nt; // whether this is a nucleotide or numbering label
};

struct label_def {
    rna_label label;
    std::string clazz;
    rna_pair_label::status_type status;
    label_info li;
};

struct line_def {
    point from;
    point to;
    int ix_from;
    int ix_to;
    bool is_base_pair;
    bool is_predicted;
    const std::string clazz;
};

struct line_def_rgb {
    point from;
    point to;
    int ix_from;
    int ix_to;
    bool is_base_pair;
    bool is_predicted;
    const RGB& color;
};

struct labels_lines_def {
    std::vector<label_def> label_defs;
    std::vector<line_def> line_defs;
};

struct  shape_options {
    double      opacity = -1;
    double      width = -1;
    std::string id = "";
    std::string color = "";
    std::string clazz = "";
    std::string g_clazz = "";
    std::string title = "";

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

    point dimensions;
    point margin;
    point shift;
    point tr, bl;
    point letter;

    /**
     * Maps point from the default coordinate frame to the one which uses SVG, i.e. [0,0] is top left, positive coordinates.
     * @param p
     * @return
     */
    point map_point(const point& p, bool use_margin = true) const;

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
                                             double radius,
                                             shape_options opts = shape_options()) const = 0;

    std::string get_edge_formatted(
                                   point from,
                                   point to,
                                   int ix_from = -1,
                                   int ix_to = -1,
                                   bool is_predicted = false,
                                   bool is_base_pair = true,
                                   shape_options opts = shape_options()) const;
    std::string get_numbering_formatted(
            rna_tree::pre_post_order_iterator it,
            int ix,
            float residue_distance,
            std::vector<point> pos_residues,
            std::vector<std::pair<point, point>> lines,
            const numbering_def& numbering) const;

    labels_lines_def create_numbering_formatted(
            rna_tree::pre_post_order_iterator it,
            int ix,
            float residue_distance,
            std::vector<point> pos_residues,
            std::vector<std::pair<point, point>> lines,
            const numbering_def& numbering) const;

    std::string get_label_formatted(
                                    rna_tree::pre_post_order_iterator it,
                                    label_info li,
                                    shape_options opts = shape_options()) const;
    virtual std::string get_label_formatted(
                                            const rna_label& label,
                                            const RGB& color,
                                            rna_pair_label::status_type status,
                                            label_info li,
                                            shape_options opts = shape_options()) const = 0;

    virtual std::string get_label_formatted(
            const rna_label& label,
            const std::string& clazz,
            rna_pair_label::status_type status,
            label_info li,
            shape_options opts = shape_options()) const = 0;
    
public:
    /**
     * returns rna backbone visualization
     */
    std::string get_rna_background_formatted(
                                             rna_tree::pre_post_order_iterator begin,
                                             rna_tree::pre_post_order_iterator end) const;

    std::vector<line_def_rgb> create_rna_background_formatted(
            rna_tree::pre_post_order_iterator begin,
            rna_tree::pre_post_order_iterator end) const;

    virtual std::string get_rna_formatted(
                                  rna_tree rna,
                                  const numbering_def& numbering,
                                  pseudoknots pn,
                                  bool labels_absolute) const;

    virtual std::string get_rna_subtree_formatted(
                                          rna_tree &rna,
                                          const numbering_def& numbering,
                                          const pseudoknots& pn,
                                          bool labels_absolute) const;
    
    std::string render_pseudoknots(
            pseudoknots &pn) const;

public:
    /**
     * initialize new document_writer on document `filename`.`suffix`
     */
    void init(
              const std::string& filename,
              const std::string& suffix,
              rna_tree& rna);
    /**
     * initialize writer, set basic properties to document (scale/..)
     */
    virtual void init(
                      const std::string& filename,
                      rna_tree& rna,
                      bool labels_template) = 0;
    
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

    void set_scaling_ratio(rna_tree& rna);
    void set_font_size(double size);
    double get_font_size() const;
    virtual double get_line_stroke_width() const;

protected:
    virtual std::string get_line_formatted(
                                           point from,
                                           point to,
                                           int ix_from,
                                           int ix_to,
                                           bool is_base_pair,
                                           bool is_predicted,
                                           const RGB& color,
                                           shape_options opts = shape_options()) const = 0;
    virtual std::string get_line_formatted(
            point from,
            point to,
            int ix_from,
            int ix_to,
            bool is_base_pair,
            bool is_predicted,
            const std::string& clazz,
            shape_options opts = shape_options()) const = 0;

    virtual std::string get_polyline_formatted(
            std::vector<point> &points,
            const RGB& color,
            shape_options opts = shape_options()) const = 0;

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

    std::vector<point> get_residues_positions(rna_tree &rna) const;

    /**
     * Obtain base-pair lines
     * @param rna
     * @return
     */
    std::vector<std::pair<point, point>> get_lines(rna_tree &rna) const;

private:
    void validate_stream() const;
    
private:
    std::ofstream out;
    bool colored = false;

    document_settings settings;


public:
    document_settings get_settings() const;

public:
    struct style
    {
        std::string name;
        std::string value;

        template<typename value_type>
        style(std::string name, value_type value): name(std::move(name)), value(to_string(value)) {}

//        virtual std::stringstream serialize(const document_writer::style& s) = 0;
    };

    typedef std::map<std::string, std::vector<style>> styles;
    styles get_document_styles(const bool labels_template) const;

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