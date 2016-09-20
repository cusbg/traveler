/*
 * File: extractor.cpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard@ba30.eu>
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

#include <map>

#include "extractor.hpp"
#include "ps_extractor.hpp"
#include "types.hpp"

using namespace std;

typedef map<string, extractor_ptr> extractor_map_type;

static extractor_map_type create_extractors()
{
    extractor_map_type map;
    map["ps"] = unique_ptr<ps_extractor>(new ps_extractor());

    return map;
}

/* static */ extractor_ptr extractor::get_extractor(
                const std::string& docfile,
                const std::string& doctype)
{
    extractor_map_type map = create_extractors();

    extractor_map_type::iterator it = map.find(doctype);

    if (it == map.end())
    {
        throw illegal_state_exception("Document type '%s' is not supported", doctype);
    }

    extractor_ptr& extractor = it->second;
    extractor->extract(docfile);

    return std::move(extractor);
}


