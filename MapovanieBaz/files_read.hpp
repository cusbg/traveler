/*
 * files_read.hpp
 * Copyright (C) 2014 eliasr <eliasr@u-pl22>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef FILES_READ_HPP
#define FILES_READ_HPP

#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "types.hpp"


class Reader
{
private:
	
public:
    std::pair<std::vector<Base>, std::vector<BasePair> > readBPSEQ(std::string file = "../InFiles/homo_sapiens.bpseq");
        
    std::pair<std::vector<Base>, std::vector<Point> > readPS_DB_format(std::string file = "../InFiles/homo_sapiens.ps");

    std::pair<std::vector<Base>, std::vector<Point> > readPS_RNAfold_format(std::string file = "../InFiles/homo_sapiens_RNAfold.ps");
	
};






#endif /* !FILES_READ_HPP */

