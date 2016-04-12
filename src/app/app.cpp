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
#include "mapping.hpp"
#include "tree_matcher.hpp"
#include "ps_extractor.hpp"
#include "ps_writer.hpp"
#include "svg_writer.hpp"
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
        string file;
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
        string file;
        string file_templated;
    } document;

public:
    static arguments parse(
                const std::vector<std::string>& args);

private:
    arguments() = default;
};



void app::run(
                std::vector<std::string> args)
{
    APP_DEBUG_FNAME;

    args.push_back("");
    run(arguments::parse(args));
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

        save(args.all.file, args.templated, true);
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
                if (args.document.mapping.empty())
                    args.document.mapping = args.gted.mapping;
            }
        }
        if (args.document.run)
        {
            assert(!args.document.mapping.empty());
            assert(!args.document.file.empty());

            mapping map = load_mapping_table(args.document.mapping);

            args.templated.set_name(args.matched.name() + "_mapped_to_" + args.templated.name());

            args.templated = matcher(args.templated, args.matched).run(map);
            compact(args.templated).run();

            save(args.document.file, args.templated, args.document.overlap_checks);
        }
    }

    INFO("END: APP");
}




void app::save(
                const std::string& filename,
                rna_tree& rna,
                bool overlaps)
{
    APP_DEBUG_FNAME;

    ps_writer ps;
    svg_writer svg;

    ps.init(filename, rna.begin());
    svg.init(filename, rna.begin());
    INFO("save(%s)", to_cstr(filename));

    save(rna, ps, overlaps);
    save(rna, svg, overlaps);
}

void app::save(
                rna_tree& rna,
                document_writer& writer,
                bool overlap)
{
    APP_DEBUG_FNAME;

    overlap_checks::overlaps overlaps;
    
    //if (overlap)
        //overlaps = overlap_checks().run(rna);

    log_overlaps(rna.name(), overlaps.size());

    writer.print(writer.get_rna_formatted(rna));

    for (const auto& p : overlaps)
        writer.print(writer.get_circle_formatted(p.centre, p.radius));
}

void app::log_overlaps(
                const std::string& name,
                size_t size)
{
    ofstream out;
    out.open("build/logs/overlaps.log", ios_base::app);

    out
        << name
        << " : "
        << size
        << endl;

    if (size != 0)
    {
        WARN("overlaps occurs in %s, count=%lu",
                to_cstr(name), size);
    }
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
                const std::string& docfile,
                const std::string& doctype,
                const std::string& foldfile,
                const std::string& name)
{
    APP_DEBUG_FNAME;

    string brackets = read_file(foldfile);
    extractor& doc = extractor::get_extractor(docfile, doctype);
    return rna_tree(brackets, doc.labels, doc.points, name);
}




void app::usage(
                const string& appname)
{
    stringstream str;

    str
        << endl
        << endl
        << "usage():"
        << endl
        << appname
            << " [-h|--help]"
            << endl
        << appname
            << " [OPTIONS]"
            << " <-mt|--match-tree> SEQ FOLD"
            << " <-tt|--template-tree> [--type TYPE] DOCUMENT FOLD"
            << endl
        << endl
        << "OPTIONS:" << endl
        << "\t[-a|--all <FILE>]" << endl
        << "\t[-r|--rted [--strategies <FILE>]]" << endl
        << "\t[-g|--gted [--strategies <FILE>]"
            << " [--ted-out <FILE>]"
            << " [--ted-in <FILE>]"
            << " [--mapping <FILE>]]" << endl
        << "\t[--image"
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
            << " file=" << args.all.file
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
            << " run=" << args.document.run
            << endl << '\t'
            << " overlaps=" << args.document.overlap_checks
            << endl << '\t'
            << " mapping-table-in-file=" << args.document.mapping
            << endl << '\t'
            << " document-out-file=" << args.document.file
            << endl
        << endl;

    INFO("%s", to_cstr(str.str()));
}


/* static */ app::arguments app::arguments::parse(
                const std::vector<std::string>& args)
{
    APP_DEBUG_FNAME;

    size_t i;
    bool mt, tt;
    string arg;
    arguments a;

    auto nextarg =
        [&args, &i]()
        {
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
            app::usage(args.at(0));
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
            a.matched = app::create_matched(seq, fold, name);
            mt = true;
            continue;
        }
        if (arg == "-tt" || arg == "--template-tree")
        {
            DEBUG("arg template-tree");
            string doc, doctype, fold, name;
            if (nextarg() == "--type")
            {
                doctype = args.at(i + 2);
                i += 2;
            }
            else
            {
                doctype = "ps";
            }
            doc = args.at(i + 1);
            fold = args.at(i + 2);
            i += 2;
            if (nextarg() == "--name")
            {
                name = args.at(i + 2);
                i+= 2;
            }
            a.templated = app::create_templated(doc, doctype, fold, name);
            a.document.file_templated = doc;
            tt = true;
            continue;
        }
        if (arg == "-a" || arg == "--all")
        {
            DEBUG("arg all");
            a.all.run = true;
            a.all.file = args.at(i + 1);
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
        if (arg == "--image")
        {
            DEBUG("arg image");
            a.document.run = true;
            while (true)
            {
                if (nextarg() == "--mapping")
                {
                    a.document.mapping = args.at(i + 2);
                    i += 2;
                }
                else if (nextarg() == "--overlaps")
                {
                    a.document.overlap_checks = true;
                    i += 1;
                }
                else
                    break;
            }
            a.document.file = args.at(i + 1);
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

    return a;
}


