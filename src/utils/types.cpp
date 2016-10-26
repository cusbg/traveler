/*
 * File: types.cpp
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


#include <iostream>

#include "types.hpp"

using namespace std;


/* static */ logger_end_of_function_priority logger_end_of_function_priority::with_priority(
                logger::priority new_priority)
{
    return logger_end_of_function_priority(new_priority);
}

/* static */ logger_end_of_function_priority logger_end_of_function_priority::with_at_least(
                logger::priority minimal_priority)
{
    logger::priority p = (logger.get_priority() < minimal_priority) ? minimal_priority : logger.get_priority();

    return logger_end_of_function_priority(p);
}

logger_end_of_function_priority::logger_end_of_function_priority(
                logger::priority new_priority)
{
    old_priority = logger.get_priority();
    logger.set_priority(new_priority);
}

logger_end_of_function_priority::~logger_end_of_function_priority()
{
    logger.set_priority(old_priority);
}

print_class_BEG_END_name::print_class_BEG_END_name(
                const std::string& _fname)
    : fname(_fname)
{
    TRACE("BEG function: %s", fname);
}

print_class_BEG_END_name::~print_class_BEG_END_name()
{
    TRACE("END function: %s", fname);
}


void wait_for_input()
{
    logger.emerg("%s", __PRETTY_FUNCTION__);
    char ch;
    std::cin.read(&ch, 1);
}
