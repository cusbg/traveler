/*
 * File: app.cpp
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

#include "app.hpp"
#include "utils.hpp"
#include "rna_tree.hpp"
#include "mapping.hpp"
#include "tree_matcher.hpp"
#include "write_ps_document.hpp"
#include "compact.hpp"
#include "overlap_checks.hpp"
#include "rted.hpp"
#include "gted.hpp"


using namespace std;

struct app::arguments
{
    rna_tree templated;
    rna_tree matched;

    struct
    {
        bool run;
        string strategies;
    } rted;
    struct
    {
        bool run;
        string strategies;
        string ted;
        string mapping;
    } gted;
    struct
    {
        bool run;
        string mapping;
        string ps;
        string ps_templated;
    } ps;
};


void app::run(
                std::vector<std::string> args)
{
    APP_DEBUG_FNAME;

    string arg;
    arguments a;
    size_t i;
    args.push_back("");
    bool mt, tt;

    auto nextarg = [&args, &i]() {
        if (i + 1 < args.size())
            return args[i + 1];
        else
            return args.back();
    };

    mt = tt = false;

    for (i = 1; i < args.size(); ++i)
    {
        arg = args.at(i);
        if (arg.empty())
            continue;

        if (arg == "-h" || arg == "--help")
        {
            DEBUG("arg help");
            usage(args.at(0));
            exit(0);
        }
        if (arg == "-mt" || arg == "--match-tree")
        {
            DEBUG("arg match-tree");
            string seq, fold, name;

            seq = args.at(i + 1);
            fold = args.at(i + 2);
            i += 2;
            if (nextarg() == "--name")
            {
                name = args.at(i + 2);
                i+= 2;
            }
            a.matched = create_matched(seq, fold, name);
            mt = true;
            continue;
        }
        if (arg == "-tt" || arg == "--template-tree")
        {
            DEBUG("arg template-tree");
            string ps, fold, name;
            ps = args.at(i + 1);
            fold = args.at(i + 2);
            i += 2;
            if (nextarg() == "--name")
            {
                name = args.at(i + 2);
                i+= 2;
            }
            a.templated = create_templated(ps, fold, name);
            a.ps.ps_templated = ps;
            tt = true;
            continue;
        }
        if (arg == "-r" || arg == "--rted")
        {
            DEBUG("arg rted");
            a.rted.run = true;
            if (nextarg() == "--strategies")
            {
                a.rted.strategies = args.at(i + 2);
                i += 2;
            }
            continue;
        }
        if (arg == "-g" || arg == "--gted")
        {
            DEBUG("arg gted");
            a.gted.run = true;
            if (nextarg() == "--strategies")
            {
                a.gted.strategies = args.at(i + 2);
                i += 2;
            }
            if (nextarg() == "--ted")
            {
                a.gted.ted = args.at(i + 2);
                i += 2;
            }
            if (nextarg() == "--mapping")
            {
                a.gted.mapping = args.at(i + 2);
                i += 2;
            }
            continue;
        }
        if (arg == "--ps")
        {
            DEBUG("arg ps");
            a.ps.run = true;
            if (nextarg() == "--mapping")
            {
                a.ps.mapping = args.at(i + 2);
                i += 2;
            }
            a.ps.ps = args.at(i + 1);
            ++i;
            continue;
        }

        WARN("wrong parameter no.%lu: '%s'", i, to_cstr(arg));
        usage(args.at(0));
        exit(1);
    }

    print(a);

    if (!mt || !tt)
    {
        ERR("trees are missing");
        abort();
    }
    run(a);
}


void app::run(
                arguments args)
{
    if (args.rted.run)
    {
        rted r(args.templated, args.matched);
        r.run();
        if (!args.rted.strategies.empty())
        {
            save_strategy_table(args.rted.strategies, r.get_strategies());

            if (args.gted.strategies.empty())
                args.gted.strategies = args.rted.strategies;
        }
    }
    if (args.gted.run)
    {
        assert(!args.gted.strategies.empty());

        gted g(args.templated, args.matched, load_strategy_table(args.gted.strategies));
        g.run();

        if (!args.gted.ted.empty())
            save_tree_distance_table(args.gted.ted, g.get_tree_distances());

        if (!args.gted.mapping.empty())
        {
            save_tree_mapping_table(args.gted.mapping, g.get_mapping());
            if (args.ps.mapping.empty())
                args.ps.mapping = args.gted.mapping;
        }
    }
    if (args.ps.run)
    {
        assert(!args.ps.mapping.empty());
        assert(!args.ps.ps.empty());

        mapping map = load_mapping_table(args.ps.mapping);

        run(args.templated, args.matched, map);

        if (!args.ps.ps.empty())
        {
            save(args.templated, args.ps.ps, args.ps.ps_templated);
        }
    }
}

void app::run(
                rna_tree& templated,
                rna_tree& matched,
                const mapping& map)
{
    APP_DEBUG_FNAME;

    rna_tree rna;

    rna = matcher(templated, matched).run(map);
    compact(rna).run();
    overlap_checks(rna).run();
}




rna_tree app::create_matched(
                const std::string& seqfile,
                const std::string& foldfile,
                const std::string& name)
{
    APP_DEBUG_FNAME;

    string brackets, labels;

    brackets = read_file(foldfile);
    labels = read_file(seqfile);

    return rna_tree(brackets, labels, name);
}

rna_tree app::create_templated(
                const std::string& psfile,
                const std::string& foldfile,
                const std::string& name)
{
    APP_DEBUG_FNAME;

    string brackets = read_file(foldfile);
    ps_document ps(psfile);
    return rna_tree(brackets, ps.labels, ps.points, name);
}

void app::save(
                rna_tree& rna,
                const std::string& filename,
                const string& templated_rna_file)
{
    APP_DEBUG_FNAME;

    ps_writer ps;
    string prolog;
    stringstream str;

    prolog = ps_document(templated_rna_file).prolog;

    ps.init(filename);
    ps.print(prolog);

    for (rna_tree::pre_post_order_iterator it = rna.begin_pre_post();
            it != rna.end_pre_post(); ++it)
    {
        str << ps_writer::sprint_formatted(it);
    }
    ps.print(str.str());
}




void app::usage(
                const string& appname)
{
    APP_DEBUG_FNAME;

    stringstream str;

    str
        << endl
        << "usage():"
        << endl
        << appname
            << " [-h|--help]"
            << endl
        << appname
            << " [OPTIONS]"
            << " <-mt|--match-tree> SEQ FOLD"
            << " <-tt|--template-tree> PS FOLD"
            << endl
        << endl
        << "OPTIONS:" << endl
        << "\t[-r|--rted [--strategies <FILE>]]" << endl
        << "\t[-g|--gted [--strategies <FILE>] [--ted <FILE>] [--mapping <FILE>]]" << endl
        << "\t[--ps [--mapping <FILE>] <FILE>]" << endl;

    INFO("%s", to_cstr(str.str()));
}

void app::print(
                const arguments& args)
{
    APP_DEBUG_FNAME;

    stringstream str;

    str
        << boolalpha
        << "ARGUMENTS:"
            << endl
        << "templated: "
            << args.templated.print_tree(false)
            << endl
        << "matched: "
            << args.matched.print_tree(false)
            << endl
        << "rted:"
            << endl << '\t'
            << " run=" << args.rted.run << ";"
            << endl << '\t'
            << " strategy-out-file=" << args.rted.strategies
            << endl
        << "gted:"
            << endl << '\t'
            << " run=" << args.gted.run << ";"
            << endl << '\t'
            << " strategy-in-file=" << args.gted.strategies
            << endl << '\t'
            << " tree-edit-distance-out-file=" << args.gted.ted
            << endl << '\t'
            << " mapping-table-out-file=" << args.gted.mapping
            << endl
        << "ps:"
            << endl << '\t'
            << " run=" << args.ps.run
            << endl << '\t'
            << " mapping-table-in-file=" << args.ps.mapping
            << endl << '\t'
            << " ps-out-file=" << args.ps.ps
            << endl
        << endl;

    logger.debugStream() << str.str();
}








