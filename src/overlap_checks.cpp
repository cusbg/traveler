/*
 * File: overlap_checks.cpp
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

#include "overlap_checks.hpp"
#include "rna_tree.hpp"
#include "point.hpp"

using namespace std;


overlap_checks::overlap_checks()
{
    APP_DEBUG_FNAME;
}

overlap_checks::overlaps overlap_checks::run(
                rna_tree& rna)
{
    APP_DEBUG_FNAME;

    INFO("BEG: OVERLAP_CHECKS(%s)", to_cstr(rna.name()));

    edges vec = get_edges(rna);
    overlaps overlaps = run(vec);

    INFO("END: OVERLAP_CHECKS(%s)", to_cstr(rna.name()));

    return overlaps;
}

overlap_checks::edges overlap_checks::get_edges(
                rna_tree& rna)
{
    APP_DEBUG_FNAME;

    edges vec;
    edge e;
    rna_tree::pre_post_order_iterator it, end;

    it = ++rna.begin_pre_post();
    end = rna_tree::pre_post_order_iterator(rna.begin(), false);

    e.p1 = it->at(it.label_index()).p;
    ++it;

    while (true)
    {
        e.p1 = it->at(it.label_index()).p;
        ++it;
        if (it == end)
            break;
        assert(it->inited_points());
        e.p2 = it->at(it.label_index()).p;

        vec.push_back(e);
    }

    return vec;
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

point overlap_checks::intersection(
                const edge& e1,
                const edge& e2) const
{
    // sin(a)/alpha == sin(b)/beta == sin(c)/gamma
    //
    double alpha, beta, gamma;
    double a, c;
    point p;

    assert(!contains<vector<point>>({e1.p1, e1.p2}, e2.p1));
    assert(!contains<vector<point>>({e1.p1, e1.p2}, e2.p2));

    c = distance(e1.p1, e2.p1);
    alpha = angle(e2.p1, e1.p1, e1.p2);
    beta = angle(e1.p1, e2.p1, e2.p2);
    gamma = 180. - alpha - beta;

    if (gamma < 0 || iszero(gamma))
        return point::bad_point();

    a = radians_to_degrees(c / sin(degrees_to_radians(gamma)) *
            sin(degrees_to_radians(alpha)));

    if (!iszero(a))
        p = move_point(e2.p1, e2.p2, a);
    else p = e2.p1;

    if (lies_between(p, e1.p1, e1.p2) &&
            lies_between(p, e2.p1, e2.p2))
        return p;
    else
        return point::bad_point();
}

