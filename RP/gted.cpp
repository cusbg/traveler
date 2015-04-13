/*
 * File: gted.cpp
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

#include "gted.hpp"
#include <algorithm>
#include <iomanip>

using namespace std;

/*
#define F_str "F[%s, %s][%s, %s]"
#define T_str "T[%s][%s]"

#define labels_LR(s) label(s.left), label(s.right)
*/

#define all_subforest_nodes_init(Subforest, value) \
    Subforest.left = Subforest.right = Subforest.path_node = value

#define GTED_CHECKS_DISABLED

#define BAD_INDEX 0xBADF00D

#define is_heavy(iter) \
        (!iter->is_root() && heavy_child(tree_type::parent(iter)) == iter)
#define is_first(iter) \
        (tree_type::is_first_child(iter))
#define is_last(iter) \
        (tree_type::is_last_child(iter))

#define lies_on_path(iter, STR) \
        (precomputed.leafs.at(id(iter)).STR == \
         precomputed.leafs.at(id(tree_type::parent(iter))).STR)
#define is_only_child(iter) \
        (tree_type::leftmost_child(iter) == tree_type::rightmost_child(iter))





#ifdef NODEF



void gted::print_TDist(tree_distance_table_type distances)
{
    APP_DEBUG_FNAME;
    swap(tree_distances, distances);
    print_TDist();
    swap(tree_distances, distances);
}
void gted::print_TDist() const
{
    APP_DEBUG_FNAME;

    stringstream out;
    bool was_catch = false;
    int odsadenie = 22;
    out << "TREE_DISTANCE_TABLE:" << endl;
    out << setw(odsadenie) << "";
    for (auto it2 = t2.tree_ptr->begin_post(); it2 != --t2.tree_ptr->end_post(); ++it2)
        out << setw(odsadenie) << std::left << *it2;
    out << endl;
    for (auto it1 = t1.tree_ptr->begin_post(); it1 != --t1.tree_ptr->end_post(); ++it1)
    {
        out << setw(odsadenie) << std::left << *it1;
        for (auto it2 = t2.tree_ptr->begin_post(); it2 != --t2.tree_ptr->end_post(); ++it2)
        {
            try
            {
                out << std::left << setw(odsadenie) << tree_distances.at(it1->get_id()).at(it2->get_id());
            } catch (...)
            {
                was_catch = true;
                out << std::left << setw(odsadenie) << tree_distances.at(it2->get_id()).at(it1->get_id());
            }
        }
        out << endl;
    }
    logger.infoStream() << out.str();
    cerr << out.str();
    if (was_catch)
        logger.warn("CATCH in TDist");
}

void gted::print_FDist(const forest_distance_table_type& table) const
{
    //APP_DEBUG_FNAME;

    // TODO: ked niesu vyplnene vsetky stlpce, tak to nebude fungovat...
    //
    int odsadenie = 15;
    stringstream out;
    out << "FOREST_DISTANCE_TABLE:" << endl;
/*
    vector<subforest> vec;
    string s;

    out << " ";
    out << setw(odsadenie) << "";
    for (auto val : table)
        for (auto val2 : val.second)
            if (find(vec.begin(), vec.end(), val2.first) == vec.end())
            {
                vec.push_back(val2.first);
                s = string() +  "[" + label(val2.first.left) + "," + label(val2.first.right) + "]";
                out << setw(odsadenie) << s;
            }
    out << endl;

    for (auto val : table)
    {
        s = string() + "[" + label(val.first.left) + "," + label(val.first.right) + "]";
        out << setw(odsadenie) << s;
        for (auto val2 : val.second)
        {
            out << setw(odsadenie) << val2.second;
        }
        out << endl;
    }


    logger.debugStream() << out.str();
    return;
*/

    for (auto val : table)
    {
        string s = string() +  "[" + label(val.first.left) + "," + label(val.first.right) + "]";
        out << setw(odsadenie) << "";
        out << s << endl;
        for (auto v2 : val.second)
        {
            s = string() + "[" + label(v2.first.left) + "," + label(v2.first.right) + "]";
            out << setw(odsadenie) << s;
            out << v2.second << "\t";
            out << endl;
        }
        out << endl;
    }
    logger.debugStream() << out.str();
}

void gted::print_subforest(const subforest& f)
{
    DEBUG("[%s, %s, %s, %s]", label(f.left), label(f.right),
            label(f.path_node), label(f.root));
}

/* GET & SET functions: */
size_t gted::get_Fdist(const subforest& index1,
                    const subforest& index2,
                    const forest_distance_table_type& forest_dist) const
{
    // vzdy vracia v danom poradi, table[index1][index2].
    try
    {
        DEBUG("GET: " F_str,
                labels_LR(index1), labels_LR(index2));
        return forest_dist.at(index1).at(index2);
    }
    catch(...)
    {
        logger.error("CATCHed exception");
        DEBUG("mozno vymenit indexy???");
        print_FDist(forest_dist);
        abort();
    }
}

size_t gted::get_Tdist(tree_type::iterator index1,
                    tree_type::iterator index2,
                    graph who_first) const
{
    size_t out;
    if (who_first == T1)
    {
        DEBUG("GET: " T_str,
                label(index2), label(index1));
        out = tree_distances.at(id(index2)).at(id(index1));
    }
    else
    {
        DEBUG("GET: " T_str,
                label(index1), label(index2));
        out = tree_distances.at(id(index1)).at(id(index2));
    }
    return out;
}

void gted::set_Fdist(const subforest& index1,
                    const subforest& index2,
                    forest_distance_table_type& forest_dist,
                    size_t value) const
{
    DEBUG("SET: " F_str " = %lu",
            labels_LR(index1), labels_LR(index2), value);
    forest_dist[index1][index2] = value;
}

void gted::set_Tdist(tree_type::iterator index1,
                    tree_type::iterator index2,
                    size_t value,
                    graph who_first)
{
    if (who_first == T1)
    {
        DEBUG("SET: " T_str " = %lu",
                label(index2), label(index1), value);
        tree_distances[id(index2)][id(index1)] = value;
    }
    else
    {
        DEBUG("SET: " T_str " = %lu",
                label(index1), label(index2), value);
        tree_distances[id(index1)][id(index2)] = value;
    }
}



void gted::init_FDist_table(forest_distance_table_type& forest_dist,
                            subforest_pair forests)
{
    //LOGGER_PRIORITY_ON_FUNCTION(INFO);
    APP_DEBUG_FNAME;

    struct
    {
        tree_type::iterator it1, it2;
    } parents, leafs;
    int i;

    assert(forest_dist.empty());

    leafs.it1 = forests.f1.right;
    leafs.it2 = forests.f2.right;
    parents.it1 = tree_type::parent(leafs.it1);
    parents.it2 = tree_type::parent(leafs.it2);

    // INIT
    DEBUG("INIT 0");
    forests.f1.left = forests.f2.left = tree_type::iterator();  // zneplatni cely subtree -> hash() = 0
    assert(forest_dist.hash_function()(forests.f1) == 0 &&
            forest_dist.hash_function()(forests.f2) == 0);

#define init_command() \
    set_Fdist(forests.f1, forests.f2, forest_dist, i++)

    i = 0;
    init_command();

    DEBUG("INIT 1");
    i = 1;
    forests.f1.left = leafs.it1;
    init_command();

    while(forests.f1.right != forests.f1.root)
    {
        ++forests.f1.left;
        while(forests.f1.left != parents.it1)
        {
            init_command();
            ++forests.f1.left;
        }
        forests.f1.left = tree_type::first_child(parents.it1);
        ++forests.f1.right;

        while(forests.f1.right != parents.it1)
        {
            init_command();
            ++forests.f1.right;
        }
        forests.f1.left = parents.it1;
        // left == right == parent
        init_command();

        parents.it1 = tree_type::parent(parents.it1);
    }

    DEBUG("INIT 2");
    i = 1;
    forests.f1.left = tree_type::iterator();
    forests.f1.right = leafs.it1;
    forests.f2.left = leafs.it2;
    init_command();

    while(forests.f2.right != forests.f2.root)
    {
        ++forests.f2.left;
        while(forests.f2.left != parents.it2)
        {
            init_command();
            ++forests.f2.left;
        }
        forests.f2.left = tree_type::first_child(forests.f2.left);
        ++forests.f2.right;

        while(forests.f2.right != parents.it2)
        {
            init_command();
            ++forests.f2.right;
        }
        forests.f2.left = parents.it2;
        // left == right == parent
        init_command();

        parents.it2 = tree_type::parent(parents.it2);
    }
    //print_FDist(forest_dist);
#undef init_command
}










