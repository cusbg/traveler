/*
 * File: exception.cpp
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include <sstream>

#include "exception.hpp"

using namespace std;

my_exception::my_exception(const std::string& _msg)
    : msg(_msg)
{ }

const char* my_exception::what() const noexcept
{
    return msg.c_str();
}


inline static std::string create_msg(
                const std::string& exception_type,
                int line,
                const std::string& file,
                const std::string& fname)
{
    ostringstream stream;
    stream
        << exception_type
        << ": in "
        << file
        << ":"
        << line
        << ":"
        << fname;
    return stream.str();
}

abort_exception::abort_exception(
                int line,
                const std::string& file,
                const std::string& fname)
    : my_exception(create_msg("abort()", line, file, fname))
{ }


assert_exception::assert_exception(
                const std::string& condition,
                int line,
                const std::string& file,
                const std::string& fname)
    : my_exception(create_msg("assert(" + condition + ") failed:", line, file, fname))
{ }



