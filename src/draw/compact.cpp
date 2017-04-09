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
    update_ends_in_rna(rna);
//    try_reposition_new_root_branches();
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
                    it->at(i).p += vector;

            return out;
        };

    recursion(parent);
}

/* static */ void compact::mirror_branch(
        iterator root)
{
    //First we obtain direction vector of the root's bp (in case of
    point pr[2] = {root->at(0).p, root->at(1).p};
    point center_root = center(pr[0], pr[1]);
    point v = pr[0] - pr[1];

    function<void(iterator)> recursion =
            [&recursion, &pr](iterator it) {

                if (it->initiated_points())
                    for (size_t i = 0; i < it->size(); ++i) {
                        //Get the vector between mirror line and point
                        point p = it->at(i).p;
                        //double a = angle(p, center_root, center_root + v);
                        //Rotate the point in the reverse directions
                        //it->at(i).p += rotate(center_root, -a, distance(p, center_root));

                        //http://stackoverflow.com/questions/3306838/algorithm-for-reflecting-a-point-across-a-line
                        //first project p on the mirror line
                        double a = (pr[0].y - pr[1].y) / (pr[0].x - pr[1].x);
                        double b = pr[0].y - pr[0].x * a;
//                        point pl(0 + (b * p.x) / (1 + m * m), b + (m * p.x) / (1 + m * m));
//                        it->at(i).p = 2 * pl - p;
                        double d = (p.x + (p.y - b)*a)/(1 + a*a);
                        it->at(i).p = point(2*d - p.x, 2*d*a - p.y + 2*b);

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

        ch->at(ch.label_index()).p = c.rotate(alpha);
    }
}


void compact::init()
{
    APP_DEBUG_FNAME;

    assert(rna.is_ordered_postorder());

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

    init_even_branches();

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
    sibling_iterator ch = get_onlyone_branch(root);
    if (rna_tree::is_valid(ch)
            && !root->remake_ids.empty()
            && ch->initiated_points())
    {
        root->remake_ids.clear();

        sibling_iterator ch2 = get_onlyone_branch(ch);
        if (rna_tree::is_valid(ch2))
        {
            point vec = normalize(orthogonal(ch->at(0).p - ch->at(1).p, ch2->center())) * rna.get_pairs_distance();

            ch->at(0).p = ch2->at(0).p - vec;
            ch->at(1).p = ch2->at(1).p - vec;
        }
    }

    DEBUG("compact::init() OK");
}

void compact::init_even_branches()
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
                    make_branch_even(ch);
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
        it->at(0).p = ch->at(0).p - p;
        it->at(1).p = ch->at(1).p - p;

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
        it->at(0).p = ch->at(0).p;
        it->at(1).p = ch->at(1).p;
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
        //TODO: Check validity of such solution
//        if (par->get_parent_center().bad()) {
            /*The parent is root, i.e. 5'3' pair*/
            /*Let's use center of gravity to find out in which direction should the new bp should be inserted*/
            point cog = get_descendatns_center_or_gravity(it);
            //We are inserting a single base-pair with no (initiated) descendants so the direction can be chosen randomly
            point aux_p[2] = {par->at(0).p, par->at(1).p};
            shift_in_direction_of_gravity(it, aux_p, cog, true);
            return;
//        }
//        else vec = normalize(par->center() - par->get_parent_center());
    }
        // ^^ direction (parent(par)->par)
    p1 = par->at(0).p + vec;
    p2 = par->at(1).p + vec;

    it->at(0).p = p1;
    it->at(1).p = p2;
    // ^^ initialize points of `it` to lie next to parent
    // .. that means only initialization, to get direction where child should be
}

point compact::get_descendatns_center_or_gravity(iterator it)
{
    point cog = point(0,0);
    int cnt = 0;
    for (iterator descendant = it.begin(); descendant != it.end(); descendant++) {
        point aux_center = it->center();
        if (!aux_center.bad()) {
            cnt++;
            cog += aux_center;
        }
    }

    if (cnt > 0) cog = cog / cnt;
    return cog;
}

void compact::shift_in_direction_of_gravity(iterator it, point p[], point cog, bool set_root)
{
    point shift_vector = orthogonal(p[0] - p[1]) * PAIRS_DISTANCE;
    if (distance(cog , p[0] + shift_vector) > distance(cog , p[0] - shift_vector)) {
        shift_vector = -shift_vector;
    }

    if (set_root) {
        for (size_t i = 0; i < it->size(); ++i)
            it->at(i).p = p[i] + shift_vector;
    }

    for (sibling_iterator sit = it.begin(); sit != it.end(); sit++) shift_branch(sit, shift_vector);
}

