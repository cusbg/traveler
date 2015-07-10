/*
 * File: util.hpp
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

#ifndef UTIL_HPP
#define UTIL_HPP

#include "rna_tree.hpp"
#include "types.hpp"

#define FILES                       {"human", "rabbit", "frog", "mouse"}
#define PS_IN(val)                  "build/files/" + val + ".ps"
#define PS_OUT(val1, val2)          "build/files/" + val1 + "-" + val2 + ".out.ps"
#define SVG_OUT(val)                "build/files/" + val + ".svg"
#define SEQ(val)                    "build/files/" + val + ".seq"
#define RNAFOLD(val)                "build/files/" + val + ".RNAfold.fold"
#define FOLD(val)                   "build/files/" + val + ".fold"
#define FOLD_IN(val)                "../InFiles/" + val + ".fold"
#define MAP(val1, val2)             "build/files/mappings/" + val1 + "-" + val2 + ".map"
#define RTED_MAP_FILE(rna1, rna2)   "build/files/mappings/map_" + rna1.name + "-" + rna2.name + ".map"
#define RTED_RUN_FILE(i, name)      "build/files/rted/run_rted."#i"." + name



class mapping
{
public:
    typedef std::vector<size_t> indexes;
    struct mapping_pair
    {
        size_t from, to;
    };

private:
    static void run_rted(
                const rna_tree& rna1,
                const rna_tree& rna2,
                const std::string& filename);

public:
    static mapping compute_mapping(
                const rna_tree& rna1,
                const rna_tree& rna2);
    static mapping read_mapping_file(
                const std::string& filename);

    indexes get_to_insert() const;
    indexes get_to_remove() const;

public:
    size_t distance;
    std::vector<mapping_pair> map;
};

struct document
{
    std::string labels;
    std::vector<Point> points;
    rna_tree rna;

    std::string prolog;
    std::string epilog;

    void update_rna_points();
    static std::string default_prologue();
};



// other functions:
//
document read_ps(
                const std::string& file);

std::string convert_to_java_format(
                const rna_tree& rna);

std::string run_RNAfold(
                const std::string& labels);


bool exist_file(
                const std::string& filename);

std::string read_file(
                const std::string& filename);

bool is_base_line(
                const std::string& line);


#endif /* !UTIL_HPP */

