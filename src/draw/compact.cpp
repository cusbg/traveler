/*
 * File: compact.cpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard.elias@matfyz.cz>, 2017 David Hoksza <david.hoksza@mff.cuni.cz>
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

#include "compact.hpp"
#include "compact_circle.hpp"
#include "compact_utils.hpp"
#include "overlap_checks.hpp"

#include "iostream"

using namespace std;

#define MULTIBRANCH_MINIMUM_SPLIT   10

#define PAIRS_DISTANCE rna.get_pair_base_distance()
#define BASES_DISTANCE rna.get_pairs_distance()

compact::compact(
                 rna_tree& _rna)
: rna(_rna)
{ }


void compact::run()
{
    APP_DEBUG_FNAME;
    
    INFO("BEG: Computing RNA layout for:\n%s", rna.print_tree(false));
    
    init();
    make();
    set_53_labels(rna);
    try_reposition_new_root_branches();
    checks();
    
    INFO("END: Computing RNA layout");
}



/* static */ void compact::shift_branch(
                                        iterator parent,
                                        point vector)
{
    function<size_t(iterator)> recursion =
    [&recursion, &vector](iterator it) {
        
        sibling_iterator ch;
        size_t out = 1;
        
        for (ch = it.begin(); ch != it.end(); ++ch)
            out += recursion(ch);
        
        if (it->initiated_points())
            for (size_t i = 0; i < it->size(); ++i)
                it->set_p(it->at(i).p + vector, i);
        
        return out;
    };
    
    recursion(parent);
}

//https://www.geeksforgeeks.org/find-mirror-image-point-2-d-plane/
pair<double, double> mirrorImage(
        double a, double b, double c,
        double x1, double y1)
{
    double temp = -2 * (a * x1 + b * y1 + c) /
                  (a * a + b * b);
    double x = temp * a + x1;
    double y = temp * b + y1;
    return make_pair(x, y);
}

bool is_leftest_pair(rna_tree::iterator it) {

    rna_tree::iterator parent = rna_tree::parent(it);
    rna_tree::sibling_iterator its = rna_tree::sibling_iterator(it);

    bool left_end = true;
    while(its != parent.begin()) {
        its--;
        if (its->paired()) {
            left_end = false;
            break;
        }
    }
    if (its->paired()) left_end = false;

    return left_end;
}

bool is_rightest_pair(rna_tree::iterator it) {

    rna_tree::iterator parent = rna_tree::parent(it);
    rna_tree::sibling_iterator its = ++rna_tree::sibling_iterator(it);

    bool right_end = true;
    while(its != parent.end()) {
        its++;
        if (its->paired()) {
            right_end = false;
            break;
        }
    }

    return right_end;
}

void compact::rotate_branch_by_angle(iterator branch, double angle){

    iterator parent = rna_tree::parent(branch);

    bool left_end = is_leftest_pair(branch);
    bool right_end = is_rightest_pair(branch);

    //if the branch is the most left among siblings, lef's try to rotate only the first residue in the root base pair
    //and the other way around if it's the most right
    if (left_end) {
        point pivot = branch->at(1).p;

        for (post_order_iterator it = parent.begin(); it != branch; it++) {
            rotate_point_around_pivot(pivot, it->at(0).p, angle);
            if (it->paired()) rotate_point_around_pivot(pivot, it->at(1).p, angle);
        }
    }

    if (right_end) {
        point pivot = branch->at(0).p;

        for (iterator it = iterator(branch); it != parent.end(); it++) {
            it->at(0).p = rotate_point_around_pivot(pivot, it->at(0).p, angle);
            if (it->paired()) it->at(1).p = rotate_point_around_pivot(pivot, it->at(1).p, angle);
        }
    }



}

/* static */ void compact::mirror_branch(
                                         iterator root)
{
    point pr[2] = {root->at(0).p, root->at(1).p};
//    double aux;
//    for (int i = 0; i <= 2; i++) {
//        aux = pr[0].x;
//        pr[0].x = pr[0].y;
//        pr[0].y = aux;
//    }



    function<void(iterator)> recursion =
    [&recursion, &pr](iterator it) {
        
        if (it->initiated_points())
            for (size_t i = 0; i < it->size(); ++i) {
                //Get the vector between mirror line and point
                point p = it->at(i).p;

                //https://bobobobo.wordpress.com/2008/01/07/solving-linear-equations-ax-by-c-0/
                double a = pr[0].y - pr[1].y;
                double b = pr[1].x - pr[0].x;
                double c = pr[0].x*pr[1].y - pr[1].x*pr[0].y;

                auto m = mirrorImage(a, b, c, p.x, p.y);
                it->set_p(point(m.first, m.second), i);


                //double a = angle(p, center_root, center_root + v);
                //Rotate the point in the reverse directions
                //it->at(i).p += rotate(center_root, -a, distance(p, center_root));
                
                //http://stackoverflow.com/questions/3306838/algorithm-for-reflecting-a-point-across-a-line
                //first project p on the mirror line
//                double xDiff = pr[1].x - pr[0].x;
//                double a = xDiff == 0 ? 0 : (pr[1].y - pr[0].y) / xDiff;
//
//                double b = pr[0].y - pr[0].x * a;
//                //                        point pl(0 + (b * p.x) / (1 + m * m), b + (m * p.x) / (1 + m * m));
//                //                        it->at(i).p = 2 * pl - p;
//                double d = (p.x + (p.y - b)*a)/(1 + a*a);
//                it->set_p(point(2*d - p.x, 2*d*a - p.y + 2*b), i);
                
            }
        
        sibling_iterator ch;
        
        for (ch = it.begin(); ch != it.end(); ++ch)
            recursion(ch);
    };
    
    //Recursively mirror each point with respect to the line defined by the parent's center and direction vector
    recursion(root);
}