void gted::compute_distances_recursive(tree_type::iterator root1,
                                    tree_type::iterator root2)
{
    APP_DEBUG_FNAME;

    // pouzivam STRATEGIES...
    //
    tree_type::iterator it;
    rted::strategy_map_type::mapped_type::mapped_type spair;
    spair = make_pair(T1, path_strategy::left);
    //spair = make_pair(T2, path_strategy::left);
    //spair = make_pair(T1, path_strategy::right);
    //spair = make_pair(T2, path_strategy::right);
    //spair = make_pair(T1, path_strategy::heavy);
    //spair = make_pair(T2, path_strategy::heavy);
    //
    //spair = strategies.at(id(root1)).at(id(root2));
    //if (spair.second == path_strategy::heavy)
        //spair.second = path_strategy::left;

    if (spair.second == path_strategy::heavy)
    {
        tree_type::iterator path_it;
        logger.error("not implemented yet");
        if (spair.first == T1)
            path_it = it = heavy_paths.T2_heavy.at(id(root2));
        else
            abort();

        while (do_decompone_H(it, root2, path_it))
            compute_distances_recursive(root1, it);

        single_path_function_H(root1, root2, spair.first);
    }
    else
    {
        // L/R
        if (spair.first == T1)
        {
            // rozkladam T1, zacinam vrcholom na ceste,
            // v do_decompone() ho menim
            if (spair.second == path_strategy::left)
                it = tree_type::leftmost_child(root1);
            else
                it = tree_type::rightmost_child(root1);

            while (do_decompone_LR(it, root1, spair.second))
                compute_distances_recursive(it, root2);
            assert(it == root1);
        }
        else
        {
            // rozkladam T2
            if (spair.second == path_strategy::left)
                it = tree_type::leftmost_child(root2);
            else
                it = tree_type::rightmost_child(root2);

            while (do_decompone_LR(it, root2, spair.second))
                compute_distances_recursive(root1, it);
            assert(it == root2);
        }
        // mam uz dopocitane rekurzivne vsetky dekompozicie {T1/T2}
        // uz len doratat vzdialenost root1, root2
        single_path_function_LR(root1, root2, spair.second, spair.first);
    }
}

void gted::init_subforest_pair(subforest_pair& forests,
                    tree_type::iterator root1,
                    tree_type::iterator root2,
                    path_strategy str,
                    graph g) const
{
    APP_DEBUG_FNAME;

    assert(heavy_paths.T1_heavy.find(id(root1)) ==
            heavy_paths.T1_heavy.end() ||
            heavy_paths.T2_heavy.find(id(root1)) ==
            heavy_paths.T2_heavy.end());
    assert(heavy_paths.T1_heavy.find(id(root2)) ==
            heavy_paths.T1_heavy.end() ||
            heavy_paths.T2_heavy.find(id(root2)) ==
            heavy_paths.T2_heavy.end());

    tree_type::iterator first, second;

    if (g == T2)
        swap(root1, root2);

    switch (str)
    {
        // situacia: 
        //  (root1.subtrees, root2.tree) je uz vypocitane pre kazdy podstrom root1.
        //  teraz idem pocitat vzdialenost root2.subtrees od root1
        //  takze v root1 sa musim postavit na cestu, v root2 zase na opacnu stranu (len pre L/R)
        //  a kedze iterujem cez forests.f1, first<-f(root2), second<-f(root1)
        case path_strategy::left:
            first  = tree_type::rightmost_child(root2);
            second = tree_type::leftmost_child (root1);
            break;
        case path_strategy::right:
            first  = tree_type::leftmost_child (root2);
            second = tree_type::rightmost_child(root1);
            break;
        case path_strategy::heavy:
            abort();
            //first  = tree_type::leftmost_child(root2);
            //second = heavy_paths.
            break;
    }

    forests.f1.root = root2;
    forests.f2.root = root1;
        
    forests.f1.left =
        forests.f1.right =
        forests.f1.path_node = first;
    forests.f2.left =
        forests.f2.right =
        forests.f2.path_node = second;


    assert(forests.f1.root !=  empty_iterator() &&
            forests.f2.root != empty_iterator());
}









gted::forest_distance_table_type gted::compute_distance(subforest_pair forests,
                            graph who_first)
{
    if (who_first == T1)
        DEBUG("computing distance between '%s' '%s'",
                label(forests.f2.root), label(forests.f1.root));
    else
        DEBUG("computing distance between '%s' '%s'",
                label(forests.f1.root), label(forests.f2.root));
    
    auto inited_subforest = [](const subforest& f)
    {
        //cout << label(f.left) << " " <<
                //label(f.right) << " " <<
                //label(f.path_node) << " " <<
                //label(f.root) << endl;

        return f.right == f.left &&
            f.right == f.path_node &&
            tree_type::is_leaf(f.right) &&
            id(f.root) >= id(f.right);
    };
    assert (inited_subforest(forests.f1) && inited_subforest(forests.f2));

    // variables:
    subforest_pair              prevs;
    forest_distance_table_type  forest_dist;
    iterator_pair               leafs,
                                parents,
                                last_tree_roots;
    struct
    {
        vector<tree_type::iterator> s1;
        vector<tree_type::iterator> s2;
    } last_tree_roots_stacks;

#define compute_fdist_command() \
    fill_table(forest_dist, forests, prevs, \
            last_tree_roots, who_first)

    /*
     * if stack is empty, return leaf
     * in stack there are last_tree roots, that are needed in computation
     *  of F[][] when we are not on the path with (1-2) nodes.
     *  for detailed formula, see formula below.
     */
#define get_back_f1() \
    (last_tree_roots_stacks.s1.empty()) ? \
        leafs.it1 : last_tree_roots_stacks.s1.back()
#define get_back_f2() \
    (last_tree_roots_stacks.s2.empty()) ? \
        leafs.it2 : last_tree_roots_stacks.s2.back()

    // VARS INIT:
    init_FDist_table(forest_dist, forests);

    // in prevs, we use only right/left...
    prevs.f1.right = forests.f1.right;
    prevs.f2.right = forests.f2.right;
    leafs.it1 = forests.f1.right;
    leafs.it2 = forests.f2.right;
    parents.it1 = tree_type::parent(leafs.it1);
    parents.it2 = tree_type::parent(leafs.it2);
    last_tree_roots.it2 = get_back_f2();

    auto del_stack_children = [](vector<tree_type::iterator>& stack, tree_type::iterator it)
    {
        // for given node, removes all his children on the top of stack
        //
        stringstream out;
        out << "STACK:" << "\t";
        for (auto i : stack)
            out << *i << " ";
        out << endl;
        logger.debugStream() << out.str();
        while(!stack.empty())
        {
            if (tree_type::parent(stack.back()) != it)
                break;
            assert(!stack.empty());
            DEBUG("stack.back == it, deleting %s", label(stack.back()));
            stack.pop_back();
        }
    };

    bool loop = true;
    do
    {
        /*
         * ABOUT: vseobecne principy:
         *  iterujem najprv cez F1, vonkajsi cyklus je cez F2.
         *  vzdy pridavam najprv lave vrcholy, az kym nedojdem do otca, potom sa 
         *  o 1 krok vratim spat (1.syn == najlavejsi) a pridavam prave vrcholy.
         *  ked aj tam dojdem do otca, nastavim aj laveho na otca a idem novy (vnutorny) cyklus
         *  az kym nedojdem do korena. 
         *  vonkajsi cyklus funguje na rovnakom principe
         *
         *  v premennych last_tree_roots su:
         *      ak je .last == undef (stojim na path_node): je tam pointer na zaciatocny list,
         *          aby sa dalo pouzit mazanie vrcholu -> index do tabulky F: (<null>, list)
         *      inac je tam posledny subtreevrchol ktory je mimo aktualneho podstromu
         *          a teda da sa pouzit index F:
         *              (<?begin?>, last_tree_root) || (last_tree_root, <?begin?>)
         *          a index T: (this.node)
         *
         *  aby fungovala prva moznost, treba si ukladat tie vrcholy do zasobnika, 
         *  a vzdy ak zasobnik.back == child(node), tak musim dat zasobnik.pop()
         *  inac pouzivam dany vrchol ako last_tree_root
         *  ak je zasobnik prazdny, tak pouzijem list z cesty.. (leafs.it)
         */

        /*
         * ABOUT: princip fungovania algoritmu:
         * ak stojim v F1, F2 na ceste:
         *      F[forests.f1][forests.f2] = MIN {
         *          F[prevs.f1  ][forests.f2] + COST_DELETE,
         *          F[forests.f1][prevs.f2  ] + COST_DELETE,
         *          F[prevs.f1  ][prevs.f2  ] + COST_MODIFY
         *      }
         *
         * ak som mimo cesty, iba s jednym, BUNO F2, .last == Rnode
         *      F[forests.f1][forests.f2] = MIN {
         *          F[prevs.f1  ][forests.f2] + COST_DELETE,
         *          F[forests.f1][prevs.f2  ] + COST_DELETE,
         *          F[EMPTY     ][prevs.left .. last_tree_root.it2]
         *                  + T[forests.f1][forests.f2]
         *      }
         *
         * obdobna situacia je pre .last == Lnode, iba sa berie podstrom
         *      [][last_tree_root.it2 .. prevs.f2.right]
         *          Pozn: prevs.right == forests.f2.right
         * a podobne aj ked su oba mimo cesty, vyberam dane podstromy..
         */

        DEBUG("******************** NEW CYCLE ********************");
        if (forests.f2.root == forests.f2.right)
            loop = false;

        // init premennych
        all_subforest_nodes_init(forests.f1, leafs.it1);

        forests.f1.last = subforest::undef;
        parents.it1 = tree_type::parent(leafs.it1);
        prevs.f1.right = forests.f1.right;
        prevs.f1.left = empty_iterator();
        last_tree_roots_stacks.s1.clear();
        last_tree_roots.it1 = get_back_f1();

        compute_fdist_command();

        while(forests.f1.root != forests.f1.right)
        {
            prevs.f1.left = forests.f1.left++;
            forests.f1.last = subforest::Lnode;

            while(parents.it1 != forests.f1.left)
            {
                if (tree_type::is_leaf(forests.f1.left))
                {
                    last_tree_roots_stacks.s1.push_back(prevs.f1.left);
                    DEBUG("L1: last_tree <- %s", label(prevs.f1.left));
                }
                // ak som v rodicovi, zmazem vsetkych synov ktore boli v zasobniku..
                del_stack_children(last_tree_roots_stacks.s1, forests.f1.left);
                last_tree_roots.it1 = get_back_f1();

                compute_fdist_command();
                prevs.f1.left = forests.f1.left++;
            }
            forests.f1.left = prevs.f1.left;
            prevs.f1.right = forests.f1.right++;
            forests.f1.last = subforest::Rnode;

            while(parents.it1 != forests.f1.right)
            {
                if (tree_type::is_leaf(forests.f1.right))
                {
                    last_tree_roots_stacks.s1.push_back(prevs.f1.right);
                    DEBUG("R1: last_tree <- %s", label(prevs.f1.right));
                }
                del_stack_children(last_tree_roots_stacks.s1, forests.f1.right);
                last_tree_roots.it1 = get_back_f1();

                compute_fdist_command();
                prevs.f1.right = forests.f1.right++;
            }

            forests.f1.last = subforest::undef;
            forests.f1.left =
                forests.f1.path_node = forests.f1.right;
            parents.it1 = tree_type::parent(forests.f1.right);
            del_stack_children(last_tree_roots_stacks.s1, forests.f1.right);
            last_tree_roots.it1 = get_back_f1();

            compute_fdist_command();

            prevs.f1.left =
                prevs.f1.right = forests.f1.right;
        }

        forests.f2.last = subforest::Lnode;
        prevs.f2.left = forests.f2.left++;

        DEBUG("LNODE");
        // vynoril som sa z podstromu, ale .right este vzdy ukazuje do praveho syna...
        if (prevs.f2.left == forests.f2.path_node)
        {
            DEBUG(".left == .path_node");
            prevs.f2.right = prevs.f2.left;
        }
        if (tree_type::is_leaf(forests.f2.left))
        {
            last_tree_roots_stacks.s2.push_back(prevs.f2.left);
            DEBUG("L2: last <- left, %s", label(prevs.f2.left));
        }
        del_stack_children(last_tree_roots_stacks.s2, forests.f2.left);
        last_tree_roots.it2 = get_back_f2();

        if (forests.f2.left == parents.it2)
        {
            DEBUG("t2.left == parent");

            forests.f2.left = prevs.f2.left;
            forests.f2.last = subforest::Rnode;
            prevs.f2.right = forests.f2.right++;

            if (tree_type::is_leaf(forests.f2.right))
            {
                last_tree_roots_stacks.s2.push_back(prevs.f2.right);
                DEBUG("R2: last <- right, %s", label(prevs.f2.right));
            }
            del_stack_children(last_tree_roots_stacks.s2, forests.f2.right);
            last_tree_roots.it2 = get_back_f2();

            if (forests.f2.right == parents.it2)
            {
                DEBUG("t2.right == parent");
                assert(last_tree_roots_stacks.s2.empty());
                last_tree_roots.it2 = get_back_f2();

                forests.f2.last = subforest::undef;
                forests.f2.left =
                    forests.f2.path_node = forests.f2.right;
                parents.it2 = tree_type::parent(parents.it2);
            }
        }
    }
    while(loop);

    print_FDist(forest_dist);
    return forest_dist;
}



