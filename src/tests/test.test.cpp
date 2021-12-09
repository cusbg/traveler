/*
 * File: test.test.cpp
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


#include "test.test.hpp"
#include "types.hpp"
#include "point.test.hpp"
#include "rna_tree.test.hpp"
#include "compact_circle.test.hpp"
#include "gted.test.hpp"
#include "rted.test.hpp"
#include "overlap_checks.test.hpp"
#include "utils.test.hpp"
#include "mprintf.test.hpp"

using namespace std;

/* static */ void test::run_tests()
{
    LOGGER_PRIORITY_ON_FUNCTION(ERROR);
    APP_DEBUG_FNAME;

    std::vector<test*> vec = {
        new test_point(),
        new rna_tree_test(),
        new compact_circle_test(),
        new gted_test(),
        new rted_test(),
        new overlap_checks_test(),
        new utils_test(),
        new mprinf_test(),
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
    LOGGER_PRIORITY_ON_FUNCTION(DEBUG);
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
}

void test::add_failed(const std::string& msg)
{
    //logger.emerg("fail msg: %s", msg.c_str());
    failed_tests.push_back(msg);
}

void test::test_ok()
{
    APP_DEBUG_FNAME;
    DEBUG("TEST #%i in %s OK", test_number, test_name.c_str());
}


