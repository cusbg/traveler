/*
 * File: compact_circle.hpp
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

#ifndef COMPACT_CIRCLE_HPP
#define COMPACT_CIRCLE_HPP


#include "compact.hpp"

struct compact::circle
{
private:
public:
    double radius() const;
    double segment_angle() const;
    double segment_length() const;

    bool lies_in_segment(
                const point& p) const;
    point rotate(
                double angle) const;
    std::vector<point> split(
                size_t n) const;

public:
    circle() = default;
    void compute_sgn();
    void init(
                size_t n);

public:
    static double min_circle_length(
                size_t n);

public:
    friend std::ostream& operator<<(
                std::ostream& out,
                const circle& c);
    std::string print_points();
public:
    point p1, p2;
    point centre;
    point direction;

private:
    /*
     * sgn =  1 ~> v protismere hod. ruciciek
     * sgn = -1 ~> v smere hodinovych ruciciek
     */
    signed char sgn = 0;
};



#endif /* !COMPACT_CIRCLE_HPP */

