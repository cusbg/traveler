/*
 * File: rna_tree_label.cpp
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

#include "rna_tree_label.hpp"

using namespace std;


bool rna_label::operator==(
            const rna_label& other) const
{
    return label == other.label;
}




rna_pair_label::rna_pair_label(
                const std::string& s)
    : labels(1, {s, point()})
{ }

const rna_label& rna_pair_label::operator[](
                size_t index) const
{
    assert(index == 0 || index == 1);

    return labels.at(index);
}

rna_label& rna_pair_label::operator[](
                size_t index)
{
    assert(index == 0 || index == 1);

    return labels.at(index);
}

const rna_label& rna_pair_label::at(
                size_t index) const
{
    return (*this)[index];
}

rna_label& rna_pair_label::at(
                size_t index)
{
    return (*this)[index];
}

bool rna_pair_label::operator==(
                const rna_pair_label& other) const
{
    if (labels.size() != other.labels.size())
        return false;

    for (size_t i = 0; i < labels.size(); ++i)
        if (labels[i] != other.labels[i])
            return false;

    return true;
}

rna_pair_label rna_pair_label::operator+(
                const rna_pair_label& other) const
{
    assert(!paired() && !other.paired());

    rna_pair_label out;
    out.labels.push_back(labels.back());
    out.labels.push_back(other.labels.back());

    return out;
}

std::ostream& operator<<(
                std::ostream& out,
                rna_pair_label lbl)
{
    if (lbl.status == rna_pair_label::inserted)
        out << "__ins`";
    if (lbl.status == rna_pair_label::deleted)
        out << "__del`";
    if (lbl.status == rna_pair_label::reinserted)
        out << "__reins`";

    for (const auto& val : lbl.labels)
        out << val.label;

    if (lbl.status == rna_pair_label::inserted ||
            lbl.status == rna_pair_label::deleted ||
            lbl.status == rna_pair_label::reinserted)
        out << "`";

    return out;
}

std::ostream& operator<<(
                std::ostream& out,
                rna_pair_label::status_type s)
{
#define ST(status) case rna_pair_label::status: out << #status; break;
    switch (s)
    {
        ST(inserted)
        ST(deleted)
        ST(touched)
        ST(untouched)
        ST(edited)
        ST(reinserted)
        ST(pair_changed)

        default:
            abort();
    }

    return out;
}


size_t rna_pair_label::size() const
{
    return (size_t)paired() + 1;
}

bool rna_pair_label::paired() const
{
    assert(labels.size() == 1 || labels.size() == 2);

    return labels.size() == 2;
}

bool rna_pair_label::inited_points() const
{
    for (const auto& lbl : labels)
        if (lbl.p.bad())
            return false;
    return true;
}

point rna_pair_label::centre() const
{
    if (!inited_points())
    {
        WARN("centre() -> points not inited");
        return point::bad_point();
    }
    if (paired())
        return ::centre((*this)[0].p, (*this)[1].p);
    else
        return (*this)[0].p;
}

void rna_pair_label::clear_points()
{
    for (auto& val : labels)
        val.p = point::bad_point();
}

void rna_pair_label::set_points_exact(
                point p,
                size_t index)
{
    //APP_DEBUG_FNAME;

    (*this)[index].p = p;
}

void rna_pair_label::set_points_nearby(
                point p,
                size_t index)
{
    //APP_DEBUG_FNAME;

    double i;
    i = 5 + rand() % 10;
    p.x += i;
    p.y += i;

    (*this)[index].p = p;
}

void rna_pair_label::set_label_strings(
                const rna_pair_label& other)
{
    //  touched         - uz len tym, ze sme vosli do funkcie
    //  edited          - ak labels su rozne
    //

    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    APP_DEBUG_FNAME;

    if (status != untouched)
    {
        ERR("set_label_strings: %s - status=%s",
                to_cstr(*this), to_cstr(status));
        abort();
    }
    if (paired() != other.paired())
    {
        ERR("set_label_strings: not compatible nodes: %s-%s",
                to_cstr(*this), to_cstr(other));
        abort();
    }

    if (to_string(*this) != to_string(other))
    {
        status = edited;
        DEBUG("edit %s -> %s", to_cstr(*this), to_cstr(other));
    }
    else
        status = touched;

    size_t n = paired() ? 2 : 1;
    for (size_t i = 0; i < n; ++i)
        (*this)[i].label = other[i].label;
}


