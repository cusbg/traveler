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

    while (++rna_tree::pre_post_order_iterator(it) != rna.end_pre_post())
    {
        e.p1 = it->at(it.label_index()).p;
        ++it;
        e.p2 = it->at(it.label_index()).p;

        vec.push_back(e);
    }

    return vec;
}

overlap_checks::line_equation overlap_checks::get_equation(
                const edge& e)
{
    point p;
    line_equation q;

    p = e.p2 - e.p1;
    assert(size(p) != 0);

    q.a = -p.y;
    q.b = p.x;
    q.c = -(q.a * e.p1.x + q.b * e.p1.y);

    assert(!q.hasnan());
    // (a != 0 != b) || c == 0
    assert((!iszero(q.a) ||
                !iszero(q.b)) ||
            iszero(q.c));

    return q;
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

        for (size_t j = i + 1; j < e.size(); ++j)
        {
            e2 = e[j];

            p = intersection(e1, e2);

            if (!p.bad())
            {
                cout << p << endl;
                abort();
            }


        }
    }
}

point overlap_checks::intersection(
                const edge& e1,
                const edge& e2)
{
    line_equation k, l;
    point p;

    k = get_equation(e1);
    l = get_equation(e2);

    p = compute(k, l);

    return p;
}

point overlap_checks::compute(
                line_equation k,
                line_equation l)
{
    line_equation m;
    point p;
    double n1, n2;

    DEBUG("BEG EQ:\n"
            "k = %s\n"
            "l = %s\n"
            "m = %s",
            to_cstr(k), to_cstr(l), to_cstr(m));
    if (!iszero(k.a) || !iszero(l.a))
    {
        n1 = k.a;
        n2 = l.a;
    }
    else
    {
        assert(!iszero(k.a) || !iszero(l.a));
        n1 = k.b;
        n2 = l.b;
    }
    //m = {0, 0, 0};

    k *= n2;
    l *= n1;

    // m = k - l
    m.a = k.a - l.a;
    m.b = k.b - l.b;
    m.c = k.c - l.c;

    DEBUG("END EQ:\n"
            "k = %s\n"
            "l = %s\n"
            "m = %s",
            to_cstr(k), to_cstr(l), to_cstr(m));

    assert(iszero(m.a) || iszero(m.b));

    if (!iszero(m.a))
    {
        p.x = m.c / m.a;

        assert(!iszero(k.b) || !iszero(l.b));
        p.y = (k.a * p.x + k.c) / (iszero(k.b) ? l.b : k.b);
    }
    else if (!iszero(m.b))
    {
        p.y = -(m.c / m.b);

        assert(!iszero(k.a) || !iszero(l.a));
        p.x = (-(k.b * p.y + k.c)) / (iszero(k.a) ? l.a : k.a);
    }
    else
        p = point::bad_point();

    return p;
}





/* inline */ bool overlap_checks::line_equation::hasnan() const
{
    return ::isnan(a) || ::isnan(b) || ::isnan(c);
}

/* inline */ overlap_checks::line_equation&
overlap_checks::line_equation::operator*=(
                double val)
{
    a *= val;
    b *= val;
    c *= val;
    return *this;
}

std::ostream& operator<<(
                std::ostream& out,
                const overlap_checks::line_equation& k)
{
    out
        << k.a << "x "
        << k.b << "y "
        << k.c
        << " = 0";
    return out;
}



