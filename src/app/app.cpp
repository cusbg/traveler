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
        bool colors = false;
        bool overlap_checks = false;
        string file;
    } all;
    struct
    {
        bool run = false;
        string mapping;
    } ted;
    struct
    {
        bool run = false;
        bool overlap_checks = false;
        bool colors = false;
        string mapping;
        string file;
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
    bool overlaps = args.all.overlap_checks || args.draw.overlap_checks;
    bool colored = args.all.colors || args.draw.colors;
    mapping map;
    string img_out = args.all.file;

    map = run_ted(args.templated, args.matched, rted, args.ted.mapping);

    if (args.draw.run)
    {
        assert(!args.draw.mapping.empty());
        map = load_mapping_table(args.draw.mapping);
        args.templated.set_name(args.matched.name() + "_mapped_to_" + args.templated.name());
        img_out = args.draw.file;
    }

    run_drawing(args.templated, args.matched, map, draw, overlaps, colored, img_out);

    INFO("END: APP");
}

mapping app::run_ted(
                rna_tree& templated,
                rna_tree& matched,
                bool run,
                const std::string& mapping_file)
{
    APP_DEBUG_FNAME;

    mapping mapping;

    if (run)
    {
        rted r(templated, matched);
        r.run();

        gted g(templated, matched);
        g.run(r.get_strategies());

        mapping = g.get_mapping();

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
                bool colored,
                const std::string& file)
{
    APP_DEBUG_FNAME;

    if (!run)
        return;

    templated = matcher(templated, matched).run(mapping);
    compact(templated).run();

    save(file, templated, run_overlaps, colored);
}


void app::save(
                const std::string& filename,
                rna_tree& rna,
                bool overlap,
                bool colored)
{
    APP_DEBUG_FNAME;

    auto writers = document_writer::get_writers(colored);

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

    for (auto it = rna.begin(); it != rna.end(); ++it)
    {
        if (it->status == rna_pair_label::rotated)
        {
            class logger rotated("build/logs/rotated.log", logger::DEBUG);

            rotated.debug("%s", rna.name());
            break;
        }
    }


    log_overlaps(rna.name(), overlaps.size());
}

void app::log_overlaps(
                const std::string& name,
                size_t size)
{
    class logger overlaps("build/logs/overlaps.log", logger::DEBUG);

    overlaps.debug("%s %u", to_cstr(name), size);

    if (size != 0)
    {
        WARN("overlaps occurs in %s, count=%u",
                to_cstr(name), size);
    }
}




rna_tree app::create_matched(
                const std::string& fastafile)
{
    APP_DEBUG_FNAME;

    fasta f = read_fasta_file(fastafile);
    try
    {
        return rna_tree(f.brackets, f.labels, f.id);
    }
    catch (const exception& e)
    {
        throw invalid_argument("creating matched rna " + f.id + " failed:" + e.what());
    }
}

rna_tree app::create_templated(
                const std::string& templatefile,
                const std::string& templatetype,
                const std::string& fastafile)
{
    APP_DEBUG_FNAME;

    fasta f = read_fasta_file(fastafile);

    extractor& doc = extractor::get_extractor(templatefile, templatetype);
    try
    {
        return rna_tree(f.brackets, doc.labels, doc.points, f.id);
    }
    catch (const exception& e)
    {
        throw invalid_argument("creating templated rna " + f.id + " failed:" + e.what());
    }
}




void app::usage(
                const string& appname)
{
    LOGGER_PRIORITY_ON_FUNCTION(INFO);

    char endl = '\n';

    logger.info_stream()
        << "usage():"
        << endl
        << endl
        << appname
            << " [-h|--help]"
            << endl
        << appname
            << " [OPTIONS]"
            << " <-mt|--match-tree> FASTA_FILE"
            << " <-tt|--template-tree> [--type TYPE] DOCUMENT FASTA_FILE"
            << endl
        << endl
        << "OPTIONS:" << endl
        << "\t[-a|--all [--colored] [--overlaps] <FILE_OUT>]" << endl
        << "\t[-t|--ted <FILE_MAPPING_OUT>]" << endl
        << "\t[-d|--draw"
            << " [--colored]"
            << " [--overlaps]]"
            << " <FILE_MAPPING_IN> <FILE_OUT>" << endl
        << "\t[--debug]" << endl;
}

void app::print(
                const arguments& args)
{
    APP_DEBUG_FNAME;

    char endl = '\n';
    logger.info_stream()
        << boolalpha
        << "ARGUMENTS:"
            << endl
        << "templated: "
            << args.templated.name()
            << ":"
            << args.templated.print_tree(false)
            << endl
        << "matched: "
            << args.matched.name()
            << ":"
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
            << " ted-mapping-file=" << args.ted.mapping
            << endl
        << "draw:"
            << endl << '\t'
            << " run=" << args.draw.run
            << endl << '\t'
            << " overlaps=" << args.draw.overlap_checks
            << endl << '\t'
            << " colored=" << args.draw.colors
            << endl << '\t'
            << " mapping-table-in-file=" << args.draw.mapping
            << endl << '\t'
            << " draw-out-file=" << args.draw.file
            << endl
        << endl;
}


/* static */ app::arguments app::arguments::parse(
                const std::vector<std::string>& args)
{
    APP_DEBUG_FNAME;

    size_t i;
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
    auto is_argument =
        [&arg](const std::vector<std::string>& arguments)
        {
            for (const string& a : arguments)
                if (a == arg)
                    return true;
            return false;
        };

    for (i = 1; i < args.size(); ++i)
    {
        arg = args.at(i);
        if (arg.empty())
            continue;

        if (is_argument({"-h", "--help"}))
        {
            DEBUG("arg help");
            app::usage(args.at(0));
            exit(0);
        }
        else if (is_argument({"-mt", "--match-tree"}))
        {
            DEBUG("arg match-tree");
            string fastafile = args.at(i + 1);
            a.matched = app::create_matched(fastafile);
            ++i;
            continue;
        }
        else if (is_argument({"-tt", "--template-tree"}))
        {
            DEBUG("arg template-tree");
            string templatefile, fastafile;
            string templatetype = "ps";
            if (nextarg() == "--type")
            {
                templatetype = args.at(i + 2);
                i += 2;
            }
            templatefile = args.at(i + 1);
            fastafile = args.at(i + 2);
            a.templated = app::create_templated(templatefile, templatetype, fastafile);
            i += 2;
        }
        else if (is_argument({"-a", "--all"}))
        {
            DEBUG("arg all");
            a.all.run = true;
            while (true)
            {
                if (nextarg() == "--colored")
                {
                    a.all.colors = true;
                    i += 1;
                }
                else if (nextarg() == "--overlaps")
                {
                    a.all.overlap_checks = true;
                    i += 1;
                }
                else
                    break;
            }
            a.all.file = args.at(i + 1);
            ++i;
        }
        else if (is_argument({"-t", "--ted"}))
        {
            DEBUG("arg ted");
            a.ted.run = true;
            a.ted.mapping = args.at(i + 1);
            i += 1;
        }
        else if (is_argument({"-d", "--draw"}))
        {
            DEBUG("arg draw");
            a.draw.run = true;
            while (true)
            {
                if (nextarg() == "--overlaps")
                {
                    a.draw.overlap_checks = true;
                    i += 1;
                }
                else if (nextarg() == "--colored")
                {
                    a.draw.colors = true;
                    i += 1;
                }
                else
                    break;
            }
            a.draw.mapping = args.at(i + 1);
            a.draw.file = args.at(i + 2);
            i += 2;
        }
        else if (is_argument({"--debug"}))
        {
            logger.set_priority(logger::DEBUG);
            DEBUG("enabled debug mode");
        }
        else
        {
            ostringstream out;
            out
                << "wrong parameter no."
                << i
                << ": '"
                << arg
                << "'; try running "
                << args[0]
                << " --help for more arguments details";
            throw invalid_argument(out.str());
        }
    }

    if (a.templated == rna_tree() || a.matched == rna_tree())
        throw invalid_argument("Trees are missing, try running " + args[0] + " -h for more arguments details");

    return a;
}


