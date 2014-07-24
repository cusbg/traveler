/*
 * types.hpp
 * Copyright (C) 2014 eliasr <eliasr@u-pl22>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TYPES_HPP
#define TYPES_HPP

#include <cassert>

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


#endif /* !TYPES_HPP */

