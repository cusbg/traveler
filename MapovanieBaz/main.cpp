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
    RNAfold f(r.readPS_DB_format(Global::HS_db).first);

    cout << f.getSeq() << endl;
    cout << f.getBrackets() << endl;


    return 0;
}

