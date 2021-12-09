/*
 * File: mprintf.test.hpp
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

#ifndef MPRINTF_TEST_HPP
#define MPRINTF_TEST_HPP

#include "test.test.hpp"

class mprinf_test : public test
{
public:
    mprinf_test();
    virtual ~mprinf_test() = default;
    virtual void run();
private:
    void test_successfull_mprintf();
    void test_fail_mprintf();
};

#endif /* !MPRINTF_TEST_HPP */

