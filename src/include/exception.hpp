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
#include "mprintf.hpp"

#define DEFAULT_EXCEPTION_METHODS(ex_name) \
    virtual ~ex_name() noexcept = default; \
    template <typename ...Args> \
    ex_name( \
            const char* _msg_format, Args... args) \
        : my_exception(_msg_format, args...) \
    { }

class my_exception : public std::exception
{
public:
    virtual const char* what() const noexcept;

protected:
    my_exception(
                const std::string& _msg);

    template <typename ...Args>
    my_exception(
                const char* _msg_format, Args... args)
        : my_exception(msprintf(_msg_format, args...))
    { }
    virtual ~my_exception() noexcept = default;

private:
    std::string msg;
};


class abort_exception : public my_exception
{
public:
    DEFAULT_EXCEPTION_METHODS(abort_exception);

    abort_exception(
                int line,
                const std::string& file,
                const std::string& fname);
};


class assert_exception : public my_exception
{
public:
    DEFAULT_EXCEPTION_METHODS(assert_exception);

    assert_exception(
                const std::string& condiniton,
                int line,
                const std::string& file,
                const std::string& fname);
};


class io_exception : public my_exception
{
public:
    DEFAULT_EXCEPTION_METHODS(io_exception);

    io_exception(const std::string& msg)
        : my_exception(msg)
    { }
};


class illegal_state_exception : public my_exception
{
public:
    DEFAULT_EXCEPTION_METHODS(illegal_state_exception);

    illegal_state_exception(const std::string& msg)
        : my_exception(msg)
    { }
};


class wrong_argument : public my_exception
{
public:
    DEFAULT_EXCEPTION_METHODS(wrong_argument);
};



#endif /* !EXCEPTION_HPP */

// should always redefine, e.g. assert(), abort(), ..

#undef abort
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


