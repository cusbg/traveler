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


using namespace std;

#define is_inserted(iter) \
    (iter->get_label().status == rna_pair_label::inserted)
#define count_inserted(iter) \
    count_if(iter, [](sibling_iterator sib) { return is_inserted(sib); })
#define count_branches(iter) \
    count_if(iter, [](sibling_iterator sib) { return sib->get_label().is_paired(); })

#define is(_iter, _status) \
    (_iter->get_label().status == rna_pair_label::_status)


void f();






void compact::make_compact()
{
    APP_DEBUG_FNAME;

    make_pairs();
    make_inserted();
    make_deleted();

    psout.save(doc.rna_out);
}

void compact::make_pairs()
{
    APP_DEBUG_FNAME;

    auto& rna = doc.rna_out;
    for (iterator it = rna.begin(); it != rna.end(); ++it)
    {
        if (rna_tree::is_only_child(it) && it->get_label().is_paired() && !is_normalized_dist(it))
            normalize_distance(it);
    }
}

void compact::normalize_distance(iterator it)
{
    APP_DEBUG_FNAME;
    iterator p = rna_tree::parent(it);
    iterator pp = rna_tree::parent(p);

    auto& label = it->get_label();
    const auto& plabel  = p->get_label();
    const auto& pplabel = pp->get_label();
    Point vec = normalize(plabel.lbl(0).point - pplabel.lbl(0).point) * BASES_DISTANCE;

    if (label.is_paired())
    {
        label.lbl(0).point = plabel.lbl(0).point + vec;
        label.lbl(1).point = plabel.lbl(1).point + vec;
    }
    else
        label.lbl(0).point = centre(plabel.lbl(0).point, plabel.lbl(1).point) + vec;
}

bool compact::is_normalized_dist(iterator it)
{
    const auto& label = it->get_label();
    return double_equals_precision(distance(label.lbl(0).point,
                label.lbl(1).point), PAIRS_DISTANCE, 0.1);
}

size_t compact::bases_count(iterator from, iterator to)
{
    APP_DEBUG_FNAME;

    size_t n = 0;
    while (from != to)
    {
        if (from->get_label().is_paired())
            n += 2;
        else
            ++n;
        ++from;
    }
    return n;
}


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









compact::circle compact::make_circle(iterator it)
{
    APP_DEBUG_FNAME;

    const auto& label = it->get_label();
    circle c;
    c.p1 = label.lbl(0).point;
    c.p2 = label.lbl(1).point;
    c.direction = rna_tree::parent(it)->get_label().get_centre();

    return c;
}

void compact::make_inserted()
{
    APP_DEBUG_FNAME;

    iterator it;
    sibling_iterator sib;

    auto& rna = doc.rna_out;
    for (it = ++rna.begin(); it != rna.end(); ++it)
    {
        if (count_inserted(it) == 0)
            continue;
        else
            rebase(it);
    }
}

std::vector<compact::sibling_iterator> compact::get_branches(iterator it)
{
    vector<sibling_iterator> vec;
    sibling_iterator sib = it.begin();

    while (sib != it.end())
    {
        if (sib->get_label().is_paired())
            vec.push_back(sib);
        ++sib;
    }
    return vec;
}

void compact::rebase(iterator it)
{
    APP_DEBUG_FNAME;

    auto vec = get_branches(it);
    Point prev = it->get_label().lbl(0).point;
    sibling_iterator beg = it.begin();
    circle c;
    Point dir = rna_tree::parent(it)->get_label().get_centre();

    auto run = [this](sibling_iterator beg, sibling_iterator end, circle c)
    {
        LOGGER_PRIORITY_ON_FUNCTION(INFO);

        size_t index = 0;
        size_t n = bases_count(beg, end);

        c.init(n);
        cout << c << endl;
        auto points = c.split(n);

        while (beg != end)
        {
            if (beg->get_label().is_paired())
            {
                ERR("is_paired");
                abort();
            }

            reinsert(beg, 0, points.at(index));
            ++index;
            ++beg;
        }
        //print(c);
        //wait_for_input();
    };

    for (size_t i = 0; i < vec.size(); ++i)
    {
        const auto& val = vec.at(i);
        const auto& label = val->get_label();

        if (label.get_centre().bad())
        {
            ERR("not initialized child point");
            abort();
        }

        c = circle();
        c.p1 = prev;
        c.p2 = label.lbl(0).point;
        c.direction = dir;

        run(beg, val, c);

        beg = val;
        prev = beg->get_label().lbl(1).point;
        ++beg;
    }

    c.p1 = prev;
    c.p2 = it->get_label().lbl(1).point;
    c.direction = rna_tree::parent(it)->get_label().get_centre();

    run(beg, it.end(), c);
}

void compact::reinsert(iterator it, size_t index, Point p)
{
    APP_DEBUG_FNAME;

    auto& label = it->get_label();

    label.lbl(index).point = p;
    if (label.status != rna_pair_label::inserted)
        label.status = rna_pair_label::reinserted;
}



void compact::make_deleted()
{
    APP_DEBUG_FNAME;

    iterator it1, it2;
    sibling_iterator sib1, sib2;

    it1 = doc.rna_out.begin();
    it2 = doc.template_rna.begin();

    while (it1 != doc.rna_out.end())
    {
        if (is(it1, inserted))
        {
            ++it1;
            continue;
        }
        if (is(it2, deleted))
        {
                    psout.print_to_ps(ps::print(blue));
                    psout.print_to_ps(ps::print(it2->get_label().get_centre(), "x"));
                    psout.print_to_ps(ps::print(black));
            ++it2;
            continue;
        }

        size_t del = count_if(it2, [](sibling_iterator ch) { 
                if (is(ch, deleted))
                {
                    psout.print_to_ps(ps::print(blue));
                    psout.print_to_ps(ps::print(ch->get_label().get_centre(), "x"));
                    psout.print_to_ps(ps::print(black));
                }
                return is(ch, deleted); });
        if (del != 0)
        {
            DEBUG("DELETED");
            //cout << *it2 << endl;
            //rebase(it1);
        }
        ++it1;
        ++it2;
    }
}
















