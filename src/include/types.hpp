/*
 * File: types.hpp
 *
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
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

#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include "logger.hpp"
#include "exception.hpp"



template <typename T>
inline std::string to_string(const T& t)
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
inline bool contains(const container_type& c, const value_type& v)
{
    return std::find(std::begin(c), std::end(c), v) != std::end(c);
}

template <typename T>
inline bool operator!=(const T& t1, const T& t2)
{
    return !(t1 == t2);
}



inline void wait_for_input()
{
    logger.emerg("%s", __PRETTY_FUNCTION__);
    char ch;
    std::cin.read(&ch, 1);
}

struct logger_end_of_function_priority
{
public:
    logger_end_of_function_priority(
                logger::priority new_priority);
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


#define DEBUG(...) \
    if (logger.is_debug_enabled()) \
        logger.debug(__VA_ARGS__)
#define INFO(...) \
    if (logger.is_info_enabled()) \
        logger.info(__VA_ARGS__)
#define WARN(...) \
    logger.warn(__VA_ARGS__)
#define ERR(...) \
    logger.error(__VA_ARGS__)


#define WAIT \
        { \
            DEBUG("%s: %lu", __PRETTY_FUNCTION__, __LINE__); \
            wait_for_input(); \
        }

#define LOGGER_PRIORITY_ON_FUNCTION(PRIORITY) \
    logger_end_of_function_priority __logger_priority(logger::PRIORITY)

#define APP_DEBUG_FNAME \
    print_class_BEG_END_name __function_name(__PRETTY_FUNCTION__)

#define LOGGER_PRINT_CONTAINER(container, name) \
        { \
            std::stringstream stream; \
            for (auto __value : container) \
                stream << to_string(__value) << " "; \
            logger.debug("%s", to_cstr(stream.str())); \
        }

#endif /* !TYPES_HPP */

