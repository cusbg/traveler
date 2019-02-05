/*
 * File: point.hpp
 *
 * Copyright (C) 2019 David Hoksza <david.hoksza@gmail.com>
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

#ifndef BOUNDING_BOX_HPP
#define BOUNDING_BOX_HPP

#include "point.hpp"


struct rectangle
{
    point top_left;
    point bottom_right;
    
public:
    rectangle();
    rectangle(point top_left, point bottom_right);


    rectangle& operator=(const rectangle& other);
    rectangle operator+(const point& other) const;
    rectangle operator+(const rectangle& other) const;
    rectangle& operator+=(const point& other);
    rectangle& operator+=(const rectangle& other);
//    bool operator&&(const rectangle& other) const;
//    bool operator&&(const point& other) const;

    bool intersects(const rectangle& rect) const;
    bool has(const point& point) const;


    bool initiated() const;

    bool includes(const rectangle& other) const;



};


// other useful functions for points

#endif /* !BOUNDING_BOX_HPP */
