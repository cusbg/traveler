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
#include <memory>

#include "extractor.hpp"
#include "ps_extractor.hpp"
#include "types.hpp"

using namespace std;

typedef map<string, shared_ptr<extractor>> extractor_map_type;

static extractor_map_type create_extractors()
{
    extractor_map_type map;
    map["ps"] = make_shared<ps_extractor>();

    return map;
}

/* static */ extractor& extractor::get_extractor(
                const std::string& docfile,
                const std::string& doctype)
{
    static extractor_map_type map = create_extractors();

    extractor_map_type::iterator it = map.find(doctype);

    assert_err(it != map.end(), "type '%s' is not supported", to_cstr(doctype));

    extractor& extractor = *it->second;
    extractor.extract(docfile);

    return extractor;
}


