/*
 * File: overlap_checks.cpp
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

#include "overlap_checks.hpp"
#include "rna_tree.hpp"

using namespace std;



overlap_checks::overlap_checks()
{ }

overlap_checks::overlaps overlap_checks::run(
                                             rna_tree& rna)
{
    APP_DEBUG_FNAME;
    
    INFO("BEG: Checking overlaps for RNA %s", rna.name());
    
    edges vec = get_edges(rna);
    overlaps overlaps = run(vec);
    //overlaps overlaps = get_overlaps(vec, vec);
    
    return overlaps;
}

overlap_checks::edges overlap_checks::get_edges(
                                                rna_tree& rna)
{
    APP_DEBUG_FNAME;
    
    edges vec;
    edge e;
    
#define get_p() it->at(it.label_index()).p
#define get_id() it->id()

    rna_tree::pre_post_order_iterator it = ++rna.begin_pre_post();
    e.p1 = get_p();
    e.id1 = get_id();
    
    for (++it; it != rna.end_pre_post(); ++it)
    {
        //assert(it->initiated_points());
        if (it->initiated_points()) {
            e.p2 = get_p();
            e.id2 = get_id();
            vec.push_back(e);
            e.p1 = e.p2;
            e.id1 = e.id2;
        }
    }

    return vec;
#undef get_p
#undef get_id
}


overlap_checks::overlaps overlap_checks::run(
                                             const edges& e)
{
    APP_DEBUG_FNAME;
    
    edge e1, e2;
    point p;
    vector<overlapping> vec;
    
    for (size_t i = 0; i < e.size(); ++i)
    {
        e1 = e[i];
        
        for (size_t j = i + 2; j < e.size(); ++j)
        {
            e2 = e[j];

            if (e1 == e2 || e1.share_point(e2)) continue;
            
            p = intersection(e1, e2);
            
            if (!p.bad())
            {
                auto distances = {
                    distance(p, e1.p1),
                    distance(p, e1.p2),
                    distance(p, e2.p1),
                    distance(p, e2.p2),
                };
                double radius = *std::max_element(distances.begin(), distances.end());
                vec.push_back({p, radius});
            }
        }
    }
    
    return vec;
}

bool onLine(overlap_checks::edge l1, point p) {   //check whether p is on the line or not
    if(p.x <= max(l1.p1.x, l1.p2.x) && p.x <= min(l1.p1.x, l1.p2.x) &&
       (p.y <= max(l1.p1.y, l1.p2.y) && p.y <= min(l1.p1.y, l1.p2.y)))
        return true;

    return false;
}

int direction(point a, point b, point c) {
    int val = (b.y-a.y)*(c.x-b.x)-(b.x-a.x)*(c.y-b.y);
    if (val == 0)
        return 0;     //colinear
    else if(val < 0)
        return 2;    //anti-clockwise direction
    return 1;    //clockwise direction
}

//point overlap_checks::intersection(const edge& l1, const edge& l2) {
//    //four direction for two lines and points of other line
//    int dir1 = direction(l1.p1, l1.p2, l2.p1);
//    int dir2 = direction(l1.p1, l1.p2, l2.p2);
//    int dir3 = direction(l2.p1, l2.p2, l1.p1);
//    int dir4 = direction(l2.p1, l2.p2, l1.p2);
//
//    if(dir1 != dir2 && dir3 != dir4)
//        return point(0,0); //they are intersecting
//
//    if(dir1==0 && onLine(l1, l2.p1)) //when p2 of line2 are on the line1
//        return point(0,0);
//
//    if(dir2==0 && onLine(l1, l2.p2)) //when p1 of line2 are on the line1
//        return point(0,0);
//
//    if(dir3==0 && onLine(l2, l1.p1)) //when p2 of line1 are on the line2
//        return point(0,0);
//
//    if(dir4==0 && onLine(l2, l1.p2)) //when p1 of line1 are on the line2
//        return point(0,0);
//
//    return point::bad_point();
//}

//point overlap_checks::intersection(const edge& e1, const edge& e2)
//{
//    float p0_x = e1.p1.x, p0_y = -e1.p1.y, p1_x = e1.p2.x, p1_y = -e1.p2.y;
//    float p2_x = e2.p1.x, p2_y = -e2.p1.y, p3_x = e2.p2.x, p3_y = -e1.p1.y;
//
//    float s1_x, s1_y, s2_x, s2_y;
//    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
//    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;
//
//    float s, t;
//    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
//    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);
//
//    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
//    {
//        // Collision detected
//        return point(p0_x + (t * s1_x), p0_y + (t * s1_y));
//    }
//
//    return point::bad_point(); // No collision
//}

/* static */ point overlap_checks::intersection(
                                                const edge& e1,
                                                const edge& e2)
{
    // a / sin(alpha) == b / sin(beta) == c / sin(gamma)
    // => a = c * sin(alpha) / sin(gamma)
    //
    double alpha, beta, gamma;
    double a, c;
    point p;

    int cnt_ends_meet = 0;
    if (contains<vector<point>>({e1.p1, e1.p2}, e2.p1)) {
        p = e2.p1;
        cnt_ends_meet++;
    }
    if (contains<vector<point>>({e1.p1, e1.p2}, e2.p2)) {
        p = e2.p2;
        cnt_ends_meet++;
    }

    if (cnt_ends_meet == 1) {
        return  p;
    } else if (cnt_ends_meet  == 2){
        return point::bad_point();
    }


    c = distance(e1.p1, e2.p1);

    alpha = angle(e2.p1, e1.p1, e1.p2);
    beta = angle(e1.p1, e2.p1, e2.p2);

    if (alpha > 180)
        alpha = 360. - alpha;
    if (beta > 180)
        beta = 360. - beta;

    gamma = 180. - alpha - beta;

    assert(double_equals(180, alpha + beta + gamma));

    if (gamma < 0 || iszero(gamma))
        return point::bad_point();

    a = c * radians_to_degrees(sin(degrees_to_radians(alpha))) /
    radians_to_degrees(sin(degrees_to_radians(gamma)));

    if (!iszero(a))
        p = move_point(e2.p1, e2.p2, a);
    else
        p = e2.p1;

    if (lies_between(p, e1.p1, e1.p2) &&
        lies_between(p, e2.p1, e2.p2))
        return p;
    else
        return point::bad_point();
}


