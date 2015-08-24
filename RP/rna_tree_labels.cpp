/*
 * File: rna_tree_labels.cpp
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

#include "rna_tree_labels.hpp"

using namespace std;


std::string to_string(rna_pair_label::label_status_type status)
{
    string out;

#define ST(status) case rna_pair_label::status: out = #status; break;

    switch (status)
    {
        ST(inserted)
        ST(deleted)
        ST(touched)
        ST(untouched)
        ST(edited)
        ST(reinserted)
        ST(pair_changed)
    }
    return out;
}


// RNA_PAIR_LABEL:
rna_pair_label::rna_pair_label(
                const std::string& s)
{
    rna_label l;
    l.label = s;
    labels.push_back(l);
}

bool rna_pair_label::operator==(
                const rna_pair_label& other) const
{
    bool ret = true;
    if (labels.size() != other.labels.size())
        ret = false;
    else
    {
        for (size_t i = 0; i < labels.size(); ++i)
            if (labels.at(i) != other.labels.at(i))
                ret = false;
    }
    return ret;
}

rna_pair_label rna_pair_label::operator+(
                const rna_pair_label& other) const
{
    assert(labels.size() == 1 && other.labels.size() == 1);

    rna_pair_label out;
    out.labels.push_back(labels.back());
    out.labels.push_back(other.labels.back());

    assert(out.is_paired());

    return out;
}

std::ostream& operator<<(
                std::ostream& out,
                const rna_pair_label& label)
{
    out << label.to_string();
    return out;
}

bool rna_pair_label::is_paired() const
{
    assert(labels.size() == 1 || labels.size() == 2);
    return labels.size() == 2;
}

void rna_pair_label::set_points_exact(
                const rna_pair_label& from)
{
    APP_DEBUG_FNAME;

    for (size_t i = 0; i < 2; ++i)
        if (i < labels.size() && i < from.labels.size())
            set_points_exact(from.labels.at(i).point, i);
}

void rna_pair_label::set_points_nearby(
                const rna_pair_label& from)
{
    APP_DEBUG_FNAME;

    if (labels.size() != from.labels.size())
        WARN("wrong lbl sizes");

    for (size_t i = 0; i < 2; ++i)
        if (i < labels.size() && i < from.labels.size())
            set_points_nearby(from.labels.at(i).point, i);
}

void rna_pair_label::set_points_nearby(
                Point p,
                size_t which)
{
    double posunutie;
    posunutie = 5 + rand() % 10;
    //posunutie = 5;
    p.x += posunutie;
    //p.y += posunutie;

    labels.at(which).point = p;
}

void rna_pair_label::set_points_exact(
                Point p,
                size_t which)
{
    labels.at(which).point = p;
}




std::string rna_pair_label::get_points_string() const
{
    string out;
    for (const auto& val : labels)
        out += val.label + ":" + val.point.to_string() + "|";
    return out;
}

std::string rna_pair_label::to_string() const
{
    string out;

    if (status == inserted)
        out += "__ins`";
    if (status == deleted)
        out += "__del`";
    if (status == reinserted)
        out += "__reins`";

    for (auto val : labels)
        out += val.label;

    if (status == inserted || status == deleted || status == reinserted)
        out += "`";

    return out;
}


void rna_pair_label::set_label_strings(
                const rna_pair_label& from)
{
    // 3 druhy statusu:
    //  touched         - uz len tym, ze sme vosli do funkcie
    //  edited          - ak labels su rozne
    //  pair_changed    - ak sa par zmenil na nepar, resp. naopak.
    //

    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    //APP_DEBUG_FNAME;

    assert(&from != nullptr);

    //DEBUG("%s <-> %s", to_string().c_str(), from.to_string().c_str());

    if (status != untouched)
    {
        logger.warnStream() << *this << " " << from << " STATUS: " << ::to_string(status);
    }
    assert(status == untouched);

    if (is_paired() != from.is_paired())
    {
        ERR("not compatible nodes: paired-nonpaired");
        abort();
    }
    if (to_string() != from.to_string())
    {
        status = edited;
        DEBUG("changing labels from %s to %s", to_string().c_str(), from.to_string().c_str());
    }
    else if (labels.size() != from.labels.size())
    {
        DEBUG("pair_changed");
        status = pair_changed;
    }
    else
        status = touched;

    size_t max = 1;
    if (is_paired() && from.is_paired())
        max = 2;

    for (size_t i = 0; i < max; ++i)
        labels.at(i).label = from.labels.at(i).label;
}

bool rna_pair_label::inited_points() const
{
    for (const auto& val : labels)
        if (val.point.bad())
            return false;
    return true;
}

Point rna_pair_label::get_centre() const
{
    if (is_paired())
    {
        if (!inited_points())
        {
            WARN("::get_centre() -> not inited");
            DEBUG("%s", this->to_string().c_str());
            //double i = rand() % 10;
            //return Point({100 + i, i});
            return Point::bad_point();
        }
        else
            return centre(labels.at(0).point, labels.at(1).point);
    }
    else
        return labels.at(0).point;
}

rna_label& rna_pair_label::lbl(size_t index)
{
    return labels.at(index);
}

const rna_label& rna_pair_label::lbl(size_t index) const
{
    return labels.at(index);
}

size_t rna_pair_label::size() const
{
    assert(labels.size() <= 2);
    return labels.size();
}



// RNA_LABEL:
std::string rna_label::to_string() const
{
    std::stringstream out;
    out 
        << label
        << ": "
        << point;
    return out.str();
}

bool rna_label::operator==(const rna_label& other) const
{
    return label == other.label;
}



