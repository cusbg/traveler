/*
 * File: types.cpp
 * Created: 2014-07-23
 * By: eliasr <eliasr@u-pl22>
 */

#include <iostream>
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
