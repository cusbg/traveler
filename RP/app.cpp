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



using namespace std;


rna_tree app::update_tree_points(
                const rna_tree& rnain,
                document doc)
{
    APP_DEBUG_FNAME;

    typedef rna_tree::pre_post_order_iterator pre_post_it;

    rna_tree rnaout = rnain;

    size_t i = -2;  // .begin() a .end(), obidva su ROOT_id.. treba ich ignorovat
    for (pre_post_it it = rnaout.begin_pre_post(); it != rnaout.end_pre_post(); ++it)
        ++i;

    assert(i == doc.labels.size() &&
            i == doc.points.size());

    i = 0;
    for (pre_post_it it = ++rnaout.begin_pre_post();
            ++pre_post_it(it) != rnaout.end_pre_post(); ++it, ++i)
    {
        auto label = it->get_label();
        size_t index;

        if (rna_tree::is_leaf(it))
            index = 0;
        else if (it.is_preorder())
            index = 0;
        else
            index = 1;
        label.labels.at(index).point = doc.points.at(i);
        it->set_label(label);
    }

    return rnaout;
}

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
            //return;
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


void app::save_doc(const document& doc, const std::string& file)
{
    APP_DEBUG_FNAME;
    DEBUG("doc: %s", file.c_str());
    ofstream out(file);
    out.setf(ios_base::unitbuf);

    typedef rna_tree::pre_post_order_iterator pre_post_it;

    out << doc.prolog << endl;
    for (auto it = ++doc.rna.begin_pre_post();
            ++pre_post_it(it) != doc.rna.end_pre_post(); ++it)
    {
        out << format_string (it) << endl;
    }
    out << doc.epilog << endl;
    assert(!out.fail());


    DEBUG("document %s was saved", file.c_str());

    //wait_for_input();
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

    ofstream out(fileOut);
    out.setf(ios_base::unitbuf);

    document doc;
    rna_tree rnaout;
    rna_tree::post_order_iterator it;
    vector<rna_tree::iterator> labels2;

    doc = read_ps(fileIn);
    rnaout = update_tree_points(rna1, doc);


    for (rna_tree::post_order_iterator it = rna2.begin_post(); it != rna2.end_post(); ++it)
        labels2.push_back(it);

    it = rnaout.begin_post();

/*
    rna_tree::post_order_iterator it2;
    auto move_it = [](rna_tree::post_order_iterator iter, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
            ++iter;
        return iter;
    };

    out << doc.prolog << endl;
    for (auto m : map.map)
    {
        if (m.from == 0)
            continue;

        if (m.to == 0)
        {
        }
        else
        {
        }
    }
    out << doc.epilog << endl;
    return;

*/

/*
    vector<size_t> to_insert;
    for (auto m : map.map)
    {
        if (m.from != 0)
            break;
        else
            to_insert.push_back(m.to - 1);
    }
    to_insert.push_back(-1);
    sort(to_insert.begin(), to_insert.end(), std::greater<size_t>());
*/

    out << doc.prolog << endl;
    size_t it_index = 0;
    for (size_t i = 5; i < map.map.size(); ++i)
    {
        mapping_pair m = map.map.at(i);

        assert(m.from - 1 == it_index);
        assert(m.from != 0);

        if (m.to == 0)
        {
            auto iter = rnaout.remove(it);
            assert(iter == it);

            rnaout.print_subtree(rna_tree::parent(it));
        }
        else
        {
            auto label = *labels2.at(m.to - 1);
            auto iter = rnaout.modify(it, label);
            assert(iter == it);
        }

        out << format_string (it) << endl;
        cout << it_index << endl;

        ++it;
        ++it_index;
    }

    out << doc.epilog << endl;
    
    return;
    doc.rna = rnaout;

    DEBUG("end");
    rnaout.print_tree();

    save_doc(doc, fileOut);
}






std::string app::format_string(pre_post_it it)
{
#define CHANGED_STR     "********************** CHANGED **********************"
#define DELETED_STR     "********************** DELETED **********************"
#define INSERTED_STR    "********************* INSERTED **********************"
#define UNTOUCHED_STR   "********************* UNTOUCHED *********************"

    enum RGBB
    {
        red, green, blue, other, black
    };
    auto set_rgbb =     [](RGBB value)
    {
        string out;

        switch (value)
        {
            case red:
                out = "1.00 0.00 0.00 setrgbcolor";
                break;
            case green:
                out = "0.00 1.00 0.00 setrgbcolor";
                break;
            case blue:
                out = "0.00 0.00 1.00 setrgbcolor";
                break;
            case black:
                out = "0.00 0.00 0.00 setrgbcolor";
                break;
            case other:
                out = "0.00 0.50 0.50 setrgbcolor";
                break;
        }
        return out;
    };
    auto get_index =    [](const pre_post_it& iter)
    {
        if (iter.is_preorder() || !iter->get_label().is_paired())
            return 0;
        else
            return 1;
    };
    auto print =        [](const rna_label& label)
    {
        stringstream out;
        out
            << "("
            << label.label
            << ") "
            << label.point.x
            << " "
            << label.point.y
            << " lwstring";
        return out.str();
    };
    auto print_colored = [&](const pre_post_it& iter, RGBB value)
    {
        string out;

        for (auto label : iter->get_label().labels)
        out = out
            + set_rgbb(value)
            + "\n"
            + print(label)
            + "\n"
            + set_rgbb(black)
            + "\n";

        logger.debugStream() << "PRINT LABELS: " << iter->get_label();

        return out;
    };
    auto print_normal = [&](const pre_post_it& iter, bool colored = true)
    {
        string out;

        if (colored)
        {
            out += print_colored(iter, blue);
        }
        else
        {
            size_t index = get_index(iter);
            auto label = iter->get_label().labels.at(index);
            out += print(label);
            out += "\n";
        }

        return out;
    };

    string out;

    auto status = it->get_label().status;

#define DELETE_COLOR    red
#define INSERT_COLOR    blue
#define EDITED_COLOR    green
#define OTHER_COLOR     other
//#define EDITED_COLOR    other
//#define OTHER_COLOR     green
    switch (status)
    {
        case rna_pair_label::deleted:
            out = print_colored(it, DELETE_COLOR);
            wait_for_input();
            break;
        case rna_pair_label::edited:
            out = print_colored(it, EDITED_COLOR);
            break;
        case rna_pair_label::touched:
            out = print_colored(it, black);
            break;
        default:
            //break;
            abort();



/*
        case rna_pair_label::pair_changed:
            WARN("changed size");
            out = print_colored(it, OTHER_COLOR);
            abort();
            break;
        case rna_pair_label::inserted:
            abort();
            wait_for_input();
            out = print_colored(it, INSERT_COLOR);
            break;
        case rna_pair_label::untouched:
            abort();
            {
            auto label = it->get_label();
            label.labels.at(0).label = UNTOUCHED_STR;
            it->set_label(label);
            }
            //out = print_colored(it, OTHER_COLOR);
            break;
        default:
            assert(status == rna_pair_label::touched);
            out = print_normal(it);
            break;
*/
    }
    cout << out << endl;

    return out;
};







