/*
 * File: write_ps_document.cpp
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


#include <iomanip>

#include "write_ps_document.hpp"


#define PS_COLUMNS_WIDTH 15


using namespace std;

/* global */
ps_writer psout;


/* virtual */ streampos ps_writer::print(
                const std::string& text)
{
    streampos pos = get_pos();

    fill();

#define ps_end_str      "showpage\n"
#define ps_end_length   (sizeof(ps_end_str) - 1)

    out
        << text
        << ps_end_str;

    out.seekp(-ps_end_length, out.cur);

    if (out.fail())
    {
        ERR("ps print fail");
        abort();
    }

    return pos;
}


/* static */ std::string ps_writer::sprint(
                const point p,
                const std::string& text)
{
    stringstream out;
    out
        << std::left
        << std::setw(PS_COLUMNS_WIDTH)
        << ("(" + text + ")")
        << std::setw(PS_COLUMNS_WIDTH)
        << p.x
        << std::setw(PS_COLUMNS_WIDTH)
        << p.y
        << std::setw(PS_COLUMNS_WIDTH)
        << "lwstring"
        << endl;

    return out.str();
}

/* static */ std::string ps_writer::sprint(
                const rna_label& label)
{
    return sprint(label.p, label.label);
}

/* static */ std::string ps_writer::sprint(
                RGB color)
{
    struct
    {
        double r, g, b;
    } rgb;

    stringstream out;

    switch (color)
    {
        case red:
            rgb = {1, 0, 0};
            break;
        case green:
            rgb = {0, 1, 0};
            break;
        case blue:
            rgb = {0, 0, 1};
            break;
        case black:
            rgb = {0, 0, 0};
            break;
        case gray:
            rgb = {0.8, 0.8, 0.8};
            break;


        case other:
            rgb = {0, 0.5, 0.5};
            break;
        default:
            ERR("no default value for rgb");
            abort();
    }

    out
        << std::left
        << std::setw(PS_COLUMNS_WIDTH)
        << rgb.r
        << std::setw(PS_COLUMNS_WIDTH)
        << rgb.g
        << std::setw(PS_COLUMNS_WIDTH)
        << rgb.b
        << std::setw(PS_COLUMNS_WIDTH)
        << "setrgbcolor"
        << endl;

    return out.str();
}

/* static */ std::string ps_writer::sprint_edge(
                point p1,
                point p2,
                bool base_pair)
{
    stringstream out;

    if (p1.bad() || p2.bad())
    {
        ERR("bad_point, cannot draw line");
        //abort();
        return "";
    }

    if (base_pair)
    {
        point pa;
        pa = base_pair_edge_point(p1, p2);
        p2 = base_pair_edge_point(p2, p1);
        p1 = pa;
    }

    out
        << std::left
        << std::setw(PS_COLUMNS_WIDTH)
        << p1.x
        << std::setw(PS_COLUMNS_WIDTH)
        << p1.y
        << std::setw(PS_COLUMNS_WIDTH)
        << p2.x
        << std::setw(PS_COLUMNS_WIDTH)
        << p2.y
        << " lwline"
        << endl;

    return out.str();
}

/* static */ std::string ps_writer::sprint_edge(
                const pre_post_it& iter)
{
    stringstream out;

    if (iter.preorder() || !iter->paired())
        return out.str();

    point p1, p2;

    p1 = (*iter)[0].p;
    p2 = (*iter)[1].p;

    return sprint_edge(p1, p2);
}

/* static */ std::string ps_writer::sprint(
                rna_tree rna)
{
    APP_DEBUG_FNAME;

    string s;

    for (auto it = ++rna.begin_pre_post();
            ++pre_post_it(it) != rna.end_pre_post(); ++it)
        s += sprint_formatted(it);

    return s;
}

/* static */ std::string ps_writer::sprint(
                pre_post_it it,
                RGB color)
{
    string out = ""
        + sprint(color)
        + sprint(it)
        + sprint(black);

    return out;
}

/* static */ std::string ps_writer::sprint(
                pre_post_it it)
{
    string out = sprint((*it)[it.label_index()]);
    return out;
}

/* static */ std::string ps_writer::sprint_formatted(
                pre_post_it it)
{
#define DELETE_COLOR    gray
#define INSERT_COLOR    red
#define REINSERT_COLOR  blue
#define EDITED_COLOR    green

    string out;

    auto status = it->status;

    switch (status)
    {
        case rna_pair_label::deleted:
            out = sprint(it, DELETE_COLOR);
            break;
        case rna_pair_label::edited:
            out = sprint(it, EDITED_COLOR);
            break;
        case rna_pair_label::inserted:
            out = sprint(it, INSERT_COLOR);
            break;
        case rna_pair_label::reinserted:
            out = sprint(it, REINSERT_COLOR);
            break;
        case rna_pair_label::touched:
            out = sprint(it);
            break;

        case rna_pair_label::untouched:
            WARN("UNTOUCHED!!");
            out = sprint(it);
            break;
        default:
            WARN("default print status!!!");
            abort();
            break;
    }

    bool edge = false;
    edge = true;
    if (edge)
    {
        if (it->paired() &&
                it->inited_points() &&
                double_equals(distance(it->at(0).p, it->at(1).p), 0))
        {
            ERR("ERR");
            cout << label(it) << (*it)[0].p << ";" << (*it)[1].p << endl;
            rna_tree::print_subtree(it);
            psout.print(
                    ps_writer::sprint(other) +
                    ps_writer::sprint((*it)[0]) +
                    ps_writer::sprint((*it)[1]));
            abort();
        }

        out += sprint_edge(it);
    }

    return out;
}



/* static */ std::string ps_writer::sprint_subtree(
                pre_post_it it)
{
    pre_post_it beg, end;
    string out;
    beg = pre_post_it(it, true);
    end = pre_post_it(it, false);

    assert(!rna_tree::is_leaf(it));

    while (beg != end)
    {
        out += psout.sprint(beg);
        ++beg;
    }
    return out;
}


/* static */ std::string ps_writer::sprint_circle(
                point p,
                double size)
{
    std::stringstream str;
    str
        << "360 0 "
        << size
        << " 1.0 1.0 "
        << p
        << " lwarc"
        << endl;
    return str.str();
}

