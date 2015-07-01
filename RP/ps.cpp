/*
 * File: ps.cpp
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

#include "ps.hpp"
#include "util.hpp"
#include <iomanip>

using namespace std;

#define PS_COLUMNS_WIDTH 15

/* global */ ps psout;


ps::ps(
                const std::string& _filename)
    : filename(_filename), out(filename, ios::out)
{
    set_io_flags();
    assert(out.good());
}

ps& ps::operator=(
                ps&& other)
{
    other.out.close();

    filename = move(other.filename);
    out.close();
    out.open(filename, ios::out | ios::in);
    set_io_flags();

    assert(out.good());

    return *this;
}

void ps::set_io_flags()
{
    out
        << std::unitbuf
        << std::scientific;
}

/* static */ ps&& ps::init(
                const std::string& filename)
{
    APP_DEBUG_FNAME;
    assert(!filename.empty());

    static ps p;
    p = ps(filename);

    return std::move(p);
}


// PRINT FUNCTIONS:
//

/* static */ std::string ps::format_string(
                const pre_post_it& it)
{
#define CHANGED_STR     "********************** CHANGED **********************"
#define DELETED_STR     "********************** DELETED **********************"
#define INSERTED_STR    "********************* INSERTED **********************"
#define UNTOUCHED_STR   "********************* UNTOUCHED *********************"

#define DELETE_COLOR    gray
#define INSERT_COLOR    red
#define REINSERT_COLOR  blue
#define EDITED_COLOR    green
#define OTHER_COLOR     other

    string out;

    auto status = it->get_label().status;

    switch (status)
    {
        case rna_pair_label::deleted:
            if (it->get_label().todo == rna_pair_label::ignore)
                return "";
            out = print_colored(it, DELETE_COLOR);
            //wait_for_input();
            break;
        case rna_pair_label::edited:
            out = print_colored(it, EDITED_COLOR);
            break;
        case rna_pair_label::touched:
            out = print_normal(it);
            break;

        case rna_pair_label::inserted:
            out = print_colored(it, INSERT_COLOR);
            break;
        case rna_pair_label::reinserted:
            out = print_colored(it, REINSERT_COLOR);
            break;

        case rna_pair_label::untouched:
            WARN("UNTOUCHED!!");
            out = print_normal(it);
            //wait_for_input();
            break;
        default:
            WARN("default status!!!");
            out = print_normal(it);
            wait_for_input();
            break;
    }

    bool edge = false;
    edge = true;
    if (edge)
        out += print_edge(it);

    return out;
};

/* static */ std::string ps::print(
                const Point p,
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

/* static */ std::string ps::print(
                const rna_label& label)
{
    return print(label.point, label.label);
}

/* static */ std::string ps::print(
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

/* static */ std::string ps::print_normal(
                const pre_post_it& iter,
                bool colored)
{
    string out;

    if (colored)
        out = print_colored(iter, other);
    else
    {
        size_t index = get_label_index(iter);
        auto label = iter->get_label().lbl(index);
        out = print(label);
    }

    return out;
}

/* static */ std::string ps::print_colored(
                const pre_post_it& iter,
                RGB rgb)
{
    string out;

    size_t index = get_label_index(iter);
    auto label = iter->get_label().lbl(index);

    out = ""
        + print(rgb)
        + print(label)
        + print(black);

    return out;
}

/* static */ std::string ps::print_edge(
                const pre_post_it& iter)
{
    stringstream out;

    if (iter.is_preorder() || !iter->get_label().is_paired())
        return out.str();

    Point p1, p2;

    p1 = iter->get_label().lbl(0).point;
    p2 = iter->get_label().lbl(1).point;

    return print_line(p1, p2);
}

/* static */ std::string ps::print_line(
                Point p1,
                Point p2)
{
    stringstream out;

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


streampos ps::save(
                const rna_tree& rna)
{
    APP_DEBUG_FNAME;

    streampos pos = get_pos();

    for (auto it = ++rna.begin_pre_post();
            ++pre_post_it(it) != rna.end_pre_post(); ++it)
    {
        psout.print_to_ps(ps::format_string(it));
    }

    return pos;
}


streampos ps::print_to_ps(
                const std::string& line)
{
    streampos pos = get_pos();

    fill();

#define ps_end_str      "showpage\n"
#define ps_end_length   (ARRAY_LENGTH(ps_end_str) - 1)
    out
        << line
        << ps_end_str;

    out.seekp(-ps_end_length, out.cur);

    if (out.fail())
    {
        ERR("ps print fail");
        abort();
    }

    return pos;
}

void ps::seek(
                streampos pos)
{
    out.seekp(pos);

    assert(out.good());
}

void ps::seek_end()
{
    out.seekp(0, out.end);

    assert(out.good());
}

streampos ps::get_pos()
{
    streampos pos = out.tellp();
    assert(pos != -1);

    return pos;
}

streampos ps::print_pair(rna_tree::iterator it)
{
    string out;

    if (it->get_label().is_paired())
        out =
            format_string(rna_tree::pre_post_order_iterator(it, true)) +
            format_string(rna_tree::pre_post_order_iterator(it, false));
    else
        out = ps::format_string(it);

    return psout.print_to_ps(out);
}

size_t ps::fill(char ch)
{
    streampos pos, end;

    pos = get_pos();
    out.seekp(0, out.end);
    end = get_pos();

    size_t n = end - pos;

    if (n != 0)
    {
        seek(pos);
        out
            << string(n - 1, ch)
            << endl;
    }
    seek(pos);
    return n;
}

