/*
 * File: utils_ps_reader.cpp
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

#include <fstream>
#include <regex>

#include "ps_reader.hpp"
#include "utils.hpp"

using namespace std;



ps_document::ps_document(const std::string& name)
{
    APP_DEBUG_FNAME;
    assert_err(exist_file(name),
            "ps_document(%s): file does not exist", to_cstr(name));

    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    string line, str;
    ifstream in(name);
    point p;
    auto stream_pos = in.tellg();
    regex regexp_base_line(
            "^\\([A-Z]\\)\\s+"                                 //(%BASE%)
            "-?[0-9]+(\\.[0-9]+)?\\s+-?[0-9]+(\\.[0-9]+)?\\s+"  //+-%DOUBLE% +-%DOUBLE%
            "lwstring\\s*$"                                     // %LWSTRING%
            );
    auto endings = {
        "lwline",
        "lwfarc",
        "lwarc",
        //"lwstring",
        "lineto",
        "moveto",
        "setlinewidth",
        "setlinecap",
        "newpath",
        "stroke",
        "setrgbcolor",
    };
    vector<regex> ignore_regexps;
    for (string val : endings)
        ignore_regexps.push_back(regex(val + "\\s*$"));

    auto ignore_line = [&ignore_regexps](const std::string& line)
    {
        smatch match;

        for (const auto& r : ignore_regexps)
            if (regex_search(line, match, r))
                return true;
        return false;
    };
    auto is_rgb_funct = [](const std::string& line)
    {
        stringstream str(line);
        string other;
        float f;
        str >> f >> f >> f >> other;
        return 
            !str.fail() &&
            str.eof() &&
            other == "setrgbcolor";
    };
    auto is_base_line = [&regexp_base_line](const std::string& line)
    {
        smatch match;

        return regex_search(line, match, regexp_base_line);
    };

    // nacita prolog suboru
    // vsetko az po bazy s bodmi..
    while(true)
    {
        getline(in, line);
        assert(!in.fail());

        if (is_base_line(line))
        {
            // vrat sa pred nacitanie riadku...
            in.seekg(stream_pos);
            break;
        }
        stream_pos = in.tellg();

        if (ignore_line(line))
            continue;
        prolog += line + "\n";
        DEBUG("prolog '%s'", line.c_str());
    }

    // nacita bazy..
    while(true)
    {
        getline(in, line);
        assert(!in.fail());

        if (is_rgb_funct(line))
        {
            DEBUG("is_rgb_funct(%s), continue", line.c_str());
            continue;
        }
        if (ignore_line(line))
            continue;
        if (!is_base_line(line))
        {
            in.seekg(stream_pos);
            break;
        }
        stream_pos = in.tellg();

        stringstream stream(line);
        stream
            >> str
            >> p.x
            >> p.y
            >> str;

        labels.push_back(line.at(1));
        points.push_back(p);

        DEBUG("base_line '%s'", line.c_str());
    }

    // nacita epilog suboru
    // vsetko od baz az po koniec suboru
    while(true)
    {
        getline(in, line);
        if (in.fail())
            break;
        epilog += line + "\n";
        DEBUG("epilog '%s'", line.c_str());
    }
}


