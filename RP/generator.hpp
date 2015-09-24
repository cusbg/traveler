/*
 * File: generator.hpp
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

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "types.hpp"

class rna_tree;

class generator
{
public:
    static void generate_files();
    static void force_generate();

    static std::string run_folder(
                const std::string& labels);
    static std::string run_mapping(
                rna_tree rna1,
                rna_tree rna2);
    static bool generate();
private:
    static void generate_seq_files();
    static void generate_fold_files();
    static void generate_mapping();
};


#endif /* !GENERATOR_HPP */

