/*
 * File: generator.cpp
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

#include "generator.hpp"
#include "utils.hpp"
#include "rna_tree.hpp"

using namespace std;


/* static */ void generator::generate_files()
{
    APP_DEBUG_FNAME;

    if (!generate())
        return;

    force_generate();
}

/* static */ void generator::force_generate()
{
    APP_DEBUG_FNAME;

    generate_seq_files();
    generate_mapping();

    INFO("generate OK");
}



/* static */ void generator::generate_seq_files()
{
    APP_DEBUG_FNAME;

    vector<string> vec = FILES;

    for (auto val : vec)
    {
        string fileIn = PS_IN(val);
        string fileOut = SEQ(val);
        string labels = ps_document(fileIn).labels;
        ofstream out(fileOut);
        out << labels;
        assert(!out.fail());
    }
    DEBUG("generate OK");
}

/* static */ void generator::generate_mapping()
{
    APP_DEBUG_FNAME;

    vector<string> vec = FILES;
    string l1, l2, b1, b2, s;
    rna_tree rna1, rna2;

    for (auto val1 : vec)
    {
        l1 = read_file(SEQ(val1));
        b1 = read_file(FOLD(val1));

        rna1 = rna_tree(b1, l1, val1);

        for (auto val2 : vec)
        {
            if (val1 == val2)
                continue;

            l2 = read_file(SEQ(val2));
            b2 = read_file(FOLD(val2));

            rna2 = rna_tree(b2, l2, val2);

            s = run_mapping(rna1, rna2);
            write_file(MAP(val1, val2), s);
        }
    }
    DEBUG("generate OK");
}

/* static */ bool generator::generate()
{
    APP_DEBUG_FNAME;

    for (auto val1 : FILES)
    {
        assert(exist_file(PS_IN(val1)));

        if (!exist_file(SEQ(val1)) ||
                !exist_file(FOLD(val1)))
            return true;
        
        for (auto val2 : FILES)
        {
            if (val1 == val2)
                continue;

            if (!exist_file(MAP(val1, val2)))
                return true;
        }
    }
    return false;
}



/* static */ string generator::run_mapping(
                rna_tree rna1,
                rna_tree rna2)
{
    APP_DEBUG_FNAME;

    stringstream str;
    string command;
    string s;

    str
        << "java -cp java_RTED util.RTEDCommandLine "
        << "--costs 1 1 0 " // del ins edit
        << "--mapping "
        << "--verbose "
        << "--trees '"
        << convert_to_java_format(rna1)
        << "' \t '"
        << convert_to_java_format(rna2)
        << "'";

    command = str.str();

    for (const auto& val : get_command_output(command))
    {
        if (val.find("distance") == val.npos &&
                val.find("->") == val.npos)
            continue;
        s += val;
    }

    return s;
}

#ifdef NODEF
/* static */ string generator::run_folder(
                const std::string& labels)
{
    APP_DEBUG_FNAME;

    stringstream str;
    string command;
    string s;

    str
        << "RNAfold --noPS << END"
        << endl
        << labels
        << endl
        << "END";

    command = str.str();

    s = get_command_output(command).at(1);
    s = s.substr(0, s.find(' '));

    logger.debugStream()
        << "fold('"
        << labels
        << "')\n= '"
        << s
        << "'";

    return s;
}

/* static */ void generator::generate_fold_files()
{
    APP_DEBUG_FNAME;

    vector<string> vec = FILES;
    string b;

    for (auto val : vec)
    {
        string lbl = read_file(SEQ(val));
        b = run_folder(lbl);
        write_file(FOLD(val), b);
    }

    DEBUG("generate OK");
}
#endif

