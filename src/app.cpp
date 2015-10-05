/*
 * File: app.cpp
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

#include "app.hpp"
#include "utils.hpp"
#include "rna_tree.hpp"
#include "mapping.hpp"
#include "tree_matcher.hpp"
#include "write_ps_document.hpp"
#include "compact.hpp"


using namespace std;

void app::run_app()
{
    APP_DEBUG_FNAME;

    for (auto val1 : FILES)
    {
        for (auto val2 : FILES)
        {
            if (val1 == val2)
                continue;

            run_between(val1, val2);
        }
    }
}

void app::run_between(
                const std::string& val1,
                const std::string& val2)
{
    APP_DEBUG_FNAME;

    psout.init(PS_OUT(val1, val2));
    psout.print(ps_document(PS_IN(val1)).prolog);

    rna_tree rna1, rna2;
    mapping map;

    rna1 = get_rna(val1);
    rna2 = get_rna(val2);

    map = mapping(rna1, rna2);
    
    rna1 = matcher(rna1, rna2).run(map);

    compact(rna1).run();
}


