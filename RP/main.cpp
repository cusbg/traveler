/*
 * File: main.cpp
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


#include <iostream>
#include <fstream>

#include "types.hpp"
#include "util.hpp"
#include "app.hpp"
#include "generator.hpp"

#include "point.hpp"
#include "compact_maker.hpp"
#include "compact_maker_utils.hpp"
#include "ps.hpp"


using namespace std;

typedef compact::circle circle;
void tests();
void def_ps_init();
void g();
void h();

int main(int argc, char** argv)
{
    cout << boolalpha;
    srand(1);

    //m();
    tests();

    if (argc == 2)
    {
        generator::generate_files();
        exit(0);
    }

    //h();

    string from, to;

    from = "human";
    to = "frog";

    //from = "mouse";
    //to = "human";
    //to = "rabbit";

    //from = "rabbit";
    //to = "human";
    from = "frog";
    to = "rabbit";

    app a;
    a.run_between(from, to);
    //a.run_app();

    return 0;
}

void tests()
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    vector<Point> vecp = {
        {100,   0},
        {0,     100},

        {100,   100},
        {-100,  100},
        {-100,  -100},
        {100,   -100},
    };
    vector<double> vecv = {
        0,
        90,

        45,
        135,
        225,
        315,
    };
    assert(vecp.size() == vecv.size());

    for (size_t i = 0; i < vecp.size(); ++i)
    {
        double x = angle(vecp[i]);
        double y = vecv[i];
        if (!double_equals(x, y))
        {
            ERR("not equal in %s: %f != %f",
                    vecp[i].to_string().c_str(), x, y);
        }
    }
}


/*
[>
    string l1, l2, b1, b2;
    rna_tree rna1, rna2;
    string ps_in, ps_out;

    l1 = "-5142aba3456978089-";
    b1 = "((.(.(.).)).(.(.)))";

    l1 = "abcdedfbg";
    b1 = ".(.(.).).";

    rna1 = rna_tree(b1, l1);
    rna_node_type node;
    rna_pair_label label(".");
    label = label + label;
    
    node.set_label(label);
    auto it = rna1.begin();
    it = move(rna1.begin(), 4);
    //for (int i = 0; i < 4; ++i)
        //++it;

    DEBUG("it = %s", ::label(it));
    rna1.print_tree();

    //it = rna1.modify(it, node);
    //DEBUG("it = %s", ::label(it));
    //rna1.print_tree();
    //wait_for_input();

    it = rna1.remove(it);
    DEBUG("it = %s", ::label(it));
    rna1.print_tree();
    wait_for_input();

    it = rna1.insert(it, node);
    DEBUG("it = %s", ::label(it));
    rna1.print_tree();
    wait_for_input();

    it = move(rna1.begin(), 3);
    it = rna1.insert(it, node);
    DEBUG("it = %s", ::label(it));
    rna1.print_tree();
    wait_for_input();

    it = move(rna1.begin(), 2);
    it = rna1.insert(it, node);
    DEBUG("it = %s", ::label(it));
    rna1.print_tree();
    wait_for_input();


    return 0;
<]

    //rna_tree rna1(
            //"((.(.(.).)).(.(.)))",
            //"-5142aba3456978089-");
    //rna1.print_tree();
    //return 0;
    //auto it = rna1.begin_post();
    //rna1.print_tree();
    //rna1.erase(it);
    //rna1.print_tree();
    //abort();

*/

/*
    string l1, l2, b1, b2;
    rna_tree rna1, rna2;
    string ps_in, ps_out;


    l1 = "-51423456978089-";
    b1 = "((.(..)).(.(.)))";

    l2 = "abcdeca";
    b2 = "(.(..))";

    rna1 = rna_tree(b1, l1);
    rna2 = rna_tree(b2, l2);


    ps_in = INPUT_PS1;
    ps_out = OUTPUT_PS;

    //l1 = get_rna_seq_from_ps(ps_in);
    //b1 = run_RNAfold(l1);

    rna1 = rna_tree(b1, l1);

    app a;
    a.run_app(rna1, rna2);

    assert(rna1.get_brackets() == b1 && rna1.get_labels() == l1);
    cout << rna1.get_brackets() << endl;
    cout << rna1.get_labels() << endl;
    abort();
*/

void def_ps_init()
{
    APP_DEBUG_FNAME;

    string s = document::default_prologue();
    psout = ps::init("build/files/ps.ps");
    psout.print_to_ps(s);
}

void g()
{

    circle c1, c2;
    string s(10, 'A');

    auto posun = [](circle c)
    {
        Point p = {50, 50};
        c.p1 = c.p1 + p;
        c.p2 = c.p2 + p;
        c.centre = c.centre + p;
        return c;
    };
    auto print_c = [](circle c, string s)
    {
        psout.print_to_ps(ps::print(c.p1, "B"));
        psout.print_to_ps(ps::print(c.p2, "E"));

        auto points = c.split(s.size());

        for (size_t i = 0; i < s.size(); ++i)
            psout.print_to_ps(ps::print(points[i], s.substr(i, 1)));
    };
    auto print = [&]()
    {
        auto pos = psout.get_pos();

        c1.draw();
        c2 = posun(c1);
        print_c(c2, s);

        psout.seek(pos);
        wait_for_input();
    };

    c1.p1 = {0.98, -1008.84};
    c1.p2 = {-13.16, -1022,98};
    c1.centre = centre(c1.p1, c1.p2);
    c1.direction = {0, 0};
    c1.compute_sgn();

    print();

    while (!cin.fail())
    {
        c1.init(s.size());
        print();
    }

    abort();
}

void h()
{
    string l, b;
    rna_tree rna;
    Point v;
    struct
    {
        Point p1, p2;
    } pairs;

    def_ps_init();

    l = "ABAAAGGGAAACCCCCCCCCCCUUUGGGGGGGGGUUUU";
    b = "(.(((...(((...........))).........))))";
    rna = rna_tree(b, l);

    pairs.p1 = {10, 10};
    pairs.p2 = {30, 10};
    v = {0, 8};

    for (auto it = rna.begin(); it != rna.end(); ++it)
    {
        auto& lbl = it->get_label();
        if (lbl.is_paired())
        {
            lbl.set_points_exact(pairs.p1, 0);
            lbl.set_points_exact(pairs.p2, 1);
            lbl.status = rna_pair_label::touched;

            pairs.p1 = pairs.p1 + v;
            pairs.p2 = pairs.p2 + v;
        }
        else
        {
            lbl.status = rna_pair_label::inserted;
        }
    }

    document doc;
    doc.rna = rna;
    compact c(doc);
    c.make_compact();

    INFO("END");

    exit(0);
}