/* static */
overlap_checks::edges overlap_checks::get_edges(const rna_tree::iterator& branch)
{
#undef get_p
    APP_DEBUG_FNAME;

    edges vec;
    edge e;

#define get_p() it->at(it.label_index()).p
#define get_id() it->id()

    rna_tree::pre_post_order_iterator it = rna_tree::pre_post_order_iterator(branch);
    e.p1 = get_p();
    e.id1 = get_id();

    for (++it; it != ++rna_tree::pre_post_order_iterator(branch, false); ++it)
    {
        if (it->initiated_points()) {
            e.p2 = get_p();
            e.id2 = get_id();
            vec.push_back(e);
            e.p1 = e.p2;
            e.id1 = e.id2;
        }
    }

    return vec;
#undef get_p
#undef get_id
}

/* static */
overlap_checks::edges overlap_checks::get_edges(const compact::sibling_iterator& begin, const compact::sibling_iterator& end)
{
#undef get_p
    APP_DEBUG_FNAME;

    edges vec;

    compact::sibling_iterator it_prev = begin, it = begin;
    for (++it; it != end; ++it)
    {
        assert(it->initiated_points())
        edge e1;

        e1.p1 = it_prev->paired() ? it_prev->at(1).p : it_prev->at(0).p;
        e1.id1 = it_prev->id();
        e1.p2 = it->at(0).p;
        e1.id2 = it->id();
        vec.push_back(e1);

        if (it->paired()) {
            edge e2;
            e2.p1 = it->at(0).p;
            e2.id1 = it->id();
            e2.p2 = it->at(1).p;
            e2.id2 = it->id();
            vec.push_back(e2);
        }
    }

    for (auto it = begin; it != end; ++it) {
        auto vec1 = overlap_checks::get_edges(rna_tree::iterator(it));
        vec.insert(vec.end(), vec1.begin(), vec1.end());
    }


    return vec;
}

/*static*/
overlap_checks::overlaps overlap_checks::get_overlaps(const overlap_checks::edges &es1,
                                                      const overlap_checks::edges &es2)
{
    APP_DEBUG_FNAME;
    
    point p;
    vector<overlapping> vec;

    
    for (edge e1: es1)
    {
        for (edge e2: es2)
        {
            p = intersection(e1, e2);

            if (e1 == e2 || e1.share_point(e2)) continue;

            if (!p.bad())
            {
                auto distances = {
                    distance(p, e1.p1),
                    distance(p, e1.p2),
                    distance(p, e2.p1),
                    distance(p, e2.p2),
                };
                double radius = *std::max_element(distances.begin(), distances.end());
                vec.push_back({p, radius});
            }
        }
    }
    
    return vec;
    
}

bool operator==(const overlap_checks::edge& e1, const overlap_checks::edge& e2) {
    return (e1.id1 == e2.id1 && e1.id2 == e2.id2) || (e1.id2 == e2.id1 && e1.id1 == e2.id2);
}

