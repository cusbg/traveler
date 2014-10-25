/*
 * main.cpp
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
 *
 * Distributed under terms of the MIT license.
 */

#include <iostream>
#include "types.hpp"
#include "tree_hh/tree.hh"
#include "tree_hh/tree_util.hh"
#include "node_base.hpp"
#include "tree_base.hpp"
#include "rna_tree.hpp"
#include <fstream>
#include "rted.hpp"




using namespace std;

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


int main(int argc, char** argv)
{
    string b, z;
    //b = "ACGT";
    //z = "(..)";
    string b2 = "ABCDEFEGCA";
    string z2 = "(.(.(.).))";

    b = b2;
    z = z2;


    vector<node_base<string> > bases;
    for (unsigned i =0 ; i < b.size(); ++i)
        bases.push_back(b.substr(i, 1));
    //bases[0].get
    tree_base<node_base<string> > tb(z, bases);


    //rna_tree t2(Global::zatvorky, Global::rnaseq);
    rna_tree t2(z2, b2);

    cout << z2 << endl << b2 << endl;
    rted r(t2, t2);

    r.test1();

	return 0;
}





