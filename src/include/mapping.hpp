/*
 * File: mapping.hpp
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */


#ifndef MAPPING_HPP
#define MAPPING_HPP

#include <vector>
#include <cstddef> // size_t

/**
 * representation for mapping between two trees
 */
class mapping
{
public:
    typedef std::vector<size_t> indexes;

    struct mapping_pair
    {
        size_t from, to;

        bool operator<(
                    const mapping_pair& other) const;
    };

public:
    mapping() = default;

    /**
     * filter only inserts
     */
    indexes get_to_insert() const;

    /**
     * filter only deletes
     */
    indexes get_to_remove() const;

public:
    size_t distance;
    std::vector<mapping_pair> map;
};


#endif /* !MAPPING_HPP */

