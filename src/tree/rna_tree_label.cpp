/*
 * File: rna_tree_label.cpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard.elias@matfyz.cz>
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
#include "types.hpp"

using namespace std;


bool rna_label::operator==(
            const rna_label& other) const
{
    return label == other.label;
}




rna_pair_label::rna_pair_label(
                const std::string& s)
{
    labels.push_back({s, point::bad_point()});
}

const rna_label& rna_pair_label::operator[](
                size_t index) const
{
    assert(index == 0 || index == 1);

    try
    {
        return labels.at(index);
    }
    catch (const out_of_range& e)
    {
        ERR("Trying to get label at illegal index %s; labels=%s", index, *this);
        throw;
    }
}

rna_label& rna_pair_label::operator[](
                size_t index)
{
    assert(index == 0 || index == 1);

    try
    {
        return labels.at(index);
    }
    catch (const out_of_range& e)
    {
        ERR("Trying to get label at illegal index %s; labels=%s", index, *this);
        throw;
    }
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
    auto status_string =
        [](rna_pair_label::status_type type)
        {
            string t;
            switch (type)
            {
#define switchcase(type, text) \
                case rna_pair_label::type: \
                    t = text; \
                    break;

                switchcase(inserted, "ins")
                switchcase(deleted, "del")
                switchcase(reinserted, "reins")
                switchcase(rotated, "rot")
                default:
                    break;
#undef switchcase
            }
            if (!t.empty())
                t = "__" + t + "`";
            return t;
        };

    string status = status_string(lbl.status);

    out << status;

    for (const auto& val : lbl.labels)
        out << val.label;

    if (!status.empty())
        out << "`";

    return out;
}

std::ostream& operator<<(
                std::ostream& out,
                rna_pair_label::status_type s)
{
#define switchcase(status) case rna_pair_label::status: out << #status; break;
    switch (s)
    {
        switchcase(inserted)
        switchcase(deleted)
        switchcase(touched)
        switchcase(untouched)
        switchcase(edited)
        switchcase(reinserted)
        switchcase(pair_changed)

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
        DEBUG("WARNING: Points for rna pair centre are not inited");
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

void rna_pair_label::set_label_strings(
                const rna_pair_label& other)
{
    //  touched         - uz len tym, ze sme vosli do funkcie
    //  edited          - ak labels su rozne
    //

    if (status != untouched)
    {
        throw illegal_state_exception("Setting bases failed, trying to modify once modified base: %s - status %s",
                *this, status);
    }
    else if (paired() != other.paired())
    {
        throw illegal_state_exception("Setting bases failed, not compatible nodes: %s-%s",
                *this, other);
    }

    if (to_string(*this) != to_string(other))
    {
        status = edited;
        DEBUG("edit %s -> %s", *this, other);
    }
    else
        status = touched;

    size_t n = paired() ? 2 : 1;
    for (size_t i = 0; i < n; ++i)
        (*this)[i].label = other[i].label;
}


