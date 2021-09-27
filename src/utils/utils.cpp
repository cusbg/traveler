/*
 * File: utils.cpp
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */


#include <fstream>

#include "utils.hpp"
#include "mapping.hpp"
#include "exception.hpp"

using namespace std;

template<typename container_type, typename value_container_type>
bool contains_one_of(
                     const container_type& container,
                     const value_container_type& values)
{
    for (const auto& val : values)
        if (contains(container, val))
            return true;
    return false;
}


/* global */ std::string read_file(
                                   const std::string& filename)
{
    if (!exist_file(filename))
        throw io_exception("read_file(%s) failed, file does not exist", filename);
    
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
    
    if (!exist_file(filename))
        throw io_exception("write_file(%s) failed", filename);
}

/* inline, local */ std::string trim(
        std::string s)
{
#define WHITESPACES " \t\n\r\f\v"
    size_t pos;

    pos = s.find_first_not_of(WHITESPACES);
    if (pos != s.npos   )
        s.erase(0, pos);
    pos = s.find_last_not_of(WHITESPACES);
    if (pos != s.npos)
        s.erase(pos + 1);

    return s;
}


/* global */ fasta read_fasta_file(
                                   const std::string& filename)
{
    if (!exist_file(filename))
        throw io_exception("read_file(%s) failed, file does not exist", filename);
    
    ifstream in(filename);
    ostringstream labels, brackets, constraints;
    string id, line;
    
    while(true)
    {
        char ch;
        in >> ch;
        if (in.fail() || ch != '>')
            throw wrong_argument_exception("starting character of dot-bracket fasta file '>' is missing");
        
        getline(in, id);
        size_t index = id.find_first_of(' ');
        if (index == string::npos)
            index = id.length();
        id = id.substr(0, index);
        break;
    }
    
    while(true)
    {
        getline(in, line);
        if (in.fail())
            break;
        if (contains(line, '>'))
            break;
        
      if ( line.size() && line[line.size()-1] == '\r' ) {
           DEBUG("Removing carriage return\n");
           line = line.substr( 0, line.size() - 1 );
        }

        if ( line.size() && line[line.size()-1] == '\n' ) {
           DEBUG("Removing line feed\n");
           line = line.substr( 0, line.size() - 1 );
        }
        if (contains_one_of(line, "[{(.)}]"))
            brackets << line;
        else if (contains_one_of(line, "*-"))
            constraints << line;
        else
            labels << line;
    }
    fasta f;
    f.id = id;
    f.brackets = trim(brackets.str());
    f.labels = trim(labels.str());
    f.constraints = trim(constraints.str());

    if (f.brackets.size() != f.labels.size()) {
        throw wrong_argument_exception(
                std::string("\nNumber of brackets != Number of labels\nBrackets: " + f.brackets + "\nLabels:   "+f.labels).c_str());
    }

    if (!f.constraints.empty() && f.brackets.size() != f.constraints.size()) {
        std::ostringstream out;
        throw wrong_argument_exception(
                std::string("\nNumber of brackets != Number of constraints\nBrackets: " + f.brackets + "\nConstraints:   " + f.constraints).c_str());
    }
    
    DEBUG("%s", to_cstr(f));
    return f;
}




template <typename table_type, typename table_value_type = size_t, typename file_value_type = size_t>
table_type load_table(
                      const std::string& filename)
{
    APP_DEBUG_FNAME;
    
    if (!exist_file(filename))
        throw io_exception("load_table(%s) failed, file does not exist", filename);
    
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
    DEBUG("save: %s", filename);
    
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
    
    if (out.fail())
        throw io_exception("save_table(%s) failed", filename);
}




void save_strategy_table(
                         const std::string& filename,
                         const strategy_table_type& table)
{
    APP_DEBUG_FNAME;
    
    save_table<strategy_table_type, strategy, size_t>(filename, table);
}

strategy_table_type load_strategy_table(
                                        const std::string& filename)
{
    APP_DEBUG_FNAME;
    
    return load_table<strategy_table_type, strategy>(filename);
}

void save_tree_distance_table(
                              const std::string& filename,
                              const std::vector<std::vector<size_t>>& table)
{
    APP_DEBUG_FNAME;
    typedef std::vector<std::vector<size_t>> tree_distance_table_type;
    
    save_table<tree_distance_table_type>(filename, table);
}

std::vector<std::vector<size_t>> load_tree_distance_table(
                                                          const std::string& filename)
{
    APP_DEBUG_FNAME;
    typedef std::vector<std::vector<size_t>> tree_distance_table_type;
    
    return load_table<tree_distance_table_type>(filename);
}



void save_tree_mapping_table(
                             const std::string& filename,
                             const mapping& map)
{
    APP_DEBUG_FNAME;
    
    DEBUG("save: %s", filename);
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
    
    if (out.fail())
        throw io_exception("save_table(%s) failed", filename);
}

mapping load_mapping_table(
                           const std::string& filename)
{
    APP_DEBUG_FNAME;
    
    if (!exist_file(filename))
        throw io_exception("load_file(%s) failed, file does not exist", filename);
    
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




std::ostream& operator<<(
                         std::ostream& out,
                         fasta f)
{
    out
    << "FASTA:"
    << endl
    << ">"
    << f.id
    << endl
    << f.labels
    << endl
    << f.brackets;
    
    return out;
}
