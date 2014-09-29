/*
 * main.cpp
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
 *
 * Distributed under terms of the MIT license.
 */

#include <iostream>
#include "types.hpp"
#include "tree.hh"
#include "nodes.hpp"
#include "tree_util.hh"
#include "rna.hpp"

using namespace std;

typedef tree<TreeNode> rna_tree;
typedef vector<rna_tree> rna_forest;


int main(int argc, char** argv)
{
    logger.info("LOGGER");
    logger.debugStream() << "text";
    try
    {
        rna r;
        auto forrest = r.create_forest(Global::zatvorky, Global::rnaseq);
        //auto forrest = ct2(Global::zatvorky, Global::rnaseq);
        for (auto& les: forrest)
        {
            //for (auto& val : les)
                //cout << val.getLabel() << endl;

            cout << endl;
            for (auto strom = les.begin_post(); strom != les.end_post(); ++strom)
            {
                cout << *strom << " ";
            }

            cout << endl << "Strom: ";
            kptree::print_tree_bracketed(les);
        }
        cout << endl << "ITERACIA PODSTROMOM" << endl;
        auto it = forrest[0].child(forrest[0].begin(), 0);
        rna_tree::post_order_iterator it2 = it;
        while(it2.number_of_children() != 0)
            it2 = forrest[0].child(it2, 0);
        cout << "IT: " << *it << endl;
        kptree::print_subtree_bracketed(forrest[0], it, cout);
        for (;it != it2; ++it2)
            cout << *it2 << endl;
        cout << *it2 << endl;
        
    }
    catch(const char* ex)
    {
        cout << "EXCEPTION: " <<  ex << endl;
    }
	return 0;
}





