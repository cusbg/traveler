/*
 * File: extractor.hpp
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

#ifndef EXTRACTOR_HPP
#define EXTRACTOR_HPP

#include <string>
#include <vector>
#include "point.hpp"

/**
 * extracts labels and points from document
 */
class extractor
{
public:
    /**
     * returns extractor with `type`, run extraction from file `filename`
     */
    static extractor& get_extractor(
                const std::string& filename,
                const std::string& type);
protected:
    /**
     * run extraction
     */
    virtual void extract(
                const std::string& filename) = 0;

public:
    std::string labels;
    std::vector<point> points;
};


#endif /* !EXTRACTOR_HPP */