void gted::fill_table(forest_distance_table_type& forest_dist,
                const subforest_pair& roots,
                const subforest_pair& prevs,
                iterator_pair prev_roots,
                graph who_first)
{
    /*
     * do forest_dist zapisujem normalne v poradi [F1][F2]
     * do tree_distances zapisujem podla who_first:
     *      ak == T1, poradie je [F2][F1], inac [F1][F2]
     *
     * inac predpoklady su take, ze v prev_roots su:
     *      ak .last == undef, je tam vrchol na root-leaf ceste (tam kde som zacal)
     *      inac je tam predchadzajuci vrchol najblizsie k aktualnemu a nieje v jeho podstrome
     *          viz. poznamka v compute_distance funkcii
     */

#define GTED_VECTOR_DELETE_LEFT     0
#define GTED_VECTOR_DELETE_RIGHT    1
#define GTED_VECTOR_DELETE_BOTH     2

#define GTED_COST_MODIFY            0
#define GTED_COST_DELETE            1

#define labels_LR(s) label(s.left), label(s.right)

    vector<size_t> vec(3, 0xBADF00D);
    size_t c_min;
    // pocitam F[p.it1][p.it2]..
    iterator_pair p;
    if (roots.f1.last == subforest::Lnode)
        p.it1 = roots.f1.left;
    else
        p.it1 = roots.f1.right;
    if (roots.f2.last == subforest::Lnode)
        p.it2 = roots.f2.left;
    else
        p.it2 = roots.f2.right;

    vec[GTED_VECTOR_DELETE_LEFT]  =
        get_Fdist(prevs.f1, roots.f2, forest_dist);
    vec[GTED_VECTOR_DELETE_RIGHT] =
        get_Fdist(roots.f1, prevs.f2, forest_dist);

    vec[GTED_VECTOR_DELETE_LEFT]  += GTED_COST_DELETE;
    vec[GTED_VECTOR_DELETE_RIGHT] += GTED_COST_DELETE;

    if (p.it1 == roots.f1.path_node &&
            p.it2 == roots.f2.path_node)
    {
        DEBUG("path node");
        vec[GTED_VECTOR_DELETE_BOTH] =
            get_Fdist(prevs.f1, prevs.f2, forest_dist);
    }
    else
    {
        DEBUG("not path node");
        DEBUG("prevroots=[%s, %s]",
                label(prev_roots.it1), label(prev_roots.it2));

        subforest_pair other = prevs;
        tree_type::iterator t_it1, t_it2;

        if (roots.f1.last == subforest::Lnode)
        {
            DEBUG("f1.last = lnode");
            other.f1.left = prev_roots.it1;
            t_it1 = roots.f1.left;
        }
        else if (roots.f1.last == subforest::Rnode)
        {
            DEBUG("f1.last == rnode");
            other.f1.right = prev_roots.it1;
            t_it1 = roots.f1.right;
        }
        else
        {
            DEBUG("f1.last == undef");
            other.f1.left = empty_iterator();
            other.f1.right = prev_roots.it1;
            t_it1 = roots.f1.right;
        }

        if (roots.f2.last == subforest::Lnode)
        {
            DEBUG("f2.last = lnode");
            other.f2.left = prev_roots.it2;
            t_it2 = roots.f2.left;
        }
        else if (roots.f2.last == subforest::Rnode)
        {
            DEBUG("f2.last == rnode");
            other.f2.right = prev_roots.it2;
            t_it2 = roots.f2.right;
        }
        else
        {
            DEBUG("f2.last == undef");
            other.f2.left = empty_iterator();
            other.f2.right = prev_roots.it2;
            t_it2 = roots.f2.right;
        }

        vec[GTED_VECTOR_DELETE_BOTH] =
            get_Fdist(other.f1, other.f2, forest_dist);

        vec[GTED_VECTOR_DELETE_BOTH] +=
            get_Tdist(t_it1, t_it2, who_first);
    }

    LOGGER_PRINT_CONTAINER(vec, "vec");

    c_min = *min_element(vec.begin(), vec.end());
    set_Fdist(roots.f1, roots.f2, forest_dist, c_min);

    if (p.it1 == roots.f1.path_node &&
        p.it2 == roots.f2.path_node)
    {
        set_Tdist(p.it1, p.it2, c_min, who_first);
    }
    DEBUG("");
}

