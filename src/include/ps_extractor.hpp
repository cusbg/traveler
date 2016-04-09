/*
 * File: utils_ps_reader.hpp
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

#ifndef UTILS_PS_READER_HPP
#define UTILS_PS_READER_HPP

#include "extractor.hpp"

/**
 * ps document extractor
 */
class ps_extractor : public extractor
{
protected:
    virtual void init(
                const std::string& filename);

public:
    std::string prolog;
    std::string epilog;
};


#endif /* !UTILS_PS_READER_HPP */

