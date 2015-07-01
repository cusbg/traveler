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

#define UPDATE_PS psout.seek(psout.save(doc.rna)); wait_for_input()


void compact::make_compact()
{
    APP_DEBUG_FNAME;

    UPDATE_PS;

    init();

    psout.seek(psout.save(doc.rna));
    //wait_for_input();

    make_inserted();
    make_deleted();
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


void compact::reinsert(vector<sibling_iterator> nodes, const circle& c)
{
    auto points = c.split(nodes.size());

    for (size_t i = 0; i < points.size(); ++i)
    {
        auto it = nodes.at(i);
        auto& label = it->get_label();

        assert(!label.is_paired());
        if (label.status == rna_pair_label::touched)
            label.status = rna_pair_label::reinserted;
        label.set_points_exact(points.at(i), 0);
    }
};

void compact::shift_nodes(iterator it, Point vector)
{
    APP_DEBUG_FNAME;

    function<size_t(iterator, Point)> recursion = [&recursion](iterator iter, Point vec)
    {
        size_t out = 1;
        auto& label = iter->get_label();
        for (size_t i = 0; i < label.size(); ++i)
            label.lbl(i).point = label.lbl(i).point + vec;
        for (sibling_iterator sib = iter.begin(); sib != iter.end(); ++sib)
            out += recursion(sib, vec);
        return out;
    };

    size_t n = recursion(it, vector);
    DEBUG("posunutie o %s, #vrcholov=%lu", vector.to_string().c_str(), n);
}



compact::circle compact::create(const interval& in)
{
    circle c;

    c.p1 = in.begin->get_label().lbl(in.b_index).point;
    c.p2 = in.end->get_label().lbl(in.e_index).point;
    c.centre = centre(c.p1, c.p2);
    if (in.begin != in.end) // hairpin
        c.direction = in.begin->get_label().get_centre();
    else
        c.direction = rna_tree::parent(in.begin)->get_label().get_centre();
    c.compute_sgn();

    return c;
}

void compact::remake(const interval& in, bool is_hairpin)
{
    APP_DEBUG_FNAME;

#define MAX_NODES_MOVE 10

    if (is_hairpin || in.vec.size() < MAX_NODES_MOVE)
    {
        circle c = create(in);
        c.init(in.vec.size());
        reinsert(in.vec, c);
    }
    else
    {
        WARN("skipping, #nodes = %lu > max = %lu",
                in.vec.size(), MAX_NODES_MOVE);
    }
}

void compact::set_distance(iterator parent, iterator child, double dist)
{
    APP_DEBUG_FNAME;

    Point p1, p2, vec, shift;
    double actual;

    p1 = parent->get_label().get_centre();
    p2 = child->get_label().get_centre();
    vec = normalize(p2 - p1);
    actual = distance(p1, p2);
    shift = vec * (dist - actual);

    shift_nodes(child, shift);
}



void compact::make_inserted()
{
    APP_DEBUG_FNAME;

    iterator it = doc.rna.begin();

    while (it != doc.rna.end())
    {
        if (has_child(it, rna_pair_label::inserted))
        {
            auto intervals = interval::create(it);
            switch(intervals.size())
            {
                case 1: // hairpin
                    remake(intervals[0], true);
                    break;
                case 2: // interial loop/bulge
                    remake_interial_loops(intervals);
                default:
                    break;
            }
            //UPDATE_PS;
        }
        ++it;
    }

    UPDATE_PS;
}

void compact::remake_interial_loops(const std::vector<interval> vec)
{
    APP_DEBUG_FNAME;

    assert(vec.size() == 2);
    assert(vec[0].begin == vec[1].end && vec[1].begin == vec[0].end);

    vector<double> def_len(2, 0xDEADBEEF);
    double avg, actual;
    sibling_iterator parent, child;

    parent = vec[0].begin;
    child =  vec[0].end;
    actual = distance(parent->get_label().get_centre(), child->get_label().get_centre());

    for (size_t i = 0; i < 2; ++i)
        def_len[i] = circle::min_circle_length(vec[i].vec.size());
    LOGGER_PRINT_CONTAINER(def_len, "lengths");

    sort(def_len.begin(), def_len.end());

    avg = def_len[0] + sqrt(def_len[1] - def_len[0]);
    if (avg < BASES_DISTANCE)
        avg = BASES_DISTANCE * 1.25;

    DEBUG("actual %f, avg %f", actual, avg);

    if (!double_equals_precision(actual, avg, 1))
        set_distance(parent, child, avg);

    remake(vec[0]);
    remake(vec[1]);
}


void compact::make_deleted()
{
    APP_DEBUG_FNAME;

    iterator it = doc.rna.begin();

    auto is_untouched_del = [](iterator iter)
    {
        return is(iter, rna_pair_label::deleted) &&
            iter->get_label().todo == rna_pair_label::undefined;
    };

    while (it != doc.rna.end())
    {
        if (count_children_if(it, is_untouched_del) != 0)
        {
            auto intervals = interval::create(it);
            switch(intervals.size())
            {
                case 1: // hairpin
                    remake(intervals[0], true);
                    break;
                case 2: // interial loop/bulge
                    remake_interial_loops(intervals);
                default:
                    remake_multibranch_loops(intervals);
                    break;
            }

            //UPDATE_PS;
        }
        ++it;
    }
    UPDATE_PS;
}

void compact::remake_multibranch_loops(const std::vector<interval> vec)
{
    APP_DEBUG_FNAME;

    for (auto in : vec)
    {
        if (!in.has_del)
            continue;

        remake(in);
    }
}






