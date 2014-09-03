/*
 * File: main.cpp
 * Created: 2014-09-01
 * By: eliasr <eliasr@u-pl22>
 */

#include <iostream>
#include "trees.hpp"

using namespace std;

string zatvorky =   "(.).((.).)";
string rna =        "UAACGGACAC";


int main(int argc, char** argv)
{
    zatvorky = "(.)";

    Node n("LABEL");
    n.makeRoot();
    n.createTree(zatvorky, 0);
    n.createTreeLabels(rna,0);
    
    n.getIDs();

    cout << endl <<n.findLongestPath() << endl;
    cout << endl <<n.getChildren()[0].findLongestPath() << endl;
    

    cout << n.toString() << endl;
    cout << n.getTreeLabels() << endl;
    return 0;
}

