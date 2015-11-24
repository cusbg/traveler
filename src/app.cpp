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
        bool run = false;
        string psout;
    } all;
    struct
    {
        bool run = false;
        string strategies;
    } rted;
    struct
    {
        bool run = false;
        string strategies;
        string ted_out;
        string ted_in;
        string mapping;
    } gted;
    struct
    {
        bool run = false;
        bool overlap_checks = false;
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
        if (arg == "-a" || arg == "--all")
        {
            DEBUG("arg all");
            a.all.run = true;
            a.all.psout = args.at(i + 1);
            ++i;
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
            bool arg = true;

            while (arg)
            {
                if (nextarg() == "--strategies")
                {
                    a.gted.strategies = args.at(i + 2);
                    i += 2;
                }
                else if (nextarg() == "--ted-out")
                {
                    a.gted.ted_out = args.at(i + 2);
                    i += 2;
                }
                else if (nextarg() == "--ted-in")
                {
                    a.gted.ted_in = args.at(i + 2);
                    i += 2;
                }
                else if (nextarg() == "--mapping")
                {
                    a.gted.mapping = args.at(i + 2);
                    i += 2;
                }
                else
                    arg = false;
            }
            continue;
        }
        if (arg == "--ps")
        {
            DEBUG("arg ps");
            a.ps.run = true;
            while (true)
            {
                if (nextarg() == "--mapping")
                {
                    a.ps.mapping = args.at(i + 2);
                    i += 2;
                }
                else if (nextarg() == "--overlaps")
                {
                    a.ps.overlap_checks = true;
                    i += 1;
                }
                else
                    break;
            }
            a.ps.ps = args.at(i + 1);
            ++i;
            continue;
        }
        if (arg == "-d" || arg == "--debug")
        {
            logger.set_priority(logger::DEBUG);
            DEBUG("enable debug mode");
            continue;
        }

        WARN("wrong parameter no.%lu: '%s'", i, to_cstr(arg));
        usage(args.at(0));
        exit(1);
    }

    if (!mt || !tt)
    {
        ERR("trees are missing");
        usage(args.at(0));
        abort();
    }

    run(a);
}


void app::run(
                arguments args)
{
    APP_DEBUG_FNAME;

    INFO("BEG: APP");

    print(args);

    if (args.all.run)
    {
        rted r(args.templated, args.matched);
        gted g(args.templated, args.matched);

        r.run();
        g.run(r.get_strategies());

        args.templated = matcher(args.templated, args.matched).run(g.get_mapping());
        compact(args.templated).run();

        ps_writer ps;
        ps.init(args.all.psout);

        ps.print(ps.default_prologue(), true);
        save(args.templated, ps, args.ps.overlap_checks);
    }
    else
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
            gted g(args.templated, args.matched);

            if (!args.gted.ted_in.empty())
                g.set_tdist_table(load_tree_distance_table(args.gted.ted_in));
            else
                g.run(load_strategy_table(args.gted.strategies));

            if (!args.gted.ted_out.empty())
                save_tree_distance_table(args.gted.ted_out, g.get_tree_distances());

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

            args.templated.set_name(args.templated.name() + "-" + args.matched.name());
            args.templated = matcher(args.templated, args.matched).run(map);
            compact(args.templated).run();

            if (!args.ps.ps.empty())
            {
                save(args.ps.ps, args.templated,
                        args.ps.ps_templated, args.ps.overlap_checks);
            }
        }
    }

    INFO("END: APP");
}




void app::save(
                const std::string& filename,
                rna_tree& rna,
                const std::string& templated_ps,
                bool overlaps)
{
    APP_DEBUG_FNAME;

    ps_writer ps;
    string prolog;

    prolog = ps_document(templated_ps).prolog;

    ps.init_default(filename, rna.begin());

    save(rna, ps, overlaps);
}

void app::save(
            rna_tree& rna,
            ps_writer& writer,
            bool overlap)
{
    APP_DEBUG_FNAME;

    stringstream str;

    rna_tree::pre_post_order_iterator end(rna.begin(), false);
    rna_tree::pre_post_order_iterator it = ++rna.begin();
    overlap_checks::overlaps overlaps;
    
    if (overlap)
        overlaps = overlap_checks().run(rna);

    if (!overlaps.empty())
    {
        ofstream out;
        out.open("build/logs/overlaps.log", ios_base::app);
        out
            << rna.name()
            << " : "
            << overlaps.size()
            << endl;
        WARN("overlaps occurs in %s, count=%lu",
                to_cstr(rna.name()), overlaps.size());
    }

    for (; it != end; ++it)
        str << ps_writer::sprint_formatted(it);

    for (const auto& p : overlaps)
        str << ps_writer::sprint_circle(p.centre, p.radius);

    writer.print(str.str());
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
        << "\t[-a|--all <FILE>]" << endl
        << "\t[-r|--rted [--strategies <FILE>]]" << endl
        << "\t[-g|--gted [--strategies <FILE>]"
            << " [--ted-out <FILE>]"
            << " [--ted-in <FILE>]"
            << " [--mapping <FILE>]]" << endl
        << "\t[--ps"
            << " [--mapping <FILE>]"
            << " [--overlaps] <FILE>]" << endl
        << "\t[-d|--debug]" << endl;

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
        << "all:"
            << endl << '\t'
            << " run=" << args.all.run << ";"
            << endl << '\t'
            << " psout=" << args.all.psout
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
            << " tree-edit-distance-in-file=" << args.gted.ted_in
            << endl << '\t'
            << " tree-edit-distance-out-file=" << args.gted.ted_out
            << endl << '\t'
            << " mapping-table-out-file=" << args.gted.mapping
            << endl
        << "ps:"
            << endl << '\t'
            << " run=" << args.ps.run
            << endl << '\t'
            << " overlaps=" << args.ps.overlap_checks
            << endl << '\t'
            << " mapping-table-in-file=" << args.ps.mapping
            << endl << '\t'
            << " ps-out-file=" << args.ps.ps
            << endl
        << endl;

    INFO("%s", to_cstr(str.str()));
}



