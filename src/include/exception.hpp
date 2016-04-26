/*
 * File: exception.hpp
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

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <stdexcept>

class my_exception : public std::exception
{
public:
    virtual const char* what() const noexcept;

protected:
    my_exception(
                const std::string& _msg);
    virtual ~my_exception() noexcept = default;

private:
    std::string msg;
};


class abort_exception : public my_exception
{
public:
    virtual ~abort_exception() noexcept = default;
    abort_exception(
                int line,
                const std::string& file,
                const std::string& fname);
};


class assert_exception : public my_exception
{
public:
    virtual ~assert_exception() noexcept = default;
    assert_exception(
                const std::string& condiniton,
                int line,
                const std::string& file,
                const std::string& fname);
};


#endif /* !EXCEPTION_HPP */

// should always redefine, e.g. assert(), abort(), ..

#define abort() \
    throw abort_exception(__LINE__, __FILE__, __PRETTY_FUNCTION__);

#undef assert
#define assert(boolean) \
    { \
        if (!(boolean)) \
        { \
            throw assert_exception(#boolean, __LINE__, __FILE__, __PRETTY_FUNCTION__); \
        } \
    }

#define assert_err(boolean, ...) \
    { \
        if (!(boolean)) \
        { \
            ERR(__VA_ARGS__); \
            throw assert_exception(#boolean, __LINE__, __FILE__, __PRETTY_FUNCTION__); \
        } \
    }