/* static */ void compact::set_distance(
                                        iterator parent,
                                        iterator child,
                                        double dist)
{
    assert(rna_tree::parent(child) == parent);
    
    set_distance(child, parent->center(), dist);
}

/* static */ void compact::set_distance(
                                        iterator it,
                                        point from,
                                        double dist)
{
    point p = it->center();
    point vec = normalize(p - from);
    double actual = distance(p, from);
    vec = vec * (dist - actual);
    
    shift_branch(it, vec);
}

/* static */ bool compact::remake_child(
                                        iterator parent,
                                        size_t n)
{
    return is(parent, rna_pair_label::inserted) ||
    contains(parent->remake_ids, n);
}

/* static */ bool compact::to_remake_children(
                                              iterator parent)
{
    return parent->paired() &&
    (!parent->remake_ids.empty() ||
     is(parent, rna_pair_label::inserted));
}

/* static */ void compact::rotate_branch(
                                         iterator it,
                                         circle c,
                                         double alpha)
{
    assert(!rna_tree::is_leaf(it));
    
    for (pre_post_order_iterator ch = pre_post_order_iterator(it, true); id(ch) <= id(it); ++ch)
    {
        if (!ch->initiated_points())
            continue;
        
        double dist = distance(c.centre, ch->at(ch.label_index()).p);
        c.p1 = ch->at(ch.label_index()).p;
        c.p2 = move_point(c.centre, c.p2, dist);
        
        ch->set_p(c.rotate(alpha), ch.label_index());
    }
}

void normalize_nodes(rna_tree& rna, std::vector<compact::sibling_iterator> nodes, bool five_end){
    for (int i = 1; i < nodes.size(); ++i) {
        point p0 = i == 1 && !five_end ? nodes[i-1]->at(1).p : nodes[i-1]->at(0).p;
        point p1 = nodes[i]->at(0).p;
        double d = distance(p0, p1);
        point shift_vect = normalize(p1 - p0) * (d-BASES_DISTANCE);

        for (int j  = i; j < nodes.size(); ++j) {
            nodes[j]->at(0).p -= shift_vect;
        }
    }
}


void normalize_3_end(rna_tree &  rna) {
    APP_DEBUG_FNAME;

    compact::iterator root = rna.begin();

    compact::sibling_iterator it = compact::sibling_iterator(root.begin().range_last());
    //locate last node with children
    while(!it->paired() && it != root.begin()) it--;

    std::vector<compact::sibling_iterator> nodes;
    while (it != root.end()) nodes.push_back(it++);
    if (nodes.size() < 2) return;

    normalize_nodes(rna, nodes, false);
}

void normalize_5_end(rna_tree& rna) {
    APP_DEBUG_FNAME;

    compact::iterator root = rna.begin();

    compact::sibling_iterator it = root.begin();
    std::vector<compact::sibling_iterator> nodes;
    while(!it->paired() && it != root.end()) nodes.push_back(it++);
    nodes.push_back(it);
    if (nodes.size() < 2) return;

    //reverse the order so that the node go in the direction from the first base pair to the beginning of the structure
    std::reverse(nodes.begin(), nodes.end());

    normalize_nodes(rna, nodes, true);
}


void normalize_53_ends(rna_tree &rna){
    normalize_5_end(rna);
    normalize_3_end(rna);
}

void shift_region(compact::iterator begin, compact::iterator end, point vec)
{
    for (compact::iterator it = begin; it != end; ++it) {
        if (!it->at(0).p.bad()) it->at(0).p += vec;
        if (it->paired() && !it->at(1).p.bad()) it->at(1).p += vec;
    }
}

/**
 * Initializes possibly inserted or shifted unpaired residues on the root level, i.e. either trailing 5' or 3' ends
 * or inner regions.
 * @param rna
 */
