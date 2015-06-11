/*
 * File: app.cpp
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

#include <fstream>

#include "util.hpp"
#include "app.hpp"
#include "rna_tree.hpp"
#include "types.hpp"
#include "macros.hpp"
#include "ps.hpp"
#include "compact_maker.hpp"


using namespace std;




void app::run_app()
{
    APP_DEBUG_FNAME;

    vector<string> vec = FILES;

    for (auto val1 : vec)
    {
        for (auto val2 : vec)
        {
            if (val1 == val2)
                continue;

            run_between(val1, val2);
        }
    }
}

void app::run_between(
                const std::string& first,
                const std::string& second)
{
    APP_DEBUG_FNAME;

    string labels1      = read_file(SEQ(first));
    string brackets1    = read_file(FOLD(first));

    string labels2      = read_file(SEQ(second));
    string brackets2    = read_file(FOLD(second));

    string fileIn       = PS_IN(first);
    string fileOut      = PS_OUT(first, second);

    psout = ps::init(fileOut);

    auto map = mapping::read_mapping_file(MAP(first, second));

    rna_tree rna1(brackets1, labels1, first);
    rna_tree rna2(brackets2, labels2, second);

    transform(rna1, rna2, map, fileIn, fileOut);
}

void app::print_default(const rna_tree& rna)
{
    APP_DEBUG_FNAME;

    psout.print_to_ps(ps::print(gray));
    for (auto it = rna.begin_pre_post(); it != rna.end_pre_post(); ++it)
    {
        psout.print_to_ps(ps::print_normal(it));
        //psout.print_to_ps(ps::print_edge(it));
    }
    psout.print_to_ps(ps::print(black));
}

size_t app::contains(const rna_tree& rna, rna_pair_label::label_status status)
{
    return count_if(rna.begin(), rna.end(),
            [status](rna_node_type node) { return node.get_label().status == status; });
}


void app::mark_removed(
                post_it it)
{
    //APP_DEBUG_FNAME;

    it->get_label().status = rna_pair_label::deleted;
}
void app::mark_inserted(
                post_it it)
{
    //APP_DEBUG_FNAME;

    it->get_label().status = rna_pair_label::inserted;
}
void app::mark_modifyied(
                post_it from,
                post_it to)
{
    //APP_DEBUG_FNAME;

    auto& label = to->get_label();
    const auto& flabel = from->get_label();

    label.set_points_exact(flabel);

    if (label.is_paired() != flabel.is_paired())
        label.status = rna_pair_label::pair_changed;
    else if (label == flabel)
        label.status = rna_pair_label::touched;
    else
        label.status = rna_pair_label::edited;
}



void app::update(
                document& doc,
                const mapping& map)
{
    APP_DEBUG_FNAME;

    auto to_ins = map.get_to_insert();
    auto to_rem = map.get_to_remove();

    {   // mark as removed.. (in rna template)
        post_it it = doc.template_rna.begin_post();
        size_t i = 0;
        for (size_t index : to_rem)
        {
            --index;
            size_t to_move = index - i;
            it = move_it_plus(it, to_move);
            mark_removed(it);
            i = index;
        }
    }
    {   // mark as inserted.. (in rna output)
        post_it it = doc.rna_out.begin_post();
        size_t i = 0;
        for (size_t index : to_ins)
        {
            --index;
            size_t to_move = index - i;
            it = move_it_plus(it, to_move);
            mark_inserted(it);
            i = index;
        }
    }

    struct
    {
        size_t inserted, deleted, modified;
    } stats = {0, 0, 0};
    
#define print_pair(x)   // vyskrtne funkciu ak je definovane..

    {   // update rna output points from rna template.. (going preorder)
        rna_tree::iterator it1, it2;
        it1 = doc.template_rna.begin();
        it2 = doc.rna_out.begin();
        while (it2 != doc.rna_out.end())
        {
            if (it1->get_label().status == rna_pair_label::deleted)
            {
                //DEBUG("deleted");
                print_pair(it1);

                ++it1;
                ++stats.deleted;
                continue;
            }
            if (it2->get_label().status == rna_pair_label::inserted)
            {
                //DEBUG("inserted");
                print_pair(it2);

                ++it2;
                ++stats.inserted;
                continue;
            }

            mark_modifyied(it1, it2);
            print_pair(it2);

            ++it1;
            ++it2;
            ++stats.modified;
        }
        assert (it1 == doc.template_rna.end() && it2 == doc.rna_out.end());
    }
#undef print_pair
    DEBUG("stats: ins %lu, del %lu, mod %lu",
            stats.inserted, stats.deleted, stats.modified);
    assert(stats.inserted == to_ins.size()
            && stats.deleted == to_rem.size());
    assert(doc.template_rna.size() + stats.inserted ==
            doc.rna_out.size() + stats.deleted);
}

void app::transform(
                const rna_tree& rna1,
                const rna_tree& rna2,
                const mapping& map,
                const std::string& fileIn,
                const std::string& fileOut)
{
    APP_DEBUG_FNAME;

    typedef mapping::mapping_pair mapping_pair;

    DEBUG("in: %s, out: %s", fileIn.c_str(), fileOut.c_str());
    assert(is_sorted(map.map.begin(), map.map.end(),
                [](mapping_pair m1, mapping_pair m2) { return m1.from < m2.from; }));

    document doc = read_ps(fileIn);
    doc.template_rna = rna1;
    doc.rna_out = rna2;
    doc.update_rna_points();

    psout.print_to_ps(doc.prolog);

    //print_default(doc.template_rna);

    update(doc, map);


    compact c(doc.rna_out);
    c.make_compact();

    return;
    //compact c(doc.rna_out);
    //c.make_compact();

    DEBUG("document %s was saved", fileOut.c_str());
}











