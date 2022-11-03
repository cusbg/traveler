#ifndef TRAVELER_GEOMETRY_HPP
#define TRAVELER_GEOMETRY_HPP

#include "point.hpp"

int orientation(point p, point q, point r);
bool lines_intersect(point p1, point q1, point p2, point q2);
point lines_intersection(point p1, point q1, point p2, point q2);


#endif //TRAVELER_GEOMETRY_HPP