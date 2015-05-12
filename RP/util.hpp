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

#include "types.hpp"
#include "rna_tree.hpp"





struct mapping_pair
{
    size_t from, to;
};

class mapping
{
private:
    static void run_rted(const rna_tree& rna1, const rna_tree& rna2, const std::string& filename);
public:
    static mapping compute_mapping(const rna_tree& rna1, const rna_tree& rna2);

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
};



document read_ps(const std::string& file);

std::string convert_to_java_format(const rna_tree& rna);

std::string run_RNAfold(const std::string& labels);


bool exist_file(const std::string& filename);

std::string read_file(const std::string& filename);

mapping read_mapping_file(const std::string& filename);

bool is_base_line(const std::string& line);


#endif /* !UTIL_HPP */

