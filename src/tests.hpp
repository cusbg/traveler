/*
 * File: tests.hpp
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

#ifndef TESTS_HPP
#define TESTS_HPP

#include "rna_tree.hpp"

class rna_tree;

class test
{
private:
    typedef rna_tree::iterator iterator;
    typedef rna_tree::pre_post_order_iterator pre_post_order_iterator;

public:
    void run();

private:
    //rna_tree remove_leaf_nodes();
    //rna_tree remove_stem_nodes(rna_tree rna);
    //rna_tree remove_interior_loop_nodes(rna_tree rna);

    //rna_tree insert_leaf_nodes(rna_tree rna);
    //rna_tree insert_stem_nodes(rna_tree rna);
    //rna_tree insert_interior_loop_nodes(rna_tree rna);

    void run1();
    void run(std::string filetempl, std::string fileother);

    point top_right_corner(iterator root);
    point bottom_left_corner(iterator root);

    void save_subtree(iterator root, std::string name);
    void save_seq_fold(rna_tree rna, std::string name);

    std::vector<std::string> create_app_arguments(const std::string& file_in, const std::string& file_out);
    //std::vector<std::string> create_app_arguments(const std::string& file);

    void save_to_psout(const std::string& filename, iterator it);
    std::string ending_strings(iterator it);
    point get_direction(iterator it);

public:
    void generate();
};


#endif /* !TESTS_HPP */

