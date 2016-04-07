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


#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>

#include "app.hpp"
#include "types.hpp"
#include "test.test.hpp"

using namespace std;


vector<string> args;

static std::ostream& operator<<(
                std::ostream& out,
                const std::vector<std::string>& vec)
{
    for (const auto& str : vec)
        out << str << endl;
    return out;
}

void signal_handler(
                int signal)
{
    ostringstream out;
    out
        << endl
        << endl
        << logger.message_header(logger::priority::EMERG)
        << "ERROR: signal "
        << to_string(signal)
        << ":"
        << strsignal(signal)
        << " caught, exiting"
        << endl
        << "Args:"
        << endl
        << args
        << endl;

    vector<int> output_fds = logger.opened_files();
    output_fds.push_back(STDERR_FILENO);

    for (int fd : output_fds)
    {
        fcntl(fd, F_SETFD, O_NONBLOCK);
        (void)(write(fd, out.str().c_str(), out.str().length()) + 1);
        // ^^ ((void) + 1)to prevent warning warn-unused-result
    }

    exit(2);
}

static void set_signal_handler()
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
    {
        if (sigaction(sig, &act, NULL) != 0)
        {
            ERR("sigaction on signal %i failed, err: %s", sig, strerror(errno));
        }
    }
}

void init()
{
    logger.set_priority(logger::DEBUG);
    set_signal_handler();
    cout << boolalpha;
    srand(1);
}

int main(int argc, char** argv)
{
#ifdef TESTS
    run_test();
    return 0;
#endif

    try
    {
        init();
        app app;
        args = vector<string>(argv, argv + argc);
        app.run(args);
    }
    catch (const exception& e)
    {
        ERR("Exception %s caught", e.what());
        return 1;
    }

    return 0;
}


