/*
 * File: types.hpp
 *
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
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

