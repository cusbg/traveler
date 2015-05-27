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



void app::transform(
                const rna_tree& rna1,
                const rna_tree& rna2,
                const mapping& map,
                const std::string& fileIn,
                const std::string& fileOut)
{
    APP_DEBUG_FNAME;
    DEBUG("in: %s, out: %s", fileIn.c_str(), fileOut.c_str());
    assert(is_sorted(map.map.begin(), map.map.end(), [](mapping_pair m1, mapping_pair m2) { return m1.from < m2.from; }));

    rna_tree::post_order_iterator it;
    document doc = read_ps(fileIn);
    psout = ps::init(fileOut);
    psout.print_to_ps(doc.prolog);

    doc.rna = rna1;
    doc.update_rna_points();
    rna_tree& rna = doc.rna;

    vector<size_t> to_insert, to_remove;
    for (auto m : map.map)
    {
        if (m.from == 0)
            to_insert.push_back(m.to);
        if (m.to == 0)
            to_remove.push_back(m.from);
    }

    DEBUG("insert %lu, remove %lu, s1 %lu, s2 %lu",
            to_insert.size(), to_remove.size(),
            rna.size(), rna2.size());

    modify(doc.rna, rna2, map);
    remove(doc.rna, map);
    //erase (doc.rna);
    //save_doc(doc);
    //return;
    //save_doc(doc);
    print_deleted(doc.rna);
    print_other(doc.rna);
    rna_tree other = rna2;
    insert(doc.rna, other, map);
    print_inserted(doc.rna);
    //save_doc(doc);

    DEBUG("document %s was saved", fileOut.c_str());
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

bool app::contains(const rna_tree& rna, rna_pair_label::label_status status)
{
    return find_if(rna.begin(), rna.end(),
            [&](rna_node_type node) { return node.get_label().status == status; }) != rna.end();
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

void app::remove(rna_tree& rna, const mapping& map)
{
    APP_DEBUG_FNAME;

    rna.print_tree();
    auto it = rna.begin_post();
    size_t i = 0;
    for (auto m : map.map)
    {
        if (m.from == 0)
            continue;
        if (m.to == 0)
        {
            it = rna.remove(it);
            ++i;
        }
        ++it;
    }
    DEBUG("zmazanych %lu vrcholov", i);
    rna.print_tree();
}

void app::erase(rna_tree& rna)
{
    APP_DEBUG_FNAME;

    rna.print_tree();
    size_t i = 0;
    for (auto it = rna.begin_post(); it != rna.end_post(); ++it)
        if (it->get_label().status == rna_pair_label::deleted)
        {
            it = --rna.erase(it);
            ++i;
        }
    DEBUG("zmazanych %lu vrcholov", i);
    rna.print_tree();
}

void app::insert(rna_tree& rna1, rna_tree& rna2, const mapping& map)
{
    APP_DEBUG_FNAME;

    vector<size_t> to_insert;
    to_insert.push_back(-1);
    for (auto m : map.map)
    {
        if (m.from != 0)
            break;
        to_insert.push_back(m.to);
    }
    sort(to_insert.begin(), to_insert.end(), greater<size_t>());
    LOGGER_PRINT_CONTAINER(to_insert, "to_insert");

    size_t i = 0;
    rna_tree::post_order_iterator it1, it2;
    it1 = rna1.begin_post();
    it2 = rna2.begin_post();

    while (it1 != rna1.end_post() && it2 != rna2.end_post())
    {
        if (it1->get_label().status == rna_pair_label::deleted)
        {
            ++it1;

            continue;
        }
        if (i == to_insert.back() - 1)
        {
            DEBUG("insert %s", label_str(*it2));
            rna1.print_subtree(rna_tree::parent(it2));

            //it2 = rna2.erase(it2);
            //to_insert.pop_back();
            //++it2;
            //
            auto label = it2->get_label();
            label.status = rna_pair_label::inserted;
            label.set_points(it1->get_label());
            it2->set_label(label);
            ++it2;
            to_insert.pop_back();

        }
        else
        {
            it2->set_label(it1->get_label());
            ++it1;
            ++it2;
        }
        ++i;
    }
    cout << (rna1 == rna2) << endl;
    cout << *it1 << " " << *it2 << endl;

    assert(it1 == rna1.end_post()
            && it2 == rna2.end_post()
            && to_insert.size() == 1);
    rna1 = rna2;

    return;


/*
    rna_tree::iterator it1, it2;
    rna_tree::iterator end1, end2;
    size_t i = 0;

    it1 = rna.begin();
    it2 = temp.begin();
    end1 = rna.end();
    end2 = temp.end();

    while (it1 != end1 && it2 != end2)
    {
        if (*it1 == *it2)
        {
            ++it1;
            ++it2;
        }
        else
        {
            DEBUG("it1 = %s, it2 = %s, i = %lu", label_str(*it1), label_str(*it2), i);
            temp.print_subtree(rna_tree::parent(it2));
            rna.print_subtree (rna_tree::parent(it1));
            auto label = it2->get_label();
            label.status = rna_pair_label::inserted;
            it2->set_label(label);
            ++it2;
            //it1 = rna.insert(it1, *it2);
            //++it2;
            temp.print_subtree (rna_tree::parent(it2));
            DEBUG("it1 = %s, it2 = %s, i = %lu", label_str(*it1), label_str(*it2), i);
            ++i;
        }
    }
    assert(it1 == end1 && it2 == end2);
    DEBUG("i = %lu, to_insert = %lu", i, to_insert.size());
    DEBUG("OK");
*/
/*
    size_t i = 0;
    rna_tree::post_order_iterator it1 = rna.begin_post();
    rna_tree::post_order_iterator it2 = temp.begin_post();

    while (it1 != rna.end_post() && it2 != temp.end_post())
    {
        if (*it1 == *it2)
        {
            ++it1;
            ++it2;
        }
        else
        {
            DEBUG("it1 %s, it2 %s", label_str(*it1), label_str(*it2));
            rna.print_subtree (rna_tree::parent(it1));
            temp.print_subtree(rna_tree::parent(it2));
            it1 = rna.insert(it1, *it2);
            ++it2;
            wait_for_input();
        }
    }

    assert(it1 == rna.end_post() && it2 == temp.end_post());
*/


    //return;
/*
    auto rna2 = temp;
    size_t i = 0;
    for (auto it = rna2.begin_post(); it != rna2.end_post(); ++it)
    {
        if (i + 1 == to_insert.back())
        {
            it = --rna2.erase(it);
            to_insert.pop_back();
        }
        ++i;
    }
    rna2.print_tree();
    cout << endl;
    rna.print_tree();
    DEBUG("s1 %lu, s2 %lu", rna.size(), rna2.size());

    wait_for_input();
    auto it1 = rna.begin_post();
    auto it2 = rna2.begin_post();

    i = 0;
    while(it1 != rna.end() && it2 != rna2.end())
    {
        if (*it1 != *it2)
        {
            ERR("%s != %s, i = %lu", label_str(*it1), label_str(*it2), i);
            rna.print_subtree(rna_tree::parent(it1));
            rna2.print_subtree(rna_tree::parent(it2));
            abort();
        }
        ++it1;
        ++it2;
        ++i;
    }
    cout << (rna == rna2) << endl;*/
}






