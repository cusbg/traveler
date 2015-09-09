
#include "types.hpp"
#include "util.hpp"
#include "app.hpp"
#include "generator.hpp"

#include "point.hpp"
#include "compact_maker.hpp"
#include "compact_maker_utils.hpp"
#include "ps.hpp"

#include "checks.hpp"
#include "rna_tree_matcher.hpp"

using namespace std;

typedef compact::circle circle;

void def_ps_init();

struct crossing_check::edge
{
    Point p1, p2;
};

inline void g()
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

        auto Point = c.split(s.size());

        for (size_t i = 0; i < s.size(); ++i)
            psout.print_to_ps(ps::print(Point[i], s.substr(i, 1)));
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

inline void h()
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


    compact c(rna);
    c.make_compact();

    rna = c.get();

    //crossing_check chck;
    //chck.split_to_edges(rna);

    INFO("END");

    exit(0);
}

inline void a()
{
    string s1, s2;
    rna_tree rna1, rna2;
    mapping m;
    matcher match;

    s1 = "rabbit";
    s2 = "human";
    //swap(s1, s2);
    
    rna1 = app::get_rna(s1);
    rna2 = app::get_rna(s2);
    m = app::get_map(s1, s2);

    match.run(rna1, rna2, m);
    rna1.print_tree();
}

inline void intersection()
{
    crossing_check::edge e1, e2;

    e1.p1 = {5, 8};
    e1.p2 = {2, 3};

    e2.p1 = {4, 5};
    e2.p2 = {8, 9};


    cout<< lies_on_line({0, 0}, {1, 1}, {-2, -2}) << endl;
    cout << lies_on_line(e1.p1, e1.p2, e2.p1) << endl;
    abort();

    cout << crossing_check().intersect(e1, e1) << endl;
    cout << crossing_check().intersect(e1, e2) << endl;
    abort();
}

inline void functs()
{
    //intersection();

    return;
    def_ps_init();
    circle c;
    c.p1 = {100, 0};
    c.p2 = {200, 0};
    c.centre = centre(c.p1, c.p2);
    c.direction = {300, -100};
    c.compute_sgn();
    c.draw();
    abort();
    return;
    //h();
    a();

    DEBUG("END functs(), aborting");
    abort();
}