void gted::compute_mapping()
{
    //return;
    APP_DEBUG_FNAME;
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    using iterator = tree_type::iterator;

    print_TDist();

    forest_distance_table_type forest_dist;
    vector<iterator_pair> to_be_matched;
    iterator_pair current, leafs;
    subforest_pair forests, prevs;

/*
#define equal_funct(first1, first2, second1, second2, plus_value) \
    (forest_dist.at(first1).at(first2) == \
        forest_dist.at(second1).at(second2) + plus_value)
*/
    auto equal_funct = [&forest_dist](
            const subforest& first1, const subforest& first2,
            const subforest& second1, const subforest& second2,
            size_t plus_value)
    {
        return forest_dist.at(first1).at(first2) ==
            forest_dist.at(second1).at(second2) + plus_value;
    };
    auto print_mapping = [this]()
    {
        cout << "MAPPING:" << endl;
        for (auto val : mapping)
            cout << label(val.it1) << "\t" << label(val.it2) << endl;
    };
    auto comp_funct = [this](iterator root1, iterator root2)
    {
        LOGGER_PRIORITY_ON_FUNCTION(INFO);
        subforest_pair forests;
        // init:
        forests.f1.root = root1;
        forests.f2.root = root2;
        all_subforest_nodes_init(forests.f1, tree_type::leftmost_child(root1));
        all_subforest_nodes_init(forests.f2, tree_type::leftmost_child(root2));
        // POZOR: ^^ nieje to ekvivalentne volaniu init_subforest_pair..

        print_subforest(forests.f1);
        print_subforest(forests.f2);
        auto Ttable = tree_distances;

        logger.info("COMPFUNCT");
        auto table = compute_distance(forests, T2);
        
        // skontroluj ci sa nezmenila tabulka tree_distances
        //assert(tree_distances == Ttable);
        logger.info("COMPFUNCT_END");
        return table;
    };
    auto step = [](subforest& s)
    {
        subforest other = s;
        if (s.left == s.right)
        {   // som vo vrchole na ceste... musim ist o patro nizsie
            // ak som ale v liste, iba vyhodim .left
            if(tree_type::is_leaf(s.right))
                s.left = empty_iterator();
            else
            {
                s.left  = tree_type::first_child(s.right);
                s.right = tree_type::last_child (s.right);
            }
        }
        else if (s.left != empty_iterator())
        {   // inac idem iba s .right v post_order dozadu
            --s.right;
        }
        DEBUG("changing from [%s, %s], to [%s, %s]",
                label(other.left), label(other.right),
                label(s.left), label(s.right));
    };
    auto jump_tree = [this](subforest& s, iterator leaf)
    {
        DEBUG("jump, %s", label(s.right));
        print_subforest(s);
        subforest other = s;
        if (s.right == leaf)
        {
            DEBUG(".right == leaf");
            s.left = empty_iterator();
        }
        else if (s.right->is_root())
        {
            // TODO:
            // tu treba rovno zmenit aj prevs aj forests. 
            // oba na (<null>, leaf)
            // !!!
            DEBUG("is_root");
            logger.error("is_root, not implemented yet");
            cerr << "isroot" << endl;
            abort();
        }
        else
        {
            DEBUG("get ancestor previous sibling");
            while(tree_type::is_first_child(s.right))
                s.right = tree_type::parent(s.right);
            tree_type::sibling_iterator it = s.right;
            --it;
            s.right = it;
        }
        DEBUG("changing from [%s, %s], to [%s, %s]",
                label(other.left), label(other.right),
                label(s.left), label(s.right));
    };

    mapping.clear();
    to_be_matched.push_back({++t1.begin(), ++t2.begin()});


    while(!to_be_matched.empty())
    {
        DEBUG("WHILE");

        current = to_be_matched.back();
        to_be_matched.pop_back();
        forest_dist = comp_funct(current.it1, current.it2);
        //print_FDist(forest_dist);

        leafs.it1 = tree_type::leftmost_child(current.it1);
        leafs.it2 = tree_type::leftmost_child(current.it2);

        forests.f1.left =
            forests.f1.right = current.it1;
        forests.f2.left =
            forests.f2.right = current.it2;

        if (tree_type::is_leaf(forests.f1.right))
            prevs.f1.right = forests.f1.right;
            // prevs.f1.left == empty_iterator()
        else
        {
            prevs.f1.left  = tree_type::first_child(forests.f1.left );
            prevs.f1.right = tree_type::last_child (forests.f1.right);
        }
        if (tree_type::is_leaf(forests.f2.right))
            prevs.f2.right = forests.f2.right;
            // prevs.f2.left == empty_iterator()
        else
        {
            prevs.f2.left  = tree_type::first_child(forests.f2.left );
            prevs.f2.right = tree_type::last_child (forests.f2.right);
        }

        bool loop = true;
        while (loop)
        {
            DEBUG("FORESTS: [%s, %s][%s, %s]",
                    label(forests.f1.left), label(forests.f1.right),
                    label(forests.f2.left), label(forests.f2.right));
            DEBUG("PREVS: [%s, %s][%s, %s]",
                    label(prevs.f1.left), label(prevs.f1.right),
                    label(prevs.f2.left), label(prevs.f2.right));

            // uz som v najlavejsom liste (na ceste)
            // tak este dopocitaj toto a skonci.
            if (prevs.f1.left == empty_iterator() &&
                    prevs.f2.left == empty_iterator())
                loop = false;

            // samotny vypocet mapovania:
            if (forests.f1.right == forests.f1.left &&
                    forests.f2.right == forests.f2.left)
            {   // edit:
                // som na ceste... takze mozem dane nody namatchovat na seba
                DEBUG("map %s, %s", label(forests.f1.right), label(forests.f2.right));
                mapping.push_back({forests.f1.right, forests.f2.right});
                forests = prevs;
                step(prevs.f1);
                step(prevs.f2);
            }
            else if (equal_funct(forests.f1, forests.f2, forests.f1, prevs.f2, GTED_COST_DELETE))
            {   // delete node from F1
                DEBUG("map <null>, %s", label(forests.f2.right));
                mapping.push_back({empty_iterator(), forests.f2.right});
                forests.f2 = prevs.f2;
                step(prevs.f2);
            }
            else if (equal_funct(forests.f1, forests.f2, prevs.f1, forests.f2, GTED_COST_DELETE))
            {   // delete node from F2
                DEBUG("map %s, <null>", label(forests.f1.right));
                mapping.push_back({forests.f1.right, empty_iterator()});
                forests.f1 = prevs.f1;
                step(prevs.f1);
            }
            else
            {   // inac musim vyratat a namatchovat dane stromy rekurzivne
                DEBUG("to match %s, %s", label(forests.f1.right), label(forests.f2.right));
                to_be_matched.push_back({forests.f1.right, forests.f2.right});
                jump_tree(forests.f1, leafs.it1);
                jump_tree(forests.f2, leafs.it2);
                prevs = forests;
                step(prevs.f1);
                step(prevs.f2);
            }
        }
    }
    print_mapping();
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

bool gted::do_decompone_H(tree_type::iterator& it_ref,
                        tree_type::iterator root,
                        tree_type::iterator& it_path_node) const
{
    //LOGGER_PRIORITY_ON_FUNCTION(INFO);

    APP_DEBUG_FNAME;

    DEBUG("it=%s, pathnode=%s, root=%s",
            label(it_ref), label(it_path_node), label(root));
    
    assert(id(it_ref) <= id(root) &&
            id(it_path_node) <= id(root));

    if (it_ref == root)
    {
        DEBUG("itref == root, return false");
        assert(it_ref == it_path_node);
        return false;
    }

    tree_type::sibling_iterator it = it_ref;

    if (it == it_path_node)
        // it <- 1. brat it.
        it = tree_type::first_child(tree_type::parent(it));
    else if (tree_type::is_last_child(it))
    {
        // it <- 1. brat od rodica..
        it = tree_type::parent(it);
        it_path_node = it;
    }
    else
        ++it;

    while(it != root && it == it_path_node)
    {
        DEBUG("while, it=%s", label(it));

        if (tree_type::is_last_child(it))
        {
            // go to first sibling of parent..
            it = tree_type::parent(it);
            it_path_node = it;

            // ak nieje root, tak ma este otca, a chcem jeho prveho syna..
            // inac, root je jediny, takze to nepotrebujem riesit..
            if (!it->is_root())
                it = tree_type::first_child(tree_type::parent(it));
        }
        else
            ++it;
    }
    it_ref = it;

    bool output = root != it_ref;
    DEBUG("itout=%s, return %s", label(it_ref), output?"true":"false");
    return output;
}

bool gted::do_decompone_H_recursive(tree_type::iterator& it_ref,
                                    tree_type::iterator& leaf,
                                    tree_type::iterator end) const
{
    APP_DEBUG_FNAME;
    DEBUG("it=%s, end=%s, str=heavy",
            label(it_ref), label(end));

    // budem vzdy zacinat v najlavejsom vrchole
    // a postupne budem prechadzat strom a ak
    //  heavy_node(parent(it_ref)) != leaf,
    //      tak to znamena, ze it_ref nieje na heavy_path otca
    //      a teda ze mam dany vrchol vyratat. inac pokracujem dalej.

    tree_type::post_order_iterator it = it_ref;

    ++it;

    //while(it != end && leaf == 
    //TODO: potrebujem tu aj tabulku heavy_paths...
    //
    //moznob y stalo zato zlucit T1/T2 heavy_paths do 1 tabulky
    //.. iba najprv skontrolovat, ze IDcka v oboch stromoch su rozne...
    //a usetri sa potom kopa roboty s parametramy a tak..

    it_ref = it;

    if (tree_type::is_leaf(it_ref))
        leaf = it_ref;

    return false;
}

void gted::single_path_function_H(tree_type::iterator root1,
                                tree_type::iterator root2,
                                graph who_first)
{
    APP_DEBUG_FNAME;
    DEBUG("roots: %s %s, whofirst=%s", 
            label(root1), label(root2), to_string(who_first));

    tree_type::iterator it;
    tree_type::iterator end_it;
    subforest_pair forests;
    tree_type::iterator leaf;

    init_subforest_pair(forests, root1, root2, path_strategy::heavy, who_first);
    print_subforest(forests.f1);
    print_subforest(forests.f2);
}



/*


#ifdef NODEF

void gted::precompute_tables()
{
    APP_DEBUG_FNAME;
#define is_heavy(iter) \
        (!iter->is_root() && heavy_child(tree_type::parent(iter)) == iter)
#define is_first(iter) \
        (tree_type::is_first_child(iter))
#define is_last(iter) \
        (tree_type::is_last_child(iter))

    auto push_back = [&](tables::vector_type& vec,
            tree_type::iterator val)
    {
        DEBUG("push_back: %s", label(val));
        vec.push_back(val);
    };
    auto comp_leafs = [&](const tree_type& t)
    {
        LOGGER_PRIORITY_ON_FUNCTION(INFO);

        size_t it_id;
        tree_type::post_order_iterator it;
        tables::LRH lrh;

        for (it = t.begin_post(); it != t.end_post(); ++it)
        {
            it_id = id(it);

            if (tree_type::is_leaf(it))
            {
                lrh = {it, it, it};
            }
            else
            {
                lrh.left  = table.leafs.at(id(
                                tree_type::leftmost_child(it))).left;
                lrh.right = table.leafs.at(id(
                                tree_type::rightmost_child(it))).right;
                lrh.heavy = table.leafs.at(id(
                                heavy_child(it))).heavy;
            }
            DEBUG("leafs for '%s' = [%s, %s, %s]",
                    label(it), label(lrh.left),
                    label(lrh.right), label(lrh.heavy));
            table.leafs[it_id] = lrh;
        }
    };
    auto comp_keyroots = [&](const tree_type& t,
            bool T1)
    {
        LOGGER_PRIORITY_ON_FUNCTION(INFO);

        size_t i;
        tree_type::iterator root, path_node;
        tree_type::sibling_iterator sibling;
        bool was_first = false;

        tables::vector_type& left  = (T1 ? table.t1_t.key_left  : table.t2_t.key_left);
        tables::vector_type& right = (T1 ? table.t1_t.key_right : table.t2_t.key_right);
        tables::vector_type& heavy = (T1 ? table.t1_t.key_heavy : table.t2_t.key_heavy);

        for (int j = 0; j < 3; ++j)
        {
            tables::vector_type& tbl = (j == 0 ? left :
                                       (j == 1 ? right : heavy));
            if (j == 0)
                DEBUG("LEFT");
            else if (j == 1)
                DEBUG("RIGHT");
            else
                DEBUG("HEAVY");

            tbl.push_back(++t.begin());
            i = 0;

            while(i != tbl.size())
            {
                root = tbl.at(i);
                tables::LRH lrh = table.leafs.at(id(root));
                path_node = (j == 0 ? lrh.left :
                            (j == 1 ? lrh.right : lrh.heavy));

                was_first = false;

                DEBUG("root = %s, path_node = %s", label(root), label(path_node));

                while(path_node != root)
                {
                    sibling = tree_type::first_child(tree_type::parent(path_node));

                    while(sibling != sibling.end())
                    {
                        if (sibling != path_node)
                        {
                            push_back(tbl, sibling);
                            if (!was_first)
                            {
#define GET_INDEX_TABLE \
        (j == 0 ? table.ind_left[id(root)].keyroot : \
        (j == 1 ? table.ind_right[id(root)].keyroot : \
                  table.ind_heavy[id(root)].keyroot))

                                GET_INDEX_TABLE = tbl.size() - 1;
                                was_first = true;
                            }
                        }
                        ++sibling;
                    }
                    path_node = tree_type::parent(path_node);
                }
                if (!was_first)
                {
                    //GET_INDEX_TABLE = id(path_node);
                    //push_back(tbl, path_node);
                    GET_INDEX_TABLE = BAD_INDEX;
                    DEBUG("index %s -> %s", label(root), "null");
                }
                else
                    DEBUG("index %s -> %s", label(root), label(tbl[GET_INDEX_TABLE]));
                ++i;
            }
        }
#undef GET_INDEX_TABLE
    };
    auto comp_subforests = [&](const tree_type& t,
            bool T1)
    {
        LOGGER_PRIORITY_ON_FUNCTION(INFO);

        tables::vector_type& left  = (T1 ? table.t1_t.sub_left : table.t2_t.sub_left);
        tables::vector_type& right = (T1 ? table.t1_t.sub_right : table.t2_t.sub_right);
        tables::vector_type& heavy = (T1 ? table.t1_t.sub_heavy : table.t2_t.sub_heavy);
        tree_type::post_order_iterator it;
        tree_type::iterator child;
        size_t index;

#define lies_on_path(iter, STR) \
        (table.leafs.at(id(iter)).STR == \
         table.leafs.at(id(tree_type::parent(iter))).STR)
#define is_only_child(iter) \
        (tree_type::leftmost_child(iter) == tree_type::rightmost_child(iter))

        for (it = t.begin_post(); it != --t.end_post(); ++it)
        {
            DEBUG("%s", label(it));

            if (!is_first(it))
            {
                DEBUG("left");
                if (is_only_child(it))
                    index = left.size();
                else
                {
                    child = tree_type::leftmost_child(it);
                    // now we want child that does not lie on my path

                    if (lies_on_path(child, left))
                        child = child.node->next_sibling;

                    index = table.ind_left.at(id(child)).subforest;
                }
                left.push_back(it);
                table.ind_left.at(id(it)).subforest = index;
                DEBUG("sub %s -> %lu:%s", label(it), index, label(left.at(index)));
            }
            if (!is_last(it))
            {
                DEBUG("right");
                if (is_only_child(it))
                    index = right.size();
                else
                {
                    child = tree_type::leftmost_child(it);
                    // now we want child that does not lie on my path
                    if (lies_on_path(child, right))
                        child = child.node->next_sibling;

                    index = table.ind_right.at(id(child)).subforest;
                }
                right.push_back(it);
                table.ind_right.at(id(it)).subforest = index;
                DEBUG("sub %s -> %lu:%s", label(it), index, label(right.at(index)));
            }
            if (!is_heavy(it))
            {
                DEBUG("heavy");
                if (is_only_child(it))
                    index = heavy.size();
                else
                {
                    child = tree_type::leftmost_child(it);
                    // now we want child that does not lie on my path
                    if (lies_on_path(child, heavy))
                        child = child.node->next_sibling;

                    index = table.ind_heavy.at(id(child)).subforest;
                }
                heavy.push_back(it);
                table.ind_heavy.at(id(it)).subforest = index;
                DEBUG("sub %s -> %lu:%s", label(it), index, label(heavy.at(index)));
            }
        }
    };
    auto init_maps = [this]()
    {
        size_t size = t1.size() + t2.size();

        table.leafs.reserve(size);
        table.ind_left.reserve(size);
        table.ind_right.reserve(size);
        table.ind_heavy.reserve(size);

#define reserve(N) \
        table.t ##N## _t.sub_left.reserve (t ##N.size()); \
        table.t ##N## _t.key_left.reserve (t ##N.size()); \
        table.t ##N## _t.sub_right.reserve(t ##N.size()); \
        table.t ##N## _t.key_right.reserve(t ##N.size()); \
        table.t ##N## _t.sub_heavy.reserve(t ##N.size()); \
        table.t ##N## _t.key_heavy.reserve(t ##N.size());

        reserve(1);
        reserve(2);

        [>size_t it_id;
        tree_type::post_order_iterator it;
        tables::indexes_pair_type bad = {0xBADF00D, 0xBADF00D};
        for (tree_type t : {t1, t2})
        {
            for (it = t.begin_post(); it != t.end_post(); ++it)
            {
                it_id = id(it);
                table.ind_left[it_id]  = bad;
                table.ind_right[it_id] = bad;
                table.ind_heavy[it_id] = bad;
            }
        }<]
    };
    auto print_maps = [&]()
    {
        //LOGGER_PRIORITY_ON_FUNCTION(INFO);

        DEBUG("***************************************************");

        for (auto val : table.ind_left)
        {
            try
            {
                auto node = t1.find(val.first);
                if (val.second.keyroot == BAD_INDEX)
                    DEBUG("node %s, \tsub %lu:%s, \tkey %lu:<null>", label(node),
                            val.second.subforest, label(table.t1_t.sub_left.at(val.second.subforest)), val.second.keyroot);
                else
                    DEBUG("node %s, \tsub %lu:%s, \tkey %lu:%s", label(node),
                            val.second.subforest, label(table.t1_t.sub_left.at(val.second.subforest)),
                            val.second.keyroot, label(table.t1_t.key_left.at(val.second.keyroot)));
            } catch (...){}
        }
        DEBUG("SECOND TREE");
        for (auto val : table.ind_left)
        {
            try
            {
                auto node = t2.find(val.first);
                if (val.second.keyroot == BAD_INDEX)
                    DEBUG("node %s, \tsub %lu:%s, \tkey %lu:<null>", label(node),
                            val.second.subforest, label(table.t2_t.sub_left.at(val.second.subforest)), val.second.keyroot);
                else
                    DEBUG("node %s, \tsub %lu:%s, \tkey %lu:%s", label(node),
                            val.second.subforest, label(table.t2_t.sub_left.at(val.second.subforest)),
                            val.second.keyroot, label(table.t2_t.key_left.at(val.second.keyroot)));
            } catch (...){}
        }
        
        DEBUG("***************************************************");
        for (auto val : table.t1_t.key_left)
            cout << label(val) << " ";
        cout << endl;
        for (auto val : table.t2_t.key_left)
            cout << label(val) << " ";
        cout << endl;

    };





    init_maps();
    DEBUG("init OK");
    comp_leafs(t1);
    comp_leafs(t2);
    DEBUG("leafs OK");
    comp_keyroots(t1, true);
    comp_keyroots(t2, false);
    DEBUG("keyroots OK");
    comp_subforests(t1, true);
    comp_subforests(t2, false);
    DEBUG("subforests OK");
    print_maps();


    DEBUG("OK");
}

void gted::compute_distances_recursive(iterator_pair roots)
{
    // rozkladam pomocou keyrootov
    //
    APP_DEBUG_FNAME;

    DEBUG("pair: <%s, %s>", label(roots.it1), label(roots.it2));

    tree_type::iterator it;
    tree_type::iterator leaf;
    size_t i;
    strategy_pair spair;

    spair = strategies.at(id(roots.it1)).at(id(roots.it2));

    spair = PATH_STRATEGY_LEFT_T1;

    if (is_T2(spair))
        swap(roots.it1, roots.it2);

    i = table.get_indexes(roots.it1, spair).keyroot;
    if (i != BAD_INDEX)
    {
        leaf = table.get_leaf(roots.it1, spair);
        it = table.get_keyroot(i, spair);

        while(leaf == table.get_leaf(tree_type::parent(it), spair))
        {
            iterator_pair p = {it, roots.it2};
            if (is_T2(spair))
            {
                p.it1 = roots.it2;
                p.it2 = it;
            }
            compute_distances_recursive(p);
            it = table.get_keyroot(++i, spair);
        }
    }
    single_path_function(roots, spair);
}

void gted::single_path_function(iterator_pair roots, strategy_pair str)
{
    APP_DEBUG_FNAME;

    DEBUG("pair: <%s, %s>", label(roots.it1), label(roots.it2));

    subforest_pair forests;
    size_t i;
    tree_type::iterator it;
    iterator_pair leafs;

    if (is_T1(str))
        swap(roots.it1, roots.it2);

    leafs.it1 = table.get_leaf(roots.it1, str);
    leafs.it2 = table.get_leaf(roots.it2, str);

    DEBUG("leafs: <%s, %s>", label(leafs.it1), label(leafs.it2));

    i = table.get_indexes(roots.it1, str).subforest;
    it = table.get_subforest(i, str);

    while(id(it) < id(roots.it1))
    {
        cout << label(it) << endl;

        all_subforest_nodes_init(forests.f1, table.get_leaf(it, str));
        all_subforest_nodes_init(forests.f2, table.get_leaf(roots.it2, str));
        forests.f1.last = subforest::undef;
        forests.f1.root = it;
        forests.f2.root = roots.it2;

        compute_distance(forests, str);

        ++i;
        it = table.get_subforest(i, str);
    }

    //all_subforest_nodes_init(forests.f1, table.get_leaf(roots.it1, str));
    //all_subforest_nodes_init(forests.f2, table.get_leaf(roots.it2, str));

    //compute_distance(forests, str);
}

void gted::compute_distance(subforest_pair forests, strategy_pair str)
{
    DEBUG("computing distance between roots <%s, %s>",
            label(forests.f1.root), label(forests.f2.root));
}

#endif


*/








void gted::precompute_paths()
{
    auto comp_LR = [this](const tree_type t)
    {
        size_t it_id;
        tree_type::iterator left, right;
        tree_type::post_order_iterator it;

        for (it = t.begin_post(); it != t.end_post(); ++it)
        {
            it_id = id(it);

            if (tree_type::is_leaf(it))
            {
                left  = it;
                right = it;
            }
            else
            {
                left = tree_type::first_child(it);
                right = tree_type::last_child(it);
            }
            paths.left [it_id] = left;
            paths.right[it_id] = right;
        }
    };

    auto comp_H = [this](const tree_type t)
    {
        size_t it_id;
        tree_type::iterator parent;
        tree_type::post_order_iterator it;

        for (it = t.begin_post(); !it->is_root(); ++it)
        {
            it_id = id(it);
            parent = tree_type::parent(it);
            tree_type::iterator heavy = paths.heavy[id(parent)];

            if (tree_type::is_leaf(it))
                paths.heavy[it_id] = it;
            if (heavy == empty_iterator() ||
                    t_sizes.at(id(heavy)) < t_sizes.at(it_id))
                paths.heavy[id(parent)] = it;
        }
    };

    comp_LR(t1);
    comp_LR(t2);
    comp_H(t1);
    comp_H(t2);
}



#endif



bool gted::iterator_pair::operator==(const gted::iterator_pair& other) const
{
    return it1 == other.it1 &&
        it2 == other.it2;
}

size_t gted::iterator_pair::hash::operator()(const gted::iterator_pair& s) const
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);
    return 0;
