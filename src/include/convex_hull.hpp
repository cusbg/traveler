//
// Created by hellb on 12.05.19.
//

#ifndef TRAVELER_CONVEX_HULL_HPP
#define TRAVELER_CONVEX_HULL_HPP

#include <vector>

#include "point.hpp"

std::vector<point> convex_hull(const std::vector<point> v);

std::vector<point> simplify_hull(const std::vector<point> v);

void add_padding(std::vector<point> &hull, double value);

#endif //TRAVELER_CONVEX_HULL_HPP