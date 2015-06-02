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

ps& ps::operator=(ps&& other)
{
    other.out.close();

    filename = move(other.filename);
    out.open(filename, std::ios::app);
    out
        << std::unitbuf
        << std::setprecision(4)
        << std::scientific;

    assert(out.good());

    return *this;
}

ps::ps(const std::string& _filename)
    : filename(_filename), out(filename)
{
    out << std::unitbuf;
    assert(out.good());
}

/* static */ ps&& ps::init(const std::string& filename)
{
    APP_DEBUG_FNAME;
    assert(!filename.empty());

    static ps p;
    p = ps(filename);

    return std::move(p);
}




void ps::print_to_ps(const std::string& line)
{
    out << line;
    if (out.fail())
    {
        ERR("ps print fail");
        abort();
    }
}

/* static */ std::string ps::print(const rna_label& label)
{
    stringstream out;
    out
        << std::left
        << std::setw(PS_COLUMNS_WIDTH)
        << ("(" + label.label + ")")
        << std::setw(PS_COLUMNS_WIDTH)
        << label.point.x
        << std::setw(PS_COLUMNS_WIDTH)
        << label.point.y
        << std::setw(PS_COLUMNS_WIDTH)
        << "lwstring"
        << endl;
    return out.str();
}

/* static */ std::string ps::print(RGB color)
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
            rgb = {0.33, 0.33, 0.33};
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

/* static */ std::string ps::print_normal(const pre_post_it& iter, bool colored)
{
    string out;

    if (colored)
        out = print_colored(iter, other);
    else
    {
        size_t index = get_label_index(iter);
        auto label = iter->get_label().labels.at(index);
        out = print(label);
    }

    return out;
}

/* static */ std::string ps::print_colored(const pre_post_it& iter, RGB rgb)
{
    string out;

    size_t index = get_label_index(iter);
    auto label = iter->get_label().labels.at(index);

    out = ""
        + print(rgb)
        + print(label)
        + print(black);

    return out;
}

/* static */ std::string ps::print_edge(const pre_post_it& iter)
{
    stringstream out;

    if (iter.is_preorder() || !iter->get_label().is_paired())
        return out.str();

    Point p1, p2;
    p1 = iter->get_label().labels.at(0).point;
    p2 = iter->get_label().labels.at(1).point;
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

/* static */ std::string ps::format_string(const pre_post_it& it)
{
#define CHANGED_STR     "********************** CHANGED **********************"
#define DELETED_STR     "********************** DELETED **********************"
#define INSERTED_STR    "********************* INSERTED **********************"
#define UNTOUCHED_STR   "********************* UNTOUCHED *********************"

#define DELETE_COLOR    blue
#define INSERT_COLOR    red
#define EDITED_COLOR    green
#define OTHER_COLOR     other

    string out;

    auto status = it->get_label().status;

/*
    if (status == rna_pair_label::untouched)
    {
        ERR("untouched");
        abort();
    }
*/

    switch (status)
    {
        case rna_pair_label::deleted:
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
            //DEBUG("inserted %s",
                    //rna_tree::print_subtree(rna_tree::parent(it)).c_str());
            out = print_colored(it, INSERT_COLOR);
            break;

        case rna_pair_label::untouched:
            WARN("UNTOUCHED!!");
            wait_for_input();
            out = print_normal(it);
        default:
            WARN("default status!!!");
            out = print_normal(it);
            break;
    }

    bool edge = false;
    edge = true;
    if (edge)
        out += print_edge(it);

    return out;
};



