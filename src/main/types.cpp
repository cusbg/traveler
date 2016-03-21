/*
 * File: types.cpp
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

#include "types.hpp"

using namespace std;



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
    DEBUG("BEG function: %s", fname.c_str());
}

print_class_BEG_END_name::~print_class_BEG_END_name()
{
    DEBUG("END function: %s", fname.c_str());
}

