/*
 * File: compact_init.cpp
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

#include "compact_init.hpp"
#include "compact_maker_utils.hpp"
#include "ps.hpp"

using namespace std;

void compact::init::init_points(
                intervals in)
{
    if (in.type == intervals::hairpin)
        return;

    APP_DEBUG_FNAME;

    if (in.type == intervals::interior_loop)
    {
        assert(in.vec[0].end == in.vec[1].begin);
        if (in.vec[0].end->get_label().inited_points())
            return;

        iterator it, ch;
        Point direction;

        it = in.vec[0].begin;
        ch = in.vec[0].end;

        direction = get_direction(it);

        ch->get_label().lbl(0).point =
            it->get_label().lbl(0).point + direction;
        ch->get_label().lbl(1).point =
            it->get_label().lbl(1).point + direction;

        assert(ch->get_label().lbl(0).point != ch->get_label().lbl(1).point);
    }
    
    if (in.type == intervals::multibranch_loop)
    {
        auto inited = [](iterator iter)
        {
            return iter->get_label().inited_points();
        };
        auto get_prev = [&inited, &in](size_t n)
        {
            auto& i = in.vec[n];
            auto vecit = find_if(i.vec.crbegin(), i.vec.crend(), inited);

            if (vecit != i.vec.crend())
                return (iterator)*vecit;
            else if (i.begin->get_label().inited_points())
                return (iterator)i.begin;
            else
                abort();
        };
        auto get_next = [&inited, &in](size_t n)
        {
            while (true)
            {
                if (n == in.vec.size())
                {
                    assert(in.vec.back().end == rna_tree::parent(in.vec.back().begin));
                    DEBUG("LAST!!!");

                    return (iterator)in.vec.back().end;
                }
                auto& i = in.vec.at(n+1);
                auto vecit = find_if(i.vec.cbegin(), i.vec.cend(), inited);

                if (vecit != i.vec.cend())
                    return (iterator)*vecit;
                else if (i.end->get_label().inited_points())
                    return (iterator) i.end;
                else
                    ++n;
            }
        };
        auto init_by_child = [&inited] (intervals::interval i)
        {
            if (
                    !i.end->get_label().inited_points() &&
                    i.end.number_of_children() == 1)
            {
                DEBUG("A");
                auto ch = rna_tree::first_child(i.end);
                if (ch->get_label().is_paired() &&
                        inited(ch))
                {
                    DEBUG("child init");
                    Point p, p1, p2;
                    i.end->get_label().set_points_exact(ch->get_label());

                    p1 = i.end->get_label().lbl(0).point;
                    p2 = i.end->get_label().lbl(1).point;

                    p = orthogonal(p1 - p2, get_direction(i.end) - p2);
                    ch->get_label().lbl(0).point += p;
                    ch->get_label().lbl(1).point += p;
                    return true;
                }
            }
            DEBUG("no child init");
            return false;
        };
        for (size_t index = 0; index < in.vec.size(); ++index)
        {
            auto& i = in.vec[index];
            assert(i.begin->get_label().inited_points());

            if (i.end->get_label().inited_points())
                continue;
            if (init_by_child(i))
                continue;

            iterator prev, next;
            Point p1, p2, p;

            prev = get_prev(index);
            next = get_next(index);

            p1 = prev->get_label().get_centre();
            p2 = next->get_label().get_centre();

            p = centre(p1, p2);

            //psout.print_to_ps(
                    //ps::print(blue) +
                    //ps::print(p1, "*pr") +
                    //ps::print(p2, "*nx") +
                    //ps::print(black)
                    //);

            p1 = move_point(p, p1, PAIRS_DISTANCE / 2);
            p2 = move_point(p, p2, PAIRS_DISTANCE / 2);

            assert(p1 != p2);

            i.end->get_label().lbl(0).point = p1;
            i.end->get_label().lbl(1).point = p2;
        }
    }
}

bool compact::init::init_point_recursive(
                iterator it)
{
    APP_DEBUG_FNAME;

    return false;
}

void compact::init::shift_branch(iterator it, Point vector)
{
    APP_DEBUG_FNAME;

    function<size_t(iterator, Point)> recursion = [&recursion](iterator iter, Point vec)
    {
        size_t out = 1;
        auto& label = iter->get_label();
        for (size_t i = 0; i < label.size(); ++i)
            label.lbl(i).point = label.lbl(i).point + vec;
        for (sibling_iterator sib = iter.begin(); sib != iter.end(); ++sib)
        {
            if (!sib->get_label().inited_points())
                continue;
            out += recursion(sib, vec);
        }
        return out;
    };

    size_t n = recursion(it, vector);
    DEBUG("shift by %s, #nodes=%lu", vector.to_string().c_str(), n);
}

void compact::init::set_distance(iterator parent, iterator child, double dist)
{
    APP_DEBUG_FNAME;

    Point p1, p2, vec, shift;
    double actual;

    p1 = parent->get_label().get_centre();
    p2 = child->get_label().get_centre();
    vec = normalize(p2 - p1);
    actual = distance(p1, p2);
    shift = vec * (dist - actual);

    shift_branch(child, shift);
}

void compact::init::normalize_pair_distance(iterator it)
{
    if (!it->get_label().is_paired())
        return;

    auto& label = it->get_label();
    Point& p1 = label.lbl(0).point;
    Point& p2 = label.lbl(1).point;
    Point dir;

    if (!label.inited_points() ||
            !double_equals_precision(distance(p1, p2),
                PAIRS_DISTANCE, PAIRS_DISTANCE_PRECISION))
    {
        DEBUG("distance=%f, p1=%s, p2=%s", distance(p1, p2), to_cstr(p1), to_cstr(p2));
        wait_for_input();

        dir = get_direction(it) * BASES_DISTANCE;

        auto plabel = rna_tree::parent(it)->get_label();

        label.set_points_exact(plabel.lbl(0).point + dir, 0);
        label.set_points_exact(plabel.lbl(1).point + dir, 1);

        assert(double_equals_precision(distance(p1, p2),
                    PAIRS_DISTANCE, PAIRS_DISTANCE_PRECISION));
    }
}

/* static */ void compact::init::check_distance(
                const intervals& in)
{
    if (in.type == intervals::hairpin)
        return;

    if (in.type == intervals::interior_loop)
        check_interior_loop(in);
    if (in.type == intervals::multibranch_loop)
        check_multibranch_loop(in);
}