void init_root_level_unpaired(rna_tree &  rna){
    APP_DEBUG_FNAME;

    typedef tree_node_<rna_pair_label> node;

    //obtain 3' unpaired
    vector<vector<compact::sibling_iterator>> intervals;
    std::vector<compact::sibling_iterator> interval;
    compact::sibling_iterator root = rna.begin();
    for (compact::sibling_iterator it = root.begin(); it != root.end(); ++it) { //traverse the tree pre-order
        if (it->paired() || it->initiated_points()){
            if (!interval.empty())
            {
                intervals.push_back(std::vector<compact::sibling_iterator>(interval));
                interval.clear();
            }

        } else {
            interval.push_back(it);
        }
    }
    if (!interval.empty()) intervals.push_back(std::vector<compact::sibling_iterator>(interval));

    for (auto interval1: intervals){
        compact::sibling_iterator s[] = {interval1[0].node->prev_sibling, interval1[interval1.size()-1].node->next_sibling};
        point p[] = {point(), point()};

        for (int i = 0; i < 2; i++) {

            if (s[i] != nullptr) {
                // if either previous or following siblings are NULL we are dealing with trailing regions
                if (!s[i]->paired()) {
                    p[i] = s[i]->at(0).p;
                } else {
                    p[i] = s[i]->at(1-i).p;
                }
            }
        }

        if (p[1].bad()) {
            // if p2 is invalid point interval1 is the 3' trailing region

            point p_aux;
            compact::post_order_iterator it = compact::iterator(interval1.front());
            it--; //obtaining last valid sibling, i.e. s[0]

            //pick previous usable point
            while (it != root) {
                it--;
                if (it->paired()) {
                    p_aux = it->at(1).p;
                    break;
                } else if (!it->at(0).p.bad()) {
                    p_aux = it->at(0).p;
                    break;
                }
            }

            //now we need to position the nodes from p[0], i.e. p_aux -> p[0] -> new_point -> new_point -> ...

            point shift = normalize(p[0] - p_aux);
            int i = 0;
            for (auto const &it_int: interval1) {
                i++;
                point new_point = p[0] + shift * i * BASES_DISTANCE;
                it_int->at(0).p = new_point;
            }

        }
        else if (p[0].bad()) {
            // if p1 is invalid point interval1 is the 5' trailing region

            point p_aux;
            compact::iterator it = compact::iterator(interval1.back());
            it++; //obtaining first valid sibling, i.e. s[0]

            //pick next usable point
            while (it != rna.end()) {
                it++;
                if (!it->at(0).p.bad()) {
                    p_aux = it->at(0).p;
                    break;
                }
            }

            //now we need to position the nodes from interval up to point p[1], i.e. new_point -> new_point -> p[0] -> p_aux...
            point shift = normalize(p_aux - p[1]);
            for (int i = 0; i < interval1.size(); ++i) {
                point new_point = p[1] - shift * (interval1.size() - i) * BASES_DISTANCE;
                interval1[i]->at(0).p = new_point;
            }

        }
        else {

            // if both p1 and p2 are valid points interval1 is an internal interval (between two branches)
            point c =  center(p[0],p[1]);
            point dir = normalize(p[1] - p[0]);
            bool odd = interval1.size() % 2;

            if (odd) {
                double ix_c =  floor(interval1.size() / 2);
                //position the central node
                interval1[ix_c]->at(0).p = c;
            }

            int ix_center_left = floor(interval1.size() / 2.0 - 1);
            for (int i = ix_center_left; i>=0; i--) {
                interval1[i]->at(0).p = c - dir * (ix_center_left - i + 1) * BASES_DISTANCE;

            }
            int ix_center_right = ceil(interval1.size() / 2.0);
            for (int i = ix_center_right; i < interval1.size(); i++) {
                interval1[i]->at(0).p = c + dir * (i - ix_center_right + 1) * BASES_DISTANCE;
            }

//            for (int i = 0; i < interval1.size(); i++) {
//                interval1[i]->at(0).label='x';
//
//            }

            double dist_original = distance(p[0], p[1]);
            double dist_new = (interval1.size() + 1) * BASES_DISTANCE;
            double dist_diff = dist_new - dist_original;

            if (dist_diff > 0) {
                // We will shift everything left and right from the center only if the region needs to accomodate
                // new residues. We could also shrink the structure, but that introduces a great danger of clashes
                // in case of large structures.

                s[0]++;
                shift_region(rna.begin(), s[0],  - dir * (dist_diff / 2));
                shift_region(s[1], rna.end(),  dir * (dist_diff / 2));
            }
        }
    }
}

/**
 * Deletion of a node (unpaired nt) introduces a gap in the layout. This is taken care of in the case of in non-root
 * level. This function does contraction for the first level.
 * @param rna
 */
void init_root_level_deleted(rna_tree &  rna) {

    compact::sibling_iterator root = rna.begin();


    compact::sibling_iterator it_prev = root.begin();
    compact::sibling_iterator it = ++compact::sibling_iterator(it_prev);
    while(it != root.end()) { //traverse the tree pre-order
        if (!it->paired() ){
            size_t id0 = it_prev->seq_id_mapped();
            size_t id1 = it->seq_id_mapped();

            if (id0 != TREE_BASE_NODE_NOT_MAPPED_ID && id1 != TREE_BASE_NODE_NOT_MAPPED_ID && id1 != id0 + 1) {
                //there was a deletion
                point p0 = it_prev->paired() ? it_prev->at(1).p : it_prev->at(0).p;
                point p1 = it->at(0).p;
                double dist = distance(p1, p0);

                if ( dist > 2*BASES_DISTANCE) {
                    //lets contract only if the distance after deletion is too big, otherwise it's better not to touch the layout
                    point dist_vect = normalize(p1 - p0) * (dist/2-BASES_DISTANCE/2);

                    shift_region(root.begin(), it,  dist_vect );
                    shift_region(it, root.end(),  -dist_vect);
                }
            }

        }

        it++; it_prev++;
    }

}

