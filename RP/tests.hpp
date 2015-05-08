/*
 * File: tests.hpp
 *
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
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

#ifdef NODEF

#ifndef TESTS_HPP
#define TESTS_HPP

#include "types.hpp"


// == figure 4, str. 337
#define LABELS1      "1234565731"
#define BRACKETS1    "(.(.(.).))"

// == figure 6, str. 341
#define LABELS21     "3123"
#define BRACKETS21   "(..)"
#define LABELS22     "212"
#define BRACKETS22   "(.)"



void rted_tests();
void gted_tests();



#endif /* !TESTS_HPP */

#endif

