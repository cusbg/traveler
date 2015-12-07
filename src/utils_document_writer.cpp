/*
 * File: utils_document_writer.cpp
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

#include "utils_document_writer.hpp"
#include "types.hpp"

using namespace std;



void document_writer::init(
                const std::string& filename)
{
    APP_DEBUG_FNAME;
    DEBUG("init(%s)", to_cstr(filename));
    assert(!filename.empty());

    out.close();

    // create file & truncate
    out.open(filename, ios::out);
    out.close();

    // open in normal mode
    out.open(filename, ios::out | ios::in);
    out
        << std::unitbuf
        << std::scientific;

    assert(out.good());
}

void document_writer::seek(
                streampos pos)
{
    out.seekp(pos);

    assert(out.good());
}

void document_writer::seek_end()
{
    out.seekp(0, out.end);

    assert(out.good());
}

streampos document_writer::get_pos()
{
    streampos pos = out.tellp();
    assert(pos != -1);

    return pos;
}

size_t document_writer::fill(char ch)
{
    streampos pos, end;

    pos = get_pos();
    seek_end();
    end = get_pos();

    size_t n = end - pos;

    if (n != 0)
    {
        seek(pos);
        out
            << string(n - 1, ch)
            << endl;
    }
    seek(pos);
    return n;
}


