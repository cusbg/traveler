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



rna_pair_label::rna_pair_label(const std::string& s)
{
    rna_label l;
    l.label = s;
    labels.push_back(l);
}

bool rna_pair_label::operator==(const rna_pair_label& other) const
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

rna_pair_label rna_pair_label::operator+(const rna_pair_label& other) const
{
    assert(labels.size() == 1 && other.labels.size() == 1);

    rna_pair_label out;
    out.labels.push_back(labels.back());
    out.labels.push_back(other.labels.back());

    assert(out.is_paired());

    return out;
}

std::ostream& operator<<(std::ostream& out, const rna_pair_label& label)
{
    out << label.to_string();
    return out;
}

bool rna_pair_label::is_paired() const
{
    assert(labels.size() == 1 || labels.size() == 2);
    return labels.size() == 2;
}

void rna_pair_label::set_points(const rna_pair_label& from)
{
    APP_DEBUG_FNAME;

    double posunutie = 5 + rand() % 10;
    //posunutie = 5;

    //posunutie = 10;

    for (size_t i = 0; i < 2; ++i)
    {
        if (labels.size() <= i || from.labels.size() <= i)
        {
            //DEBUG("size < %lu", i);
        }
        else
        {
            Point p = from.labels.at(i).point;
            Point& to = labels.at(i).point;
            to = p;

            //to.x = p.x + posunutie;
            to.y = p.y + posunutie;
        }
    }
}

std::string rna_pair_label::get_points_string() const
{
    string out;
    for (const auto& val : labels)
        out += val.label + ":" + val.point_to_string() + "|";
    return out;
}

std::string rna_pair_label::to_string() const
{
    string out;
    if (status == inserted)
        out += "__ins`";
    if (status == deleted)
        out += "__del`";

    for (auto val : labels)
        out += val.label;

    if (status == inserted || status == deleted)
        out += "`";

    return out;
}


void rna_pair_label::set_label_strings(const rna_pair_label& from)
{
    // 3 druhy statusu:
    //  touched         - uz len tym, ze sme vosli do funkcie
    //  edited          - ak labels su rozne
    //  pair_changed    - ak sa par zmenil na nepar, resp. naopak.
    //
    //APP_DEBUG_FNAME;

    DEBUG("changing labels from %s to %s", to_string().c_str(), from.to_string().c_str());

    status = touched;

    if (to_string() != from.to_string())
        status = edited;
    if (labels.size() != from.labels.size())
    {
        DEBUG("pair_changed");
        status = pair_changed;
    }

    size_t m = 1;
    if (is_paired() && from.is_paired())
        m = 2;

    for (size_t i = 0; i < m; ++i)
        labels.at(i).label = from.labels.at(i).label;
    
    //return;
    //if (status == edited)
    //{
        //for (size_t i = 0; i < m; ++i)
            //labels.at(i).label += "|" + from.labels.at(i).label;
    //}
    //else
    //{
        //for (size_t i = 0; i < m; ++i)
            //labels.at(i).label = from.labels.at(i).label;
    //}
}




std::string rna_label::point_to_string() const
{
    if (point.x == 0xBADF00D && point.y == 0xBADF00D)
    {
        WARN("BAD_POINT");
        abort();
    }
    std::stringstream out;
    out << point.x << " " << point.y;
    return out.str();
}

bool rna_label::operator==(const rna_label& other) const
{
    return label == other.label;
}




