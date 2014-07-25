/*
 * rnafold.hpp
 * Copyright (C) 2014 eliasr <eliasr@u-pl22>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef RNAFOLD_HPP
#define RNAFOLD_HPP

#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "types.hpp"


class RNAfold
{
private:
    std::string seq;
    std::string brackets;
    void Run();
    void Run2();
    int MakePairsRecursive(int from, std::vector<BasePair>& vec);
public:
    RNAfold(std::vector<Base> vec);
    RNAfold(std::string _seq = "../InFiles/zatvorky");
    std::vector<BasePair> makePairs();
    std::string getSeq();
    std::string getBrackets();
};


#endif /* !RNAFOLD_HPP */

