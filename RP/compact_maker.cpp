/*
 * File: compact_maker.cpp
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

#include "compact_maker.hpp"
#include "util.hpp"
#include "ps.hpp"
#include "compact_maker_utils.hpp"


using namespace std;


#define PAIRS_DISTANCE              20
#define PAIRS_DISTANCE_PRECISION    1
#define BASES_DISTANCE              8


void compact::make_compact()
{
    APP_DEBUG_FNAME;

    init();

    psout.seek(psout.save(doc.rna));
    wait_for_input();

    make_inserted();
}

void compact::normalize_pair_distance(iterator it)
{
    assert(it->get_label().is_paired());

    auto& label = it->get_label();
    Point& p1 = label.lbl(0).point;
    Point& p2 = label.lbl(1).point;

    if (!double_equals_precision(distance(p1, p2),
                PAIRS_DISTANCE, PAIRS_DISTANCE_PRECISION))
    {
        DEBUG("distance=%f", distance(p1, p2));
        if (!rna_tree::parent(it)->is_root() &&
                (rna_tree::is_only_child(it) || branches_count(rna_tree::parent(it)) == 1))
        {   // => predok je uz nakresleny...
            auto plabel = rna_tree::parent(it)->get_label();
            Point vec = normalize(orthogonal(plabel.lbl(0).point - plabel.lbl(1).point, p1));
            p1 = plabel.lbl(0).point + vec * BASES_DISTANCE;
            p2 = plabel.lbl(1).point + vec * BASES_DISTANCE;
        }
        else
        {   // predok nieje nakresleny... 
            WARN("make_pairs() -> parent is root / iter is not only child, init by centre of p1,p2..");
            Point p = centre(p1, p2);
            p1 = p + (normalize(p1 - p2) * (PAIRS_DISTANCE / 2));
            p2 = p + (normalize(p2 - p1) * (PAIRS_DISTANCE / 2));
        }
    }
}

void compact::init_points(iterator it)
{
    // TODO: hladat inicializovane vrcholy, predkov/potomkov... nie len parent(parent(iter))
    auto& label = it->get_label();
    iterator par = rna_tree::parent(it);
    auto plabel = par->get_label();
    Point vec = normalize(plabel.get_centre() - rna_tree::parent(par)->get_label().get_centre());

    assert(is(it, rna_pair_label::inserted));
    assert(label.get_centre().bad());

    if (label.is_paired())
    {
        label.lbl(0).point = plabel.lbl(0).point + vec;
        label.lbl(1).point = plabel.lbl(1).point + vec;
    }
    else
        label.lbl(0).point = plabel.lbl(0).point + vec;
};

void compact::init()
{
    APP_DEBUG_FNAME;

    for (iterator it = ++doc.rna.begin(); it != doc.rna.end(); ++it)
    {
        auto& label = it->get_label();

        if (!label.inited_points())
        {
            init_points(it);
        }
        if (label.is_paired())
        {
            normalize_pair_distance(it);
        }
    }
}


void compact::reinsert(interval in, circle& c)
{
    auto points = c.split(in.vec.size());

    for (size_t i = 0; i < points.size(); ++i)
    {
        auto it = in.vec.at(i);
        auto& label = it->get_label();

        assert(!label.is_paired());
        label.status = rna_pair_label::reinserted;
        label.set_points_exact(points.at(i), 0);
    }
};

void compact::shift_nodes(iterator it, Point vector)
{
    APP_DEBUG_FNAME;

    cout << "posunutie o " << vector << endl;

    auto& label = it->get_label();
    for (size_t i = 0; i < label.size(); ++i)
        label.lbl(i).point = label.lbl(i).point + vector;

    for (sibling_iterator sib = it.begin(); sib != it.end(); ++sib)
        shift_nodes(sib, vector);
}


void compact::make_inserted()
{
    APP_DEBUG_FNAME;

    iterator it = doc.rna.begin();

    while (it != doc.rna.end())
    {
        if (has_child(it, rna_pair_label::inserted))
        {
            auto i = interval::create(it);
            make(i);
        }

        ++it;
    }
}

void compact::make(vector<interval> vec)
{
    APP_DEBUG_FNAME;
    circle c;
    size_t n;
    size_t minl, len;
    iterator b, e;

    if (vec.size() > 2)
    {
        ERR("vec.size > 2");
        return;
        abort();
    }
    auto init_c = [&c](interval i)
    {
        //LOGGER_PRIORITY_ON_FUNCTION(WARN);

        c.p1 = i.begin->get_label().lbl(i.b_index).point;
        c.p2 = i.end->get_label().lbl(i.e_index).point;
        c.centre = centre(c.p1, c.p2);
        if (i.begin != i.end)
            c.direction = i.begin->get_label().get_centre();
        else
            c.direction = rna_tree::parent(i.begin)->get_label().get_centre();
        c.compute_sgn();
        c.init(i.vec.size());
    };

#define update psout.seek(psout.save(doc.rna)); wait_for_input()
    if (vec.size() == 2)
    {
        for (auto i : vec)
        {
            if (i.vec.empty())
                continue;

            i.print();
            n = i.vec.size();
            minl = circle::min_circle_length(n);

            init_c(i);
            len = c.segment_length();

            b = i.begin;
            e = i.end;

            if (i.e_index == 1)
                swap(b, e);

            cout << *b << " " << *e << endl;

            if (minl < len && len < minl + n)
                normalize_branch_distance(b, e, n);
        }
    }
    for (auto i : vec)
    {
        init_c(i);
        n = i.vec.size();
        reinsert(i, c);
        cout << n << endl;
        //print(c);
        //wait_for_input();
        update;
    }
}

void compact::normalize_branch_distance(iterator parent, iterator child, size_t n)
{
    APP_DEBUG_FNAME;

    Point v;
    double dist_final, dist_actual;

    v = parent->get_label().get_centre() - child->get_label().get_centre();
    dist_actual = size(v);
    dist_final = (circle::min_circle_radius(n) + circle::max_circle_radius(n)) / 2;
    DEBUG("final %f, actual %f", dist_final, dist_actual);
    v = normalize(v) * (dist_final - dist_actual);

    shift_nodes(child, v);

    psout.print_pair(child);
    //abort();
}





