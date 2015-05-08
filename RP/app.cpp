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



using namespace std;

#define FILES               {"human", "mouse", "rabbit", "frog"}
#define PS_IN(val)          "build/files/" + val + ".ps"
#define PS_OUT(val1, val2)  "build/files/" + val1 + "-" + val2 + ".out.ps"
#define SEQ(val)            "build/files/" + val + ".seq"
#define RNAFOLD(val)        "build/files/" + val + ".RNAfold.fold"
#define FOLD(val)           "build/files/" + val + ".fold"
#define FOLD_IN(val)        "../InFiles/" + val + ".fold"
#define MAP(val1, val2)     "build/files/" + val1 + "-" + val2 + ".map"

void app::run_app()
{
    APP_DEBUG_FNAME;

    vector<string> vec = FILES;

    for (auto val1 : vec)
    {
        string labels1 = read_file(SEQ(val1));
        for (auto val2 : vec)
        {
            if (val1 == val2)
                continue;

            // copy input .ps file to build-dir
            string command = "cp ../InFiles/" + val1 + ".ps " +
                                PS_IN(val1);
            system(command.c_str());

            string labels2 = read_file(SEQ(val2));
            vector<mapping> map = read_mapping_file(MAP(val1, val2));

            rna_tree rna1(read_file(FOLD(val1)), read_file(SEQ(val1)), val1);
            rna_tree rna2(read_file(FOLD(val2)), read_file(SEQ(val2)), val2);

            transform(rna1, rna2, map, PS_IN(val1), PS_OUT(val1, val2));
        }
    }
}

void app::transform(
                const rna_tree& rna1,
                const rna_tree& rna2,
                std::vector<mapping> m,
                const std::string& fileIn,
                const std::string& fileOut)
{
    APP_DEBUG_FNAME;
    DEBUG("in: %s, out: %s", fileIn.c_str(), fileOut.c_str());

    ifstream in(fileIn);
    //ofstream out(fileOut);
    string line;
    document doc;

    doc = read_ps(fileIn);
    rna_tree rna1points = rna1;
    update_tree_points(rna1points, doc);
    rna_tree rnaout = rna1points;

    //sort(map.begin(), map.end(), [](mapping i, mapping j) {return i.to < j.to;});

    vector<rna_node_type> labels1, labels2, labelsOut;
    for (rna_tree::post_order_iterator it = rna1.begin_post(); it != rna1.end_post(); ++it)
    {
        labels1.push_back(*it);
        cout << labels1.back() << " ";
    }
    cout << endl;
    for (rna_tree::post_order_iterator it = rna2.begin_post(); it != rna2.end_post(); ++it)
    {
        labels2.push_back(*it);
        cout << labels2.back() << " ";
    }
    cout << endl;

    rnaout.print_tree();

    rna_tree::post_order_iterator it = rnaout.begin_post();
    size_t i = 0;

    //for (auto val : m)
        //cout << val.from << " " << val.to << endl;

    for (; it != rnaout.end(); ++it, ++i)
    {
        assert(find_if(m.begin(), m.end(), [](mapping value) { return value.from == 1 + 1;}) != m.end());
        auto val = *find_if(m.begin(), m.end(), [&](mapping value) { return value.from == i + 1;});
        
        if (val.to == 0)
        {
            //DEBUG("delete index %lu, node %s", val.from, label(it));
            //it = rnaout.remove(it);
            auto label = it->get_label();
            label.labels.at(0).label = "***************DELETED_NODE***********************";
            //it->set_label(label);
            it = rnaout.modify(it, label);
            //it = rnaout.modify(it, 
        }
        else
        {
            //DEBUG("modify index %lu, from %s, to %s", val.from, label(it), labels2.at(val.to - 1).get_label().to_string().c_str());
            it = rnaout.modify(it, labels2.at(val.to - 1));
        }
    }
    
    doc.rna = rnaout;
    save_doc(doc, fileOut);
    return;


    for (auto val : m)
    {
        assert(val.from != 0 || val.to != 0);
        cout << val.from << " " << val.to << endl;

        if (val.from == 0)
        {
            DEBUG("insert index %lu", val.to);
            labelsOut.push_back(labels2.at(val.to - 1));
        }
        else if (val.to == 0)
        {
            DEBUG("delete index %lu", val.from);
        }
        else
        {
            labelsOut.push_back(labels2.at(val.to - 1));
        }
    }
    for (auto val : labelsOut)
        cout << val << " ";
    cout << endl;
}

void app::update_tree_points(
                rna_tree& rna,
                document doc)
{
    APP_DEBUG_FNAME;

    size_t i = -2;
    for (auto it = rna.begin_pre_post(); it != rna.end_pre_post(); ++it, ++i);
    assert(i == doc.labels.size() &&
            i == doc.points.size());

    i = 0;
    for (auto it = ++rna.begin_pre_post(); ++rna_tree::pre_post_order_iterator(it) != rna.end_pre_post(); ++it, ++i)
    {
        auto label = it->get_label();
        size_t index;

        if (rna_tree::is_leaf(it))
            index = 0;
        else if (it.is_preorder())
            index = 0;
        else
            index = 1;
        label.change_point(doc.points.at(i), index);
        it->set_label(label);
    }

    for (auto it : rna)
        DEBUG("points: %s", it.get_label().get_points().c_str());
}
















