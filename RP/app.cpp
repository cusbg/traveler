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


using namespace std;


ps psout;


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

    auto map = read_mapping_file(MAP(first, second));

    rna_tree rna1(brackets1, labels1, first);
    rna_tree rna2(brackets2, labels2, second);

    transform(rna1, rna2, map, PS_IN(first), PS_OUT(first, second));
}




void posun_vrcholy(rna_tree::iterator it, Point vector)
{
    //cout << "posunutie o " << vector << endl;

    auto label = it->get_label();
    for (size_t i = 0; i < label.labels.size(); ++i)
    {
        label.labels.at(i).point = label.labels.at(i).point + vector;
    }
    it->set_label(label);

    // prejdi vsetky deti, a tie posun tiez.
    for (rna_tree::sibling_iterator sib = it.begin(); sib != it.end(); ++sib)
        posun_vrcholy(sib, vector);
}


void app::save_doc(const document& doc)
{
    APP_DEBUG_FNAME;
    
    for (auto it = ++doc.rna.begin_pre_post();
            ++pre_post_it(it) != doc.rna.end_pre_post(); ++it)
    {
        psout.print_to_ps(ps::format_string(it));
    }
}

void app::print_default(const rna_tree& rna)
{
    APP_DEBUG_FNAME;

    psout.print_to_ps(ps::print(gray));
    for (auto it = rna.begin_pre_post(); it != rna.end_pre_post(); ++it)
    {
        psout.print_to_ps(ps::print_normal(it));
        //if (it->get_label().is_paired() && !it.is_preorder())
            //psout.print_to_ps(ps::print_edge(it));
    }
    psout.print_to_ps(ps::print(black));
}



void app::modify(rna_tree& rna, const rna_tree& temp, const mapping& map)
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    APP_DEBUG_FNAME;

    auto it = rna.begin_post();
    for (auto m : map.map)
    {
        if (m.from == 0)
            continue;
        if (m.to != 0)
            it = rna.modify(it, *move_it_plus(temp.begin_post(), m.to - 1));
        ++it;
    }
}

void app::remove(rna_tree& rna, const indexes& to_remove)
{
    APP_DEBUG_FNAME;

    rna.print_tree();
    auto it = rna.begin_post();
    size_t i = 0;

    for (auto index : to_remove)
    {
        index -= 1;
        size_t to_move = index - i;
        it = move_it_plus(it, to_move);
        it = rna.remove(it);
        i = index;
    }

    DEBUG("zmazanych (soft) %lu vrcholov", to_remove.size());
    logger.debugStream() << rna.print_tree();
}

void app::erase(rna_tree& rna)
{
    APP_DEBUG_FNAME;

    rna.print_tree();
    size_t i = 0;
    for (auto it = rna.begin_post(); it != rna.end_post(); ++it)
    {
        if (it->get_label().status == rna_pair_label::deleted)
        {
            it = --rna.erase(it);
            ++i;
        }
    }
    DEBUG("zmazanych (hard) %lu vrcholov", i);
    logger.debugStream() << rna.print_tree();
}

void app::insert(rna_tree& rna1, rna_tree& rna2, const indexes& to_insert)
{
    APP_DEBUG_FNAME;

    for (auto val : to_insert)
    {
        auto it = move_it_plus(rna2.begin_post(), val-1);
        auto label = it->get_label();
        label.status = rna_pair_label::inserted;
        it->set_label(label);
    }
    size_t in_count = to_insert.size();
    size_t del_count = contains(rna1, rna_pair_label::deleted);
    DEBUG("rna1 %lu, rna2 %lu, inserted # %lu, deleted # %lu", rna1.size(), rna2.size(), in_count, del_count);
    assert(rna1.size() + to_insert.size() == rna2.size() + del_count);

    {
        rna_tree::post_order_iterator it1, it2;
        it1 = rna1.begin_post();
        it2 = rna2.begin_post();

        while(it2 != rna2.end_post())
        {
            auto label1 = it1->get_label();
            auto label2 = it2->get_label();
            if (label2.status == rna_pair_label::inserted)
            {
                cout << label2 << endl;
                ++it2;
                continue;
            }
            if (label1.status == rna_pair_label::deleted)
            {
                cout << label1 << endl;
                ++it1;
                continue;
            }

            if (label1 == label2)
            {
                cout << label1 << "\t" << label2 << endl;
                it2->set_label(it1->get_label());
                ++it1;
                ++it2;
            }
            else
            {
                cout << label1 << endl << label2 << endl;
                abort();
            }
        }
    }

    {
        rna_tree::iterator it = rna2.begin();
        while(it != rna2.end())
        {
            auto label = it->get_label();
            if (label.status == rna_pair_label::inserted)
            {
                label.set_points(rna_tree::parent(it)->get_label());
                it->set_label(label);
            }
            //print_pair(it);
            ++it;
        }
    }

    {
        rna_tree::post_order_iterator it1, it2;
        it1 = rna1.begin_post();
        it2 = rna2.begin_post();

        while (it2 != rna2.end_post())
        {
            auto label1 = it1->get_label();
            auto label2 = it2->get_label();
            if (label2.status == rna_pair_label::inserted)
            {
                //print_pair(it2);
                //cout << label2 << endl;
                //wait_for_input();
                ++it2;
                continue;
            }
            if (label1.status == rna_pair_label::deleted)
            {
                //print_pair(it1);
                //cout << label1 << endl;
                ++it1;
                continue;
            }

            assert(label1 == label2);

            //print_pair(it2);
            //cout << label1 << "\t" << label2 << endl;
            ++it1;
            ++it2;
        }
    }

    swap(rna1, rna2);
}







