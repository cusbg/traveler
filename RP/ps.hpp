/*
 * File: ps.hpp
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

#ifndef PS_HPP
#define PS_HPP

#include "types.hpp"
#include "rna_tree.hpp"
#include <fstream>

class ps
{
public:
    typedef rna_tree::pre_post_order_iterator pre_post_it;

public:
    ps& operator=(ps&& other);
    ps() = default;

    static ps&& init(const std::string& _filename);
    void print_to_ps(const std::string& line);

    // vv budu vracat len stringy na vypisanie..
    static std::string print(const rna_label& label);
    static std::string print(RGB rgb);
    static std::string print_normal(const pre_post_it& iter, bool colored = false);
    static std::string print_colored(const pre_post_it& iter, RGB rgb);
    static std::string print_edge(const pre_post_it& iter);
    static std::string format_string(const pre_post_it& iter);
    // ^^ 

private:
    ps(const std::string& filename);

private:
    std::string filename;
    std::ofstream out;
};


#endif /* !PS_HPP */

