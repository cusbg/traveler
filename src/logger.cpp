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

#include "logger.hpp"


#define LOGGER_PRIORITY_FUNCTION(priority) \
    void logger::priority(const char* msg, ...)
#define LOGGER_PRIORITY_STREAM_FUNCTION(priority) \
    logger::logger_stream logger::priority ##_stream()



using namespace std;
using namespace chrono;


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
    //                    HH : MM : SS : mm [prior]
#define PRINT_PATTERN   "%02lu:%02lu:%02lu:%03lu %lu:\t[%s]\t"

    auto now = chrono::system_clock::now().time_since_epoch();

    size_t hour, minute, second, millisecond, cputacts;
    timespec cputime;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cputime);

    hour = ((size_t)chrono::duration_cast<chrono::hours>(now).count()) % 24 + 1;
    minute = ((size_t)duration_cast<minutes>(now).count()) % 60;
    second = ((size_t)duration_cast<seconds>(now).count()) % 60;
    millisecond = ((size_t)duration_cast<milliseconds>(now).count()) % 1000;
    cputacts = cputime.tv_sec * 1000000LL + cputime.tv_nsec / 1000;

    for (FILE* f : out)
    {
        va_list copy;
        va_copy(copy, va);

        fprintf(f, PRINT_PATTERN,
                hour,
                minute,
                second,
                millisecond,
                cputacts,
                to_cstr(p));

        vfprintf(f, msg, copy);

        fprintf(f, "\n");

        va_end(copy);
    }

#undef PRINT_PATTERN
}

logger::logger()
{
    string file = "build/logs/program.log";
    FILE * f = fopen(file.c_str(), "a");
    out.push_back(stdout);

    if (f == nullptr ||
            ferror(f))
    {
        error("cannot open file %s",
                file.c_str());
        abort();
    }
    else
        out.push_back(f);
}

class logger logger;

