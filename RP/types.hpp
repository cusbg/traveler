/*
 * types.hpp
 * Copyright (C) 2014 eliasr <eliasr@u-pl22>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>


#include <log4cpp/Category.hh>

class Base
{
private:
    enum _base { Adenin, Cytosin, Guanin, Uracil } b;
    void setBase(char ch);
public:
    Base(char ch);
    static bool isBase(char ch);
    char getBase() const;
};


struct Point
{
    float X;
    float Y;
};

enum path_strategy
{
    left, heavy, right
};


struct Global
{
    // definicie in-suborov:
    static std::string HS_seq; 
    static std::string HS_db;
    static std::string HS_bpseq;
    static std::string HS_rnafold;
    static std::string HS_zatvorky;

    static std::string zatvorky;
    static std::string rnaseq;
};



extern log4cpp::Category& logger; // globalna premenna...

#ifndef APP_DEBUG_FNAME
#define APP_DEBUG_FNAME logger.debug("entering function: %s", __PRETTY_FUNCTION__)
#endif



#endif /* !TYPES_HPP */

