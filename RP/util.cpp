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

#include "util.hpp"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "app.hpp"
#include <unistd.h>


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
        doc.prolog += line + "\n";
        DEBUG("prolog '%s'", line.c_str());
    }

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

std::string run_RNAfold(
                const std::string& labels)
{
    APP_DEBUG_FNAME;

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



/* static */ std::vector<mapping> mapping::compute_mapping(
                const rna_tree& rna1,
                const rna_tree& rna2)
{
    APP_DEBUG_FNAME;

    vector<mapping> vec;
    string line;
    string filename;
    string distance;

    filename = "build/files/mapping_" + rna1.name + "-" + rna2.name;

    run_rted(rna1, rna2, filename);

    ifstream in(filename);
    getline(in, line);
    getline(in, distance);
    mapping map;

    assert(!in.fail());
    while(true)
    {
        getline(in, line);
        if (in.fail())
            break;
        auto i = line.find("->");
        if (i == string::npos)
            continue;
        string s1, s2;
        s1 = line.substr(0, i);
        s2 = line.substr(i + 2);
        map.from = atoi(s1.c_str());
        map.to = atoi(s2.c_str());
        vec.push_back(map);
    }

    DEBUG("distance: %s", distance.c_str());

    return vec;
}

/* static */ void mapping::run_rted(
                const rna_tree& rna1,
                const rna_tree& rna2,
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    string s1, s2;
    string command;

    if(exist_file(filename))
    {
        DEBUG("RTED: file %s exist, return", filename.c_str());
        return;
    }

    s1 = convert_to_java_format(rna1);
    s2 = convert_to_java_format(rna2);

    command =
                "java -cp java_RTED util.RTEDCommandLine "
                    "--costs 1 1 0 "
                    "-m "
                    "--trees "
                " '" + s1 + "' "
                " '" + s2 + "' " +
                " > " + filename;

    logger.debugStream() << "RUN COMMAND: " << command;
    
    system(command.c_str());
    assert(exist_file(filename));
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
    string out;
    string line;

    while(true)
    {
        getline(in, line, '\0');
        if (in.fail())
            break;
        out += line;
    }
    return out;
 }



std::vector<mapping> read_mapping_file(
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    vector<mapping> vec;
    ifstream in(filename);
    mapping m;

    while(true)
    {
        in >> m.from >> m.to;
        if (in.fail())
            break;
        vec.push_back(m);
    }
    return vec;
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
            line.size() < 3 ||
            find(bases.begin(), bases.end(), line.at(1)) == bases.end() ||
            line.find("lwstring") == string::npos ||
            str.fail() ||
            other != "lwstring" ||
            !str.eof()
        );
}


// GENERATOR: 

#define FILES               {"human", "mouse", "rabbit", "frog"}
#define PS_IN(val)          "build/files/" + val + ".ps"
#define PS_OUT(val1, val2)  "build/files/" + val1 + "-" + val2 + ".out.ps"
#define SEQ(val)            "build/files/" + val + ".seq"
#define RNAFOLD(val)        "build/files/" + val + ".RNAfold.fold"
//#define FOLD(val)           RNAFOLD(val)
#define FOLD(val)           "build/files/" + val + ".fold"
#define FOLD_IN(val)        "../InFiles/" + val + ".fold"
#define MAP(val1, val2)     "build/files/" + val1 + "-" + val2 + ".map"

/* static */ void generator::generate_seq_files()
{
    APP_DEBUG_FNAME;

    vector<string> vec = FILES;

    for (auto val : vec)
    {
        string fileIn = PS_IN(val);
        string fileOut = SEQ(val);
        string labels = read_ps(fileIn).labels;
        ofstream out(fileOut);
        out << labels;
        assert(!out.fail());
    }
}

/* static */ void generator::generate_fold_files()
{
    APP_DEBUG_FNAME;

    vector<string> vec = FILES;

    for (auto val : vec)
    {
        string fileIn = FOLD_IN(val);
        string fileOut = FOLD(val);
        
        if (!exist_file(fileIn))
        {
            logger.error("FILE %s does not exist", fileIn.c_str());
            abort();
        }
        string brackets = read_file(fileIn);

        ofstream out(fileOut);
        out << brackets;
        assert(!out.fail());
    }
}

/* static */ void generator::generate_ps_files()
{
    APP_DEBUG_FNAME;

    vector<string> vec = FILES;

    for (auto val : vec)
    {
        string fileIn = "../InFiles/" + val + ".ps";
        string fileOut = PS_IN(val);
        string command = "cp " + fileIn + " " + fileOut;

        if (!exist_file(fileIn))
        {
            logger.error("FILE %s does not exist", fileIn.c_str());
            abort();
        }
        system(command.c_str());
        assert(exist_file(fileOut));
    }
}

/* static */ void generator::generate_RNAfold_fold_files()
{
    APP_DEBUG_FNAME;

    logger.error("do not use!!!");
    abort();

    vector<string> vec = FILES;

    for (auto val : vec)
    {
        string fileIn = SEQ(val);
        string fileOut = FOLD(val);
        
        assert(exist_file(fileIn));
        string labels = read_file(fileIn);

        string brackets = run_RNAfold(labels);

        ofstream out(fileOut);
        out << brackets;
        assert(!out.fail());
    }
}

/* static */ void generator::generate_mapping()
{
    APP_DEBUG_FNAME;

    vector<string> vec;
    vec = FILES;

    for (auto val1 : vec)
    {
        string labels1, brackets1;
        labels1     = read_file(SEQ(val1));
        brackets1   = read_file(FOLD(val1));

        rna_tree rna1(brackets1, labels1, val1);

        for (auto val2 : vec)
        {
            if (val1 == val2)
                continue;

            string labels2, brackets2;
            labels2     = read_file(SEQ(val2));
            brackets2   = read_file(FOLD(val2));

            rna_tree rna2(brackets2, labels2, val2);

            auto map = mapping::compute_mapping(rna1, rna2);

            sort(map.begin(), map.end(), [](mapping m1, mapping m2) { return m1.from < m2.from; });

            string fileOut = MAP(val1, val2);
            ofstream out(fileOut);

            DEBUG("^^ mapping-> %s", fileOut.c_str());

            for (auto val : map)
                out << val.from << "\t" << val.to << endl;

            assert(!out.fail());
        }
    }
}

/* static */ void generator::generate_files()
{
    APP_DEBUG_FNAME;

    generate_ps_files();
    generate_seq_files();
    generate_fold_files();
    generate_mapping();
}














void save_doc(const document& doc, const std::string& file)
{
    APP_DEBUG_FNAME;
    ofstream out(file);

    typedef rna_tree::pre_post_order_iterator pre_post_it;

    auto format_string = [](pre_post_it it)
    {
        stringstream out;
        size_t index;
        if (it.is_preorder() || rna_tree::is_leaf(it))
            index = 0;
        else
            index = 1;

        auto label = it->get_label().labels.at(index);

        out
            << "("
            << label.label
            << ") "
            << label.point.x
            << " "
            << label.point.y
            << " lwstring";

        return out.str();
    };

    out << doc.prolog << endl;
    for (auto it = ++doc.rna.begin_pre_post();
            ++pre_post_it(it) != doc.rna.end_pre_post(); ++it)
    {
        //cout << format_string(it) << endl;
        out << format_string (it) << endl;
    }
    out << doc.epilog << endl;
    assert(!out.fail());
}


