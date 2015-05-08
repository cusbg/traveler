/*
 * File: main.cpp
 *
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
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


#include <iostream>
#include <fstream>

#include "types.hpp"
#include "rted.hpp"
#include "tests.hpp"
#include "gted.hpp"
#include "util.hpp"
#include "app.hpp"


#define INPUT_PS1   "../InFiles/homo_sapiens.ps"
#define OUTPUT_PS   "build/out.ps"



using namespace std;



int main(int argc, char** argv)
{
    cout << boolalpha;
    srand(1);

    if (argc == 2)
    {
        generator::generate_files();
        exit(0);
    }

    string l1, l2, b1, b2;
    rna_tree rna1, rna2;
    string ps_in, ps_out;

    l1 = "-5142aba3456978089-";
    b1 = "((.(.(.).)).(.(.)))";

    rna1 = rna_tree(b1, l1);
    rna_node_type node(rna_pair_label("ahoj"));
    auto it = rna1.begin();
    for (int i = 0; i < 4; ++i)
        ++it;
    rna1.print_tree();
    it = rna1.modify(it, node);
    rna1.print_tree();
    cout << *it << endl;
    it = rna1.remove(it);
    cout << *it << endl;
    rna1.print_tree();

    rna1.print_tree();
    for (auto it = rna1.begin_pre_post(); it != rna1.end_pre_post(); ++it)
        cout << *it << " ";
    cout << endl;

    app a;
    a.run_app();


    return 0;
}


/*
    string l1, l2, b1, b2;
    rna_tree rna1, rna2;
    string ps_in, ps_out;


    l1 = "-51423456978089-";
    b1 = "((.(..)).(.(.)))";

    l2 = "abcdeca";
    b2 = "(.(..))";

    rna1 = rna_tree(b1, l1);
    rna2 = rna_tree(b2, l2);


    ps_in = INPUT_PS1;
    ps_out = OUTPUT_PS;

    //l1 = get_rna_seq_from_ps(ps_in);
    //b1 = run_RNAfold(l1);

    rna1 = rna_tree(b1, l1);

    app a;
    a.run_app(rna1, rna2);

    assert(rna1.get_brackets() == b1 && rna1.get_labels() == l1);
    cout << rna1.get_brackets() << endl;
    cout << rna1.get_labels() << endl;
    abort();
*/



string readseq()
{
    ifstream input("../InFiles/seq");
    string s;
    input >> s;
    return s;
}
string readbrackets()
{
    ifstream input("../InFiles/zatvorky");
    string s;
    input >> s;
    return s;
}
