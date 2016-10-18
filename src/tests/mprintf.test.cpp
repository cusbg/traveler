/*
 * File: mprintf.test.cpp
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


#ifndef TEST
#define TEST
#endif

#include "mprintf.test.hpp"
#include "logger.hpp"

using namespace std;

#define MSG_FORMAT "MSG %s FORMAT %i"

template<typename T1, typename T2>
string get_printed(const T1& value1, const T2& value2)
{
    return "MSG " + to_string(value1) + " FORMAT " + to_string(value2);
}

struct test_class1
{
    friend std::ostream& operator<<(
                std::ostream& out,
                const test_class1& value);
};

std::ostream& operator<<(
                std::ostream& out,
                const test_class1& value)
{
    out << "HELLO";
    return out;
}



mprinf_test::mprinf_test()
    : test("mprintf_test")
{ }

void mprinf_test::run()
{
    APP_DEBUG_FNAME;

    test_successfull_mprintf();
    test_fail_mprintf();
}

void mprinf_test::test_successfull_mprintf()
{
    APP_DEBUG_FNAME;

#define assert_printed(val1, val2) assert_equals(msprintf(MSG_FORMAT, val1, val2), get_printed(val1, val2))
    assert_printed(1, 2);
    assert_printed("XYZ", 'a');
    assert_printed(true, false);
    assert_printed(test_class1(), 3);
    assert_equals(msprintf("%% %s", "XYZ"), "% XYZ");

#undef assert_printed
}

void mprinf_test::test_fail_mprintf()
{
    APP_DEBUG_FNAME;

    assert_fail(msprintf(MSG_FORMAT, 1));
    assert_fail(msprintf(MSG_FORMAT, 1, 2, 3, 4, "XYZ"));
}


