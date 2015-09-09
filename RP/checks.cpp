/*
 * File: checks.cpp
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

#include "checks.hpp"
#include "rna_tree.hpp"

using namespace std;


struct crossing_check::edge
{
    Point p1, p2;
};
struct crossing_check::line_equation
{
    double a, b, c;
    // equation: a*x + b*y + c = 0
};


crossing_check::edges crossing_check::split_to_edges(rna_tree rna)
{
    APP_DEBUG_FNAME;

    edges vec;
    edge e;
    rna_tree::pre_post_order_iterator it;

    it = ++rna.begin_pre_post();

    e.p1 = it->get_label().lbl(get_label_index(it)).point;
    ++it;

    for (; !it->is_root();)
    {
        DEBUG("1");
        cout << label(it) << endl;
        DEBUG("2");
        e.p2 = it->get_label().lbl(get_label_index(it)).point;
        DEBUG("3");

        DEBUG("%s: %s\t->\t%s", clabel(it), e.p1.to_string().c_str(), e.p2.to_string().c_str());
        DEBUG("4");
        vec.push_back(e);
        DEBUG("5");
        INFO("BEG: plusplus");
        ++it;
        INFO("END: plusplus");
        e.p1 = e.p2;
        DEBUG("6");
    }

    for (auto val : vec)
        DEBUG("%s\t->\t%s", val.p1.to_string().c_str(), val.p2.to_string().c_str());

    return vec;
}

bool crossing_check::intersect(rna_tree rna)
{
    APP_DEBUG_FNAME;

    auto vec = split_to_edges(rna);
    edge e1, e2;

    for (size_t i = 0; i < vec.size(); ++i)
    {
        for (size_t j = 0; j < vec.size(); ++i)
        {
            if (i == j)
                continue;

            e1 = vec[i];
            e2 = vec[j];

            if (intersect(e1, e2))
                return true;
        }
    }

    return false;
}

crossing_check::line_equation crossing_check::create_equation(edge e)
{
    line_equation q;
    Point p;

    p = e.p2 - e.p1;
    if (size(p) == 0)
        WARN("%s, %s", to_string(e.p1).c_str(), to_string(e.p2).c_str());
    assert(size(p) != 0);

    q.a = -p.y;
    q.b = p.x;
    q.c = -(q.a * e.p1.x + q.b * e.p1.y);

    /* equation: a*x + b*y + c = 0 */
    return q;
}

bool crossing_check::intersect(edge e1, edge e2)
{
    line_equation k, l, m;
#define multiply(line, n) { line.a *= n; line.b *= n; line.c *=n; }

    k = create_equation(e1);
    l = create_equation(e2);

    // remove (k/l).x
    
    double n1, n2;
    if (k.a != 0 || l.a != 0)
    {
        n1 = k.a;
        n2 = l.a;
    }
    else
    {
        assert(k.b != 0 || l.b != 0);
        n1 = k.b;
        n2 = l.b;
    }
    multiply(k, n2);
    multiply(l, n1);

    m.a = k.a - l.a;
    m.b = k.b - l.b;
    m.c = k.c - l.c;

    DEBUG("k = %fx + %fy + %f = 0", k.a, k.b, k.c);
    DEBUG("l = %fx + %fy + %f = 0", l.a, l.b, l.c);
    DEBUG("m = %fx + %fy + %f = 0", m.a, m.b, m.c);

    Point p = {0, 0};

    if (m.a != 0)
    {
        DEBUG("A");

        p.x = m.c / m.a;
        p.y = (k.a * p.x + k.c) / k.b;
    }
    else if (m.b != 0)
    {
        DEBUG("B");

        assert(m.b != 0);

        p.y = -(m.c / m.b);
        //cout << (k.b * p.y) << " " << k.c << " " << k.a << endl;
        p.x = (-(k.b * p.y + k.c)) / k.a;
    }
    else
        return m.c == 0;

    cout << e1.p1 << "; " << e1.p2 << " | " << p << endl;
    cout << e2.p1 << "; " << e2.p2 << " | " << p << endl;

    // TODO: lies_between()
    return lies_between(p, e1.p1, e1.p2) &&
        lies_between(p, e2.p1, e2.p2);
}




