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
#include "write_ps_document.hpp"

using namespace std;

inline bool iszero(
                double val)
{
    return double_equals(val, 0);
}

overlap_checks::overlap_checks(
                rna_tree& _rna)
    : rna(_rna)
{
    APP_DEBUG_FNAME;
}

void overlap_checks::run()
{
    APP_DEBUG_FNAME;

    edges vec = get_edges();
    run(vec);
}

overlap_checks::edges overlap_checks::get_edges()
{
    APP_DEBUG_FNAME;

    edges vec;
    edge e;
    rna_tree::pre_post_order_iterator it;

    it = ++rna.begin_pre_post();

    e.p1 = it->at(it.label_index()).p;
    ++it;

    while (true)
    {
        e.p1 = it->at(it.label_index()).p;
        ++it;
        if (++rna_tree::pre_post_order_iterator(it) == rna.end_pre_post())
            break;
        assert(it->inited_points());
        e.p2 = it->at(it.label_index()).p;

        vec.push_back(e);
    }
    //for (auto val : vec)
        //psout.print(psout.sprint_edge(val.p1, val.p2, false));

    //abort();

    return vec;
}

void overlap_checks::run(
                const edges& e)
{
    APP_DEBUG_FNAME;

    edge e1, e2;
    point p;

    for (size_t i = 0; i < e.size(); ++i)
    {
        e1 = e[i];

        for (size_t j = i + 2; j < e.size(); ++j)
        {
            e2 = e[j];

            p = intersection(e1, e2);

            if (!p.bad())
                has_intersection(e1, e2);
        }
    }
}

point overlap_checks::intersection(
                const edge& e1,
                const edge& e2)
{
    // sin(a)/alpha == sin(b)/beta == sin(c)/gamma
    //
    double alpha, beta, gamma;
    double a, c;
    point p;

    assert(!contains<vector<point>>({e1.p1, e1.p2}, e2.p1));
    assert(!contains<vector<point>>({e1.p1, e1.p2}, e2.p2));

    DEBUG("e1: <%s | %s>, e2: <%s | %s>",
            to_cstr(e1.p1), to_cstr(e1.p2),
            to_cstr(e2.p1), to_cstr(e2.p2));

    c = distance(e1.p1, e2.p1);
    alpha = angle(e2.p1, e1.p1, e1.p2);
    beta = angle(e1.p1, e2.p1, e2.p2);
    gamma = 180. - alpha - beta;

    if (gamma < 0 || iszero(gamma))
        return point::bad_point();

    a = radians_to_degrees(c / sin(degrees_to_radians(gamma)) *
            sin(degrees_to_radians(alpha)));

    DEBUG("alpha %f, beta %f, gamma %f, c %f, a %f",
            alpha, beta, gamma, c, a);

    if (!iszero(a))
        p = move_point(e2.p1, e2.p2, a);
    else p = e2.p1;

    if (lies_between(p, e1.p1, e1.p2) &&
            lies_between(p, e2.p1, e2.p2))
        return p;
    else
        return point::bad_point();
}

void overlap_checks::has_intersection(
                const edge& e1,
                const edge& e2)
{
    APP_DEBUG_FNAME;
    WARN("intersection occurs");

    auto vec = {
        distance(e1.p1, e1.p2),
        distance(e1.p1, e2.p1),
        distance(e1.p1, e2.p2),
        distance(e2.p1, e2.p2)
    };

    double max = *std::max_element(vec.begin(), vec.end());

    psout.print(psout.sprint_circle(e1.p1, max));
}



