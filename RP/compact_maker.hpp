/*
 * File: compact_maker.hpp
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

#ifndef COMPACT_MAKER_HPP
#define COMPACT_MAKER_HPP

#include "util.hpp"
#include "point.hpp"



class compact
{
public: //TODO: remove
    using iterator = rna_tree::iterator;
    using sibling_iterator = rna_tree::sibling_iterator;
    struct interval;
    struct circle;
public:
    compact(const document& _doc) : doc(_doc) {}
    void make_compact();

private:
    void shift_nodes(iterator it, Point vector);

    void make_pairs();
    void make_inserted();
    void make_deleted();
    void rebase(iterator it);

    circle make_circle(iterator i);


    std::vector<sibling_iterator> get_branches(iterator it);
    size_t bases_count(iterator from, iterator to);


    void reinsert(iterator it, size_t index, Point p);

    bool is_normalized_dist(iterator it);
    void normalize_distance(iterator it);

private:
    document doc;
};



struct compact::circle
{
    void compute_sgn();

    void inited() const;
    void init(size_t n);
    Point rotate(double angle) const;
    std::vector<Point> split(size_t n) const;
    bool lies_in_segment(Point p) const;

    double radius() const;
    double segment_angle() const;
    double segment_length() const;

    static double min_length(size_t nodes_count);

public:
    Point centre;
    Point p1, p2;
    Point direction;

private:
    /*
     * sgn =  1 ~> v protismere hod. ruciciek
     * sgn = -1 ~> v smere hodinovych ruciciek
     */
    char sgn = 0;
};

void print(compact::circle c);

#define PAIRS_DISTANCE      20
#define BASES_DISTANCE      8
#define BASES_RATIO         1.5

#define min_circle_length(nodes_count) \
    (nodes_count * ( BASES_DISTANCE + BASES_RATIO))
#define min_circle_radius(nodes_count) \
    (nodes_count * 0.5 * BASES_DISTANCE)
#define max_circle_radius(nodes_count) \
    (nodes_count * 1.5 * BASES_DISTANCE)

std::ostream& operator<<(std::ostream& out, const compact::circle& c);

#endif /* !COMPACT_MAKER_HPP */

