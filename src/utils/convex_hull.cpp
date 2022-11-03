// A C++ program to find convex hull of a set of points. Refer
// https://www.geeksforgeeks.org/orientation-3-ordered-points/
// for explanation of orientation()
#include <bits/stdc++.h>
#include "point.hpp"
#include "geometry.hpp"

using namespace std;

vector<point> convex_hull(const vector<point> points)
{
    // Initialize Result
    vector<point> hull;

    // There must be at least 3 points
    if (points.size() < 3) return hull;

    // Find the leftmost point
    int l = 0;
    for (int i = 1; i < points.size(); i++)
        if (points[i].x < points[l].x)
            l = i;

    // Start from leftmost point, keep moving counterclockwise
    // until reach the start point again.  This loop runs O(h)
    // times where h is number of points in result or output.
    int p = l, q;
    do
    {
        // Add current point to result
        hull.push_back(points[p]);

        // Search for a point 'q' such that orientation(p, x,
        // q) is counterclockwise for all points 'x'. The idea
        // is to keep track of last visited most counterclock-
        // wise point in q. If any point 'i' is more counterclock-
        // wise than q, then update q.
        q = (p+1)%points.size();
        for (int i = 0; i < points.size(); i++)
        {
            // If i is more counterclockwise than current q, then
            // update q
            if (orientation(points[p], points[i], points[q]) == 2)
                q = i;
        }

        // Now q is the most counterclockwise with respect to p
        // Set p as q for next iteration, so that q is added to
        // result 'hull'
        p = q;

    } while (p != l);  // While we don't come to first point

    // Print Result
//    for (int i = 0; i < hull.size(); i++)
//        cout << "(" << hull[i].x << ", "
//             << hull[i].y << ")\n";

    return hull;
}

void add_padding(std::vector<point>& hull, double value){

    point com = point(0,0);
    for (int i = 0; i< hull.size() - 2; i++) {
//    for (point p: hull) {
        com += hull[i];
    }
    com = com / hull.size();

    for (int i = 0; i < hull.size(); i++){
        hull[i] += normalize(hull[i]-com) * value;
    }

}

std::vector<point> simplify_hull(const std::vector<point> v) {

//    if (v.size() < 4) {
//        return v;
//    }
//
//    std::vector<point> sf_tmp = v;
//    std::vector<point> sf;
//    for (int i = 0; i < sf_tmp)

}