/*
    // zdroj hash funkcie:
    // http://stackoverflow.com/questions/682438/hash-function-providing-unique-uint-from-an-integer-coordinate-pair
    auto hash_f = [](size_t x, size_t y) 
    {
        return (x * 0x1F1F1F1F) ^ y;
    };
    assert(s.right.node != NULL);

    size_t out;
    if (s.left == s.right || s.left.node == NULL)
        out = s.right->get_id();
    else
        out = hash_f(s.left->get_id(), s.right->get_id());

#ifdef GTED_CHECKS_DISABLED
    static vector<subforest> hashes;
    if (find(hashes.begin(), hashes.end(), s) == hashes.end())
    {
        hashes.push_back(s);
        DEBUG("HASH [%s, %s] = %i", label(s.left), label(s.right), out);
    }
#endif
    return out;*/
}





gted::gted(const tree_type& _t1, const tree_type& _t2)
    : t1(_t1), t2(_t2)
{
    auto merge = [this](rted::map_type m1, rted::map_type m2)
    {
        if (t2.size() > t1.size())
            m1.swap(m2);

        t_sizes = std::move(m1);
        t_sizes.insert(m2.begin(), m2.end());
    };

    APP_DEBUG_FNAME;
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    rted r(t1, t2);
    r.run_rted();

    strategies = r.get_strategies();
    merge(r.get_t1_sizes(), r.get_t2_sizes());
}

