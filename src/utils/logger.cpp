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

#include <chrono>
#include <iomanip>
#include <unistd.h>

#include "logger.hpp"
#include "types.hpp"


using namespace std;
using namespace chrono;

#define LOG_FILE "build/logs/program.log"

/* global */
class logger logger(logger::WARN);



logger::logger(
                priority priority)
{
    p = priority;
    out.push_back(stdout);
    FILE* f = fopen(LOG_FILE, "a");

    assert_err(f != nullptr && !ferror(f),
            "cannot open file '%s' to log in", LOG_FILE);

    out.push_back(f);

    for (FILE *f : out)
        setvbuf(f, NULL, _IOFBF, 0);

    debug("*****************************************");
    debug("************ RUNNING PROGRAM ************");
    debug("*****************************************");
}

logger::~logger()
{
    for (FILE* f : out)
        fclose(f);
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
                priority p) const
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
        << p
        << "]\t";

    return stream.str();
}

void logger::check_errors()
{
    for (FILE* f : out)
        assert_err(!ferror(f), "LOGGER FILE OUTPUT ERROR");
}

std::vector<int> logger::opened_files() const
{
    std::vector<int> vec;
    for (FILE* f : out)
        vec.push_back(fileno(f));
    return vec;
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



std::ostream& operator<<(
                    std::ostream& out,
                    logger::priority p)
{
#define switchcase(p) \
    case logger::priority::p: \
        out << #p; \
        break;
    switch (p)
    {
        switchcase(DEBUG);
        switchcase(INFO);
        switchcase(WARN);
        switchcase(ERROR);
        switchcase(EMERG);
        default:
            abort();
    }
#undef switchcase

    return out;
}