void compact::init()
{
    APP_DEBUG_FNAME;
    
    assert(rna.is_ordered_postorder());

//    for (iterator r = rna.begin(); r!= rna.end(); r++) {
//        stringstream m;
//        m << r->at(0).label << " " << r->id() << " " << r->_id_mapped;
//
//        r->at(0).label  = m.str();
//    }

//    compact::sibling_iterator it =   rna_tree::last_child(rna.begin());
//    for (int i = 0; i < 20; i++) {
//        it--;
//
//        std::cout << it->at(0).label << " " << it->at(0).tmp_label << " " << it->status << " " << it->id() << " " << it->seq_id_mapped() << endl;
//    }
    
    for (iterator it = ++rna.begin(); it != rna.end(); ++it)
    { //traverse the tree pre-order
        if (it->initiated_points() || !it->paired())
            continue;
        
        //For non-initiated points or paired nodes, get their parents
        iterator par = rna_tree::parent(it);
        point p = par->center();
        
        assert(!p.bad());
        
        if (rna_tree::is_root(par))
        {
            /*
             * init_branch_recursive works in situations when new bps are added at the top of
             * an existing branch. In such a case, the whole branch is moved in the desired direction
             * and new items are inserted.
             */
            if (init_branch_recursive(it).bad())
            {
                // In case we are adding a brand new branch or we are adding into a branch which is part of a multibranch loop
                init_multibranch(it, true);
            }
        }
        else if (!rna_tree::is_root(par) && !init_branch_recursive(it, p).bad())    // => is good
        {
            // init OK
        }
        else
        {
            if (rna_tree::is_valid(get_onlyone_branch(par)))   // 1 branch
            {
                init_by_ancestor(it);
            }
            else
            {
                init_multibranch(par);
            }
        }
    }

//    straighten_branches();
    
    auto log = logger.debug_stream();
    log << "Points initialization:\n";
    auto f = [&](pre_post_order_iterator it)
    {
        if (it->paired())
        {
            if (it.preorder())
            {
                mprintf("it[%s][%s = %s][%s = %s]\n", log,
                        it->status,
                        it->at(0).label, it->at(0).p,
                        it->at(1).label, it->at(1).p);
            }
        }
        else
        {
            mprintf("it[%s][%s = %s]\n", log,
                    it->status,
                    it->at(0).label, it->at(0).p);
        }
    };
    rna_tree::for_each_in_subtree(rna.begin_pre_post(), f);
    
    // if first node was inserted and it is only one branch - do not remake it
    // because it shares parents (3'5' node) position: 3'-NODE1 <-> NODE2-5'
    sibling_iterator root = rna.begin();

    //TODO CHECK WHETHER THIS IS NOT ACTUALLY NEEDED
//    sibling_iterator ch = get_onlyone_branch(root);
//    if (rna_tree::is_valid(ch)
//        && !root->remake_ids.empty()
//        && ch->initiated_points())
//    {
//        sibling_iterator ch2 = get_onlyone_branch(ch);
//        if (rna_tree::is_valid(ch2))
//        {
//            point vec = normalize(orthogonal(ch->at(0).p - ch->at(1).p, ch2->center())) * rna.get_pairs_distance();
//
//            ch->set_p(ch2->at(0).p - vec, 0);
//            ch->set_p(ch2->at(1).p - vec, 0);
//        }
//    }

    init_root_level_unpaired(rna);
    init_root_level_deleted(rna);
    normalize_53_ends(rna);
    root->remake_ids.clear();
    
    DEBUG("compact::init() OK");
}

void compact::straighten_branches()
{
    // for nodes in one branch, set them to lie on a straight line
    for (iterator it = rna.begin(); it != rna.end(); ++it)
    {
        if (rna_tree::is_leaf(it))
            continue;
        // run only when branching
        if (!rna_tree::is_valid(get_onlyone_branch(it)))
        {
            for (sibling_iterator ch = it.begin(); ch != it.end(); ++ch)
                if (!rna_tree::is_leaf(ch))
                    straighten_branch(ch);
        }
    }
}

point compact::init_branch_recursive(
                                     sibling_iterator it,
                                     point from)
{
    point p;
    sibling_iterator ch;
    
    if (it->initiated_points())
    {
        p = normalize(it->center() - from) * BASES_DISTANCE;
        return p;
    }
    
    ch = get_onlyone_branch(it);
    if (!rna_tree::is_valid(ch))
        return point::bad_point();
    
    p = init_branch_recursive(ch, from);
    if (!p.bad())
    {
        assert(ch->paired());
        
        shift_branch(it, p);
        it->set_p(ch->at(0).p - p, 0);
        it->set_p(ch->at(1).p - p, 1);
        
        return p;
    }
    
    return point::bad_point();
}

point compact::init_branch_recursive(
                                     sibling_iterator it)
{
    // init for root children
    
    point p;
    sibling_iterator ch;
    
    if (it->initiated_points())
    {
        for (ch = it.begin(); ch != it.end(); ++ch)
        {
            if (ch->initiated_points())
            {
                p = it->center() - ch->center();
                return p;
            }
        }
        abort();
    }
    
    /*
     * The structure obtained from an image does not need to be a tree with a common root but rather a forrest
     * Eg. homo sapiens's rRNA (http://www.rna.ccbb.utexas.edu/RNA/Structures/d.16.e.H.sapiens.pdf)
     * is basically a structure with 4 or 5 main substreed formin a virtual multibranch loop
     */
    ch = get_onlyone_branch(it);
    /*
     * Expecting that we are inserting into a branch at a position which does not split (only one non-leaf
     * child). Otherwise, we do not know how to connect that node to the children
     */
    if (!rna_tree::is_valid(ch))
        return point::bad_point();
    
    p = init_branch_recursive(ch);
    
    if (!p.bad()) {
        it->set_p(ch->at(0).p, 0);
        it->set_p(ch->at(1).p, 1);
        shift_branch(ch, -p);
    }
    
    return p;
}

void compact::init_by_ancestor(
                               sibling_iterator it)
{
    assert(rna_tree::is_valid(get_onlyone_branch(rna_tree::parent(it))));   // => 1 branch
    
    point p1, p2, vec;
    iterator par = rna_tree::parent(it);
    assert(!rna_tree::is_root(par));
    
    iterator grandpar = rna_tree::parent(par);
    if (!rna_tree::is_root(grandpar))
        vec = normalize(par->center() - rna_tree::parent(par)->center());
    else {
        assert(!par->get_parent_center().bad())
        vec = normalize(par->center() - par->get_parent_center());
    }
    // ^^ direction (parent(par)->par)
    p1 = par->at(0).p + vec;
    p2 = par->at(1).p + vec;
    
    it->set_p(p1, 0);
    it->set_p(p2, 1);
    // ^^ initialize points of `it` to lie next to parent
    // .. that means only initialization, to get direction where child should be
}

