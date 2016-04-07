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

#include <fstream>
#include <cfloat>

#include "utils.hpp"
#include "mapping.hpp"
#include "ps_reader.hpp"

using namespace std;


/* global */ std::string read_file(
                const std::string& filename)
{
    assert_err(exist_file(filename),
            "read_file(%s) failed, file does not exist", to_cstr(filename));

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

    assert_err(out.good(),
            "write_file(%s) failed", to_cstr(filename));
}











template <typename table_type, typename table_value_type = size_t, typename file_value_type = size_t>
table_type load_table(
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    assert_err(exist_file(filename),
            "load_table(%s) failed, file does not exist", to_cstr(filename));

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

    assert_err(!out.fail(),
            "save_table(%s) failed", to_cstr(filename));
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

    assert_err(!out.fail(),
            "save_table(%s) failed", to_cstr(filename));
}

mapping load_mapping_table(
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    assert_err(exist_file(filename),
            "load_file(%s) failed, file does not exist", to_cstr(filename));

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



point top_right_corner(
                rna_tree::iterator root)
{
    LOGGER_PRIORITY_ON_FUNCTION(DEBUG);

    // x, y should be maximal in subtree
    point p = { -DBL_MAX, -DBL_MAX };

    auto f = [&p] (rna_tree::pre_post_order_iterator it) {
        if (rna_tree::is_root(it) || !it->inited_points())
            return;
        point o = it->at(it.label_index()).p;
        if (o.x > p.x)
            p.x = o.x;
        if (o.y > p.y)
            p.y = o.y;
    };

    rna_tree::for_each_in_subtree(root, f);

    assert(p.x != -DBL_MAX && p.y != -DBL_MAX);
    DEBUG("top_right_corner = %s", to_cstr(p));

    return p;
}

point bottom_left_corner(
                rna_tree::iterator root)
{
    LOGGER_PRIORITY_ON_FUNCTION(DEBUG);

    // x, y should be minimal in subtree
    point p = { DBL_MAX, DBL_MAX };

    auto f = [&p] (rna_tree::pre_post_order_iterator it) {
        if (rna_tree::is_root(it) || !it->inited_points())
            return;
        point o = it->at(it.label_index()).p;
        if (o.x < p.x)
            p.x = o.x;
        if (o.y < p.y)
            p.y = o.y;
    };

    rna_tree::for_each_in_subtree(root, f);

    assert(p.x != DBL_MAX && p.y != DBL_MAX);
    DEBUG("bottom_left_corner = %s", to_cstr(p));

    return p;
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

rna_tree get_rna(const string& name)
{
    DEBUG("get_rna(%s)", to_cstr(name));

    string l, b;
    l = read_file(name + ".seq");
    b = read_file(name + ".fold");

    ps_document doc(name + ".ps");

    return rna_tree(b, l, doc.points, name);
}



