/*
 * File: mapping.cpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard.elias@matfyz.cz>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */


#include "mapping.hpp"
#include "types.hpp"

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


std::vector<mapping::mapping_pair> mapping::get_to_update() const
{
    APP_DEBUG_FNAME;

    std::vector<mapping::mapping_pair> vec;
    for (auto m : map)
    {
        if (m.to != 0 && m.from != 0)
            vec.push_back(m);
    }

    return vec;
}

std::map<size_t , size_t> mapping::get_target_template_map() const
{
    APP_DEBUG_FNAME;

    std::map<size_t , size_t> tgt_tmp_map;

    for (auto m : map)
    {
        if (m.to != 0)
            tgt_tmp_map.insert(std::make_pair(m.to, m.from));
    }

    return tgt_tmp_map;
}

bool mapping::mapping_pair::operator<(
                                      const mapping_pair& other) const
{
    return from < other.from ||
    (from == other.from &&
     to < other.to);
}