void compact::init_multibranch(
                sibling_iterator it, bool root)
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

                root->at(0).p = p1;
                root->at(1).p = p2;
                return;
            }

            point rp1 = root->at(0).p;
            point rp2 = root->at(1).p;

            root->at(0).p = p1;
            root->at(1).p = p2;

            double beta = angle(rp1 - rp2) - angle(p1 - p2);

            for (pre_post_order_iterator it = ++pre_post_order_iterator(root, true); id(it) < id(root); ++it)
            {
                point from = it->at(it.label_index()).p;
                if (from.bad())
                    continue;

                double alpha = angle(from - rp1);
                double radius = distance(from, rp1);

                point to = rotate(p1, alpha - beta, radius);
                it->at(it.label_index()).p = to;
            }
        };

    if (root)  {
        /*
         * New branch created at the root level and it that case, the iterator is the root of the subtree to be
         * inserted into the root level. Since we are inserting into root, which is the 5'3' pair which does not need to be base-paired,
         * but can be far apart in the image (e.g. http://www.rna.ccbb.utexas.edu/RNA/Structures/d.16.e.H.sapiens.pdf),
         * we need to find an anchor point which will be used for the multibranch (normally we use the parent)
         */

        iterator first_initiated = rna.get_leftest_initiated_descendant(it);
        iterator last_initiated = rna.get_rightest_initiated_descendant(it);

        if (first_initiated->initiated_points()
            && last_initiated->initiated_points())
        {

            //Installing a new root into an existing branch in depth 1 which is part of a multibranch loop

            //The idea is to position the new root at the position of the intiated points and rotate the subtree to
            //accommodate this change


            point p1, p2;
            if (rna.depth(first_initiated) == rna.depth(last_initiated))
            {
                /*
                 * If the initiated descendants are on the same level, we interpret the insertion as an insertion
                 * at a position where something was before, so we simply take that position.
                 */
                p1 = (*first_initiated)[0].p;
                last_initiated->paired() ? p2 = (*last_initiated)[1].p : p2 = (*last_initiated)[0].p;
            } else
            {
                /*
                 * If the initiated descendants are not on the same level, we need to take the siblings as guides.
                 */
                sibling_iterator left, right;
                left = right = it;
                left--; right++;
                while (left.node->prev_sibling && !left->initiated_points()) left--;
                while (right.node->next_sibling && !right->initiated_points()) right++;

                p1 = left->at(left->size()-1).p;
                p2 = right->at(0).p;
                if (p1.bad())
                {//no initiated node on the left of "it"
                    right++;
                    while (right.node->next_sibling && !right->initiated_points()) right++;
                    point p_aux = right->at(0).p;
                    p1 = p2 - normalize(p_aux - p2) * BASES_DISTANCE;
                }
                if (p2.bad())
                {//no initiated node on the right of "it"
                    left--;
                    while (left.node->prev_sibling && !left->initiated_points()) left--;
                    point p_aux = left->at(left->size()-1).p;
                    p2 = p1 + normalize(p1 - p_aux) * BASES_DISTANCE;
                }
            }
//
            point c = center(p1, p2) - orthogonal(p1 - p2) * BASES_DISTANCE;
            //Whether it wouldn't be better to position the center in the opposite orthogonal direction
            //is checked later in the try_reposition_new_root_branches function

            /*
             * We need to remember the parent's center be used later when intializing position for the child of current node.
             * Normally, the positin is obtain from the parent, but in case of root parent, that is the position between
             * 5' and 3' end which might be far apart.
             */
            it->set_parent_center(c);

            rotate_subtree(it, c, p1, p2);

            /*
             * Now we need to move all the descendants. We will move them in the perpendicular direction to the
             * new base pair. The orientation is based on the direction of descendants with respect to p1 and p2
             */
            point cog = get_descendatns_center_or_gravity(it);
            point aux_p[2] = {p1, p2};
            shift_in_direction_of_gravity(it, aux_p, cog);

        }
        else {
            //inserting a brand new branch

            //iterator prev = rna.previous_sibling(it), next = rna.next_sibling(it);
            iterator prev = it, next = it;
            iterator it_aux;

            do {
                it_aux = rna.previous_sibling(prev);
                if (it_aux.node == nullptr && prev.node->parent != nullptr) it_aux = rna.parent(prev);
                prev = it_aux;
            } while (prev.node != nullptr && !prev->initiated_points());
            do {
                it_aux = rna.next_sibling(next);
                if (it_aux.node == nullptr && next.node->parent != nullptr) it_aux = rna.parent(next);
                next = it_aux;
            } while (next.node != nullptr && !next->initiated_points());

            assert(prev.node != nullptr && next.node != nullptr && prev->initiated_points() && next->initiated_points());

           point p1, p2;

            prev->paired() ? p1 = (*prev)[1].p : p1 = (*prev)[0].p;
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
                ch->at(0).p = points[i];

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

void compact::make_branch_even(
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

        vec[0]->at(0).p = p1;
        vec[0]->at(1).p = p2;
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

        vec[i]->at(0).p = p1 + shift;
        vec[i]->at(1).p = p2 + shift;
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
        val->at(0).p = p2[j++];
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

        nodes[i]->at(0).p = points[i];
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
        if (!it->initiated_points())
        {
            ERR("Some bases positions were not initialized and therefore not drawn.");
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
        {//newly inserted pair (not neccessary whole new branch)

            //Try to mirror the branch
            mirror_branch(it);
            //Get the number of overlaps
            //TODO: should be optimized to check only intersections in the current branch
            overlap_checks::overlaps overlaps_aux = overlap_checks().run(rna);
            //If by mirroring we got more overlaps, mirror back
            if (overlaps_aux.size() > overlaps.size()) mirror_branch(it);
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


