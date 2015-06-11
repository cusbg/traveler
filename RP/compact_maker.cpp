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

#define PAIRS_DISTANCE      8

typedef compact::circle circle;

void f();
void print(circle c);


struct compact::circle
{
    Point rotate(double angle) const;
    std::vector<Point> split(size_t n) const;
    double radius() const;
    bool inited() const;
    double segment_angle() const;
    bool lies_in_segment(Point p) const;
    void compute_sgn();

public:
    Point centre;
    Point p1, p2;
    Point direction;

private:
    /*
     * sgn =  1 ~> v protismere hod. ruciciek
     * sgn = -1 ~> v smere hodinovych ruciciek
     */
    char sgn = 0;
};

Point compact::circle::rotate(double delta) const
{
    APP_DEBUG_FNAME;

    assert(inited());
    assert(double_equals(size(p1 - centre), size(p2 - centre)));

    if (sgn != 1 && sgn != -1)
    {
        ERR("circle::sgn not/bad initialized");
        abort();
    }

    double r = radius();
    double a = sgn * delta + angle(p1 - centre);

    Point p({
            centre.x + r * cos(degrees_to_radians(a)),
            centre.y + r * sin(degrees_to_radians(a))
            });

    DEBUG("centre %s: point %s rotated by %.3g -> %s",
            centre.to_string().c_str(), p1.to_string().c_str(),
            delta, p.to_string().c_str());

    return p;
}

std::vector<Point> compact::circle::split(size_t n) const
{
    vector<Point> vec;
    assert(inited());

    double delta = segment_angle() / (double)(n + 1);
    for (size_t i = 1; i <= n; ++i)
        vec.push_back(rotate(delta * i));

    return vec;
}

bool compact::circle::inited() const
{
    return (!centre.bad() || !p1.bad() || !p2.bad() || !direction.bad());
}

double compact::circle::radius() const
{
    assert(inited());
    assert(double_equals(distance(p1, centre), distance(p2, centre)));

    return distance(p1, centre);
}

double compact::circle::segment_angle() const
{
    assert (sgn == 1 || sgn == -1);
    assert(inited());

    if (sgn == 1)
        return angle(p1, centre, p2);
    else
        return angle(p2, centre, p1);
}

bool compact::circle::lies_in_segment(Point p) const
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    assert(inited());
    bool b1, b2;
    b1 = double_equals(
            fmod(angle(p1, centre, p) + angle(p, centre, p2) + 360, 360),
            segment_angle());
    b2 = double_equals(
            fmod(angle(p2, centre, p) + angle(p, centre, p1) + 360, 360),
            segment_angle());

    INFO("p1cp=%f, pcp2=%f, pcp1=%f, p2cp=%f, sa=%f",
            angle(p1, centre, p),
            angle(p, centre, p2),
            angle(p, centre, p1),
            angle(p2, centre, p),
            segment_angle());
    //cout << angle(p1, centre, p) << endl;
    //cout << (angle(p1, centre, p) + angle(p, centre, p2)) << endl;
    //cout << segment_angle() << endl;
    cout << b1 << b2 << endl;
    assert(b1 || b2);

    return b1;
}

void compact::circle::compute_sgn()
{
    APP_DEBUG_FNAME;

    assert(inited());
    
    if (angle(p1, centre, direction) < angle(p1, centre, p2))
        sgn = -1;
    else
        sgn = 1;

    DEBUG("sgn=%i", (int)sgn);
#define cstr(x) (x).to_string().c_str()
    DEBUG("circle: p1=%s, p2=%s, centre=%s, direction=%s",
            cstr(p1), cstr(p2), cstr(centre), cstr(direction));
}






void compact::make_compact()
{
    APP_DEBUG_FNAME;

    make_pairs();
    make_inserted();
}

