/*
 * File: util.cpp
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

#include <fstream>

#include "util.hpp"
#include "app.hpp"
#include "macros.hpp"


using namespace std;



document read_ps(
                const std::string& file)
{
    APP_DEBUG_FNAME;
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    document doc;
    string line, str;
    ifstream in(file);
    Point p;
    auto stream_pos = in.tellg();

    auto ignore_line = [](const std::string& line)
    {
        // ignorovanie parovacich hran a pod
        //
        auto ends_with = [](const std::string& str, const std::string& what)
        {
            return str.find(what) == str.size() - what.size();
        };
        auto vec = {
            "lwline",
            "lwfarc",
            "lwarc",
            //"lwstring",
            "lineto",
            "moveto",
            "setlinewidth",
            "setlinecap",
        };
        for (auto val : vec)
            if (ends_with(line, val))
                return true;
        return false;
    };
    auto is_rgb_funct = [](const std::string& line)
    {
        stringstream str(line);
        string other;
        float f;
        str >> f >> f >> f >> other;
        return 
            !str.fail() &&
            str.eof() &&
            other == "setrgbcolor";
    };

    // nacita prolog suboru
    // vsetko az po bazy s bodmi..
    while(true)
    {
        getline(in, line);
        assert(!in.fail());

        if (is_base_line(line))
        {
            // vrat sa pred nacitanie riadku...
            in.seekg(stream_pos);
            break;
        }
        stream_pos = in.tellg();

        if (ignore_line(line))
            continue;
        doc.prolog += line + "\n";
        DEBUG("prolog '%s'", line.c_str());
    }

    // nacita bazy..
    while(true)
    {
        getline(in, line);
        assert(!in.fail());

        if (is_rgb_funct(line))
        {
            DEBUG("is_rgb_funct(%s), continue", line.c_str());
            continue;
        }
        if (!is_base_line(line))
        {
            in.seekg(stream_pos);
            break;
        }
        stream_pos = in.tellg();

        stringstream stream(line);
        stream
            >> str
            >> p.x
            >> p.y
            >> str;

        doc.labels.push_back(line.at(1));
        doc.points.push_back(p);

        DEBUG("base_line '%s'", line.c_str());
    }

    // nacita epilog suboru
    // vsetko od baz az po koniec suboru
    while(true)
    {
        getline(in, line);
        if (in.fail())
            break;
        doc.epilog += line + "\n";
        DEBUG("epilog '%s'", line.c_str());
    }

    return doc;
}

std::string convert_to_java_format(
                const rna_tree& rna)
{
    APP_DEBUG_FNAME;

    auto it = ++rna.begin();
    stringstream stream;

    std::function<void(rna_tree::sibling_iterator,
            stringstream&)> print_recursive =
        [&](rna_tree::sibling_iterator sib, stringstream& out)
    {
        out << "{";
        out << label(sib) << " ";

        if (!rna_tree::is_leaf(sib))
            print_recursive(rna_tree::first_child(sib), out);

        out << "}";

        if (!rna_tree::is_last_child(sib))
        {
            ++sib;
            print_recursive(sib, out);
        }
    };

    stream << "{" << label(rna.begin());
    print_recursive(it, stream);
    stream << "}";

    string s = stream.str();
    int left, right;
    left = right = 0;
    for (auto val : s)
    {
        assert(left >= right);
        if (val == '{')
            ++left;
        else if (val == '}')
            ++right;
    }
    assert(left == right);
    return stream.str();
}




// do not use:
std::string run_RNAfold(
                const std::string& labels)
{
    APP_DEBUG_FNAME;

    logger.error("do not use!");
    abort();

    string out;
    string command;
    string filename;

    filename = "build/rna_fold_out" + to_string(labels.size());

#define RNAFOLD_PATH    "/afs/ms.mff.cuni.cz/u/e/eliasr/Projects/RocnikovyProjekt/RNAPlot/bin/RNAfold"

    command = "echo '" +
        labels +
        "' | " RNAFOLD_PATH " --noPS > " +
        filename;

    logger.debugStream() << "RUN COMMAND: " << command;

    system(command.c_str());

    ifstream in(filename);
    getline(in, out);

    in >> out;

    if (in.fail())
    {
        logger.error("fold_rna input failed");
        abort();
    }
    
    return out;
}




bool exist_file(
                const std::string& filename)
{
    return ifstream(filename).good();
}

std::string read_file(
                const std::string& filename)
{
    //APP_DEBUG_FNAME;

    assert(exist_file(filename));

    ifstream in(filename);
    stringstream s;
    s << in.rdbuf();
    return s.str();
 }



mapping read_mapping_file(
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    ifstream in(filename);
    mapping map;
    mapping_pair m;

    string distance;
    getline(in, distance);
    map.distance = atoi(distance.c_str());

    while(true)
    {
        in >> m.from >> m.to;
        if (in.fail())
            break;
        map.map.push_back(m);
    }
    return map;
}

bool is_base_line(
                const std::string& line)
{
    static vector<char> bases = {'A', 'C', 'G', 'U'};
    Point p;
    string other;
    stringstream str(line);

    str 
        >> other
        >> p.x
        >> p.y
        >> other;

    return
        !(
            false
            || line.size() < 3
            || find(bases.begin(), bases.end(), line.at(1)) == bases.end()
            || line.find("lwstring") == string::npos
            || str.fail()
            || other != "lwstring"
            || !str.eof()
         );
}

Point stred(rna_tree::iterator iter)
{
    auto label = iter->get_label();
    if (!label.is_paired())
        return label.labels.at(0).point;
    else
        return stred(label.labels.at(0).point,
                    label.labels.at(1).point);
}



// MAPPING:

/* static */ mapping mapping::compute_mapping(
                const rna_tree& rna1,
                const rna_tree& rna2)
{
    APP_DEBUG_FNAME;

    string filename;
    string line;
    string distance;
    mapping out_mapping;
    mapping_pair m;

    filename = RTED_MAP_FILE(rna1, rna2);
    run_rted(rna1, rna2, filename);

    ifstream in(filename);

    getline(in, line);
    getline(in, distance);
    assert(!in.fail());

    out_mapping.distance = atoi(distance.c_str());
    while(true)
    {
        getline(in, line);
        if (in.fail())
            break;
        auto i = line.find("->");
        if (i == string::npos)
        {
            WARN("pattern '->' not found in line '%s'",
                    line.c_str());
            continue;
        }

        string s1, s2;
        s1 = line.substr(0, i);
        s2 = line.substr(i + 2);
        m.from = atoi(s1.c_str());
        m.to = atoi(s2.c_str());
        out_mapping.map.push_back(m);
    }

    DEBUG("distance: %lu", out_mapping.distance);

    return out_mapping;
}

