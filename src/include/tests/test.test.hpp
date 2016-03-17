/*
 * File: test.hpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard@ba30.eu>
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

#ifndef TEST_HPP
#define TEST_HPP

#include "types.hpp"

class test
{
public:
    virtual ~test();
    test(const std::string& test_name);
    virtual void run() = 0;

protected:
    void test_assert_true(bool condition);

protected:
    void add_failed_test();

    void set(int line_number, const std::string& file, const std::string& condition);
    void test_ok();


protected:
    std::vector<std::string> failed_tests;

private:
    int line_number;
    std::string condition;
    std::string test_name;
    std::string file;
};

#define set_args(str) set(__LINE__, __FILE__, str)

#define assert_true(condition) \
        { \
            set_args(#condition); \
            try \
            { \
                bool out = (condition); \
                test_assert_true(out); \
                test_ok(); \
            } \
            catch (...) \
            { \
                add_failed_test(); \
            } \
        }

#define assert_equals(val1, val2) \
        { \
            set_args(#val1 " == " #val2); \
            try  \
            { \
                bool result = (val1) == (val2); \
                INFO("%s == %s", to_cstr(val1), to_cstr(val2)); \
                test_assert_true(result); \
                test_ok(); \
            } \
            catch (...) \
            { \
                add_failed_test(); \
            } \
        }

#define assert_fail(operation) \
        { \
            set_args(#operation); \
            try  \
            { \
                operation; \
                add_failed_test(); \
            } \
            catch (...) \
            { \
                test_ok(); \
            } \
        }


void run_test();

#endif /* !TEST_HPP */