void gted::run_gted()
{
    precompute_tables();

    iterator_pair roots;
    roots.it1 = ++t1.begin();
    roots.it2 = ++t2.begin();

    //compute_distances_recursive(roots);
}

gted::tree_type::iterator gted::heavy_child(tree_type::iterator root) const
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    assert(!tree_type::is_leaf(root));
    tree_type::sibling_iterator it(tree_type::first_child(root));
    tree_type::iterator out;
    size_t index = 0;
    size_t biggest_subtree = 0;
    size_t i = 0;

    while(it != it.end())
    {
        auto val = t_sizes.at(id(it));
        if (val > biggest_subtree)
        {
            index = i;
            biggest_subtree = val;
            out = it;
        }
        ++it;
        ++i;
    }
    logger.debug("child # '%lu - %s' of node '%s' has biggest subtree (size == %lu)",
            index, label(out), label(root), biggest_subtree);
    return out;
}

void gted::precompute_tables()
{
    APP_DEBUG_FNAME;

    auto push_back = [&](tables::vector_type& vec,
            tree_type::iterator val)
    {
        DEBUG("push_back: %s", label(val));
        vec.push_back(val);
    };
    auto comp_leafs = [&](const tree_type& t)
    {
        size_t it_id;
        tree_type::post_order_iterator it;
        tables::LRH lrh;

        for (it = t.begin_post(); it != t.end_post(); ++it)
        {
            it_id = id(it);

            if (tree_type::is_leaf(it))
                lrh = {it, it, it};
            else
            {
                lrh.left = precomputed.leafs.at(
                        id(tree_type::leftmost_child(it))).left;
                lrh.right = precomputed.leafs.at(
                        id(tree_type::rightmost_child(it))).right;
                lrh.heavy = precomputed.leafs.at(
                        id(heavy_child(it))).heavy;
            }
            DEBUG("leafs for '%s' = [%s, %s, %s]",
                    label(it), label(lrh.left),
                    label(lrh.right), label(lrh.heavy));
            precomputed.leafs[it_id] = lrh;
        }
    };
    auto comp_keyroots = [&](const tree_type& t, bool T1)
    {
#define LEFT_INDEX    0
#define RIGHT_INDEX   1
#define HEAVY_INDEX   2
        tree_type::iterator root, path_node;
        tree_type::sibling_iterator sibling;
        tables::vector_type& left = precomputed.keyroots[
            (T1 ? PATH_STRATEGY_LEFT_T1 : PATH_STRATEGY_LEFT_T2)];
        tables::vector_type& right = precomputed.keyroots[
            (T1 ? PATH_STRATEGY_RIGHT_T1 : PATH_STRATEGY_RIGHT_T2)];
        tables::vector_type& heavy = precomputed.keyroots[
            (T1 ? PATH_STRATEGY_HEAVY_T1 : PATH_STRATEGY_HEAVY_T2)];

        enum lrh_list
        {
            L, R, H
        };
        auto do_loop = [&push_back](tables::vector_type& tbl, tables::root_leaf_path_type& leafs, tables::indexes_type& indexes, lrh_list l)
        {
            bool was_first;
            size_t i = 0;
            tree_type::iterator root, path_node, prev;
            tree_type::sibling_iterator sibling;
            while(i != tbl.size())
            {
                root = tbl.at(i);
                tables::LRH lrh = leafs.at(id(root));
                path_node = (l == L ? lrh.left : 
                        (l == R ? lrh.right : lrh.heavy));
                was_first = false;
                prev = empty_iterator();

                //DEBUG("root = %s, path_node = %s", label(root), label(path_node));

                indexes[id(root)].keyroot = BAD_INDEX;
                if (path_node == root)
                {}
                else
                {
                    while(true)
                    {
                        // get first sibling..
                        sibling = tree_type::first_child(tree_type::parent(path_node));
                        indexes[id(path_node)].keyroot = BAD_INDEX;

                        while(sibling != sibling.end())
                        {
                            if (sibling != path_node)
                            {
                                push_back(tbl, sibling);
                                if (!was_first)
                                {
                                    indexes[id(root)].keyroot = tbl.size() - 1;
                                    was_first = true;
                                }
                            }
                            ++sibling;
                        }
                        prev = path_node;
                        path_node = tree_type::parent(path_node);

                        if (path_node == root)
                            break;

/*
                        // set keyroot for pathnode
                        if (is_only_child(prev))
                        {
                            //indexes[id(path_node)].keyroot = indexes[id(prev)].keyroot;
                        }
                        else
                        {
                            if (indexes[id(prev)].keyroot == BAD_INDEX)
                            {
                                cout << label(prev) << endl;
                                if (!tree_type::is_last_child(prev))
                                    prev = ++tree_type::sibling_iterator(prev);
                                cout << label(prev) << endl;
                            }
                        }
                        indexes[id(path_node)].keyroot = indexes[id(prev)].keyroot;
                        cout << "PN, prev " << label(path_node) << ":" << label(prev) << endl;
*/
                    }
                }
                //DEBUG("index %s -> %s", label(root), "null");
                //DEBUG("index %s -> %s", label(root), label(tbl.at(indexes.at(id(root)).keyroot)));
                size_t j = indexes[id(root)].keyroot;
                stringstream s;
                if (j == BAD_INDEX)
                    s << "<null>";
                else
                    while(j < tbl.size())
                        s << label(tbl.at(j++)) << " ";
                DEBUG("keyroots for %s: \t%s", label(root), s.str().c_str());

                ++i;
            }
        };

        auto begin = ++t.begin();
        left.push_back( begin);
        right.push_back(begin);
        heavy.push_back(begin);

        DEBUG("LEFT");
        do_loop(left,  precomputed.leafs, precomputed.indexes.at(LEFT_INDEX), L);
        DEBUG("RIGHT");
        do_loop(right, precomputed.leafs, precomputed.indexes.at(RIGHT_INDEX), R);
        DEBUG("HEAVY");
        do_loop(heavy, precomputed.leafs, precomputed.indexes.at(HEAVY_INDEX), H);
    };
    auto comp_subforests = [&](const tree_type& t, bool T1)
    {
        tables::vector_type& left = precomputed.subforests[
            (T1 ? PATH_STRATEGY_LEFT_T1 : PATH_STRATEGY_LEFT_T2)];
        tables::vector_type& right = precomputed.subforests[
            (T1 ? PATH_STRATEGY_RIGHT_T1 : PATH_STRATEGY_RIGHT_T2)];
        tables::vector_type& heavy = precomputed.subforests[
            (T1 ? PATH_STRATEGY_HEAVY_T1 : PATH_STRATEGY_HEAVY_T2)];

        tables::indexes_type& ind_left  = precomputed.indexes.at(LEFT_INDEX);
        tables::indexes_type& ind_right = precomputed.indexes.at(RIGHT_INDEX);
        tables::indexes_type& ind_heavy = precomputed.indexes.at(HEAVY_INDEX);


        tree_type::post_order_iterator it;
        tree_type::sibling_iterator child;
        size_t index;
        
        for (it = t.begin_post(); it != --t.end_post(); ++it)
        {
            // if it does not lie on path, we add it to vector
            // Left  -> !is_first
            // Right -> !is_last
            // Heavy -> !is_heavy
            //
            // and then we copy value of first non-path-node (first/second child)
            //
            if (!is_first(it))
            {
                DEBUG("left");
                if (is_only_child(it))
                    index = left.size();
                else
                {
                    // we want child that does not lie on my path
                    child = tree_type::leftmost_child(it);
                    if (lies_on_path(child, left))
                        ++child;

                    index = ind_left.at(id(child)).subforest;
                }
                left.push_back(it);
                ind_left.at(id(it)).subforest = index;
                DEBUG("sub %s -> %lu:%s", label(it), index, label(left.at(index)));
            }
            if (!is_last(it))
            {
                DEBUG("right");
                if (is_only_child(it))
                    index = right.size();
                else
                {
                    child = tree_type::leftmost_child(it);
                    if (lies_on_path(child, right))
                        child = child.node->next_sibling;

                    index = ind_right.at(id(child)).subforest;
                }
                right.push_back(it);
                ind_right.at(id(it)).subforest = index;
                DEBUG("sub %s -> %lu:%s", label(it), index, label(right.at(index)));
            }
            if (!is_heavy(it))
            {
                DEBUG("heavy");
                if (is_only_child(it))
                    index = heavy.size();
                else
                {
                    child = tree_type::leftmost_child(it);
                    if (lies_on_path(child, heavy))
                        child = child.node->next_sibling;

                    index = ind_heavy.at(id(child)).subforest;
                }
                heavy.push_back(it);
                ind_heavy.at(id(it)).subforest = index;
                DEBUG("sub %s -> %lu:%s", label(it), index, label(heavy.at(index)));
            }
        }
        assert(it == --t.end_post());
        it = tree_type::first_child(it);

        left.push_back(it);
        right.push_back(it);
        heavy.push_back(it);

        ind_left.at(id(it)).subforest = 0;
        ind_right.at(id(it)).subforest = 0;
        ind_heavy.at(id(it)).subforest = 0;

        DEBUG("sub LRH %s -> <0:%s, 0:%s, 0:%s>",
                label(it), label(left.at(0)),
                label(right.at(0)), label(heavy.at(0)));
    };
    auto print_tables = [&]()
    {
        auto print = [](tables::vector_type& vec)
        {
            for (auto val : vec)
                cout << label(val) << " ";
            cout << endl;
        };

        DEBUG("SUBFORESTS:");
        print(precomputed.subforests[PATH_STRATEGY_LEFT_T1]);
        print(precomputed.subforests[PATH_STRATEGY_LEFT_T2]);
        print(precomputed.subforests[PATH_STRATEGY_RIGHT_T1]);
        print(precomputed.subforests[PATH_STRATEGY_RIGHT_T2]);
        print(precomputed.subforests[PATH_STRATEGY_HEAVY_T1]);
        print(precomputed.subforests[PATH_STRATEGY_HEAVY_T2]);
        DEBUG("KEYROOTS:");
        print(precomputed.keyroots[PATH_STRATEGY_LEFT_T1]);
        print(precomputed.keyroots[PATH_STRATEGY_LEFT_T2]);
        print(precomputed.keyroots[PATH_STRATEGY_RIGHT_T1]);
        print(precomputed.keyroots[PATH_STRATEGY_RIGHT_T2]);
        print(precomputed.keyroots[PATH_STRATEGY_HEAVY_T1]);
        print(precomputed.keyroots[PATH_STRATEGY_HEAVY_T2]);
    };

    DEBUG("LEAFS T1:");
    comp_leafs(t1);
    DEBUG("LEAFS T2:");
    comp_leafs(t2);
    DEBUG("KEYROOTS T1:");
    comp_keyroots(t1, true);
    DEBUG("KEYROOTS T2:");
    comp_keyroots(t2, false);

    //DEBUG("SUBFORESTS T1:");
    //comp_subforests(t1, true);
    //DEBUG("SUBFORESTS T2:");
    //comp_subforests(t2, false);

    print_tables();

    //cout << label(precomputed.keyroots.at(PATH_STRATEGY_LEFT_T1).at(precomputed.indexes.at(LEFT_INDEX).at(id(t1.find(string("55")))).keyroot)) << endl;
    //cout << label(precomputed.subforests.at(PATH_STRATEGY_LEFT_T1).at(precomputed.indexes.at(LEFT_INDEX).at(id(t1.find(string("55")))).subforest)) << endl;
}




