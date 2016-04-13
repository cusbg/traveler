/*
 * File: test.cpp
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

#include "test.test.hpp"
#include "types.hpp"
#include "point.test.hpp"
#include "rna_tree.test.hpp"
#include "compact_circle.test.hpp"
#include "gted.test.hpp"
#include "rted.test.hpp"
#include "overlap_checks.test.hpp"
#include "utils.test.hpp"

using namespace std;

void run_test()
{
    logger.set_priority(logger::ERROR);
    APP_DEBUG_FNAME;

    std::vector<test*> vec = {
        new test_point(),
        new rna_tree_test(),
        new compact_circle_test(),
        new gted_test(),
        new rted_test(),
        new overlap_checks_test(),
        new utils_test(),
    };

    for (test* t : vec)
    {
        t->run();
        delete t;
    }
}


test::test(
                const std::string& test_name)
    : test_name(test_name)
{
    logger.set_priority(logger::EMERG);
}

test::~test()
{
    logger.set_priority(logger::DEBUG);

    if (failed_tests.empty())
    {
        INFO("TESTS %s: OK", test_name.c_str());
    }
    else
    {
        auto stream = logger.error_stream();
        stream
            << "TESTS "
            << test_name
            << ": FAILED:\n";

        for (auto val : failed_tests)
            stream << val << "\n";
    }
    logger.set_priority(logger::ERROR);
}

void test::add_failed_test()
{
    LOGGER_PRIORITY_ON_FUNCTION(DEBUG);

    logger.emerg("TEST %s on line %li of %s failed: %s",
            test_name.c_str(), line_number, file.c_str(), condition.c_str());

    failed_tests.push_back(file + ":" + to_string(line_number) + " - " + condition);
}

void test::add_failed_test(
                const my_exception& e)
{
    LOGGER_PRIORITY_ON_FUNCTION(DEBUG);

    logger.emerg("TEST %s on line %li of %s failed: %s",
            test_name.c_str(), line_number, file.c_str(), condition.c_str());

    failed_tests.push_back(file + ":" + to_string(line_number) + " - " + condition + ": Exception: " + e.what());
}

void test::test_assert_true(
                bool condition)
{
    if (!condition)
        add_failed_test();
}

void test::set(
                int _line_number,
                const std::string& _file,
                const std::string& _condition)
{
    line_number = _line_number;
    condition = _condition;
    file = _file;

    DEBUG("TEST line #%li: %s", line_number, condition.c_str());
}

void test::set(
                const std::string& expected,
                const std::string& value)
{
    condition = condition + ", " + expected + " == " + value;
}

void test::test_ok()
{
    APP_DEBUG_FNAME;
    DEBUG("TEST %s OK", test_name.c_str());
}


