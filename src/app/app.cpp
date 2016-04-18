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
        string distances;
        string mapping;
    } ted;
    struct
    {
        bool run = false;
        bool overlap_checks = false;
        string mapping;
        string file;
        string file_templated;
    } draw;

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
    bool rted = args.all.run || args.ted.run;
    bool draw = args.all.run || args.draw.run;
    mapping map;
    string img_out = args.all.file;

    run_ted(args.templated, args.matched, rted, args.ted.distances, args.ted.mapping);

    if (args.draw.run)
    {
        assert(!args.draw.mapping.empty());
        map = load_mapping_table(args.draw.mapping);
        args.templated.set_name(args.matched.name() + "_mapped_to_" + args.templated.name());
        img_out = args.draw.file;
    }

    run_drawing(args.templated, args.matched, map, draw, args.draw.overlap_checks, img_out);

    INFO("END: APP");
}

mapping app::run_ted(
                rna_tree& templated,
                rna_tree& matched,
                bool run,
                const std::string& distances_file,
                const std::string& mapping_file)
{
    APP_DEBUG_FNAME;

    mapping mapping;

    if (run)
    {
        rted r(templated, matched);
        gted g(templated, matched);

        r.run();
        g.run(r.get_strategies());

        mapping = g.get_mapping();

        if (!distances_file.empty())
            save_tree_distance_table(distances_file, g.get_tree_distances());
        if (!mapping_file.empty())
            save_tree_mapping_table(mapping_file, mapping);
    }
    else
    {
        DEBUG("skipping rted run, returning default mapping");
    }

    return mapping;
}

void app::run_drawing(
                rna_tree& templated,
                rna_tree& matched,
                const mapping& mapping,
                bool run,
                bool run_overlaps,
                const std::string& file)
{
    APP_DEBUG_FNAME;

    if (!run)
        return;
    // TODO overlaps

    templated = matcher(templated, matched).run(mapping);
    compact(templated).run();

    save(file, templated, run_overlaps);
}


void app::save(
                const std::string& filename,
                rna_tree& rna,
                bool overlap)
{
    APP_DEBUG_FNAME;

    auto writers = document_writer::get_writers();

    overlap_checks::overlaps overlaps;
    if (overlap)
        overlaps = overlap_checks().run(rna);

    for (auto& writer : writers)
    {
        writer->init(filename, rna.begin());
        writer->print(writer->get_rna_formatted(rna));

        for (const auto& p : overlaps)
            writer->print(writer->get_circle_formatted(p.centre, p.radius));
    }

    log_overlaps(rna.name(), overlaps.size());
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
        << "\t[-t|--ted <FILE-DISTANCES> <FILE-MAPPING>]" << endl
        << "\t[--draw"
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
        << "ted:"
            << endl << '\t'
            << " run=" << args.ted.run << ";"
            << endl << '\t'
            << " ted-distances-file=" << args.ted.distances
            << endl << '\t'
            << " ted-mapping-file=" << args.ted.mapping
            << endl
        << "draw:"
            << endl << '\t'
            << " run=" << args.draw.run
            << endl << '\t'
            << " overlaps=" << args.draw.overlap_checks
            << endl << '\t'
            << " mapping-table-in-file=" << args.draw.mapping
            << endl << '\t'
            << " draw-out-file=" << args.draw.file
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
            a.draw.file_templated = doc;
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
        if (arg == "-t" || arg == "--ted")
        {
            DEBUG("arg ted");
            a.ted.run = true;
            a.ted.distances = args.at(i + 1);
            a.ted.mapping = args.at(i + 2);
            i += 2;
            continue;
        }
        if (arg == "--draw")
        {
            DEBUG("arg draw");
            a.draw.run = true;
            while (true)
            {
                if (nextarg() == "--mapping")
                {
                    a.draw.mapping = args.at(i + 2);
                    i += 2;
                }
                else if (nextarg() == "--overlaps")
                {
                    a.draw.overlap_checks = true;
                    i += 1;
                }
                else
                    break;
            }
            a.draw.file = args.at(i + 1);
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


