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
    /**
     * run generator if needed (generate() returns true)
     */
    static void generate_files();
    /**
     * run generator
     */
    static void force_generate();

#ifdef NODEF
private:
    static void generate_fold_files();
public:
    /*DO NOT USE! brackets -> from database..*/
    static std::string run_folder(
                const std::string& labels);
#endif

    /**
     * runs java mapping between `rna1` and `rna2`
     */
    static std::string run_mapping(
                rna_tree rna1,
                rna_tree rna2);
    /**
     * returns if files dont exist (=> need to run generator)
     */
    static bool generate();
private:
    static void generate_seq_files();
    static void generate_mapping();
};


#endif /* !GENERATOR_HPP */