/* static */ void mapping::run_rted(
                const rna_tree& rna1,
                const rna_tree& rna2,
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    string command;
    string f1, f2;

    if(exist_file(filename))
    {
        DEBUG("RTED: file %s exist, return", filename.c_str());
        return;
    }

    f1 = RTED_RUN_FILE(1, rna1.name);
    f2 = RTED_RUN_FILE(2, rna2.name);

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
    LOGGER_PRINT_CONTAINER(vec, "to_insert");

    return vec;
}


// MAPPING END


void document::update_rna_points()
{
    // TODO: prerobit funkciu.. zbytocne ++.. 
    APP_DEBUG_FNAME;

    typedef rna_tree::pre_post_order_iterator pre_post_it;

    rna_tree& r = template_rna;

    // test ci ma rovnako labelov/pointov/baz
    size_t i = 0;
    for (pre_post_it it = ++r.begin_pre_post(); ++pre_post_it(it) != r.end_pre_post(); ++it)
        ++i;
    assert(i == labels.size() && i == points.size());

    i = 0;
    for (pre_post_it it = ++r.begin_pre_post(); ++pre_post_it(it) != r.end_pre_post(); ++it, ++i)
    {
        auto label = it->get_label();
        size_t index = get_label_index(it);

        Point p = points.at(i);
        //p = p + Point({1000, 1000});
        label.labels.at(index).point = p;

        it->set_label(label);
    }
}








