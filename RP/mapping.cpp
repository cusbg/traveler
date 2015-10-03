/*
 * File: mapping.cpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard.elias@matfyz.cz>
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

#include "mapping.hpp"
#include "rna_tree.hpp"
#include "generator.hpp"

using namespace std;


mapping::indexes mapping::get_to_insert() const
{
    APP_DEBUG_FNAME;

    indexes vec;
    for (auto m : map)
    {
        if (m.from == 0)
            vec.push_back(m.to);
    }
    sort(vec.begin(), vec.end(), less<size_t>());
    LOGGER_PRINT_CONTAINER(vec, "to_insert");

    return vec;
}

mapping::indexes mapping::get_to_remove() const
{
    APP_DEBUG_FNAME;

    indexes vec;
    for (auto m : map)
    {
        if (m.to == 0)
            vec.push_back(m.from);
    }
    sort(vec.begin(), vec.end(), less<size_t>());
    LOGGER_PRINT_CONTAINER(vec, "to_remove");

    return vec;
}

mapping::mapping(
                const rna_tree& rna1,
                const rna_tree& rna2)
    : mapping(MAP(rna1.name(), rna2.name()))
{ }

mapping::mapping(
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    assert(exist_file(filename));

    ifstream in(filename);
    *this = read_mapping(in);
}

/* static */ mapping mapping::read_mapping(
                std::istream& in)
{
    APP_DEBUG_FNAME;

    mapping map;
    mapping_pair m;

    string s;
    in
        >> s
        >> s;
    //DEBUG("distance: %s", s.c_str());
    map.distance = stoi(s);

    while(true)
    {
        in >> s;
        if (in.fail())
            break;
        m = split(s);
        map.map.push_back(m);
    }
    return map;
}

/* static */ mapping::mapping_pair mapping::split(
                const std::string& text)
{
    mapping_pair m;
    stringstream str;
    char ch;

    str << text;

    str
        >> m.from
        >> ch
        >> ch
        >> m.to;

    assert(!str.fail());

    //DEBUG("%lu -> %lu",
            //m.from, m.to);

    return m;
}

