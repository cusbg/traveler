/*
 * File: utils.test.cpp
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


#include <fstream>

#include "utils.test.hpp"
#include "utils.hpp"

#define TEST_FILE "/tmp/utils-test"

using namespace std;

static bool operator==(fasta f1, fasta f2)
{
    return f1.id == f2.id &&
        f1.labels == f2.labels &&
        f1.brackets == f2.brackets;
}

utils_test::utils_test()
    : test("utils")
{ }

void utils_test::run()
{
    APP_DEBUG_FNAME;

    test_exist_file();
    test_io();
    test_read_fasta_file();
}

void utils_test::test_exist_file()
{
    ofstream o(TEST_FILE);
    assert_true(o.good());

    assert_true(exist_file(TEST_FILE));
}

void utils_test::test_io()
{
    string text = create_fasta_text();
    write_file(TEST_FILE, text);

    assert_equals(read_file(TEST_FILE), text);
}

void utils_test::test_read_fasta_file()
{
    write_file(TEST_FILE, create_fasta_text());

    fasta expected = create_fasta();
    fasta f = read_fasta_file(TEST_FILE);

    assert_equals(f, expected);

    write_file(TEST_FILE, "XYZ");
    assert_fail(read_fasta_file(TEST_FILE));
}

fasta utils_test::create_fasta()
{
    fasta f;
    f.id = "FASTA_ID";
    f.labels = string(10, 'A');
    f.brackets = string(10, '.');

    return f;
}

string utils_test::create_fasta_text()
{
    fasta f = create_fasta();

    ostringstream out;
    out
        << ">"
        << f.id
        << " SHOULD SKIP THIS PART"
        << endl
        << f.labels
        << endl
        << f.brackets
        << endl;

    return out.str();
}