void compact::init::check_interior_loop(
                intervals in)
{
    APP_DEBUG_FNAME;

    assert(in.vec.size() == 2);
    assert(in.vec[0].begin == in.vec[1].end &&
            in.vec[1].begin == in.vec[0].end);

    double actual, normal;
    vector<double> lengths(2);
    sibling_iterator par, ch;

    par = in.vec[0].begin;
    ch = in.vec[0].end;

    assert(rna_tree::parent(ch) == par);

    actual = distance(par->get_label().get_centre(), ch->get_label().get_centre());

    for (auto i : {0, 1})
        lengths[i] = circle::min_circle_length(in.vec[i].vec.size());

    LOGGER_PRINT_CONTAINER(lengths, "lengths");

    sort(lengths.begin(), lengths.end());

    normal = lengths[0] + sqrt(lengths[1] - lengths[0]);
    if (normal < BASES_DISTANCE)
        normal = BASES_DISTANCE * BASES_RATIO;

    DEBUG("actual %f, avg %f", actual, normal);

    auto pos = psout.get_pos();

    psout.print_subtree(par);
    psout.print_to_ps(ps::print_line(par->get_label().get_centre(), ch->get_label().get_centre()));
    psout.print_to_ps(ps::print(red) + ps::print(ch->get_label().get_centre(), "*") + ps::print(black));

    wait_for_input();
    if (!double_equals_precision(actual, normal, 1))
    {
        rna_tree::print_subtree(par);
        set_distance(par, ch, normal);
    }
    psout.seek(pos);
    pos = psout.print_subtree(par);
    psout.print_to_ps(ps::print_line(par->get_label().get_centre(), ch->get_label().get_centre()));
    psout.print_to_ps(ps::print(red) + ps::print(ch->get_label().get_centre(), "*") + ps::print(black));
    psout.seek(pos);
    wait_for_input();
}

void compact::init::check_multibranch_loop(
                intervals in)
{
    APP_DEBUG_FNAME;

    ERR("not implemented");
}

