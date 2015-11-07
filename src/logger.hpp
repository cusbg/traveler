/*
 * File: logger.hpp
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

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdarg>
#include <cstdio>
#include <vector>


class logger
{
#define LOGGER_PRIORITY_FUNCTION(_fname, _priority) \
    inline void _fname(const char* msg, ...) \
    { \
        va_list va; \
        va_start(va, msg); \
        log(priority::_priority, msg, va); \
        va_end(va); \
    }

#define LOGGER_ENABLED_PRIORITY_FUNCTION(_fname, _priority) \
    inline bool is_ ## _fname ## _enabled() const \
    { \
        return can_log(priority::_priority); \
    }

#define LOGGER_FUNCTION(_f, _p) \
    LOGGER_PRIORITY_FUNCTION(_f, _p); \
    LOGGER_ENABLED_PRIORITY_FUNCTION(_f, _p);

public:
    enum priority : char
    {
        DEBUG   = 0,
        INFO    = 1,
        WARN    = 2,
        ERROR   = 3,
        EMERG   = 4,
    };

    class logger_stream;
public:
    logger();

private:
    void log(
                priority p,
                const char* msg,
                va_list va);

    inline bool can_log(
                priority other) const
    {
        return other >= p;
    }

public:
    LOGGER_FUNCTION(debug, DEBUG);
    LOGGER_FUNCTION(info,   INFO);
    LOGGER_FUNCTION(warn,   WARN);
    LOGGER_FUNCTION(error,  ERROR);
    LOGGER_FUNCTION(emerg,  EMERG);

public:
    inline priority get_priority() const
    {
        return p;
    }
    inline void set_priority(
                priority other)
    {
        p = other;
    }

private:
    priority p = priority::DEBUG;
    std::vector<FILE*> out;

#undef LOGGER_PRIORITY_FUNCTION
#undef LOGGER_PRIORITY_STREAM_FUNCTION
#undef LOGGER_FUNCTION
};

extern logger logger;



#endif /* !LOGGER_HPP */

