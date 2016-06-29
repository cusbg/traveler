/*
 * File: generate_test_cases.hpp
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

class generate_test_cases
{
private:
    typedef rna_tree::iterator iterator;
    typedef rna_tree::sibling_iterator sibling_iterator;
    typedef rna_tree::pre_post_order_iterator pre_post_order_iterator;

public:
    void run();

private:
    void run_app(std::string filetempl, std::string fileother);
    void run_hairpin();
    void run_interior();
    void run_multibranch();

    void run_multibranch_loop();
    void run_fullbranch();

    void run_delete(size_t n, rna_tree& rna, sibling_iterator sib);
    void run_delete_leafs(size_t n, rna_tree& rna, sibling_iterator sib);
    void run_insert(size_t n, rna_tree& rna, sibling_iterator sib);
    void run_insert_leafs(size_t n, rna_tree& rna, sibling_iterator sib);

private:

    void save_seq_fold_subtree(iterator root, std::string name);
    void save_seq_fold(rna_tree rna, std::string name);

    std::vector<std::string> create_app_arguments(const std::string& file_in, const std::string& file_out);

    void save_to_psout(const std::string& filename, iterator it);

    void insert_hairpin(
                rna_tree& rna,
                sibling_iterator where,
                size_t n);

public:
    void generate();
};

#define INDIR           (string("precomputed/test/"))
#define OUTDIR_OP       (string("build/files/"))

#define FILES   (std::vector<string>({"1.hairpin", "2.interior", "3.multibranch", "4.fullbranch"}))

#define FILEIN(index)  (INDIR + (FILES.at(index)))
#define FILEOUT(index) (OUTDIR_OP + (FILES.at(index)) + ".out")


#endif /* !TESTS_HPP */
