/*
 * File: types.hpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard.elias@matfyz.cz>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */

#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>
#include <algorithm>

#include <math.h>

#include "logger.hpp"

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

inline std::string to_string(
                bool value)
{
    return value ? "true" : "false";
}

template <typename T>
inline std::string to_string(
                const T& t)
{
    // pri funkcii operator<<() to nefunguje!!
    // funkcia sa pouziva aj pri operator<<(bool/char[]/..)
    // takze treba napisat vsade operator<< a to_string() bude fungovat ok
    std::stringstream str;
    str << t;
    return str.str();
}

#define to_cstr(val) to_string(val).c_str()

template <typename container_type, typename value_type>
inline bool contains(
                const container_type& container,
                const value_type& value)
{
    return std::find(std::begin(container), std::end(container), value) != std::end(container);
}

template <typename T, typename value_type>
inline bool contains(
                const std::initializer_list<T>& container,
                const value_type& value)
{
    return contains(std::vector<T>(container), value);
}

template <typename T>
inline bool operator!=(
                const T& t1,
                const T& t2)
{
    return !(t1 == t2);
}


void wait_for_input();

#define WAIT \
        { \
            WARN("WAIT: %s: %lu", __PRETTY_FUNCTION__, __LINE__); \
            wait_for_input(); \
        }


struct logger_end_of_function_priority
{
private:
    logger_end_of_function_priority(
                logger::priority new_priority);
public:
    static logger_end_of_function_priority with_priority(
                logger::priority new_priority);
    static logger_end_of_function_priority with_at_least(
                logger::priority minimal_priority);

    ~logger_end_of_function_priority();

private:
    logger::priority old_priority;
};

struct print_class_BEG_END_name
{
    print_class_BEG_END_name(
                const std::string& _name);
    ~print_class_BEG_END_name();

private:
    std::string fname;
};


#define LOGGER_PRIORITY_ON_FUNCTION(PRIORITY) \
    logger_end_of_function_priority __logger_priority(logger_end_of_function_priority::with_priority(logger::PRIORITY))

#define LOGGER_PRIORITY_ON_FUNCTION_AT_LEAST(PRIORITY) \
    logger_end_of_function_priority __logger_priority(logger_end_of_function_priority::with_at_least(logger::PRIORITY))


#define APP_DEBUG_FNAME \
    print_class_BEG_END_name __function_name(__PRETTY_FUNCTION__)

#define LOGGER_PRINT_CONTAINER(container, name) \
        { \
            auto stream = logger.debug_stream(); \
            stream << (name) << ":\n"; \
            for (auto __value : container) \
                stream << __value << " "; \
        }

#endif /* !TYPES_HPP */

/* always include and redefine assert and abort macros */
#include "exception.hpp"

