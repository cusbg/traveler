/*
 * File: mprintf.hpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard@ba30.eu>
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

#ifndef MPRINTF_HPP
#define MPRINTF_HPP

#include <sstream>
#include <stdexcept>



// Printf functions with varargs of T type

/**
 * printf using variadic c++ template
 */
template<typename Stream, typename T, typename... Args>
Stream&& mprintf(const char* format, Stream&& stream, const T& value, Args... args);

/**
 * printf using variadic c++ template
 */
template<typename Stream>
Stream&& mprintf(const char* format, Stream&& stream);

/**
 * printf using variadic c++ template
 */
template<typename... Args>
std::string msprintf(const char* format, Args... args)
{
    return mprintf(format, std::ostringstream(), args...).str();
}

/**
 * how to print value to stream
 */
template<typename Stream, typename T>
void print(Stream& stream, const T& value)
{
    stream << value;
}

/**
 * specialization how to print bool value to stream - use true/false instead of 1/0
 */
template<typename Stream>
void print(Stream& stream, bool value)
{
    stream << (value == true ? "true" : "false");
}

template<typename Stream, typename T, typename... Args>
Stream&& mprintf(const char* format, Stream&& stream, const T& value, Args... args)
{
    while (*format != '\0')
    {
        if (*format == '%')
        {
            if (*(format + 1) == '%')
                ++format;
            else
            {
                print(stream, value);
                format += 2;
                return mprintf(format, std::move(stream), args...);
            }
        }
        stream << *format++;
    }
    throw std::runtime_error(msprintf("invalid number of arguments: +%i more than expected", sizeof...(Args) + 1));
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
                throw std::runtime_error(
                        msprintf("invalid format string: missing arguments, format string '%s'", format));
        }
        stream << *format++;
    }
    return std::move(stream);
}

#endif /* !MPRINTF_HPP */

