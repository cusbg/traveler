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
//#define PS_IN(val)                  "precomputed/" + val + ".ps"
//#define PS_OUT(val1, val2)          "build/files/" + val1 + "-" + val2 + ".out.ps"
//#define SEQ(val)                    "precomputed/" + val + ".seq"
//#define FOLD(val)                   "build/files/" + val + ".fold"
//#define FOLD_IN(val)                "precomputed/" + val + ".fold"
//#define MAP(val1, val2)             "build/files/mappings/" + val1 + "-" + val2 + ".map"
//#define RTED_MAP_FILE(rna1, rna2)   "build/files/mappings/map_" + rna1.name() + "-" + rna2.name() + ".map"
//#define RTED_RUN_FILE(i, name)      "build/files/rted/run_rted."#i"." + name


class rna_tree;

class reader
{
public:
    static std::string read_file(const std::string& filename);
    static bool exist_file(const std::string& filename);
};

class writer
{
public:
    static void save(
                const std::string& filename,
                const std::string& text);
};

class ps_document : public reader
{
public:
    ps_document() = default;
    ps_document(const std::string& filename);

    static bool is_base_line(
                const std::string& line);
    static std::string default_prologue();

    rna_tree make_rna() const;

    std::string labels;
    std::string brackets;
    std::vector<point> points;

    std::string prolog;
    std::string epilog;
};


std::string convert_to_java_format(rna_tree rna);

rna_tree get_rna(const std::string& name);

std::vector<std::string> get_command_output(const std::string& command);

#endif /* !READER_HPP */

