/*
 * File: tests.cpp
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

#include <cfloat>

#include "tests.hpp"
#include "rna_tree.hpp"
#include "utils.hpp"
#include "app.hpp"
#include "compact.hpp"

#include "write_ps_document.hpp"

#define INDIR           (string("precomputed/"))
#define OUTDIR_PART          (string("build/files/run-part/"))
#define OUTDIR_OP       (string("build/files/run-op/"))

#define FILES   (std::vector<string>({"frog", "human", "mouse", "rabbit"}))
#define INDEXES vector<size_t> ({1, 3, 7, 9})

#define print(string)   psout.print((string), true)
#define parent(iter)    rna_tree::parent(iter)


#define FILEIN(index) (OUTDIR_PART + "rna_part." + to_string(index))
#define FILEOUT(index) (OUTDIR_OP + "rna_part." + to_string(index))


ps_writer psout;
rna_pair_label lbl_leaf("I");
rna_pair_label lbl_pair = lbl_leaf + lbl_leaf;

using namespace std;




rna_tree get_rna(const string& name)
{
    string l, b;
    l = read_file(name + ".seq");
    b = read_file(name + ".fold");

    ps_document doc(name + ".ps");

    return rna_tree(b, l, doc.points, name);
}

std::vector<std::string> test::create_app_arguments(const std::string& file_in, const std::string& file_out)
{
    vector<string> vec = {
        "program_name",
        "-tt",
            file_in + ".ps",
            file_in + ".fold",
            "--name", file_in + "_templ",
        "-mt",
            file_out + ".seq",
            file_out + ".fold",
            "--name", file_out + "_match",
        "-r",
            "--strategies", file_out + ".matched.rted",
        "-g",
            "--ted-out", file_out + ".matched.ted",
            "--mapping", file_out + ".matched.map",
        "--ps",
            "--mapping", file_out + ".matched.map",
            file_out + ".matched.ps",
        //"-a",
            //file_out + ".ps",
    };

    return vec;
}

void test::run()
{
    APP_DEBUG_FNAME;

    //generate();

    run1();
}

void test::save_subtree(iterator it, string name)
{
    APP_DEBUG_FNAME;

    name = OUTDIR_PART + name;

    write_file(name + ".seq", rna_tree::get_labels(it));
    write_file(name + ".fold", rna_tree::get_brackets(it));
}

void test::save_seq_fold(rna_tree rna, std::string name)
{
    APP_DEBUG_FNAME;

    write_file(name + ".seq", rna.get_labels());
    write_file(name + ".fold", rna.get_brackets());
}

void test::generate()
{
    APP_DEBUG_FNAME;

    size_t n = 100;
    size_t i = 0;
    rna_tree rna = get_rna(INDIR + "frog");

    for (i = 0; i < 19; ++i)
    {
        iterator it = rna.begin();
        it = plusplus(it, n);

        while (rna_tree::is_leaf(it))
            ++it;

        string name = OUTDIR_PART + "rna_part." + to_string(i);
        save_subtree(it, name);
        save_to_psout(name + ".ps", it);

        n += 100;
    }
}

point test::top_right_corner(iterator root)
{
    //APP_DEBUG_FNAME;

    // x, y should be maximal in subtree
    point p = { -DBL_MAX, -DBL_MAX };

    auto f = [&p] (const pre_post_order_iterator& it) {
        if (rna_tree::is_root(it))
            return;
        point o = it->at(it.label_index()).p;
        if (o.x > p.x)
            p.x = o.x;
        if (o.y > p.y)
            p.y = o.y;
    };

    rna_tree::for_each_in_subtree(root, f);

    assert(p.x != -DBL_MAX && p.y != -DBL_MAX);

    return p;
}

point test::bottom_left_corner(iterator root)
{
    //APP_DEBUG_FNAME;

    // x, y should be minimal in subtree
    point p = { DBL_MAX, DBL_MAX };

    auto f = [&p] (const pre_post_order_iterator& it) {
        if (rna_tree::is_root(it))
            return;
        point o = it->at(it.label_index()).p;
        if (o.x < p.x)
            p.x = o.x;
        if (o.y < p.y)
            p.y = o.y;
    };

    rna_tree::for_each_in_subtree(root, f);

    assert(p.x != DBL_MAX && p.y != DBL_MAX);

    return p;
}

void test::save_to_psout(const std::string& filename, iterator it)
{
    APP_DEBUG_FNAME;

    point tr, bl, letter, scale;

    scale = {0.8, 0.8};
    letter = {612, 792};

    letter.x /= scale.x;
    letter.y /= scale.y;

    tr = top_right_corner(it);
    bl = bottom_left_corner(it);

    assert(distance(tr, bl) < size(letter));
    tr = tr * -1;
    bl = bl * -1;

    bl.x += 50;
    bl.y = letter.y + tr.y - 50;

    psout.init(filename);
    print(ps_document::default_prologue());

    print(to_string(scale) + " scale\n");
    print(to_string(bl) + " translate\n");

    print(psout.sprint_subtree(it));
    print(ending_strings(it));
}

string test::ending_strings(iterator it)
{
    if (rna_tree::is_root(it))
        return "";

    point dir = get_direction(it) * BASES_DISTANCE * 1.5;
    point p1, p2;
    p1 = it->at(0).p;
    p2 = it->at(1).p;
    ostringstream out;

    out
        << "(5') "
            << (p1 + dir)
            << " lwstring"
            << endl
        << "(3') "
            << (p2 + dir)
            << " lwstring"
            << endl
        << ps_writer::sprint_edge(p1, p1 + dir, true)
        << ps_writer::sprint_edge(p2, p2 + dir, true);

    return out.str();
}

point test::get_direction(iterator it)
{
    assert(!rna_tree::is_leaf(it));

    point p1, p2, p, ch;

    p1 = it->at(0).p;
    p2 = it->at(1).p;

    ch = it.begin()->centre();

    p = -orthogonal(p2 - p1, ch - p1);

    return p;
}

void test::run1()
{
    APP_DEBUG_FNAME;

    rna_tree rna = get_rna(FILEIN(1));
    iterator it = rna.begin();
    size_t i = 3;

    it = plusplus(rna.begin(), 3);

    rna.print_tree();

    it = rna.insert(it, lbl_leaf);
    it = rna.insert(it, lbl_leaf);
    it = rna.insert(it, lbl_pair, 1);
    while (--i != 0)
        it = rna.insert(it, lbl_pair, 2);
    //it = rna.insert(it, lbl_leaf);
    
    rna.print_tree();

    save_seq_fold(rna, FILEOUT(1));
    run(FILEIN(1), FILEOUT(1));
}

void test::run(std::string filetempl, std::string fileother)
{
    APP_DEBUG_FNAME;

    auto args = create_app_arguments(filetempl, fileother);

    app app;
    app.run(args);

    rna_tree rna = get_rna(fileother);
    save_to_psout(fileother + ".ps", rna.begin());
}



