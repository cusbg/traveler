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

#include <iomanip>
#include <unistd.h>

#include "logger.hpp"
#include "types.hpp"


using namespace std;

#define LOG_FILE "build/logs/program.log"

/* global */
class logger logger(LOG_FILE, logger::INFO);



logger::logger(
                const std::string& filename,
                priority priority)
{
    p = priority;
    out.push_back(stdout);
    FILE* f = fopen(filename.c_str(), "a");

    assert_err(f != nullptr && !ferror(f),
            "cannot open file '%s' to log in", to_cstr(filename));

    out.push_back(f);

    for (FILE *f : out)
        setvbuf(f, NULL, _IOFBF, 0);
}

logger::~logger()
{
    for (FILE* f : out)
        if (!contains({stdout, stderr}, f))
            fclose(f);
}

void logger::log(
                priority p,
                const std::string& text)
{
    if (!can_log(p))
        return;

    for (FILE* f : out)
    {
        fprintf(f, "%s%s\n", message_header(p).c_str(), text.c_str());
        fflush(f);
    }
    check_errors();
}

string logger::message_header(
                priority p) const
{
    size_t hour, minute, second, millisecond, cputacts;
    timespec cputime, clocks;
    std::ostringstream stream;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cputime);
    clock_gettime(CLOCK_REALTIME, &clocks);
    tm c = *localtime(&clocks.tv_sec);

    hour = c.tm_hour;
    minute = c.tm_min;
    second = c.tm_sec;
    millisecond = clocks.tv_sec / 1000000;
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
        << setfill(' ') << setw(9) << left
        << cputacts
        << ' '
        << setfill(' ') << setw(10) << left
        << ("<" + to_string(getpid()) + ">")
        << ' '
        << setfill(' ') << setw(8) << left
        << ("[" + to_string(p) + "]");

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
    l.log(p, stream.str());
    stream.str("");
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


