/*
 * File: logger.cpp
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

#include <cstdlib>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <sys/types.h>
#include <unistd.h>

#include "logger.hpp"


using namespace std;
using namespace chrono;

/* global */
class logger logger;



logger::logger()
{
    string file = "build/logs/program.log";
    FILE * f = fopen(file.c_str(), "a");
    out.push_back(stdout);

    if (f == nullptr || ferror(f))
    {
        error("cannot open file '%s' to log in", file.c_str());
        abort();
    }

    out.push_back(f);

    debug("*****************************************");
    debug("************ RUNNING PROGRAM ************");
    debug("*****************************************");

    //p = WARN;
    p = INFO;
    //p = DEBUG;
}

logger::~logger()
{
    for (FILE* f : out)
        fclose(f);
}

inline const char* to_cstr(
                logger::priority p)
{
#define t(p) logger::priority::p
    switch (p)
    {
        case t(DEBUG):
            return "DEBUG";
        case t(INFO):
            return "INFO";
        case t(WARN):
            return "WARN";
        case t(ERROR):
            return "ERROR";
        case t(EMERG):
            return "EMERG";
        default:
            abort();
    }
#undef t
}

void logger::log(
                priority p,
                const char* msg,
                va_list va)
{
    if (!can_log(p))
        return;

    for (FILE* f : out)
    {
        va_list copy;
        va_copy(copy, va);

        fprintf(f, "%s", message_header(p).c_str());
        vfprintf(f, msg, copy);
        fprintf(f, "\n");
        fflush(f);

        va_end(copy);
    }

    check_errors();
}

string logger::message_header(
                priority p)
{
    auto now = chrono::system_clock::now().time_since_epoch();

    size_t hour, minute, second, millisecond, cputacts;
    timespec cputime;
    std::ostringstream stream;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cputime);

    hour = ((size_t)chrono::duration_cast<chrono::hours>(now).count()) % 24 + 1;
    minute = ((size_t)duration_cast<minutes>(now).count()) % 60;
    second = ((size_t)duration_cast<seconds>(now).count()) % 60;
    millisecond = ((size_t)duration_cast<milliseconds>(now).count()) % 1000;
    cputacts = cputime.tv_sec * 1000000LL + cputime.tv_nsec / 1000;

    // PATTERN:
    //  %TIME% %CPUTACTS% <%PID%> [%PRIORITY%] %MESSAGE%
    //
    stream
        << setfill('0') << setw(2)
        << hour
        << ':'
        << setfill('0') << setw(2)
        << minute
        << ':'
        << setfill('0') << setw(2)
        << second
        << ':'
        << setfill('0') << setw(3)
        << millisecond
        << ' '
        << cputacts
        << " <"
        << (int)getpid()
        << ">\t["
        << to_cstr(p)
        << "]\t";

    return stream.str();
}

void logger::check_errors()
{
    for (FILE* f : out)
    {
        if (ferror(f))
        {
            for (FILE* f : out)
                fprintf(f, "LOGGER FILE OUTPUT ERROR\n");
            abort();
        }
    }
}




logger::logger_stream::logger_stream(
                logger& _l,
                priority _p)
    : l(_l), p(_p)
{ }

logger::logger_stream::~logger_stream()
{
    flush();
}

logger::logger_stream::logger_stream(
        const logger_stream& other)
    : l(other.l), p(other.p)
{
    stream << other.stream.str();
}

void logger::logger_stream::flush()
{
    if (!l.can_log(p))
        return;
    l.log(p, "%s", stream.str().c_str());
}


