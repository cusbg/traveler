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
#include "crw_extractor.hpp"
#include "varna_extractor.hpp"
#include "types.hpp"
#include "utils.hpp"

#define ERR_OLD_GCC "You need newer compiler. Actual does not support necessary regex patterns. Please, use g++ version >= 4.9.2."

using namespace std;

struct regex_exception : public my_exception
{
    regex_exception(const std::regex_error& e)
        : my_exception(msprintf("%s [error=%s]", ERR_OLD_GCC, e.what()))
    { }

    virtual ~regex_exception() noexcept = default;
    virtual std::string get_type() const
    {
        return "regex_exception";
    }
};


/* static */ std::vector<extractor_ptr> extractor::get_all_extractors()
{
    std::vector<extractor_ptr> extractors;
    for (extractor* e : std::vector<extractor*>({new crw_extractor(), new varna_extractor()}))
        extractors.push_back(extractor_ptr(e));
    return extractors;
}

/* static */ extractor_ptr extractor::get_extractor(
                const std::string& docfile,
                const std::string& doctype)
{
    extractor_ptr extractor;
    for (extractor_ptr& e : extractor::get_all_extractors())
    {
        if (e->get_type() == doctype)
        {
            swap(e, extractor);
            break;
        }
    }
    if (extractor.get() == nullptr)
        throw wrong_argument_exception("Document type '%s' is not supported", doctype);

    if (!exist_file(docfile))
        throw io_exception("Document '%s' does not exist. Cannot extract RNA structure", docfile);

    INFO("Extracting RNA structure from file %s with extractor %s", docfile, extractor->get_type());
    extractor->extract(docfile);

    if (extractor->labels.size() != extractor->points.size())
    {
        throw illegal_state_exception("Number of extracted bases does not match number of extracted points");
    }

    return extractor;
}

regex extractor::create_regex(
                const std::string& pattern)
{
    try
    {
        return regex(pattern);
    }
    catch (const std::regex_error& e)
    {
        throw regex_exception(e);
    }
}

