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
    void add_failed_test(const my_exception& e);

    void set(int line_number, const std::string& file, const std::string& condition);
    void set(const std::string& expected, const std::string& value);
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
            catch (const my_exception& e) \
            { \
                DEBUG("Catched unexpected exception"); \
                add_failed_test(e); \
            } \
        }

#define assert_false(condition) \
    assert_true(!condition)

#define assert_equals(value, expected) \
        { \
            set_args(#expected " == " #value); \
            set(to_string(expected), to_string(value)); \
            try  \
            { \
                bool result = (expected) == (value); \
                test_assert_true(result); \
                if (result) \
                    test_ok(); \
            } \
            catch (const my_exception& e) \
            { \
                DEBUG("Catched unexpected exception"); \
                add_failed_test(e); \
            } \
        }

#define assert_fail(operation) \
        { \
            set_args(#operation); \
            try  \
            { \
                { \
                    LOGGER_PRIORITY_ON_FUNCTION(EMERG); \
                    operation; \
                } \
                DEBUG("Expected test to fail"); \
                add_failed_test(); \
            } \
            catch (...) \
            { \
                DEBUG("Catched expected exception"); \
                test_ok(); \
            } \
        }


void run_test();

#endif /* !TEST_HPP */

