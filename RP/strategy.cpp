/*
 * File: strategy.cpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard.elias@matfyz.cz>
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

#include "strategy.hpp"

using namespace std;

int strategy_type::to_index() const
{
    int out;
    bool t1 = is_T1();

    if (is_left())
        out = t1 ? RTED_T1_LEFT : RTED_T2_LEFT;
    else if (is_right())
        out = t1 ? RTED_T1_RIGHT : RTED_T2_RIGHT;
    else
        out = t1 ? RTED_T1_HEAVY : RTED_T2_HEAVY;

    return out;
}

/* static */
strategy_type strategy_type::from_index(int index)
{
    assert(index >= 0 && index < 6);

    strategy_type s;

    if (is_left(index))
        s.strategy = left;
    else if (is_right(index))
        s.strategy = right;
    else
        s.strategy = heavy;

    if (is_T1(index))
        s.tree = T1;
    else
        s.tree = T2;

    s.inited = true;

    return s;
}

bool strategy_type::is_left() const
{
    assert(inited);
    return strategy == left;
}

bool strategy_type::is_right() const
{
    assert(inited);
    return strategy == right;
}

bool strategy_type::is_heavy() const
{
    assert(inited);
    return strategy == heavy;
}

/* static */
bool strategy_type::is_left(int index)
{
    assert(index >= 0 && index < 6);
    return index >= 0 && index < 2;
}

/* static */
bool strategy_type::is_right(int index)
{
    assert(index >= 0 && index < 6);
    return index >= 2 && index < 4;
}

/* static */
bool strategy_type::is_heavy(int index)
{
    assert(index >= 0 && index < 6);
    return index >= 4 && index < 6;
}

bool strategy_type::is_T1() const
{
    assert(inited);
    return tree == T1;
}

bool strategy_type::is_T2() const
{
    assert(inited);
    return tree == T2;
}

/* static */
bool strategy_type::is_T1(int index)
{
    assert(index >= 0 && index < 6);
    return index % 2 == 0;
}

/* static */
bool strategy_type::is_T2(int index)
{
    assert(index >= 0 && index < 6);
    return index % 2 == 1;
}

std::ostream& operator<<(
                std::ostream& out,
                strategy_type str)
{
    if (!str.inited)
        return out;

    if (str.is_left())
        out << "left";
    else if (str.is_right())
        out << "right";
    else
        out << "heavy";

    out << ":";

    if (str.is_T1())
        out << "T1";
    else
        out << "T2";

    return out;
}

