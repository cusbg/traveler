/*
 * File: app.hpp
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

#ifndef APP_HPP
#define APP_HPP

#include "types.hpp"

class rna_tree;
class mapping;

class app
{
private:
    struct arguments;

public:
    void run(
                std::vector<std::string> args);
    void run(
                arguments args);
    void run(
                rna_tree& templated,
                rna_tree& matched,
                const mapping& map);

    void save(
                rna_tree& rna,
                const std::string& filename,
                const std::string& templated_rna_file);

    void usage(
                const std::string& appname = "");

private:
    rna_tree create_matched(
                const std::string& seqfile,
                const std::string& foldfile,
                const std::string& name);
    rna_tree create_templated(
                const std::string& psfile,
                const std::string& foldfile,
                const std::string& name);

    void print(
                const arguments& args);

private:
};


#endif /* !APP_HPP */

