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

#ifdef NODEF

#ifdef NDEBUG
// always use asserts...
#undef NDEBUG
#endif

#include "tests.hpp"


#define TESTS

#include "rted.hpp"
//#include "gted.hpp"

using namespace std;

void rted_tests()
{
    APP_DEBUG_FNAME;
    logger.info("BEGIN tests");
    auto priority = logger.getPriority();
    logger.setPriority(log4cpp::Priority::WARN);

    rted::test();

    logger.setPriority(priority);
    logger.info("END tests");
}

void rted::test()
{
    APP_DEBUG_FNAME;

    auto test_funct = [](string b1, string l1, string b2, string l2)
    {
        rna_tree rna1, rna2;

        rna1 = rna_tree(b1, l1);
        rna2 = rna_tree(b2, l2);

        rted r(rna1, rna2);

        r.run_rted();
        auto val = r.get_strategies();
        auto t1_id = rted::tree_type::child(rna1.begin(), 0)->get_id();
        auto t2_id = rted::tree_type::child(rna2.begin(), 0)->get_id();
        return val.at(t1_id).at(t2_id);
    };

    assert(is_right_path(test_funct(BRACKETS1, LABELS1, BRACKETS1, LABELS1)));
    assert(is_left_path (test_funct(BRACKETS21, LABELS21, BRACKETS22, LABELS22)));
}





#ifdef NODEF
void gted::test()
{
    APP_DEBUG_FNAME;

    auto test_funct = [](rna_tree rna1, rna_tree rna2, vector<vector<size_t>> vec)
    {
        rna1.set_ids_postorder();
        rna2.set_ids_postorder();

        gted g(rna1, rna2);
        g.run_gted();
        auto dist = g.tree_distances;

        size_t i, j;
        i = j = 0;
        tree_type::post_order_iterator it1, it2;
        for (it1 = g.t1.begin_post(); it1 != --g.t1.end_post(); ++it1)
        {
            for (it2 = g.t2.begin_post(); it2 != --g.t2.end_post(); ++it2)
            {
                if (dist.at(id(it1)).at(id(it2)) != vec[i][j])
                {
                    logger.fatal("zle vyratane");
                    cout << *it1 << " " << *it2 << endl <<
                        "indexy: " << i << " " << j << endl;
                    abort();
                }
                ++j;
            }
            ++i;
            j = 0;
        }
        logger.notice("TEST OK");
    };
    string l1, l2, b1, b2;
    vector<vector<size_t>> vec;
    rna_tree rna1, rna2;

    vec = { /*  A   B   CC  D   EE  */
    /*  1   */{ 0,  0,  1,  0,  4   },
    /*  2   */{ 0,  0,  1,  0,  4   },
    /*  3   */{ 0,  0,  1,  0,  4   },
    /*  44  */{ 2,  2,  1,  2,  2   },
    /*  55  */{ 4,  4,  3,  4,  2   }
    };

    l1 = "5142345";
    b1 = "(.(..))";

    l2 = "EACBCDE";
    b2 = "(.(.).)";

    rna1 = rna_tree(b1, l1);
    rna2 = rna_tree(b2, l2);

    test_funct(rna1, rna2, vec);

    vec = { /*  A   B   C   DD  */
    /*  1   */{ 0,  0,  0,  3,  },
    /*  2   */{ 0,  0,  0,  3,  },
    /*  3   */{ 0,  0,  0,  3,  },
    /*  44  */{ 2,  2,  2,  1,  },
    /*  55  */{ 4,  4,  4,  1,  }
    };

    l1 = "5142345";
    b1 = "(.(..))";

    l2 = "DABCD";
    b2 = "(...)";

    rna1 = rna_tree(b1, l1);
    rna2 = rna_tree(b2, l2);

    test_funct(rna1, rna2, vec);

}

void gted_tests()
{
    APP_DEBUG_FNAME;
    logger.info("BEGIN tests");
    auto priority = logger.getPriority();
    logger.setPriority(log4cpp::Priority::WARN);

    gted::test();

    logger.setPriority(priority);
    logger.info("END tests");
}
#endif


#endif

