/*
 * File: rted.test.hpp
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

#ifndef RTED_TEST_HPP
#define RTED_TEST_HPP

#include "test.test.hpp"
#include "rted.hpp"

class rted_test : public test
{
public:
    rted_test();
    virtual ~rted_test() = default;
    virtual void run();

private:
    template<typename funct>
    void test_rted(
                const std::string& b1,
                const std::string& l1,
                const std::string& b2,
                const std::string& l2,
                funct test_funct);
};

#endif /* !RTED_TEST_HPP */

