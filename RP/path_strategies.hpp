/*
 * File: path_strategies.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard@ba30.eu>
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

#ifndef PATH_STRATEGIES_HPP
#define PATH_STRATEGIES_HPP

#include "types.hpp"

typedef char strategy_pair;

#define PATH_STRATEGY_LEFT_T1   0
#define PATH_STRATEGY_RIGHT_T1  1
#define PATH_STRATEGY_HEAVY_T1  2
#define PATH_STRATEGY_LEFT_T2   3
#define PATH_STRATEGY_RIGHT_T2  4
#define PATH_STRATEGY_HEAVY_T2  5

inline std::string graph_to_string(strategy_pair ch);
inline std::string path_to_string(strategy_pair ch);
inline std::string strategy_to_string(strategy_pair ch);

inline bool is_path_strategy(strategy_pair ch);

inline bool is_right_path(strategy_pair ch);
inline bool is_left_path (strategy_pair ch);
inline bool is_heavy_path(strategy_pair ch);

inline bool is_T1(strategy_pair ch);
inline bool is_T2(strategy_pair ch);


std::string graph_to_string(strategy_pair ch)
{
    if (is_T1(ch))
        return "T1";
    if (is_T2(ch))
        return "T2";

    throw std::invalid_argument("only T1/T2 supported");
}

std::string path_to_string(strategy_pair ch)
{
    if (is_left_path(ch))
        return "left";
    if (is_right_path(ch))
        return "right";
    if (is_heavy_path(ch))
        return "heavy";

    throw std::invalid_argument("only LRH path strategies supported");
}

std::string strategy_to_string(strategy_pair ch)
{
    return graph_to_string(ch) + "-" + path_to_string(ch);
}

bool is_path_strategy(strategy_pair ch)
{
    return is_T1(ch) || is_T2(ch);
}

bool is_right_path(strategy_pair ch)
{
    return ch == PATH_STRATEGY_RIGHT_T1 ||
        ch == PATH_STRATEGY_RIGHT_T2;
}
bool is_left_path (strategy_pair ch)
{
    return ch == PATH_STRATEGY_LEFT_T1 ||
        ch == PATH_STRATEGY_LEFT_T2;
}
bool is_heavy_path(strategy_pair ch)
{
    return ch == PATH_STRATEGY_HEAVY_T1 ||
        ch == PATH_STRATEGY_HEAVY_T2;
}

bool is_T1(strategy_pair ch)
{
    switch (ch)
    {
        case PATH_STRATEGY_LEFT_T1:
        case PATH_STRATEGY_RIGHT_T1:
        case PATH_STRATEGY_HEAVY_T1:
            return true;
        default:
            return false;
    }
}
bool is_T2(strategy_pair ch)
{
    switch (ch)
    {
        case PATH_STRATEGY_LEFT_T2:
        case PATH_STRATEGY_RIGHT_T2:
        case PATH_STRATEGY_HEAVY_T2:
            return true;
        default:
            return false;
    }
}

#endif /* !PATH_STRATEGIES_HPP */

