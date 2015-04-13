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


#define PATH_STRATEGY_LEFT_T1   0
#define PATH_STRATEGY_RIGHT_T1  1
#define PATH_STRATEGY_HEAVY_T1  2
#define PATH_STRATEGY_LEFT_T2   3
#define PATH_STRATEGY_RIGHT_T2  4
#define PATH_STRATEGY_HEAVY_T2  5

typedef char strategy_pair;

std::string graph_to_string(strategy_pair ch);
std::string path_to_string(strategy_pair ch);
std::string strategy_to_string(strategy_pair ch);

bool is_path_strategy(strategy_pair ch);

bool is_right_path(strategy_pair ch);
bool is_left_path (strategy_pair ch);
bool is_heavy_path(strategy_pair ch);

bool is_T1(strategy_pair ch);
bool is_T2(strategy_pair ch);



#include <log4cpp/Category.hh>
extern log4cpp::Category& logger; // globalna premenna...





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

#define LOGGER_PRIORITY_ON_FUNCTION(PRIORITY) \
    set_logger_priority_to_return_function __value(log4cpp::Priority::PRIORITY)

#define APP_DEBUG_FNAME \
    logger.debug("Entering function: %s", __PRETTY_FUNCTION__)

/*
#define SUBTREE_DEBUG_PRINT(tree, iterator) \
        { \
            std::stringstream stream; \
            kptree::print_subtree_bracketed(tree, iterator, stream); \
            logger.debug("SUBTREE: %s", stream.str().c_str()); \
        }

#define TREE_DEBUG_PRINT(tree) \
        { \
            std::stringstream stream; \
            kptree::print_tree_bracketed(tree, stream); \
            logger.debug("TREE: %s", stream.str().c_str()); \
        }
*/

#define LOGGER_PRINT_CONTAINER(container, name) \
        { \
            std::stringstream stream; \
            for (auto __value : container) \
                stream << __value << " "; \
            logger.debug(name": %s", stream.str().c_str()); \
        }


#endif /* !TYPES_HPP */