gted::tables::tables()
{
    subforests.resize(6);
    keyroots.resize(6);
    indexes.resize(3);
}

gted::tables::indexes_pair_type gted::tables::get_indexes(tree_type::iterator it, strategy_pair str) const
{
    //APP_DEBUG_FNAME;
    assert(is_path_strategy(str));

    size_t it_id = id(it);
    if (is_left_path(str))
        return indexes.at(LEFT_INDEX).at(it_id);
    if (is_right_path(str))
        return indexes.at(RIGHT_INDEX).at(it_id);
    return indexes.at(HEAVY_INDEX).at(it_id);
}

gted::tree_type::iterator gted::tables::get_leaf(tree_type::iterator it, strategy_pair str) const
{
    //APP_DEBUG_FNAME;
    assert(is_path_strategy(str));

    LRH lrh = leafs.at(id(it));
    if (is_left_path(str))
        return lrh.left;
    if (is_right_path(str))
        return lrh.right;
    return lrh.heavy;
}

gted::tree_type::iterator gted::tables::get_subforest(size_t index, strategy_pair str) const
{
    //APP_DEBUG_FNAME;
    assert(is_path_strategy(str));

    if (is_T1(str))
    {
        if (str == PATH_STRATEGY_LEFT_T1)
            str = PATH_STRATEGY_LEFT_T2;
        else if (str == PATH_STRATEGY_RIGHT_T1)
            str = PATH_STRATEGY_RIGHT_T2;
        else
            str = PATH_STRATEGY_HEAVY_T2;
    }
    else
    {
        if (str == PATH_STRATEGY_LEFT_T2)
            str = PATH_STRATEGY_LEFT_T1;
        else if (str == PATH_STRATEGY_RIGHT_T2)
            str = PATH_STRATEGY_RIGHT_T1;
        else
            str = PATH_STRATEGY_HEAVY_T1;
    }

    return subforests.at(str).at(index);
}

