/*
 * File: write_document.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard.elias@matfyz.cz>
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

#ifndef WRITE_DOCUMENT_HPP
#define WRITE_DOCUMENT_HPP

#include <fstream>

class document_writer
{
public:
    typedef std::fstream::streampos             streampos;

protected:
    document_writer() = default;

public:
    /**
     * initialize new document_writer on document `filename`
     */
    void init(
                const std::string& filename);

    /**
     * fill document from actual position to end of file with `ch`-chars
     * and seek to actual position
     */
    size_t fill(char ch = ' ');

public:
    /**
     * seek to `pos`
     */
    void seek(
                streampos pos);

    /**
     * seeks to the end of file
     */
    void seek_end();

    /**
     * return actual position in stream
     */
    streampos get_pos();

protected:
    std::fstream out;
};


#endif /* !WRITE_DOCUMENT_HPP */

