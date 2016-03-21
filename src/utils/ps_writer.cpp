/*
 * File: utils_ps_writer.cpp
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

#include <iomanip>
#include "ps_writer.hpp"

#define PS_COLUMNS_WIDTH    15
#define LETTER              {612, 792}
#define PS_END_STRING       "showpage\n"
#define PS_END_LENGTH       (sizeof(PS_END_STRING) - 1)

using namespace std;

/* virtual */ streampos ps_writer::print(
                const std::string& text)
{
    streampos pos = get_pos();

    fill();

    out
        << text
        << PS_END_STRING;

    seek(-PS_END_LENGTH);

    validate_stream();

    return pos;
}

/* virtual */ std::string ps_writer::get_circle_formatted(
                point centre,
                double radius) const
{
    ostringstream out;

    out
        << "360 0 "
        << radius
        << " 1.0 1.0 "
        << centre
        << " lwarc"
        << endl;

    return out.str();
}

/* virtual */ std::string ps_writer::get_edge_formatted(
                point from,
                point to,
                bool is_base_pair) const
{
    ostringstream out;

    if (from.bad() || to.bad())
    {
        WARN("cannot draw line between bad points");
        return "";
    }
    if (is_base_pair)
    {
        point tmp = base_pair_edge_point(from, to);
        to = base_pair_edge_point(to, from);
        from = tmp;
    }

    for (double coordinates : {from.x, from.y, to.x, to.y})
    {
        out
            << std::left
            << std::setw(PS_COLUMNS_WIDTH)
            << coordinates;
    }
    out
        << " lwline"
        << endl;

    return out.str();
}

/* virtual */ std::string ps_writer::get_text_formatted(
                point p,
                const std::string& text) const
{
    ostringstream out;

    out
        << std::left
        << std::setw(PS_COLUMNS_WIDTH)
        << ("(" + text + ")")   // full text should have aligned width
        << " "
        << std::setw(PS_COLUMNS_WIDTH)
        << p.x
        << std::setw(PS_COLUMNS_WIDTH)
        << p.y
        << std::setw(PS_COLUMNS_WIDTH)
        << " lwstring"
        << endl;

    return out.str();
}

/* virtual */ std::string ps_writer::get_label_formatted(
                const rna_label& label,
                const RGB& color) const
{
    ostringstream out;

    if (color == RGB::BLACK)
    {
        out
            << get_text_formatted(label.p, label.label);
    }
    else
    {
        out
            << get_color_formatted(color)
            << get_text_formatted(label.p, label.label)
            << get_color_formatted(RGB::BLACK);
    }

    return out.str();
}

std::string ps_writer::get_color_formatted(
                const RGB& color) const
{
    ostringstream out;

    for (double fragment : {color.get_red(), color.get_green(), color.get_blue()})
    {
        out
            << std::left
            << std::setw(PS_COLUMNS_WIDTH)
            << fragment;
    }
    out
        << " setrgbcolor"
        << endl;

    return out.str();
}


void ps_writer::init_default(
                const std::string& filename,
                rna_tree::iterator root)
{
    APP_DEBUG_FNAME;

    init(filename);

    print(get_default_prologue(root));
}

std::string ps_writer::get_default_prologue(
                rna_tree::pre_post_order_iterator root) const
{
    APP_DEBUG_FNAME;

    ostringstream str;
    point tr, bl, letter, scale;

    scale = {0.54, 0.54};
    letter = LETTER;

    letter.x /= scale.x;
    letter.y /= scale.y;

    tr = rna_tree::top_right_corner(root);
    bl = rna_tree::bottom_left_corner(root);

    if (size(letter) > distance(tr, bl))
        WARN("rna probably wont fit document");

    tr = -tr;
    bl = -bl;

    bl.x += 50;
    bl.y = letter.y + tr.y - 50;

    str
        << get_default_prologue()
        << scale
            << " scale"
            << endl
        << bl
            << " translate"
            << endl;

    return str.str();
}

std::string ps_writer::get_default_prologue() const
{
    return
        "%!\n"
        "/lwline {newpath moveto lineto stroke} def\n"
        "/lwstring {moveto show} def\n"
        "/lwarc {newpath gsave translate scale /rad exch def /ang1 exch def"
            " /ang2 exch def 0.0 0.0 rad ang1 ang2 arc stroke grestore} def\n"
        "/Helvetica findfont 8.00 scalefont setfont\n"
        ;
}

/* virtual */ void ps_writer::after_text_print()
{
    out
        << PS_END_STRING;
}












#ifdef NODEF




// end of ps document..
#define ps_end_str      "showpage\n"
#define ps_end_length   (sizeof(ps_end_str) - 1)

#define LETTER          {612, 792}

/* virtual */ streampos ps_writer::print(
                const std::string& text,
                bool seek)
{
    streampos pos = get_pos();

    fill();

    out
        << text
        << ps_end_str;

    if (seek)
        out.seekp(-ps_end_length, out.cur);

    if (out.fail())
    {
        ERR("ps print fail");
        exit(1);
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

    if (!it->inited_points())
    {
        //WARN("points not inited, returning");
        return out;
    }
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
            //WARN("UNTOUCHED!!");
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
            abort();
        }

        if (!rna_tree::is_root(it))
            out += sprint_edge(it);
    }

    return out;
}



/* static */ std::string ps_writer::sprint_subtree(
                pre_post_it it)
{
    if (rna_tree::is_leaf(it))
        return sprint_formatted(it);

    assert(!rna_tree::is_leaf(it));

    pre_post_it end;
    string out;
    it  = pre_post_it(it, true);
    end = pre_post_it(it, false);
    ++end;

    while (it != end)
    {
        out += sprint_formatted(it);
        ++it;
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

/* static */ std::string ps_writer::sprint_fromto(
                point from,
                point to)
{
    return sprint_edge(from, to, false) + sprint(from, "From") + sprint(to, "To");
}


/* static */ std::string ps_writer::default_prologue()
{
    APP_DEBUG_FNAME;

    return
        "%!\n"
        "/lwline {newpath moveto lineto stroke} def\n"
        "/lwstring {moveto show} def\n"
        "/lwarc {newpath gsave translate scale /rad exch def /ang1 exch def"
            " /ang2 exch def 0.0 0.0 rad ang1 ang2 arc stroke grestore} def\n"
        "/Helvetica findfont 8.00 scalefont setfont\n"
        ;
}

void ps_writer::init_default(
                const std::string& filename,
                rna_tree::iterator root)
{
    APP_DEBUG_FNAME;

    init(filename);

    print(default_prologue(root));
}

streampos ps_writer::print(
                rna_tree& rna)
{
    APP_DEBUG_FNAME;

    streampos pos = get_pos();
    point p;

    p.x = rna_tree::top_right_corner(rna.begin()).x - 100;
    p.y = rna_tree::bottom_left_corner(rna.begin()).y + 100;

    print(default_prologue(rna.begin()));
    print(sprint(p, rna.name()));

    for (pre_post_it it = rna.begin_pre_post(); it != rna.end_pre_post(); ++it)
        print(sprint_formatted(it));

    return pos;
}




#endif