void compact::init_multibranch(
                               sibling_iterator it, bool is_root)
{
    APP_DEBUG_FNAME;
    
#define LEAF_POINTS 1
#define PAIRED_POINTS 5 /* we use only # 1 and 3; # 0,2,4 will be free space */
    
    auto get_number_of_places_for_bases =
    [](sibling_iterator root)
    {
        int n = 0;
        for (sibling_iterator ch = root.begin(); ch != root.end(); ++ch)
        {
            if (ch->paired())
                n += PAIRED_POINTS;
            else
                n += LEAF_POINTS;
        }
        return n;
    };
    auto rotate_subtree =
    [this](sibling_iterator root, point center, point p1, point p2)
    {
        //root is the root of the subtree to be rotated
        //points p1 and p2 are positions of where it should be positioned
        //center is the center of the parent (of all the branches to be rotated) pair
        if (init_branch_recursive(root, center).bad())
        {
            rna_tree::for_each_in_subtree(root,
                                          [](iterator iter)
                                          {
                                              rna_tree::parent(iter)->remake_ids.push_back(child_index(iter));
                                          });
            
            root->set_p(p1, 0);
            root->set_p(p2, 1);
            return;
        }
        
        point rp1 = root->at(0).p;
        point rp2 = root->at(1).p;
        
        root->set_p(p1, 0);
        root->set_p(p2, 1);
        
        double beta = angle(rp1 - rp2) - angle(p1 - p2);
        
        for (pre_post_order_iterator it = ++pre_post_order_iterator(root, true); id(it) < id(root); ++it)
        {
            point from = it->at(it.label_index()).p;
            if (from.bad())
                continue;
            
            double alpha = angle(from - rp1);
            double radius = distance(from, rp1);
            
            point to = rotate(p1, alpha - beta, radius);
            it->set_p(to, it.label_index());
        }
    };
    
    if (is_root)  {
        /*
         * New branch created at the root level and it that case, the iterator is the root of the subtree to be
         * inserted into the root level. Since we are inserting into root, which is a node labeled 5'3' (an artificial node)
         * that does not need to be base-paired ,
         * but can be far apart in the image (e.g. http://www.rna.ccbb.utexas.edu/RNA/Structures/d.16.e.H.sapiens.pdf),
         * we need to find an anchor point which will be used for the multibranch (normally we use the parent)
         */
        
        
        iterator first_initiated = rna.get_leftest_initiated_descendant(it);
        iterator last_initiated = rna.get_rightest_initiated_descendant(it);

        point p1 = (*first_initiated)[0].p;
        point p2 = last_initiated->paired() ? last_initiated->at(1).p : last_initiated->at(0).p;

        if (first_initiated->initiated_points()
            && last_initiated->initiated_points() && first_initiated != last_initiated &&
            distance(p1, p2) < 2 * PAIRS_DISTANCE)
        {
            
            //Installing a new root into an existing branch in depth 1 which is part of a multibranch loop
            
            //The idea is to position the new root at the position of the intiated points and rotate the subtree to
            //accommodate this change
            
            //            point c = point(0, 0);
            //            int cnt_branches = 0;
            

            //it can happen that the first initiated and last initiated descendants are not at the same level (see the
            // image), so the following assert is not valid. However, in such a case it is probable, that the resulting layout
            //won't be very nice because target and template are probably diverge quite a lot
            //

            //                      G-G
            //                  /   |   \
            //        U(initiated)  U   A-U
            //                          |   \
            //                          UC   A (initiated)

            // assert(rna.depth(first_initiated) == rna.depth(last_initiated));
            //            for (sibling_iterator si = sibling_iterator(first_initiated); si != sibling_iterator(last_initiated); si++)
            //            {
            //                if (si->initiated_points() && si->paired())
            //                {
            //                    cnt_branches++;
            //                    c += si->center();
            //                }
            //            }
            //            c = c / cnt_branches;
            

//            p1 = (*first_initiated)[0].p;
//            last_initiated->paired() ? p2 = (*last_initiated)[1].p : p2 = (*last_initiated)[0].p;
//
            point c = center(p1, p2) - orthogonal(p1 - p2) * BASES_DISTANCE;
            //Testing whether it wouldn't be better to position the center in the opposite orthogonal direction
            //is done later in the try_reposition_new_root_branches function
            
            /*
             * We need to remember the parent's center be used later when initializing position for the child of current node.
             * Normally, the position is obtained from the parent, but in case of root parent, that is the position between
             * 5' and 3' end which might be far apart.
             */
            it->set_parent_center(c);
            
            rotate_subtree(it, c, p1, p2);
            
            /*
             * Now we need to move all the descendants. We will move them in the perpendicular direction to the
             * new base pair. The orientation is based on the direction of descendants with respect to p1 and p2
             */
            point center_of_gravity = point(0,0);
            int cnt = 0;
            for (iterator descendant = it.begin(); descendant != it.end(); descendant++) {
                point aux_center = it->center();
                if (!aux_center.bad()) {
                    cnt++;
                    center_of_gravity += aux_center;
                }
            }
            center_of_gravity = center_of_gravity / cnt;
            point shift_vector = orthogonal(p1 - p2) * BASES_DISTANCE;
            if (distance(center_of_gravity , p1 + shift_vector) > distance(center_of_gravity , p1 - shift_vector)) {
                shift_vector = -shift_vector;
            }
            
            for (sibling_iterator sit = it.begin(); sit != it.end(); sit++) shift_branch(sit, shift_vector);
        }
        else {
            //inserting a brand new branch

            //            do {
//                it_aux = rna.previous_sibling(prev);
//                if (it_aux.node == nullptr && prev.node->parent != nullptr) it_aux = rna.parent(prev);
//                prev = it_aux;
//            } while (prev.node != nullptr && !prev->initiated_points());
//            do {
//                it_aux = rna.next_sibling(next);
//                if (it_aux.node == nullptr && next.node->parent != nullptr) it_aux = rna.parent(next);
//                next = it_aux;
//            } while (next.node != nullptr && !next->initiated_points());

//            assert(prev.node != nullptr && next.node != nullptr && prev->initiated_points() && next->initiated_points());


            iterator root = rna.begin();

            //iterator prev = rna.previous_sibling(it), next = rna.next_sibling(it);
            sibling_iterator prev = --sibling_iterator(it), next = ++sibling_iterator(it);

            while(prev != root.begin() && !prev->initiated_points()) prev--;
            while(next != root.end() && !next->initiated_points()) next++;

            assert(prev->initiated_points() || next->initiated_points())

            if (!prev->initiated_points()) {
                // branch inserted at 5' end there exists no initiated point before it
                p1 = next->at(0).p;
                //if he next node is paired than position of first and second nucleotie are enough
                // to get the position of the new branch (although it should also be slightly rotated otherwise
                // we will have two parallel branches next to each other
                if (next->paired()) p1 = next->at(1).p;
                else {
                    iterator next_next = ++sibling_iterator(next);
                    while(next_next != root.end() && !next_next->initiated_points()) next_next++;
                    assert(next_next->initiated_points())
                    p2 = next_next->at(0).p;
                }

                //get position for the bases of the root of the inserted hairpin
                point p1p2 = normalize(p2-p1);
                point new_pos2 = p1 - p1p2*BASES_DISTANCE;
                point new_pos1 = new_pos2 - p1p2*PAIRS_DISTANCE;
                point c = center(new_pos1, new_pos2) - orthogonal(p1p2) * BASES_DISTANCE;
                it->set_parent_center(c);
                rotate_subtree(it, c, new_pos1, new_pos2);

            } else if (next == root.end() || !next->initiated_points()){
                // branch inserted at 3' end there exists no initiated point after it

                //p1 is the first initated residue before the inserted branch, p2 is the one (initiated) before
                if (prev->paired()) {
                    p1 = prev->at(1).p;
                    p2 = next->at(0).p;
                }
                else {
                    p1 = prev->at(0).p;
                    iterator prev_prev = --sibling_iterator(prev);
                    while(prev_prev != root.begin() && !prev_prev->initiated_points()) prev_prev--;
                    assert(prev_prev->initiated_points())
                    p2 = prev_prev->paired() ?  prev_prev->at(1).p : prev_prev->at(0).p;
                }

                //get position for the bases of the root of the inserted hairpin
                point p1p2 = normalize(p2-p1);
                point new_pos1 = p1 - p1p2*BASES_DISTANCE;
                point new_pos2 = new_pos1 - p1p2*PAIRS_DISTANCE;
                point c = center(new_pos1, new_pos2) - orthogonal(p1p2) * BASES_DISTANCE;
                it->set_parent_center(c);
                rotate_subtree(it, c, new_pos1, new_pos2);

            } else {
                point p1, p2;

                prev->paired() && !rna_tree::is_root(prev) ? p1 = (*prev)[1].p : p1 = (*prev)[0].p;
                p2 = (*next)[0].p;

                point orig_vector = p2 - p1;

                p1 = move_point(p1, p1 + orig_vector, BASES_DISTANCE);
                p2 = move_point(p1, p1 + orig_vector, PAIRS_DISTANCE);

                point c = center(p1, p2) - orthogonal(p2 - p1) * BASES_DISTANCE;
                //Whether it wouldn't be better to position the center in the opposite orthogonal direction
                //is checked later in the try_reposition_new_root_branches function

                /*
                 * We need to remember the parent's center be used later when intializing position for the child of current node.
                 * Normally, the positin is obtain from the parent, but in case of root parent, that is the position between
                 * 5' and 3' end which might be far apart.
                 */
                it->set_parent_center(c);

                rotate_subtree(it, c, p1, p2);

                //Move the p2 nucleotide and all the following siblings
                for (sibling_iterator s = next; s != it.end(); ++s) {
                    shift_branch(s, normalize(orig_vector) * (BASES_DISTANCE + PAIRS_DISTANCE));
                }

            }


        }
    }
    else if (it.number_of_children() == 2) {
        /*
         * In case we inserted into a stem (one child is the stem the other is the new branch) we do not want
         * to put the tho branches on a circle, but rather place the new branch perpendicular to the existing stem.
         * So we need to move the non-parent part of the existing stem in its current direction (elongation) and then
         * add insert the new branch perpendicular to it so that it does not intersect much with the rest of the tree.
         */
        
        //Find out which child is new
        iterator it_existing, it_new;
        if (it.node->first_child->data.initiated_points()) {
            it_existing = it.node->first_child;
            it_new = it.node->last_child;
        } else {
            it_existing = it.node->last_child;
            it_new = it.node->first_child;
        }
        
        //Move the existing branch in its current direction so that there is enough space for the new branch
        //to be located perpendicular to it
        point p[2] = {it->at(0).p, it->at(1).p};
        point e[2] = {it_existing->at(0).p, it_existing->at(1).p};
        point c_parent = center(p[0], p[1]);
        point points_existing_new[2] = {move_point(p[0], e[0], PAIRS_DISTANCE),
            move_point(p[1], e[1], PAIRS_DISTANCE)};
        point c_junction = (c_parent + center(points_existing_new[0], points_existing_new[1])) / 2; //center of the new junction
        rotate_subtree(it_existing, c_junction, points_existing_new[0], points_existing_new[1]);
        
        //Position the new branch
        point points_new[2] = {move_point(p[0], p[1], -BASES_DISTANCE),
            move_point(points_existing_new[0], points_existing_new[1], -BASES_DISTANCE)};
        rotate_subtree(it_new, c_junction, points_new[0], points_new[1]);
        
    } else {
        circle c;
        c.p1 = it->at(0).p;
        c.p2 = it->at(1).p;
        c.direction = rna_tree::parent(it)->at(0).p;
        c.centre = center(c.p1, c.p2);
        c.compute_sgn();
        auto points = c.init(get_number_of_places_for_bases(it), BASES_DISTANCE);
        
        int i = 0;
        for (sibling_iterator ch = it.begin(); ch != it.end(); ++ch) {
            if (rna_tree::is_leaf(ch)) {
                ch->set_p(points[i], 0);
                
                i += LEAF_POINTS;
            } else {
                rotate_subtree(ch, c.centre, points[i + 1], points[i + 3]);
                i += PAIRED_POINTS;
            }
        }
    }
    auto set_label_status =
    [](iterator iter)
    {
#define lbl(type) rna_pair_label::type
        if (!contains({lbl(edited), lbl(deleted), lbl(inserted), lbl(reinserted)}, iter->status))
        {
            iter->status = lbl(rotated);
        }
#undef lbl
    };
    rna_tree::for_each_in_subtree(it, set_label_status);
}

