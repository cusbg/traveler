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
#include <vector>
#include <sstream>

#include <stdexcept>

template<typename Stream, typename T, typename... Args>
Stream&& mprintf(const char* format, Stream&& stream, const T value, Args... args);

template<typename Stream>
Stream&& mprintf(const char* format, Stream&& stream);

template<typename... Args>
std::string msprintf(const char* format, Args... args)
{
    return mprintf(format, std::ostringstream(), args...).str();
}

template<typename Stream, typename T, typename... Args>
Stream&& mprintf(const char* format, Stream&& stream, const T value, Args... args)
{
    while (*format != '\0')
    {
        if (*format == '%')
        {
            if (*(format + 1) == '%')
                ++format;
            else
            {
                stream << value;
                format += 2;
                return mprintf(format, std::move(stream), args...);
            }
        }
        stream << *format++;
    }
    throw std::runtime_error(mprintf("invalid number of arguments", std::ostringstream()).str());
}

template<typename Stream>
Stream&& mprintf(const char* format, Stream&& stream)
{
    while (*format != '\0')
    {
        if (*format == '%')
        {
            if (*(format + 1) == '%')
                ++format;
            else
                throw std::runtime_error(mprintf("invalid format string: missing arguments, format string '%s'",
                        std::ostringstream(),
                        format).str());
        }
        stream << *format++;
    }
    return std::move(stream);
}




class logger
{
#define LOGGER_PRIORITY_FUNCTION(_fname, _priority) \
    template<typename ...Args> \
    inline void _fname(const char* msg, Args... args) \
    { \
        mprintf(msg, get_stream(_priority), args...); \
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
        friend logger;

    protected:
        logger_stream(
                    logger& l,
                    priority p);
    public:
        logger_stream(
                    const logger_stream& other);
        ~logger_stream();

        void flush();

        template<typename T>
        logger_stream& operator<<(
                    const T& value);
    private:
        logger& l;
        priority p;
        std::ostringstream stream;
    };

public:
    logger(
                const std::string& filename,
                priority priority);
    logger(
                const std::string& filename,
                priority priority,
                std::vector<FILE*> streams);
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
                priority p) const;

    void log(
                priority p,
                const std::string& text);

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
    std::vector<int> opened_files() const;

protected:
    priority p;
    std::vector<FILE*> out;

#undef LOGGER_PRIORITY_FUNCTION_BODY
#undef LOGGER_PRIORITY_FUNCTION
#undef LOGGER_ENABLED_PRIORITY_FUNCTION
#undef LOGGER_STREAM_FUNCTION
#undef LOGGER_FUNCTIONS

    friend void signal_handler(int);
};



/* global */
extern class logger logger;



template<typename T>
logger::logger_stream& logger::logger_stream::operator<<(
                    const T& value)
{
    if (!l.can_log(p))
        return *this;

    stream << value;
    return *this;
}

std::ostream& operator<<(
                    std::ostream& out,
                    logger::priority p);


#define DEBUG(...) \
    if (logger.is_debug_enabled()) \
        ::logger.debug(__VA_ARGS__)
#define INFO(...) \
    if (logger.is_info_enabled()) \
        ::logger.info(__VA_ARGS__)
#define WARN(...) \
    ::logger.warn(__VA_ARGS__)
#define ERR(...) \
    ::logger.error(__VA_ARGS__)


#endif /* !LOGGER_HPP */

