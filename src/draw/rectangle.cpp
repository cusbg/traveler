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

rectangle::rectangle(point _top_left, point _bottom_right)
:top_left(_top_left), bottom_right(_bottom_right)
{ }

rectangle rectangle::operator+(const rectangle& other) const
{
    return rectangle(
            point(min(top_left.x, other.top_left.x), max(top_left.y, other.top_left.y)),
            point(max(bottom_right.x, other.bottom_right.x), min(bottom_right.y, other.bottom_right.y)));
}

rectangle rectangle::operator+(const point& other) const
{
    return rectangle(
            point(min(top_left.x, other.x), max(top_left.y, other.y)),
            point(max(bottom_right.x, other.x), min(bottom_right.y, other.y)));
}

rectangle& rectangle::operator=(const rectangle& other){
    top_left = other.top_left;
    bottom_right = other.bottom_right;
    return *this;

}

bool rectangle::intersects(const rectangle& rect) const{
    // If one rectangle is on left side of other

    if ((rect.has(top_left) && rect.has(bottom_right)) || (has(rect.top_left) && has(rect.bottom_right)))
        return true;

    if (bottom_right.x < rect.top_left.x || top_left.x > rect.bottom_right.x)
        return false;

    // If one rectangle is above other
    if (top_left.y < rect.bottom_right.y || bottom_right.y > rect.top_left.y)
        return false;

    return true;
}

bool rectangle::has(const point& point) const{
    return top_left.x <= point.x && point.x  <= bottom_right.x
           && top_left.y >= point.y && point.y >= bottom_right.y;
}


bool rectangle::initiated() const {
    return !top_left.bad() && !bottom_right.bad();
}
