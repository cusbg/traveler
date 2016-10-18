/*
 * File: varna_extractor.hpp
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */


#ifndef VARNA_EXTRACTOR_HPP
#define VARNA_EXTRACTOR_HPP

#include "extractor.hpp"

/**
 * varna SVG document extractor
 */
class varna_extractor : public extractor
{
protected:
    virtual void extract(
                const std::string& filename);
    virtual std::string get_type() const
    {
        return "varna";
    }
};


#endif /* !VARNA_EXTRACTOR_HPP */

