/*
 * File: app.hpp
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

#ifndef APP_HPP
#define APP_HPP

#include "types.hpp"

class rna_tree;
class mapping;
struct document;

class app
{
public:
    void run_app();

private:
    void transform(
                const rna_tree& rna1,
                const rna_tree& rna2,
                std::vector<mapping> map,
                const std::string& fileIn,
                const std::string& fileOut);
    void update_tree_points(
                rna_tree& rna,
                document doc);
};






#endif /* !APP_HPP */

