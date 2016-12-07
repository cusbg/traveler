/*
 * File: varna_extractor.cpp
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


#include <fstream>

#include "varna_extractor.hpp"
#include "types.hpp"

#define PROPERTY_BEG_END "['\"]"
#define property(prop) (msprintf("%s%s%s", PROPERTY_BEG_END, prop, PROPERTY_BEG_END))

using namespace std;

void varna_extractor::extract(
                const std::string& filename)
{
    APP_DEBUG_FNAME;

    labels.clear();
    points.clear();

    ifstream in(filename);

    regex regexp_base_line = create_regex(
            msprintf("^<text.*\\s+x=%s.*\\s+y=%s\\s+.*>%s</text>",
                property(DOUBLE_REGEX), property(DOUBLE_REGEX), BASE_REGEX));

    smatch match;
    string line;
    point p;
    string base;

    while(true)
    {
        getline(in, line);
        if (in.fail())
            break;

        if (regex_search(line, match, regexp_base_line))
        {
            stringstream str;

            str << match[1] << " " << match[3] << " " << match[5];

            str
                >> p.x
                >> p.y
                >> base;

            assert(!str.fail() && str.eof() && base.size() == 1);

            labels.push_back(base[0]);
            points.push_back(-p);
        }
    }
}

