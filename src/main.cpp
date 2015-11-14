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
    string out;
    out += "\n\n"
        + logger.message_header(logger::priority::EMERG)
        + "ERROR: signal "
        + to_string(signal)
        + ":"
        + strsignal(signal)
        + " caught, exiting\n";

    vector<int> output_fds = logger.opened_files();
    output_fds.push_back(STDERR_FILENO);

    for (int fd : output_fds)
        (void)(write(fd, out.c_str(), out.length()) + 1);
    // ^^ ((void) + 1)to prevent warning warn-unused-result

    exit(2);
}

void set_signal_handler()
{
    APP_DEBUG_FNAME;
    struct sigaction act;
    bzero(&act, sizeof(struct sigaction));
    act.sa_handler = signal_handler;

    auto signals = {
        SIGINT,
        SIGSEGV,
        SIGABRT,
        SIGQUIT,
        SIGTERM,
    };
    for (int sig : signals)
        if (sigaction(sig, &act, NULL) != 0)
        {
            ERR("sigaction on signal %i failed, err: %s", sig, strerror(errno));
        }
}

void init()
{
    set_signal_handler();
    cout << boolalpha;
    srand(1);
}



void f();

int main(int argc, char** argv)
{
    init();
    f();
    app app;
    app.run(vector<string>(argv, argv + argc));

    return 0;
}

#include "tests.hpp"
#include "overlap_checks.hpp"
#include "write_ps_document.hpp"

void f()
{
    test t;
    t.run();

    exit(0);
}
#endif