void app::transform(
                const rna_tree& rna1,
                const rna_tree& rna2,
                const mapping& map,
                const std::string& fileIn,
                const std::string& fileOut)
{
    APP_DEBUG_FNAME;
    DEBUG("in: %s, out: %s", fileIn.c_str(), fileOut.c_str());
    assert(is_sorted(map.map.begin(), map.map.end(),
                [](mapping_pair m1, mapping_pair m2) { return m1.from < m2.from; }));

    document doc = read_ps(fileIn);
    doc.rna = rna1;
    doc.update_rna_points();

    psout = ps::init(fileOut);
    psout.print_to_ps(doc.prolog);

    modify(doc.rna, rna2, map);
    remove(doc.rna, map.get_to_remove());

    wait_for_input();

    print_deleted(doc.rna);
    print_other(doc.rna);

    rna_tree other = rna2;
    insert(doc.rna, other, map.get_to_insert());
    assert(!contains(doc.rna, rna_pair_label::untouched));
    print_inserted(doc.rna);

    DEBUG("document %s was saved", fileOut.c_str());
}


void app::print_pair(rna_tree::iterator it)
{
    string out;
    if (it->get_label().is_paired())
    {
        rna_tree::pre_post_order_iterator it1(it);
        rna_tree::pre_post_order_iterator it2(it, false);
        out =
            ps::format_string(it1) + ps::format_string(it2);
    }
    else
        out = ps::format_string(it);

    psout.print_to_ps(out);
}

void app::print_deleted(const rna_tree& rna)
{
    APP_DEBUG_FNAME;

    for (auto it = ++rna.begin_pre_post();
            ++pre_post_it(it) != rna.end_pre_post(); ++it)
    {
        if (it->get_label().status == rna_pair_label::deleted)
            psout.print_to_ps(ps::format_string(it));
    }
}

void app::print_other(const rna_tree& rna)
{
    APP_DEBUG_FNAME;

    for (auto it = ++rna.begin_pre_post();
            ++pre_post_it(it) != rna.end_pre_post(); ++it)
    {
        if (it->get_label().status != rna_pair_label::deleted &&
                it->get_label().status != rna_pair_label::inserted)
            psout.print_to_ps(ps::format_string(it));
    }
}

void app::print_inserted(const rna_tree& rna)
{
    APP_DEBUG_FNAME;

    for (auto it = ++rna.begin_pre_post();
            ++pre_post_it(it) != rna.end_pre_post(); ++it)
    {
        if (it->get_label().status == rna_pair_label::inserted)
        {
            psout.print_to_ps(ps::format_string(it));
            //rna.print_subtree(rna.parent(it));
        }
    }
}

size_t app::contains(const rna_tree& rna, rna_pair_label::label_status status)
{
    return count_if(rna.begin(), rna.end(),
            [status](rna_node_type node) { return node.get_label().status == status; });
}


void app::make_compact(
                rna_tree& rna)
{
    typedef rna_tree::iterator iterator;

    APP_DEBUG_FNAME;

    auto vyrataj_posunutie = [&](iterator par, iterator it)
    {
        if (par.number_of_children() == 1)
            return stred(par) - stred(it);
        else
        {
            Point it_str = stred(it);
            Point other_str;
            if(!rna_tree::is_first_child(it) && !rna_tree::is_last_child(it))
            {
                Point p1, p2;
                auto sib1 = --rna_tree::sibling_iterator(it);
                auto sib2 = ++rna_tree::sibling_iterator(it);
                cout << *sib1 << " " << *sib2 << endl;
                p1 = stred(sib1);
                p2 = stred(sib2);

                other_str = stred(p1, p2);
            }
            else
            {
                DEBUG("else");
                other_str = stred(par);
            }

            return other_str - it_str;
        }
    };

    //rna_tree::post_order_iterator it = rna.begin_post();
    rna_tree::iterator it, end;
    it = rna.begin();
    end = rna.end();

    for (; it != end; ++it)
    {
        if (it->get_label().status != rna_pair_label::deleted)
        {
            //psout.print_to_ps(ps::format_string(rna_tree::pre_post_order_iterator(it, false)));
            //psout.print_to_ps(ps::format_string(it));
            continue;
        }

        Point p = stred(it);
        if (it->get_label().is_paired())
        {
            DEBUG("deleting node %s", label_str(*it));
            Point posun = vyrataj_posunutie(rna_tree::parent(it), it);
            //wait_for_input();
            posun_vrcholy(it, posun);
            it = --rna.erase(it);
            //wait_for_input();
        }
        else
            it = --rna.erase(it);

        bool printx = false;
        printx = true;
        if (printx)
        {
            stringstream str;
            str 
                << ps::print(red)
                << endl
                << "(x) "
                << p
                << " lwstring"
                << endl
                << ps::print(black)
                << endl;
            psout.print_to_ps(str.str());
        }

    }

    rna.print_tree();
    //wait_for_input();


    INFO("exit compact funct");
    //abort();
}







