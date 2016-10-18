/*
 * File: overlap_checks.test.hpp
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


#ifndef OVERLAP_CHECKS_TEST_HPP
#define OVERLAP_CHECKS_TEST_HPP

#include "test.test.hpp"

struct point;

class overlap_checks_test : public test
{
public:
    overlap_checks_test();
    virtual ~overlap_checks_test() = default;
    virtual void run();

private:
    void test_intersection(
                point p1,
                point p2,
                bool intersects);
};


#endif /* !OVERLAP_CHECKS_TEST_HPP */

