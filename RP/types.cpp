/*
 * File: types.cpp
 * Created: 2014-09-01
 * By: eliasr <eliasr@u-pl22>
 */

#include "types.hpp"

using namespace std;

bool Base::isBase(char ch)
{
    string s = "ACGUacgu";
    return s.find(ch) != s.npos;
}

void Base::setBase(char ch)
{
    switch (ch)
    {
        case 'A':
        case 'a':
            b = _base::Adenin;
            break;
        case 'C':
        case 'c':
            b = _base::Cytosin;
            break;
        case 'G':
        case 'g':
            b = _base::Guanin;
            break;
        case 'U':
        case 'u':
            b = _base::Uracyl;
            break;
        default:
            throw "ERROR: not a base";
    }
}

char Base::getBase() const
{
    switch (b)
    {
        case Adenin:
            return 'A';
        case Guanin:
            return 'G';
        case Uracyl:
            return 'U';
        case Cytosin:
            return 'C';
    }
    throw "";
}

Base::Base(char ch)
{
    setBase(ch);
}


std::string Global::HS_seq = "../InFiles/seq";
std::string Global::HS_db = "../InFiles/homo_sapiens.ps";
std::string Global::HS_bpseq = "../InFiles/homo_sapiens.bpseq";
std::string Global::HS_rnafold = "../InFiles/homo_sapiens_RNAfold.ps";
std::string Global::HS_zatvorky = "../InFiles/zatvorky";

