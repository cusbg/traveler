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
    /**
     * run app with arguments from command line
     *
     * see usage() for detail arguments description
     */
    void run(
                std::vector<std::string> args);

    /**
     * print usage
     */
    static void usage(
                const std::string& appname = "");

private:
    /**
     * run with handled command line arguments
     */
    void run(
                arguments args);

    mapping run_ted(
                rna_tree& templated,
                rna_tree& matched,
                bool save,
                const std::string& distances_file = "",
                const std::string& mapping_file = "");

    void run_drawing(
                rna_tree& templated,
                rna_tree& matched,
                const mapping& mapping,
                bool run,
                bool run_overlaps,
                bool colored,
                const std::string& file);

    /**
     * save rna to document,
     * take prolog from `templated_ps` document
     */
    void save(
                const std::string& filename,
                rna_tree& rna,
                bool overlaps,
                bool colored);

private:
    /**
     * reads both seq & fold file and construct rna tree
     */
    static rna_tree create_matched(
                const std::string& fastafile);

    /**
     * reads ps & fold file and construct rna tree
     * from ps extract rna sequence and node positions in image
     */
    static rna_tree create_templated(
                const std::string& templatefile,
                const std::string& templatetype,
                const std::string& fastafile);

    void log_overlaps(
                const std::string& name,
                size_t size);

    /**
     * print arguments
     */
    void print(
                const arguments& args);

};

#endif /* !APP_HPP */