void compact::make_pairs()
{
    APP_DEBUG_FNAME;

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
    const auto& plbl  = p->get_label().labels;
    const auto& pplbl = pp->get_label().labels;
    Point vec = normalize(plbl.at(0).point - pplbl.at(0).point);

    if (label.is_paired())
    {
        label.labels.at(0).point = plbl.at(0).point + vec * PAIRS_DISTANCE;
        label.labels.at(1).point = plbl.at(1).point + vec * PAIRS_DISTANCE;
    }
    else
        label.labels.at(0).point = centre(plbl.at(0).point, plbl.at(1).point) + vec * PAIRS_DISTANCE;
}

bool compact::is_normalized_dist(iterator it)
{
    const auto& lbl = it->get_label().labels;
    cout << distance(lbl.at(0).point, lbl.at(1).point) << endl;
    return double_equals_precision(distance(lbl.at(0).point, lbl.at(1).point), 20, 0.1);
}



void compact::shift_nodes(iterator it, Point vector)
{
    APP_DEBUG_FNAME;

    cout << "posunutie o " << vector << endl;

    for (auto& lbl : it->get_label().labels)
        lbl.point = lbl.point + vector;

    for (sibling_iterator sib = it.begin(); sib != it.end(); ++sib)
        shift_nodes(sib, vector);
}



compact::circle compact::make_circle(iterator it)
{
    APP_DEBUG_FNAME;


    auto lbl = it->get_label().labels;
    circle c;
    c.p1 = lbl.at(0).point;
    c.p2 = lbl.at(1).point;
    c.centre = centre(c.p1, c.p2);
    c.direction = (rna_tree::parent(it)->get_label().get_centre());
    c.compute_sgn();

    //print(c);

    return c;
}

void compact::make_inserted()
{
    APP_DEBUG_FNAME;

    iterator it;
    sibling_iterator sib;

    for (it = ++rna.begin(); it != rna.end(); ++it)
    {
        if (count_inserted(it) == 0)
            continue;

        circle c = make_circle(it);

        Point from = c.p1;
        auto points = c.split(bases_count(it));
        for (auto p : points)
        {
            psout.print_to_ps(ps::print_line(from, p));
            from = p;
            wait_for_input();
        }

        if (count_branches(it) == 1)
        {}
        else
        {}
    }
    

    psout.save(rna);


    abort();
}


size_t compact::bases_count(iterator it)
{
    APP_DEBUG_FNAME;

    cout << *it << "(";

    size_t count = 0;
    sibling_iterator sib = it.begin();
    while (sib != it.end())
    {
        if (sib->get_label().is_paired())
            count += 2;
        else
            ++count;
        cout << *sib << ",";
        ++sib;
    }
    cout << ")" << endl;
    DEBUG("%lu", count);

    return count;
}






void print(circle c)
{
    APP_DEBUG_FNAME;

    psout.print_to_ps(ps::print(red));
    psout.print_to_ps(ps::print_line(c.centre, c.direction));
    psout.print_to_ps(ps::print(c.direction, "d"));
    psout.print_to_ps(ps::print(c.p1, "p1"));
    psout.print_to_ps(ps::print(c.p2, "p2"));

    psout.print_to_ps(ps::print(blue));
    psout.print_to_ps(ps::print_line(c.centre, c.p1));
    psout.print_to_ps(ps::print_line(c.centre, c.p2));
    psout.print_to_ps(ps::print(black));

    int angle;
    angle = 10;

    for (int i = 0; i <= c.segment_angle(); i+=angle)
    {
        wait_for_input();

        Point p = c.rotate(i);
        psout.print_to_ps(ps::print_line(c.centre, p));

    }
}


void f()
{
    APP_DEBUG_FNAME;

    circle c;
    c.centre    = {238, -497};
    c.p1        = {228, -497};
    c.p2        = {248, -497};
    c.direction = {238, -489};

    c.compute_sgn();

    DEBUG("r=%f, seg_angle=%f",
            c.radius(),
            c.segment_angle());
    DEBUG("lies=%s",
            to_string(c.lies_in_segment(c.direction)).c_str());

    //f(c);

    //abort();
}














