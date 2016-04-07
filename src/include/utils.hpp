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

#include "strategy.hpp"
#include "rna_tree.hpp"


class mapping;


// TABLE-SAVING FUNCTIONS:
    
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

///


bool exist_file(
                const std::string& filename);

std::string read_file(
                const std::string& filename);

void write_file(
                const std::string& filename,
                const std::string& text);



point top_right_corner(
                rna_tree::iterator root);

point bottom_left_corner(
                rna_tree::iterator root);

#endif /* !READER_HPP */

