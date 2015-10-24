/*
 * File: utils.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard.elias@matfyz.cz>
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

#ifndef READER_HPP
#define READER_HPP

#include <fstream>

#include "types.hpp"
#include "strategy.hpp"
#include "point.hpp"

#define FILES                       std::vector<std::string>({"human", "rabbit", "frog", "mouse"})
#define PS_IN(val)                  (std::string() + "precomputed/" + val + ".ps")
#define PS_OUT(val1, val2)          (std::string() + "build/files/" + val1 + "-" + val2 + ".out.ps")
#define SEQ(val)                    (std::string() + "precomputed/" + val + ".seq")
#define FOLD(val)                   (std::string() + "precomputed/" + val + ".fold")
#define MAP(val1, val2)             (std::string() + "precomputed/" + val1 + "-" + val2 + ".map")

#define RTED(val1, val2)            (std::string() + "precomputed/" + val1 + "-" + val2 + ".rted")
#define GTED(val1, val2)            (std::string() + "precomputed/" + val1 + "-" + val2 + ".gted")

class mapping;

/**
 * save strategy `table` to `filename`
 *  format:
 *      0-th line:      'm n'
 *      (i+1)-th line:  STR[i][*]
 *  where m = #rows, n = #cols
 */
void save_strategy_table(
                const std::string& filename,
                const strategy_table_type& table);

/**
 * loads previously saved STR from `filename`
 *  format:
 *      0-th line:      'm n'
 *      (i+1)-th line:  STR[i][*]
 *  where m = #rows, n = #cols
 */
strategy_table_type load_strategy_table(
                const std::string& filename);

void save_tree_distance_table(
                const std::string& filename,
                const std::vector<std::vector<size_t>>& table);

std::vector<std::vector<size_t>> load_tree_distance_table(
                const std::string& filename);

void save_tree_mapping_table(
                const std::string& filename,
                const mapping& map);

mapping load_mapping_table(
                const std::string& filename);




class rna_tree;


bool exist_file(
                const std::string& filename);

std::string read_file(
                const std::string& filename);

void write_file(
                const std::string& filename,
                const std::string& text);


/**
 * convert rna_tree to java-implementation rted fromat
 */
std::string convert_to_java_format(
                rna_tree rna);

rna_tree get_rna(
                const std::string& name);

/**
 * runs command and return vector<lines> from output
 */
std::vector<std::string> get_command_output(
                const std::string& command);


/**
 * ps document reader
 */
class ps_document
{
public:
    ps_document() = default;
    ps_document(
                const std::string& filename);

    static bool is_base_line(
                const std::string& line);
    static std::string default_prologue();

    rna_tree make_rna() const;

    std::string labels;
    std::vector<point> points;

    std::string prolog;
    std::string epilog;
};

#endif /* !READER_HPP */

