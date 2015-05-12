/*
 * File: generator.cpp
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

#include "generator.hpp"
#include "types.hpp"

#include "util.hpp"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "app.hpp"
#include <unistd.h>
#include "macros.hpp"

using namespace std;


// GENERATOR: 

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
            ERR("FILE %s does not exist", fileIn.c_str());
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
            ERR("FILE %s does not exist", fileIn.c_str());
            abort();
        }
        system(command.c_str());
        assert(exist_file(fileOut));
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

            sort(map.map.begin(), map.map.end(), [](mapping_pair m1, mapping_pair m2) { return m1.from < m2.from; });

            string fileOut = MAP(val1, val2);
            ofstream out(fileOut);

            out << map.distance << endl;
            for (auto val : map.map)
                out << val.from << "\t" << val.to << endl;

            assert(!out.fail());
        }
    }
}

/* static */ void generator::generate_files()
{
    APP_DEBUG_FNAME;

    generate_ps_files();
    //generate_in_human();
    //generate_in_mouse();
    generate_seq_files();
    generate_fold_files();
    generate_mapping();
}

// GENERATOR END

