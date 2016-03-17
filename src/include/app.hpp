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
class ps_writer;

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
    void usage(
                const std::string& appname = "");

private:
    /**
     * run with handled command line arguments
     */
    void run(
                arguments args);

    /**
     * save rna to document
     */
    void save(
                rna_tree& rna,
                ps_writer& writer,
                bool overlaps);

    /**
     * save rna to document,
     * take prolog from `templated_ps` document
     */
    void save(
                const std::string& filename,
                rna_tree& rna,
                bool overlaps);

private:
    /**
     * reads both seq & fold file and construct rna tree
     */
    rna_tree create_matched(
                const std::string& seqfile,
                const std::string& foldfile,
                const std::string& name);

    /**
     * reads ps & fold file and construct rna tree
     * from ps extract rna sequence and node positions in image
     */
    rna_tree create_templated(
                const std::string& psfile,
                const std::string& foldfile,
                const std::string& name);

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

