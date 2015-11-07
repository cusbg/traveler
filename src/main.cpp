/*
 * File: main.cpp
 *
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
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

//#define TESTS

#ifdef TESTS

#include "tests/tests.cpp"

#else

#include <iostream>
#include <fstream>

#include "types.hpp"
#include "utils.hpp"
#include "app.hpp"
#include "rna_tree.hpp"
#include "mapping.hpp"

#include "point.hpp"
#include "rted.hpp"
#include "gted.hpp"




#include <csignal>
#include <unistd.h>
#include <strings.h>
#include <string.h>

using namespace std;

void signal_handler(int signal)
{
    // TODO: lepsi handler
    string out =
        "signal "
        + to_string(signal)
        + ":"
        + strsignal(signal)
        + " caught, exiting\n";

    write(2, out.c_str(), out.length());
    exit(2);
}

void set_signal_handler()
{
    APP_DEBUG_FNAME;
    struct sigaction act;
    bzero(&act, sizeof(struct sigaction));
    act.sa_handler = signal_handler;

    for (int i = 0; i < 40; ++i)
        sigaction(i, &act, NULL);
}


void generate_seq_from_ps(
                const std::string& psfile,
                const std::string& seqfile)
{
    ps_document doc(psfile);
    write_file(seqfile, doc.labels);
}
void generate_seq()
{
    std::vector<std::string> vec({"human", "rabbit", "frog", "mouse"});
    for (auto val : vec)
        generate_seq_from_ps("precomputed/" + val + ".ps", "precomputed/" + val + ".seq");
    abort();
}


int main(int argc, char** argv)
{
    set_signal_handler();
    cout << boolalpha;
    srand(1);
    //LOGGER_PRIORITY_ON_FUNCTION(INFO);

    app app;
    app.run(vector<string>(argv, argv + argc));

    return 0;
}

#endif
