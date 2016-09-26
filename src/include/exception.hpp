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
            const char* _msg_format, const Args& ... args) \
        : my_exception(_msg_format, args...) \
    { } \
    std::string get_type() const \
    { \
        return #ex_name; \
    }

class my_exception : public std::exception
{
public:
    virtual const char* what() const noexcept;
    virtual std::string get_type() const = 0;

protected:
    my_exception(
                const std::string& _msg);
    virtual ~my_exception() noexcept = default;

    template <typename ...Args>
    my_exception(
                const char* _msg_format, const Args& ... args)
        : my_exception(msprintf(_msg_format, args...))
    { }
    friend std::ostream& operator<<(
                std::ostream& out,
                const my_exception& ex)
    {
        out << ex.get_type() << ": " << ex.msg;
        return out;
    }

private:
    std::string msg;
};


class abort_exception : public my_exception
{
public:
    DEFAULT_EXCEPTION_METHODS(abort_exception);
};


class assert_exception : public my_exception
{
public:
    DEFAULT_EXCEPTION_METHODS(assert_exception);
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


class wrong_argument_exception : public my_exception
{
public:
    DEFAULT_EXCEPTION_METHODS(wrong_argument_exception);
};



#endif /* !EXCEPTION_HPP */

// should always redefine, e.g. assert(), abort(), ..

#undef abort
#define abort() \
    throw abort_exception("abort() called in function %s; line %s; file %s", __PRETTY_FUNCTION__, __LINE__, __FILE__);

#undef assert
#define assert(boolean) \
    { \
        if (!(boolean)) \
        { \
            throw assert_exception("assert(%s) failed; look in function %s; line %s; file %s", #boolean, __PRETTY_FUNCTION__, __LINE__, __FILE__); \
        } \
    }