gted::tree_type::iterator gted::tables::get_keyroot(size_t index, strategy_pair str) const
{
    //APP_DEBUG_FNAME;
    assert(is_path_strategy(str));

    if (keyroots.at(str).size() > index)
        return keyroots.at(str).at(index);
    DEBUG("empty it");
    return empty_iterator();
}





void gted::compute_distances_recursive(iterator_pair roots)
{
    // rozkladam pomocou keyrootov
    //
    APP_DEBUG_FNAME;

    DEBUG("pair: <%s, %s>", label(roots.it1), label(roots.it2));

    tree_type::iterator it;
    tree_type::iterator leaf;
    size_t i;
    strategy_pair str;

    str = strategies.at(id(roots.it1)).at(id(roots.it2));

    //str = PATH_STRATEGY_LEFT_T1;
    //str = PATH_STRATEGY_LEFT_T2;
    //str = PATH_STRATEGY_RIGHT_T1;
    //str = PATH_STRATEGY_RIGHT_T2;
    //str = PATH_STRATEGY_HEAVY_T1;
    //str = PATH_STRATEGY_HEAVY_T2;

    logger.info("strategy: %s", strategy_to_string(str).c_str());

    if (is_T2(str))
        swap(roots.it1, roots.it2);

    i = precomputed.get_indexes(roots.it1, str).keyroot;
    if (i != BAD_INDEX)
    {
        leaf = precomputed.get_leaf(roots.it1, str);
        it = precomputed.get_keyroot(i, str);
        //cout << label(leaf) << ":" << label(it) << endl;
        //cout << (it != empty_iterator()) << endl;
        //cout << (leaf == precomputed.get_leaf(tree_type::parent(it), str)) << endl;

        // while it is own keyroot => leaf(parent(it)) == leaf
        while(it != empty_iterator() && leaf == precomputed.get_leaf(tree_type::parent(it), str))
        {
            iterator_pair p = {it, roots.it2};
            if (is_T2(str))
            {
                p.it1 = roots.it2;
                p.it2 = it;
            }
            compute_distances_recursive(p);
            //cout << "while: " << label(leaf) << ":" << label(it) << endl;
            it = precomputed.get_keyroot(++i, str);
        }
    }
    else
        DEBUG("BAD_INDEX");
    single_path_function(roots, str);
}

void gted::single_path_function(iterator_pair roots, strategy_pair str)
{
    APP_DEBUG_FNAME;

    DEBUG("SPF: pair: <%s, %s>", label(roots.it1), label(roots.it2));

    subforest_pair forests;
    size_t i;
    tree_type::iterator it;
    iterator_pair leafs;

    swap(roots.it1, roots.it2);

    leafs.it1 = precomputed.get_leaf(roots.it1, str);
    leafs.it2 = precomputed.get_leaf(roots.it2, str);

    DEBUG("leafs: <%s, %s>", label(leafs.it1), label(leafs.it2));

    i = precomputed.get_indexes(roots.it1, str).subforest;
    it = precomputed.get_subforest(i, str);

    while(id(it) < id(roots.it1))
    {
        all_subforest_nodes_init(forests.f1, precomputed.get_leaf(it, str));
        all_subforest_nodes_init(forests.f2, precomputed.get_leaf(roots.it2, str));
        forests.f2.last = forests.f1.last = subforest::undef;
        forests.f1.root = it;
        forests.f2.root = roots.it2;

        compute_distance(forests, str);

        ++i;
        it = precomputed.get_subforest(i, str);
    }

    all_subforest_nodes_init(forests.f1, precomputed.get_leaf(roots.it1, str));
    all_subforest_nodes_init(forests.f2, precomputed.get_leaf(roots.it2, str));
    forests.f1.root = roots.it1;
    forests.f2.root = roots.it2;
    forests.f2.last = forests.f1.last = subforest::undef;

    compute_distance(forests, str);
}

void gted::compute_distance(subforest_pair forests, strategy_pair str)
{
    if (is_T1(str))
        DEBUG("computing distance between roots <%s, %s>",
                label(forests.f2.root), label(forests.f1.root));
    else
        DEBUG("computing distance between roots <%s, %s>",
                label(forests.f1.root), label(forests.f2.root));
}










