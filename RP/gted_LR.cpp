/*
 * File: gted_LR.cpp
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

#include "gted.hpp"

using namespace std;

#define all_subforest_nodes_init(Subforest, value) \
    Subforest.left = Subforest.right = Subforest.path_node = value
#define F_str "F[%s, %s][%s, %s]"
#define T_str "T[%s][%s]"

#define labels_LR(s) label(s.left), label(s.right)



void print_subforest(const gted::subforest& f)
{
    DEBUG("[%s, %s, %s, %s]", label(f.left), label(f.right),
            label(f.path_node), label(f.root));
}

void gted::test()
{
    //return;
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

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






















bool gted::do_decompone_LR(tree_type::iterator& it_ref,
                    tree_type::iterator root,
                    path_strategy str) const
{
    /*
     * podla strategie vracia keyroots
     */
    APP_DEBUG_FNAME;
    DEBUG("it=%s, end=%s", label(it_ref), label(root));

    assert(str != path_strategy::heavy && "only LR strategies");
    assert(id(it_ref) <= id(root));

    if (it_ref == root)
    {
        DEBUG("it_ref == root, return=false");
        return false;
    }

    tree_type::sibling_iterator it = it_ref;
    
    if (str == path_strategy::left)
    {
        // prechadzam bratov zlava doprava
        // ak som nakraji, idem do otca.
        //
        DEBUG("str == left");
        
        if (tree_type::is_last_child(it))
            it = tree_type::parent(it);
        else
            ++it;

        // sice som sa posunul, ale som na ceste, musim is to dalsieho vrcholu
        while(it != root && tree_type::is_first_child(it))
        {
            DEBUG("while, it=%s", label(it));
            assert(id(it) < id(root));

            if (tree_type::is_last_child(it))
                it = tree_type::parent(it);
            else
                ++it;
        }
    }
    else
    {
        // prechadzam bratov zprava dolava, obdoba ::left
        DEBUG("str == right");

        if (tree_type::is_first_child(it))
            it = tree_type::parent(it);
        else
            --it;

        while(it != root && tree_type::is_last_child(it))
        {
            DEBUG("while, it=%s", label(it));
            assert(id(it) < id(root));

            if (tree_type::is_first_child(it))
                it = tree_type::parent(it);
            else
                --it;
        }
    }

    it_ref = it;

    bool output = (root != it_ref);
    DEBUG("itout=%s, return %s", label(it_ref), output?"true":"false");
    return output;
}

bool gted::do_decompone_LR_recursive(tree_type::iterator& it_ref,
                                tree_type::iterator& leaf,
                                tree_type::iterator end,
                                path_strategy str) const
{
    APP_DEBUG_FNAME;
    DEBUG("it=%s, end=%s, str=%s",
            label(it_ref), label(end), to_string(str));

    assert(str != path_strategy::heavy && "only LR strategies");

    if (str == path_strategy::left)
    {
        assert(id(end) < id(it_ref));

        // idem zprava dolava
        tree_type::reverse_post_order_iterator it = it_ref;
        ++it;
        while(it != end && tree_type::is_first_child(it))
        {
            if (tree_type::is_leaf(it))
                leaf = it;

            DEBUG("while, it=%s", label(it));
            ++it;
        }
        it_ref = it;
    }
    else
    {
        DEBUG("right str");
        // idem zlava doprava..
        assert(id(end) > id(it_ref));

        tree_type::post_order_iterator it = it_ref;
        ++it;
        while(it != end && tree_type::is_last_child(it))
        {
            if (tree_type::is_leaf(it))
                leaf = it;

            DEBUG("while, it=%s", label(it));
            ++it;
        }
        it_ref = it;
    }
    // pripad ze while neprebehne ani raz..
    if (tree_type::is_leaf(it_ref))
        leaf = it_ref;

    bool output = end != it_ref;
    DEBUG("itout=%s, return %s", label(it_ref), output?"true":"false");
    return output;
}









void gted::single_path_function_LR(tree_type::iterator root1,
                                tree_type::iterator root2,
                                path_strategy str,
                                graph who_first)
{
    APP_DEBUG_FNAME;
    DEBUG("roots: %s %s, str=%s, whofirst=%s", 
            label(root1), label(root2), to_string(str), to_string(who_first));
    
    assert(str != path_strategy::heavy);

    tree_type::iterator it;
    tree_type::iterator end_it;
    subforest_pair forests;
    tree_type::iterator leaf;

    init_subforest_pair(forests, root1, root2, str, who_first);
    print_subforest(forests.f1);
    print_subforest(forests.f2);

    end_it = (str == path_strategy::left ? 
            tree_type::leftmost_child (forests.f1.root) :
            tree_type::rightmost_child(forests.f1.root));
    it = forests.f1.path_node;
    forests.f1.root = it;
    leaf = it;

    if ((str == path_strategy::left && !tree_type::is_first_child(it)) ||
        (str == path_strategy::right && !tree_type::is_last_child(it)))
    {
        compute_distance(forests, who_first);
    }

    while(do_decompone_LR_recursive(it, leaf, end_it, str))
    {
        all_subforest_nodes_init(forests.f1, leaf);
        forests.f1.root = it;

        //print_subforest(forests.f1);

        compute_distance(forests, who_first);
    }
    all_subforest_nodes_init(forests.f1, end_it);
    forests.f1.root = (who_first == T1) ? root2 : root1;
    DEBUG("compute between roots in single_path_f");
    compute_distance(forests, who_first);
}








