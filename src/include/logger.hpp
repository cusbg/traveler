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
#include <string>
#include <sstream>


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

#define LOGGER_STREAM_FUNCTION(_fname, _priority) \
    inline logger_stream _fname ##_stream() \
    { \
        return get_stream(_priority); \
    }

#define LOGGER_FUNCTIONS(_f, _p) \
    LOGGER_PRIORITY_FUNCTION(_f, _p); \
    LOGGER_ENABLED_PRIORITY_FUNCTION(_f, _p); \
    LOGGER_STREAM_FUNCTION(_f, _p);


public:
    enum priority : char
    {
        DEBUG   = 0,
        INFO    = 1,
        WARN    = 2,
        ERROR   = 3,
        EMERG   = 4,
    };

    class logger_stream
    {
    public:
        logger_stream(
                    logger& l,
                    priority p);
        logger_stream(
                    const logger_stream& other);
        void flush();
        ~logger_stream();

        template <typename T>
        logger_stream& operator<<(
                    const T value)
        {
            stream << value;
            return *this;
        }
    private:
        logger& l;
        priority p;
        std::ostringstream stream;
    };

public:
    logger();
    ~logger();

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

protected:
    std::string message_header(
                priority p);

    void log(   priority p,
                const char* msg,
                ...)
    {
        va_list va;
        va_start(va, msg);
        log(p, msg, va);
        va_end(va);
    }

    void log(
                priority p,
                const char* msg,
                va_list va);

    inline logger_stream get_stream(
                priority p)
    {
        return logger_stream(*this, p);
    }

    void check_errors();

protected:
    inline bool can_log(
                priority other) const
    {
        return other >= p;
    }

public:
    LOGGER_FUNCTIONS(debug, DEBUG);
    LOGGER_FUNCTIONS(info,  INFO);
    LOGGER_FUNCTIONS(warn,  WARN);
    LOGGER_FUNCTIONS(error, ERROR);
    LOGGER_FUNCTIONS(emerg, EMERG);

public:
    inline std::vector<int> opened_files()
    {
        std::vector<int> vec;
        for (FILE* f : out)
            vec.push_back(fileno(f));
        return vec;
    }

protected:
    priority p;
    std::vector<FILE*> out;

#undef LOGGER_PRIORITY_FUNCTION
#undef LOGGER_ENABLED_PRIORITY_FUNCTION
#undef LOGGER_STREAM_FUNCTION
#undef LOGGER_FUNCTIONS

    friend void signal_handler(int);
};



/* global */
extern class logger logger;



#endif /* !LOGGER_HPP */

