/*
 * File: main.cpp
 * Created: 2014-07-23
 * By: eliasr <eliasr@u-pl22>
 */

#include <iostream>
#include "files_read.hpp"
#include "rnafold.hpp"

using namespace std;



int main(int argc, char** argv)
{
    Reader r;
    auto pair = r.readPS_DB_format("../InFiles/homo_sapiens.ps");

    //for (auto& val: pair.second)
        //cout << val.X << " " << val.Y << endl;

    RNAfold f;
    f.makePairs();
    
    auto pair2 = r.readPS_RNAfold_format();

    for (int i=0; i< pair.first.size(); ++i)
        assert (pair.first[i].getBase() == pair2.first[i].getBase());

    cout << "all ok" << endl;

    return 0;
}

