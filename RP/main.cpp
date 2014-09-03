/*
 * File: main.cpp
 * Created: 2014-09-01
 * By: eliasr <eliasr@u-pl22>
 */

#include <iostream>
#include "trees.hpp"
#include "other.hpp"

using namespace std;

string zatvorky =   "(.).((.).)";
string rna =        "UAACGiAjAC";


int main(int argc, char** argv)
{
    Node n;
    n.createTree(rna, zatvorky,0);

    cout << rna << endl << zatvorky << endl << endl;
    cout << printLabels(n) << endl;
    
    cout << "R_path"<<endl;

    return 0;
}

