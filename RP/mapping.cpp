/*
 * File: mapping.cpp
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

#include "mapping.hpp"
#include "rna_tree.hpp"
#include "generator.hpp"

using namespace std;


mapping::indexes mapping::get_to_insert() const
{
    APP_DEBUG_FNAME;

    indexes vec;
    for (auto m : map)
    {
        if (m.from == 0)
            vec.push_back(m.to);
    }
    sort(vec.begin(), vec.end(), less<size_t>());
    LOGGER_PRINT_CONTAINER(vec, "to_insert");

    return vec;
}

mapping::indexes mapping::get_to_remove() const
{
    APP_DEBUG_FNAME;

    indexes vec;
    for (auto m : map)
    {
        if (m.to == 0)
            vec.push_back(m.from);
    }
    sort(vec.begin(), vec.end(), less<size_t>());
    LOGGER_PRINT_CONTAINER(vec, "to_remove");

    return vec;
}

mapping::mapping(
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    assert(reader::exist_file(filename));

    ifstream in(filename);
    *this = read_mapping(in);
}

/* static */ mapping mapping::read_mapping(
                std::istream& in)
{
    mapping map;
    mapping_pair m;

    string distance;
    in
        >> distance
        >> distance;
    map.distance = stoi(distance);

    while(true)
    {
        in >> m.from >> m.to;
        if (in.fail())
            break;
        map.map.push_back(m);
    }
    return map;
}

mapping::mapping(
                const rna_tree& rna1,
                const rna_tree& rna2)
    : mapping(MAP(rna1.name(), rna2.name()))
{ }

/* static */ void mapping::run_rted(
                const rna_tree& rna1,
                const rna_tree& rna2,
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    string command;
    string f1, f2;

    if(reader::exist_file(filename))
    {
        DEBUG("RTED: file %s exist, return", filename.c_str());
        return;
    }

    //f1 = RTED_RUN_FILE(1, rna1.name());
    //f2 = RTED_RUN_FILE(2, rna2.name());

    ofstream out;
    out.open(f1);
    out << convert_to_java_format(rna1);
    out.close();
    out.open(f2);
    out << convert_to_java_format(rna2);
    out.close();

    command =
                "java -cp java_RTED util.RTEDCommandLine "
                    "--costs 1 1 0 "
                    "-m "
                    "--files "
                    " '" + f1 + "' "
                    " '" + f2 + "' "
                    " > " + filename;

    DEBUG("RUN COMMAND: %s", command.c_str());
    
    if (system(command.c_str()) != 0)
    {
        ERR("rted command failed");
        abort();
    }
}


