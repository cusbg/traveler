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

int strategy::to_index() const
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

strategy::strategy(int index)
{
    assert(index >= 0 && index < 6);

    if (is_left(index))
        str = left;
    else if (is_right(index))
        str = right;
    else
        str = heavy;

    if (is_T1(index))
        tree = T1;
    else
        tree = T2;

    inited = true;
}

strategy::strategy(
                const std::string& text)
{
    assert(text.size() == 2);
    assert(contains("LRH", text[0]) && contains("12", text[2]));

    if (text[0] == 'L')
        str = left;
    if (text[0] == 'R')
        str = right;
    if (text[0] == 'H')
        str = heavy;

    if (text[1] == '1')
        tree = T1;
    if (text[1] == '2')
        tree = T2;

    inited = true;
}

bool strategy::is_left() const
{
    assert(inited);
    return str == left;
}

bool strategy::is_right() const
{
    assert(inited);
    return str == right;
}

bool strategy::is_heavy() const
{
    assert(inited);
    return str == heavy;
}

/* static */
bool strategy::is_left(int index)
{
    assert(index >= 0 && index < 6);
    return index >= 0 && index < 2;
}

/* static */
bool strategy::is_right(int index)
{
    assert(index >= 0 && index < 6);
    return index >= 2 && index < 4;
}

/* static */
bool strategy::is_heavy(int index)
{
    assert(index >= 0 && index < 6);
    return index >= 4 && index < 6;
}

bool strategy::is_T1() const
{
    assert(inited);
    return tree == T1;
}

bool strategy::is_T2() const
{
    assert(inited);
    return tree == T2;
}

/* static */
bool strategy::is_T1(int index)
{
    assert(index >= 0 && index < 6);
    return index % 2 == 0;
}

/* static */
bool strategy::is_T2(int index)
{
    assert(index >= 0 && index < 6);
    return index % 2 == 1;
}



std::ostream& operator<<(
                std::ostream& out,
                strategy str)
{
    assert(str.inited);

    if (str.is_left())
        out << "L";
    else if (str.is_right())
        out << "R";
    else
        out << "H";

    if (str.is_T1())
        out << "1";
    else
        out << "2";

    return out;
}

std::ostream& operator<<(
                std::ostream& out, strategy_table_type strategies)
{
    // do not output root line
    for (size_t i = 0; i < strategies.size(); ++i)
    {
        for (auto val : strategies[i])
            out << val << " ";
        out << endl;
    }
    return out;
}

std::ostream& operator<<(
                std::ostream& out, rted_strategy str)
{
    out << strategy(str);
    return out;
}