void compact::straighten_branch(
        sibling_iterator it)
{
    assert(!rna_tree::is_leaf(it));
    
    vector<sibling_iterator> vec;
    
    vec.push_back(it);
    
    while(!rna_tree::is_leaf(it))
    {
        it = get_onlyone_branch(it);
        if (!rna_tree::is_valid(it))
            break;
        vec.push_back(it);
    }
    
    point p1, p2, p, shift, newpos;
    
    if (vec.size() < 2)
        return;
    
    p1 = vec[0]->at(0).p;
    p2 = vec[0]->at(1).p;
    if (!double_equals(distance(p1, p2), BASES_DISTANCE))
    {
        p = center(p1, p2);
        p1 = move_point(p, p1, PAIRS_DISTANCE / 2);
        p2 = move_point(p, p2, PAIRS_DISTANCE / 2);
        
        vec[0]->set_p(p1, 0);
        vec[0]->set_p(p2, 1);
    }
    
    shift = orthogonal(p1 - p2, vec[1]->center() - p2);
    
    for (size_t i = 1; i < vec.size(); ++i)
    {
        // distances between nodes will be same..
        shift = normalize(shift) * distance(vec[0]->center(), vec[i]->center());
        
        newpos = p1 + shift;
        p = newpos - vec[i]->at(0).p;
        
        for (it = vec[i - 1].begin(); it != vec[i]; ++it)
        {
            // shift leaf children
            assert(rna_tree::is_leaf(it));
            shift_branch(it, p);
        }
        
        newpos = p2 + shift;
        p = newpos - vec[i]->at(1).p;
        
        for (it = ++sibling_iterator(vec[i]); it != vec[i-1].end(); ++it)
        {
            // shift leaf children
            assert(rna_tree::is_leaf(it));
            shift_branch(it, p);
        }
        
        vec[i]->set_p(p1 + shift, 0);
        vec[i]->set_p(p2 + shift, 1);
    }
    
    // shift rest of tree
    // (subtree of this branch)
    for (it = vec.back().begin(); it != vec.back().end(); ++it)
        shift_branch(it, p);
}




