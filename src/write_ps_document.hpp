/*
 * File: write_ps_document.hpp
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

#ifndef WRITE_PS_DOCUMENT_HPP
#define WRITE_PS_DOCUMENT_HPP

#include "write_document.hpp"
#include "rna_tree.hpp"


struct point;

enum RGB
{
    red,
    green,
    blue,
    black,
    gray,

    other,
};


class ps_writer
    : public document_writer
{
public:
    typedef rna_tree::pre_post_order_iterator pre_post_it;

public:
    void init_default(
                const std::string& filename,
                rna_tree::iterator root);
    void print_rna_name(
                rna_tree& rna);

public:
    static std::string sprint(
                const point p,
                const std::string& text);
    static std::string sprint(
                const rna_label& label);
    static std::string sprint(
                rna_tree rna);
    static std::string sprint_subtree(
                pre_post_it it);
    static std::string sprint(
                RGB rgb);
    static std::string sprint_edge(
                point p1,
                point p2,
                bool base_pair = true);
    static std::string sprint_edge(
                const pre_post_it& iter);
    static std::string sprint(
                pre_post_it it,
                RGB color);
    static std::string sprint(
                pre_post_it it);
    static std::string sprint_formatted(
                pre_post_it it);
    static std::string sprint_circle(
                point p,
                double size);

    static std::string default_prologue();

public:
    /**
     * print `text` to document
     */
    streampos print(
                const std::string& text,
                bool seek = true);
};


#endif /* !WRITE_PS_DOCUMENT_HPP */

