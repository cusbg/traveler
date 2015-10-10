/*
 * File: overlap_checks.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard.elias@matfyz.cz>
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

#ifndef OVERLAP_CHECKS_HPP
#define OVERLAP_CHECKS_HPP

#include "types.hpp"
#include "point.hpp"
#include "rna_tree.hpp"


class overlap_checks
{
public:
    struct edge
    {
        point p1, p2;
    };
    struct line_equation
    {
        /* equation: a*x + b*y + c = 0 */
        double a, b, c;

        inline bool hasnan() const;
        inline line_equation& operator*=(
                double val);
        friend std::ostream& operator<<(
                std::ostream& out,
                const line_equation& k);
    };
    typedef std::vector<edge> edges;

public:
    overlap_checks(
                    rna_tree& _rna);
    void run();

private:
    void run(
                    const edges& e);
    edges get_edges();
    inline line_equation get_equation(
                    const edge& e);
    point intersection(
                    const edge& e1,
                    const edge& e2);
    point compute(
                    line_equation k,
                    line_equation l);

private:
    rna_tree& rna;
};


#endif /* !OVERLAP_CHECKS_HPP */
