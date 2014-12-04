/*
 * File: tests.cpp
 *
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
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

#include "tests.hpp"

using namespace std;
void rted_test1()
{
    APP_DEBUG_FNAME;

    string b1, l1, b2, l2;
    l2 = l1 = LABELS1;
    b2 = b1 = BRACKETS1;
    
    rna_tree rna1(b1, l1);
    rna_tree rna2(b2, l2);

    rted r(rna1, rna2);
    auto val = r.run_rted();


    auto t1_id = rted::tree_type::child(rna1.tree_ptr->begin(), 0)->get_id();
    auto t2_id = rted::tree_type::child(rna2.tree_ptr->begin(), 0)->get_id();

    assert(val.at(t1_id).at(t2_id).second == path_strategy::right);
}

void rted_test2()
{
    APP_DEBUG_FNAME;

    string b1, l1, b2, l2;
    l1 = LABELS21;
    b1 = BRACKETS21;
    l2 = LABELS22;
    b2 = BRACKETS22;
    
    rna_tree rna1(b1, l1);
    rna_tree rna2(b2, l2);

    rted r(rna1, rna2);
    auto val = r.run_rted();


    auto t1_id = rted::tree_type::child(rna1.tree_ptr->begin(), 0)->get_id();
    auto t2_id = rted::tree_type::child(rna2.tree_ptr->begin(), 0)->get_id();

    assert(val.at(t1_id).at(t2_id).second == path_strategy::left);
}

void rted_tests()
{
    APP_DEBUG_FNAME;
    logger.info("BEGIN tests");
    auto priority = logger.getPriority();
    logger.setPriority(log4cpp::Priority::WARN);


    rted_test1();
    rted_test2();

    logger.setPriority(priority);
    logger.info("END tests");
}





