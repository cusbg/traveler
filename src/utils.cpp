/*
 * File: utils.cpp
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

#include "utils.hpp"
#include "rna_tree.hpp"
#include "mapping.hpp"

using namespace std;


/* global */ std::string read_file(
                const std::string& filename)
{
    assert(exist_file(filename));

    ifstream in(filename);
    stringstream s;
    s << in.rdbuf();
    return s.str();
}

/* global */ bool exist_file(
                const std::string& filename)
{
    return ifstream(filename).good();
}

/* global */ void write_file(
                const std::string& filename,
                const std::string& what)
{
    ofstream out(filename);
    out << what;
    out.flush();
    assert(out.good());
}

std::vector<std::string> get_command_output(const std::string& command)
{
    APP_DEBUG_FNAME;

    vector<string> vec;
    FILE* f;
    char *l = nullptr;
    size_t n = 0;

    logger.debugStream()
        << "RUN: "
        << command;

    f = popen(command.c_str(), "r");

    while(getline(&l, &n, f) >= 0)
        vec.push_back(l);

    assert(!ferror(f));
    pclose(f);
    free(l);

    return vec;
}




ps_document::ps_document(const std::string& name)
{
    APP_DEBUG_FNAME;
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    string line, str;
    assert(exist_file(name));

    ifstream in(name);
    point p;
    auto stream_pos = in.tellg();

    auto ignore_line = [](const std::string& line)
    {
        // ignorovanie parovacich hran a pod
        //
        auto ends_with = [](const std::string& str, const std::string& what)
        {
            return str.find(what) == str.size() - what.size();
        };

        auto endings = {
            "lwline",
            "lwfarc",
            "lwarc",
            //"lwstring",
            "lineto",
            "moveto",
            "setlinewidth",
            "setlinecap",
        };
        auto lines = {
            "newpath",
            "stroke",
        };
        for (auto val : endings)
            if (ends_with(line, val))
                return true;
        for (auto val : lines)
            if (line == val)
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
        prolog += line + "\n";
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

        labels.push_back(line.at(1));
        points.push_back(p);

        DEBUG("base_line '%s'", line.c_str());
    }

    // nacita epilog suboru
    // vsetko od baz az po koniec suboru
    while(true)
    {
        getline(in, line);
        if (in.fail())
            break;
        epilog += line + "\n";
        DEBUG("epilog '%s'", line.c_str());
    }
}

bool ps_document::is_base_line(const std::string& line)
{
    const static vector<char> bases = {'A', 'C', 'G', 'U'};
    point p;
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

/* static */ std::string ps_document::default_prologue()
{
    return
        "%!\n"
        "/lwline {newpath moveto lineto stroke} def\n"
        "/lwstring {moveto show} def\n"
        "306.00 396.00 translate\n"
        "0.54 0.54 scale\n"
        "-182.50 481.00 translate\n"
        "/Helvetica findfont 8.00 scalefont setfont\n";
}







template <typename table_type, typename table_value_type = size_t, typename file_value_type = size_t>
table_type load_table(
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    DEBUG("load: %s", to_cstr(filename));
    assert(exist_file(filename));

    std::ifstream in(filename);
    size_t m, n;
    file_value_type val;

    table_type table;

    in
        >> m
        >> n;

    assert(!in.fail());

    table.resize(m, typename table_type::value_type(n));

    for (size_t i = 0; i < m; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            in >> val;

            table[i][j] = table_value_type(val);
        }
    }
    assert(!in.fail());
    string s;
    in >> s;    // no other words, only EOF
    assert(in.eof());

    return table;
}

template <typename table_type, typename table_value_type = size_t, typename file_value_type = size_t>
void save_table(
                const std::string& filename,
                const table_type& table)
{
    APP_DEBUG_FNAME;
    DEBUG("save: %s", to_cstr(filename));

    using std::endl;

    std::ofstream out(filename);
    size_t m, n;

    m = table.size();
    n = table.at(0).size();

    out
        << m
        << " "
        << n
        << endl;

    for (size_t i = 0; i < m; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            const table_value_type& val = table[i][j];

            out << file_value_type(val) << " ";
        }
        out << endl;
    }
    assert(!out.fail());
}




void save_strategy_table(
                const std::string& filename,
                const strategy_table_type& table)
{
    APP_DEBUG_FNAME;

    save_table<strategy_table_type,
                strategy, size_t>(filename, table);
}

strategy_table_type load_strategy_table(
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    return load_table<strategy_table_type,
                strategy>(filename);
}

void save_tree_distance_table(
                const std::string& filename,
                const std::vector<std::vector<size_t>>& table)
{
    APP_DEBUG_FNAME;
    typedef std::vector<std::vector<size_t>> tree_distance_table_type;

    save_table<tree_distance_table_type,
                size_t, size_t>(filename, table);
}

std::vector<std::vector<size_t>> load_tree_distance_table(
                const std::string& filename)
{
    APP_DEBUG_FNAME;
    typedef std::vector<std::vector<size_t>> tree_distance_table_type;

    return load_table<tree_distance_table_type,
                size_t, size_t>(filename);
}



void save_tree_mapping_table(
                const std::string& filename,
                const mapping& map)
{
    APP_DEBUG_FNAME;

    DEBUG("save: %s", to_cstr(filename));
    ofstream out(filename);

    out
        << "DISTANCE: "
        << map.distance
        << endl;

    for (const auto& m : map.map)
    {
        out
            << m.from 
            << " "
            << m.to
            << endl;
    }
    assert(!out.fail());
}

mapping load_mapping_table(
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    DEBUG("load: %s", to_cstr(filename));
    assert(exist_file(filename));

    string s;
    mapping map;
    mapping::mapping_pair m;
    ifstream in(filename);

    in
        >> s
        >> map.distance;

    assert(!in.fail());
    assert(s == "DISTANCE:");

    while (true)
    {
        in
            >> m.from
            >> m.to;
        if (in.fail())
            break;

        map.map.push_back(m);
    }

    return map;
}




#ifdef NODEF

/**
 * convert rna_tree to java-implementation rted fromat
 */
std::string convert_to_java_format(
                rna_tree rna)
{
    APP_DEBUG_FNAME;

    auto it = ++rna.begin();
    stringstream stream;

    std::function<void(rna_tree::sibling_iterator,
            stringstream&)> print_recursive =
        [&](rna_tree::sibling_iterator sib, stringstream& out)
    {
        out << "{" << " ";
        out << label(sib);

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

#endif

