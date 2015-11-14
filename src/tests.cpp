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

#include "tests.hpp"
#include "rna_tree.hpp"
#include "utils.hpp"
#include "app.hpp"

#include "write_ps_document.hpp"

#define INDIR           (string("precomputed/"))
#define OUTDIR          (string("build/files/run/"))

#define print(string)   psout.print((string), true)
#define parent(iter)    rna_tree::parent(iter)
ps_writer psout;

using namespace std;




rna_tree get_rna(const string& name)
{
    string l, b;
    l = read_file(INDIR + name + ".seq");
    b = read_file(INDIR + name + ".fold");

    ps_document doc(INDIR + name + ".ps");

    psout.init(OUTDIR + "document.ps");
    string p = doc.prolog;
    DEBUG("%s", to_cstr(p));

    print(p);

    return rna_tree(b, l, doc.points, name);
}

vector<string> create_arguments(string file)
{
    vector<string> vec = {
        file,
        "-tt",
            INDIR + file + ".ps",
            INDIR + file + ".fold",
            "--name", file + "_templ",
        "-mt",
            OUTDIR + file + ".matched.seq",
            OUTDIR + file + ".matched.fold",
            "--name", file + "_match",
        //"-r",
            //"--strategies", OUTDIR + file + ".matched.rted",
        //"-g",
            //"--ted-out", OUTDIR + file + ".matched.ted",
            //"--mapping", OUTDIR + file + ".matched.map",
        "--ps",
            "--mapping", OUTDIR + file + ".matched.map",
            OUTDIR + file + ".matched.ps",
    };

    return vec;
}

void test::run()
{
    APP_DEBUG_FNAME;

    remove_leaf_nodes();
}

rna_tree test::remove_leaf_nodes()
{
    APP_DEBUG_FNAME;

    auto human = [&]() {
        rna_tree rna = get_rna("human");

        size_t n, m;
        iterator it = rna.begin();
        rna_pair_label lbl("I");

        n = 975;
        m = 10;

        it = plusplus(it, n);

        print(psout.sprint_subtree(it));

        while (m-- != 0)
        {
            rna.insert(it, lbl, 0);
        }

        save(rna);

        app app;
        app.run(create_arguments("human"));

        return rna;
    };

    return human();
}


void test::save(const rna_tree& rna)
{
    APP_DEBUG_FNAME;

    string file = OUTDIR + rna.name();

    write_file(file + ".matched.fold", rna.get_brackets());
    write_file(file + ".matched.seq", rna.get_labels());
}

