/*
 * File: test.test.hpp
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

#ifndef TEST_TEST_HPP
#define TEST_TEST_HPP

#include "types.hpp"

class test
{
public:
    static void run_tests();

public:
    virtual ~test();
    test(const std::string& test_name);
    virtual void run() = 0;

    void add_failed(const std::string& msg);
    void test_ok();

protected:
    std::vector<std::string> failed_tests;
    std::string test_name;
    int test_number = 0;
};


#define format_failed(condition) \
    msprintf("TEST %s:%s on line %li of %s failed: %s", test_name.c_str(), test_number, __LINE__, __FILE__, #condition)

#define print_test_number() \
    DEBUG("test #%s", test_number); \
    ++test_number;

#define assert_true(condition) \
        { \
            try \
            { \
                if (!(condition)) \
                    add_failed(format_failed(condition)); \
                else \
                    test_ok(); \
            } \
            catch (const exception& e) \
            { \
                DEBUG("Catched unexpected exception"); \
                add_failed(format_failed(condition) + "Exception: " + e.what()); \
            } \
            print_test_number(); \
        }

#define assert_false(condition) \
    assert_true(!condition)

#define assert_equals(value, expected) \
        { \
            try  \
            { \
                if ((expected) != (value)) \
                    add_failed(msprintf("TEST FAILED: %s == %s \n Expected: %s \n Found: %s", #expected, #value, (expected), (value))); \
                else \
                    test_ok(); \
            } \
            catch (const exception& e) \
            { \
                DEBUG("Catched unexpected exception"); \
                add_failed(msprintf("%s (=%s) != %s (=%s)", #expected, (expected), #value, (value)) + "Exception: " + e.what()); \
            } \
            print_test_number(); \
        }

#define assert_fail(operation) \
        { \
            try  \
            { \
                DEBUG("Running operation %s", #operation); \
                { \
                    LOGGER_PRIORITY_ON_FUNCTION(EMERG); \
                    static_cast<void>((operation)); \
                } \
                add_failed(format_failed((operation) " should fail" )); \
            } \
            catch (const std::exception& e) \
            { \
                DEBUG("Catched expected exception: msg=%s", e.what()); \
                test_ok(); \
            } \
            print_test_number(); \
        }

#endif /* !TEST_TEST_HPP */

