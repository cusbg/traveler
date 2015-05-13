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

void posun_vrcholy(rna_tree::iterator it, Point vector);



std::string app::format_string(pre_post_it it)
{
#define CHANGED_STR     "********************** CHANGED **********************"
#define DELETED_STR     "********************** DELETED **********************"
#define INSERTED_STR    "********************* INSERTED **********************"
#define UNTOUCHED_STR   "********************* UNTOUCHED *********************"

    enum RGB
    {
        red, green, blue, other, black
    };
    auto set_rgbb =     [](RGB value)
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
    auto print_colored = [&](const pre_post_it& iter, RGB value)
    {
        string out;

        size_t index = get_index(iter);
        auto label = iter->get_label().labels.at(index);

        //for (auto label : iter->get_label().labels)
        out = out
            + set_rgbb(value)
            + "\n"
            + print(label)
            + "\n"
            + set_rgbb(black)
            + "\n";

        return out;
    };
    auto print_normal = [&](const pre_post_it& iter, bool colored = false)
    {
        string out;

        if (colored)
            out = print_colored(iter, other);
        else
        {
            size_t index = get_index(iter);
            auto label = iter->get_label().labels.at(index);
            out = print(label);
        }
        out += "\n";

        return out;
    };

    string out;

    auto status = it->get_label().status;

#define DELETE_COLOR    red
#define INSERT_COLOR    blue
#define EDITED_COLOR    green
#define OTHER_COLOR     other

    if (status == rna_pair_label::untouched)
    {
        ERR("untouched");
        abort();
    }

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
            out = print_normal(it);
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

    return out;
};


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

    wait_for_input();
    typedef rna_tree::pre_post_order_iterator pre_post_it;

    print_ps(doc.prolog);

    //posun_vrcholy(doc.rna.begin(), Point({10, 10}));
    
    for (auto it = ++doc.rna.begin_pre_post();
            ++pre_post_it(it) != doc.rna.end_pre_post(); ++it)
    {
        print_ps(format_string(it));
    }
    print_ps(doc.epilog);

    DEBUG("document %s was saved", file.c_str());
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

    document doc = read_ps(fileIn);
    doc.rna = update_tree_points(rna1, doc);
    rna_tree::post_order_iterator it;
    auto move_it = [](rna_tree::post_order_iterator iter, size_t count)
    {
        while(count--)
            ++iter;
        return iter;
    };

    it = doc.rna.begin_post();
    for (auto m : map.map)
    {
        if (m.from == 0)
            continue;
        
        if (m.to == 0)
            //it = doc.rna.erase(it);
            it = doc.rna.remove(it);
        else
            it = doc.rna.modify(it, *move_it(rna2.begin_post(), m.to - 1));

        ++it;
    }

    print_ps(doc.prolog, fileOut);
    make_compact(doc.rna);
    save_doc(doc, fileOut);
}

void posun_vrcholy(rna_tree::iterator it, Point vector)
{
    cout << "posunutie o " << vector << endl;

    auto label = it->get_label();
    for (size_t i = 0; i < label.labels.size(); ++i)
    {
        cout << label.labels.at(i).point << endl;
        label.labels.at(i).point = label.labels.at(i).point + vector;
        cout << label.labels.at(i).point << endl;
    }
    it->set_label(label);

    // prejdi vsetky deti, a tie posun tiez.
    for (rna_tree::sibling_iterator sib = it.begin(); sib != it.end(); ++sib)
        posun_vrcholy(sib, vector);
}

void app::make_compact(
                rna_tree& rna)
{
    typedef rna_tree::iterator iterator;

    APP_DEBUG_FNAME;

    auto stred = [](iterator iter)
    {
        auto label = iter->get_label();
        assert(label.is_paired());

        Point p1 = label.labels.at(0).point;
        Point p2 = label.labels.at(1).point;

        return Point({(p1.x + p2.x) / 2, (p1.y + p2.y) / 2});
    };

    //rna_tree::post_order_iterator it = rna.begin_post();
    rna_tree::iterator it, end;
    it = rna.begin();
    end = rna.end();

    for (; it != end; ++it)
    {
        if (it->get_label().status != rna_pair_label::deleted)
        {
            print_ps(format_string(rna_tree::pre_post_order_iterator(it, false)));
            print_ps(format_string(it));
            continue;
        }

        if (it->get_label().is_paired())
        {
            rna.print_subtree(it);
            Point parent_str = stred(rna_tree::parent(it));
            Point my_str = stred(it);

            //print_ps(format_string(it));
            posun_vrcholy(it, parent_str - my_str);
            //wait_for_input();
            //print_ps(format_string(it));
        }
    }


    INFO("exit compact funct");
    abort();
}







void app::print_ps(const std::string& line, const string& filename)
{
    static ofstream out(filename);

    out << line << endl;
    assert(!out.fail());
}



