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

void try_tree(rna_tree rna)
{
    auto t = *rna.tree_ptr;
    kptree::print_tree_bracketed(t);
    cout << endl << boolalpha;


    auto post = t.begin_post();
    cout << post->get_label() << endl;
    cout << t.sibling(post, 0)->get_label() << endl;
    cout << (t.sibling(post, 0) == post) << endl;

    exit(0);


    auto it1 = t.begin_post();
    cout << it1->get_label() << endl;
    auto par1 = t.parent(it1);
    cout << par1->get_label() << endl;
    auto ch1 = t.child(par1, 0);
    cout << ch1->get_label() << endl;

    rna_tree::tree_type::sibling_iterator sib(it1);
    cout << boolalpha << (sib == sib.begin()) << endl;
    cout << sib.begin()->get_label() << endl;
    cout << boolalpha << (t.child(t.parent(it1), 0).begin() == it1) << endl;
    cout << (it1 == it1.begin()) << endl;
    cout << t.sibling(it1, 0)->get_label() << endl;


    exit(0);
}


int main(int argc, char** argv)
{
    cout << boolalpha << endl;
    string b1, l1, b2, l2;
    l1 = "3123";
    b1 = "(..)";
    l2 = "212";
    b2 = "(.)";

    rna_tree rna1(b1, l1);
    rna_tree rna2(b2, l2);

    //try_tree(rna1);

    rted r(rna1, rna2);
    r.run_rted();


    
	return 0;
}





