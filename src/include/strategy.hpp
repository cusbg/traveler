/*
 * File: strategy.hpp
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

#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include <vector>
#include <ostream>

enum rted_strategy : char
{
    RTED_T1_LEFT  = 0,
    RTED_T2_LEFT  = 1,
    RTED_T1_RIGHT = 2,
    RTED_T2_RIGHT = 3,
    RTED_T1_HEAVY = 4,
    RTED_T2_HEAVY = 5,
};

struct strategy
{
    strategy() = default;
    strategy(
                int index);
    strategy(
                rted_strategy index);
    strategy(
                const std::string& text);

public:
    int to_index() const;
    bool is_left() const;
    bool is_right() const;
    bool is_heavy() const;
    bool is_T1() const;
    bool is_T2() const;

    static bool is_left(
                int index);
    static bool is_right(
                int index);
    static bool is_heavy(
                int index);
    static bool is_T1(
                int index);
    static bool is_T2(
                int index);

    inline operator size_t() const  // cast: '(size_t)strategy'
    {
        return to_index();
    }

    friend std::ostream& operator<<(
                std::ostream& out,
                strategy str);

private:
    enum _strategy
    {
        left,
        right,
        heavy
    };
    enum _tree
    {
        T1,
        T2
    };

    _strategy   str;
    _tree       tree;
    bool inited = false;
};


// rted/gted type:
typedef std::vector<std::vector<strategy>>  strategy_table_type;

std::ostream& operator<<(
                std::ostream& out,
                strategy_table_type strategies);

std::ostream& operator<<(
                std::ostream& out,
                rted_strategy str);

#endif /* !STRATEGY_HPP */

