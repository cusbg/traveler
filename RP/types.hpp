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
#include <cassert>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <utility>

using namespace std::rel_ops;

#define radians_to_degrees(x)   (x * 180 / M_PI)
#define degrees_to_radians(x)   (x * M_PI / 180)

#define ARRAY_LENGTH(x)     ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define double_equals_precision(val1, val2, precision) \
    (abs(val1 - val2) < abs(precision))
#define double_equals(val1, val2) \
    double_equals_precision(val1, val2, 0.0001)



#include <log4cpp/Category.hh>
extern log4cpp::Category& logger; // globalna premenna...



inline void wait_for_input()
{
    logger.notice("%s", __PRETTY_FUNCTION__);
    char ch;
    std::cin.read(&ch, 1);
}

template <typename T>
inline T squared(T t)
{
    return t * t;
}

inline std::string to_string(bool b)
{
    return b ? "true" : "false";
}



class set_logger_priority_to_return_function
{
public:
    set_logger_priority_to_return_function(log4cpp::Priority::Value new_priority)
    {
        old_priority = logger.getPriority();
        logger.setPriority(new_priority);
    }
    ~set_logger_priority_to_return_function()
    {
        logger.setPriority(old_priority);
    }

private:
    log4cpp::Priority::Value old_priority;
};

#define DEBUG(...) \
    logger.debug(__VA_ARGS__)
#define INFO(...) \
    logger.info(__VA_ARGS__)
#define ERR(...) \
    logger.error(__VA_ARGS__)
#define WARN(...) \
    logger.warn(__VA_ARGS__)


#define LOGGER_PRIORITY_ON_FUNCTION(PRIORITY) \
    set_logger_priority_to_return_function __logger_priority(log4cpp::Priority::PRIORITY)

#define APP_DEBUG_FNAME \
    logger.debug("Entering function: %s", __PRETTY_FUNCTION__)

#define LOGGER_PRINT_CONTAINER(container, name) \
        { \
            std::stringstream stream; \
            for (auto __value : container) \
                stream << __value << " "; \
            DEBUG(name": %s", stream.str().c_str()); \
        }



#endif /* !TYPES_HPP */

