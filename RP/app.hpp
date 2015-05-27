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
#include "rna_tree.hpp"

class rna_tree;
class mapping;
struct document;



class app
{
private:
    typedef rna_tree::pre_post_order_iterator pre_post_it;
public:
    void run_app();
    void run_between(
                const std::string& first,
                const std::string& second);

private:
    void transform(
                const rna_tree& rna1,
                const rna_tree& rna2,
                const mapping& map,
                const std::string& fileIn,
                const std::string& fileOut);
    void remove(rna_tree& rna, const mapping& map);
    void erase(rna_tree& rna);
    void modify(rna_tree& rna, const rna_tree& temp, const mapping& map);
    void insert(rna_tree& rna, rna_tree& temp, const mapping& map);
    void save_doc(
                const document& doc);

    void make_compact(
                rna_tree& rna);
    void print_default(const rna_tree& tree);

    void print_deleted(const rna_tree& rna);
    void print_other(const rna_tree& rna);
    void print_inserted(const rna_tree& rna);

    bool contains(const rna_tree& rna, rna_pair_label::label_status status);
};






#endif /* !APP_HPP */

