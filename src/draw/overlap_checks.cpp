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

/*static*/
overlap_checks::overlaps overlap_checks::get_overlaps(const overlap_checks::edges &es1,
                                                      const overlap_checks::edges &es2,
                                                      const vector<int> &es1_filter,
                                                      const vector<int> &es2_filter)
{
    APP_DEBUG_FNAME;
    
    point p;
    vector<overlapping> vec;

    std::vector<int> ixs1(es1_filter), ixs2(es2_filter);
    if (es1_filter.size() == 0) for (int i = 0; i< es1.size(); i++) ixs1.push_back(i);
    if (es2_filter.size() == 0) for (int i = 0; i< es2.size(); i++) ixs2.push_back(i);

    
    for (size_t i = 0; i < ixs1.size(); ++i)
    {
        overlap_checks::edge e1 = es1[ixs1[i]];
        
        for (size_t j = 0; j < ixs2.size(); ++j)
        {
            overlap_checks::edge e2 = es2[ixs2[j]];

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

