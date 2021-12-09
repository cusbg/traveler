/*
 * File: extractor.hpp
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

#ifndef EXTRACTOR_HPP
#define EXTRACTOR_HPP

#include <string>
#include <vector>
#include <memory>
#include <regex>

#include "point.hpp"

#define BASE_REGEX      "([A-Za-z])"
#define DOUBLE_REGEX    "(-?[0-9]+(\\.[0-9]+)?)"


class extractor;

typedef std::unique_ptr<extractor> extractor_ptr;

/**
 * extracts labels and points from document
 */
class extractor
{
public:
    /**
     * returns extractor with `type`, run extraction from file `filename`
     */
    static extractor_ptr get_extractor(
                                       const std::string& filename,
                                       const std::string& type);
protected:
    /**
     * run extraction
     */
    virtual void extract(
                         const std::string& filename) = 0;
    virtual std::string get_type() const = 0;
    std::regex create_regex(
                            const std::string& pattern);

    void mirror_y();
    
private:
    static std::vector<extractor_ptr> get_all_extractors();
    
public:
    std::string labels;
    std::vector<std::string> numbering_labels;
    std::vector<point> points;


    /* gives the extractor the possibility to adjust list of residues which were
     * extracted. This can be used by the CRW extractor to remove residues which
     * matched the regex, but are part of metadata and not the data themselves*/
    virtual void adjust_residues_lists(const int cnt){};
};

#endif /* !EXTRACTOR_HPP */
