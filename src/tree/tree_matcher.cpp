/*
 * File: tree_matcher.cpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard.elias@matfyz.cz>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */


#include "tree_matcher.hpp"
#include "mapping.hpp"

using namespace std;

#define set_remake(iter) \
rna_tree::parent(iter)->remake_ids.push_back(child_index(iter));

//maps trees
matcher::matcher(
                 const rna_tree& templated,
                 const rna_tree& other /*target*/)
: t1(templated), t2(other)
{ }

rna_tree& matcher::run(
                       const mapping& map)
{
    INFO("BEG: Transforming trees with mapping function");

//    post_order_iterator it = t1.begin_post();
//    size_t i = 0;
//    for (auto mp : map.get_to_update())
//    {
//        size_t index = mp.from;
//        --index;    // indexed from one
//        size_t to_move = index - i;
//        it = plusplus(it, to_move);
//
////        assert(!rna_tree::is_root(it));
//        it->_id_mapped = mp.from;
//        i = index;
//    }

    //Sizes of the trees after deletion from one and insertion into the other should match
    if (t1.size() - map.get_to_remove().size() != t2.size() - map.get_to_insert().size())
    {
        throw illegal_state_exception("Computed sizes of removing/inserting does not match current trees");
    }


    
    mark(t1, map.get_to_remove(), rna_pair_label::deleted); //t1 is template -> remove superfluous nodes
    mark(t2, map.get_to_insert(), rna_pair_label::inserted); //t2 is target -> add nodes which are missing in template

    t1.set_pre_postorder_ids();


    //remove nodes from t1 and mark this in parent using set_remake
    erase();
    
    t1.set_postorder_ids();
    t2.set_postorder_ids();
    
    compute_sizes();

    //template (t1) has already deleted nodes which are not supposed to be in target(t2)
    //now we need to add nodes into template which are in target but not in template and rename the nodes which were mapped
    //after that, t1 will be the structure which we need (basically target with position from template)
    merge();
    
//    if (!t1.correct_pairing() || !t2.correct_pairing())
//    {
//        throw illegal_state_exception("Uncorrect tree pairing after transforming template to target tree");
//    }
    
//    set_53_labels(t1);
    t1.set_postorder_ids();
    
    INFO("END: Transforming trees with mapping function");
    return t1; //Resulting tree which will be used from now on (we are done with T2 at this point)
}

void matcher::mark(
                   rna_tree& rna,
                   const indexes_type& postorder_indexes,
                   rna_pair_label::status_type status)
{
    post_order_iterator it = rna.begin_post();
    size_t i = 0;
    
    for (size_t index : postorder_indexes)
    {
        --index;    // indexed from one
        size_t to_move = index - i;
        it = plusplus(it, to_move);
        
        assert(!rna_tree::is_root(it));
        it->status = status;
        i = index;
    }
    
    rna.print_tree();
}

void matcher::erase()
{
    iterator it;
    sibling_iterator ch;
    
    for (it = t1.begin(); it != t1.end(); ++it)
    {
        for (ch = it.begin(); ch != it.end();)
        {
            if (is(ch, rna_pair_label::deleted))
            {
                /*
                 * Set Information for the drawing algorithm that descendants will need to be moved
                 * and siblings (loop) will need to be repositioned on the circle
                 */
                set_remake(ch);
                ch = t1.erase(ch);
                
                continue;
            }
            ++ch;
        }
    }
}

//mapovani jednoho stromu na druhy
void matcher::merge()
{
    iterator it1, it2;
    sibling_iterator ch1, ch2, ins;
    size_t actual, needed, steal;
    
    it1 = t1.begin();
    it2 = t2.begin();
    
    it1->set_label_strings(*it2);
    
    while (it1 != t1.end() && it2 != t2.end())
    {
        ch1 = it1.begin();
        ch2 = it2.begin();

        while (ch2 != it2.end())
        {
            if (is(ch2, rna_pair_label::inserted))
            {
                /*
                 * When inserting into a position with N siblings, steal denotes how many siblings will be taken
                 * as children of the just inserted node. The reason is that there are more possible ways how to
                 * carry out insertion at a position where siblings exist.
                 */
                
                steal = 0;
                ins = ch1;
                
                if (ch2->paired())
                {
                    actual = 0;
                    needed = s2.at(id(ch2)); //Taking into account the number of siblings needed in order for the tree to have the required structure
                    
                    while (actual < needed)
                    {
                        actual += s1.at(id(ch1));
                        ++steal;
                        ++ch1;
                    }
                    assert(actual == needed);
                }
                ch1 = t1.insert(ins, *ch2, steal);
                ch1->clear_points();
                set_remake(ch1);
            }
            else
            {
                ch1->set_label_strings(*ch2);
            }
            
            ++ch2;
            ++ch1;
        }
        
        assert(ch1 == it1.end() && ch2 == it2.end());
        
        make_unique(it1);
        
        ++it1;
        ++it2;
    }
    
    assert(it1 == t1.end() && it2 == t2.end());
    assert(t1 == t2);
}


void matcher::compute_sizes()
{
    /*
     * Counts the size of every subtree (s1 and s2 vectors) for furhter utilization (does not tak into account
     * inserted and deleted nodes).
     */
    APP_DEBUG_FNAME;
    
    auto comp_f =
    [](rna_tree& rna, vector<size_t>& sizes) {
        post_order_iterator it;
        sibling_iterator ch;
        
        sizes.resize(rna.size());
        
        for (auto it = rna.begin(); it != rna.end(); ++it)
            assert(id(it) < sizes.size());
        
        for (it = rna.begin_post(); it != rna.end_post(); ++it)
        {
            sizes[id(it)] =
            (is(it, rna_pair_label::inserted) ||
             is(it, rna_pair_label::deleted)) ? 0 : 1;
            
            if (!rna_tree::is_leaf(it))
                for (ch = it.begin(); ch != it.end(); ++ch)
                    sizes[id(it)] += sizes[id(ch)];
        }
    };
    
    comp_f(t1, s1);
    comp_f(t2, s2);
    
    assert(s1.at(id(t1.begin())) == s2.at(id(t2.begin())));
    
}

/* inline */
void matcher::make_unique(
                          iterator it)
{
    auto& vec = it->remake_ids;
    sort(vec.begin(), vec.end());
    auto end = unique(vec.begin(), vec.end());
    vec.erase(end, vec.end());
}