void compact::make()
{
    APP_DEBUG_FNAME;
    
    iterator it;
    intervals in;
    
    for (it = rna.begin(); it != rna.end(); ++it)
    {
        if (!to_remake_children(it))
            continue;
        
        in.init(it);
        set_distances(in);
        for (auto& i : in.vec)
            if (i.remake)
                remake(i, in.get_circle_direction());
    }
}

void compact::set_distances(
                            intervals& in)
{
    switch (in.type)
    {
        case intervals::hairpin:
            break;
        case intervals::interior_loop:
            set_distance_interior_loop(in);
            break;
        case intervals::multibranch_loop:
            set_distance_multibranch_loop(in);
            break;
    }
}

void compact::set_distance_interior_loop(
                                         intervals& in)
{
    assert(in.type == intervals::interior_loop &&
           in.vec.size() == 2);
    
    iterator parent, child;
    double actual, avg;
    vector<double> l(2);
    
    parent = in.vec[0].beg.it;
    child = in.vec[0].end.it;
    
    actual = distance(parent->center(), child->center());
    l[0] = circle::min_circle_length(in.vec[0].vec.size(), BASES_DISTANCE);
    l[1] = circle::min_circle_length(in.vec[1].vec.size(), BASES_DISTANCE);
    sort(l.begin(), l.end());
    avg = l[0] + (l[1] - l[0])/4;
    
    if (avg < BASES_DISTANCE)
        avg = BASES_DISTANCE;
    
    if (!double_equals_precision(actual, avg, 1))
    {
        set_distance(parent, child, avg);
        in.vec[0].remake = true;
        in.vec[1].remake = true;
    }
}

