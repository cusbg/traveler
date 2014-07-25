/*
 * types.hpp
 * Copyright (C) 2014 eliasr <eliasr@u-pl22>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TYPES_HPP
#define TYPES_HPP

#include <cassert>
#include <string>
#include <vector>

class Base
{
private:
    enum _base { Adenin, Cytosin, Guanin, Uracyl } b;
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


typedef std::pair<int, int> BasePair;


struct Global
{
    // definicie in-suborov:
    static std::string HS_seq; 
    static std::string HS_db;
    static std::string HS_bpseq;
    static std::string HS_rnafold;
    static std::string HS_zatvorky;
};


#endif /* !TYPES_HPP */

