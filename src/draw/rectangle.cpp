/*
 * File: point.cpp
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


#include "rectangle.hpp"

using namespace std;



rectangle::rectangle()
: top_left(point::bad_point()), bottom_right(point::bad_point())
{
}

point::point(point _top_left, point _bottom_right)
:top_left(_top_left), bottom_right(_bottom_right)
{ }

rectangle rectangle::operator+(const rectangle& other) const
{
    return rectangle(
            point(min(top_left.x, other.top_left.x), min(top_left.y, other.top_left.y)),
            point(max(bottom_right.x, other.bottom_right.x), max(bottom_right.y, other.bottom_right.y)))
}

rectangle rectangle::operator+(const point& other) const
{
    return rectangle(
            point(min(top_left.x, other.x), min(top_left.y, other.y)),
            point(max(bottom_right.x, other.x), max(bottom_right.y, other.y)))
}

rectangle& rectangle::operator=(const rectangle& other){
    top_left = other.top_left;
    bottom_right = other.bottom_right;
    return *this;

}

bool rectangle::initiated() const {
    return !top_left.bad() && !bottom_right.bad();
}