void compact::set_distance_multibranch_loop(
                                            intervals& in)
{
    for (size_t i = 0; i < in.vec.size(); ++i)
    {
        try {
            if (in.vec[i].vec.size() > MULTIBRANCH_MINIMUM_SPLIT)
            { //if the number of unpaired residues between a pair of basepairs is greater than MULTIBRANCH_MINIMUM_SPLIT
                split(in.vec[i]);
                in.vec[i].remake = false;
            }
        } catch (...)
        {
            ERR("Error occured in drawing multibranch loop. Some bases could not be drawn");
            // TODO handle error
            return;
        }
    }
}

void compact::split(
                    const interval& in)
{
    vector<point> p1, p2;
    point p;
    double length;
    size_t i, j;
    
    length = get_length(in);
    
    p1.push_back(in.beg.it->at(in.beg.index).p);
    p2.resize(in.vec.size());
    for (const auto& i : in.vec)
        if (i->initiated_points())
            p1.push_back(i->at(0).p);
    p1.push_back(in.end.it->at(in.end.index).p);
    p = p1[0];
    
    i = 1;
    j = 0;
    
    for (; j < p2.size(); ++j)
    {
        double l = length;
        
        assert(length > 0);
        
        while (distance(p, p1.at(i)) < l)
        {
            l -= distance(p, p1.at(i));
            p = p1[i];
            ++i;
        }
        p = move_point(p, p1.at(i), l);
        p2[j] = p;
    }
    
    assert(j == p2.size());
    
    j = 0;
    for (auto val : in.vec)
        val->set_p(p2[j++], 0);
}

void compact::reinsert(
                       const points_vec& points,
                       const nodes_vec& nodes)
{
    if (nodes.empty())
        return;
    
    assert(!nodes.empty());
    
    assert(points.size() == nodes.size());
    for (size_t i = 0; i < points.size(); ++i)
    {
        assert(rna_tree::is_leaf(nodes[i]));
        
        nodes[i]->set_p(points[i], 0);
        if (is(nodes[i], rna_pair_label::touched))
            nodes[i]->status = rna_pair_label::reinserted;
    }
}

void compact::remake(
                     const interval& i,
                     point direction)
{
    assert(i.remake);
    
    circle c;
    c.p1 = i.beg.it->at(i.beg.index).p;
    c.p2 = i.end.it->at(i.end.index).p;
    if (c.p2 == c.p1){
        c.p2 += direction;
    }
    c.direction = direction;
    c.centre = center(c.p1, c.p2);
    c.compute_sgn();
    
    reinsert(c.init(i.vec.size(), BASES_DISTANCE), i.vec); // reinsertion of the bases between pairs of basepairs
}

double compact::get_length(
                           const interval& in)
{
    if (in.vec.empty())
        return 0;
    
    double len = 0;
    point p1 = in.beg.it->at(in.beg.index).p;
    point p2;
    
    for (const auto& val : in.vec)
    {
        assert(!val->paired());
        if (val->initiated_points())
        {
            p2 = val->at(0).p;
            len += distance(p1, p2);
            p1 = p2;
        }
    }
    p2 = in.end.it->at(in.end.index).p;
    len += distance(p1, p2);
    len = len / (in.vec.size() + 1);
    return len;
}


/* inline */ void compact::checks()
{
    // all but root should be inited
    for (iterator it = ++rna.begin(); it != rna.end(); ++it)
    {
        for (int i = 0; i < 2; i++) {
            if ((i == 0 || (i == 1 && it->paired())) && (it->at(i).p.bad())) {
                std::stringstream  lbl;

                lbl <<  it->at(i).label << " (id: " << it->id() << ", depth: " << rna.depth(it) << ", status: "<< it->status << ")";
                ERR("Base %s was not initialized and therefore not drawn.", lbl.str());
            }
        }
    }
}



void compact::try_reposition_new_root_branches()
{
    
    overlap_checks::overlaps overlaps = overlap_checks().run(rna);
    
    sibling_iterator root = rna.begin();
    for (sibling_iterator it = root.begin(); it != root.end(); ++it)
    {
        if (it->paired() && it->status == rna_pair_label::inserted)
        {
            int angles[] = {10, 20, 30, 40, 50, 60, 70, 80, 90};
            for (auto a: angles) {


            }
            //Try to mirror the branch
            mirror_branch(it);
//            rotate_branch_by_angle(it, -45);
            //Get the number of overlaps
            //TODO: should be optimized to check only intersections in the current branch
            overlap_checks::overlaps overlaps_aux = overlap_checks().run(rna);
            //If by mirroring we got more overlaps, mirror back
            if (overlaps_aux.size() >= overlaps.size())
                mirror_branch(it);
        }
    }
    //    sibling_iterator root = rna.begin();
    //
    //    overlap_checks::edges edges_all = overlap_checks::get_edges(root);
    //
    //    for (sibling_iterator it = root.begin(); it != root.end(); ++it)
    //    {
    //        if (it->paired() && it->status == rna_pair_label::inserted)
    //        {//newly inserted pair (not neccessary whole new branch)
    //            overlap_checks::edges edges_branch = overlap_checks::get_edges(it);
    //
    //            overlap_checks::overlaps overlaps1 = overlap_checks::get_overlaps(edges_all,edges_branch);
    //
    //            //Try to mirror the branch
    //            mirror_branch(it);
    //            //Get the number of overlaps
    //            //TODO: should be optimized to check only intersections in the current branch
    //            //overlap_checks::overlaps overlaps_aux = overlap_checks().run(rna);
    //            overlap_checks::overlaps overlaps2 = overlap_checks::get_overlaps(edges_all,edges_branch);
    //            //If by mirroring we got more overlaps, mirror back
    //            if (overlaps2.size() > overlaps1.size()) mirror_branch(it);
    //        }
    //    }
}
