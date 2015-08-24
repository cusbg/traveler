/*
 * File: compact_circle.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard@ba30.eu>
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

#include "compact_maker.hpp"


#define PAIRS_DISTANCE              20
#define PAIRS_DISTANCE_PRECISION    1
#define BASES_DISTANCE              8
#define BASES_RATIO                 1.4


struct compact::circle
{
    void compute_sgn();

    void init(size_t n);
    Point rotate(double angle) const;
    std::vector<Point> split(size_t n) const;
    bool lies_in_segment(Point p) const;

    double radius() const;
    double segment_angle() const;
    double segment_length() const;

    static double min_circle_length(size_t nodes_count);
    static double max_circle_length(size_t nodes_count);
    static double min_circle_radius(size_t nodes_count);
    static double max_circle_radius(size_t nodes_count);

    void draw();   //TODO remove

public:
    Point centre;
    Point p1, p2;
    Point direction;    // ukazuje smer neobsahujuci vysek kruznice
                        // =>  !lies_in_segment(direction) 

//#ifdef TESTS
public:
    void tests() const;
//#endif

private:
    /*
     * sgn =  1 ~> v protismere hod. ruciciek
     * sgn = -1 ~> v smere hodinovych ruciciek
     */
    char sgn = 0;
};


#endif /* !COMPACT_CIRCLE_HPP */

