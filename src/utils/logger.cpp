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
#include "mprintf.hpp"


using namespace std;

#ifndef NO_LOGGING

#ifndef LOG_FILE
#define LOG_FILE "/tmp/traveler.log"
#endif


/* global */
class logger logger(LOG_FILE, logger::ERROR, {stdout});
#else
class logger logger;

logger::logger()
{
    p = priority::EMERG;
    // no logging output streams
}
#endif


logger::logger(
                const std::string& filename,
                priority priority)
    : logger(filename, priority, {})
{ }

logger::logger(
                const std::string& filename,
                priority priority,
                const std::vector<FILE*>& streams)
{
    p = priority;
    out = streams;
    FILE* f = fopen(filename.c_str(), "a");

    if (f == nullptr || ferror(f))
        throw io_exception("Cannot open log file %s", filename);

    out.push_back(f);
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
    size_t hour, minute, second, millisecond;
    timespec cputime, clocks;
    std::ostringstream stream;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cputime);
    clock_gettime(CLOCK_REALTIME, &clocks);
    tm c = *localtime(&clocks.tv_sec);

    hour = c.tm_hour;
    minute = c.tm_min;
    second = c.tm_sec;
    millisecond = clocks.tv_nsec / 1000000LL;

    // PATTERN:
    //  %TIME% [%PRIORITY%] %MESSAGE%
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
        << setfill(' ') << setw(8) << left
        << ("[" + to_string(p) + "]");

    return stream.str();
}

void logger::check_errors()
{
    for (FILE* f : out)
    {
        if (ferror(f))
            throw io_exception("Error occured while printing log messages");
    }
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
        switchcase(TRACE);
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


