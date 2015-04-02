/*
 * File: types.cpp
 *
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
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

#include "types.hpp"

using namespace std;

/*
std::string Global::HS_seq = "../InFiles/seq";
std::string Global::HS_db = "../InFiles/homo_sapiens.ps";
std::string Global::HS_bpseq = "../InFiles/homo_sapiens.bpseq";
std::string Global::HS_rnafold = "../InFiles/homo_sapiens_RNAfold.ps";
std::string Global::HS_zatvorky = "../InFiles/zatvorky";

std::string Global::rnaseq =    "ABCDEFGHIJKLM";
std::string Global::zatvorky =  "((...).(.).).";
*/

//#define PATH_STRATEGY_LEFT_T1   1
//#define PATH_STRATEGY_RIGHT_T1  2
//#define PATH_STRATEGY_HEAVY_T1  4
//#define PATH_STRATEGY_LEFT_T2   8
//#define PATH_STRATEGY_RIGHT_T2  16
//#define PATH_STRATEGY_HEAVY_T2  32


std::string graph_to_string(strategy_pair ch)
{
    if (is_T1(ch))
        return "T1";
    if (is_T2(ch))
        return "T2";

    throw invalid_argument("only T1/T2 supported");
}

std::string path_to_string(strategy_pair ch)
{
    if (is_left_path(ch))
        return "left";
    if (is_right_path(ch))
        return "right";
    if (is_heavy_path(ch))
        return "heavy";

    throw invalid_argument("only LRH path strategies supported");
}

std::string all_to_string(strategy_pair ch)
{
    return graph_to_string(ch) + "-" + path_to_string(ch);
}

bool is_path_strategy(strategy_pair ch)
{
    return is_T1(ch) || is_T2(ch);
}

bool is_right_path(strategy_pair ch)
{
    return ch == PATH_STRATEGY_RIGHT_T1 ||
        ch == PATH_STRATEGY_RIGHT_T2;
}
bool is_left_path (strategy_pair ch)
{
    return ch == PATH_STRATEGY_LEFT_T1 ||
        ch == PATH_STRATEGY_LEFT_T2;
}
bool is_heavy_path(strategy_pair ch)
{
    return ch == PATH_STRATEGY_HEAVY_T1 ||
        ch == PATH_STRATEGY_HEAVY_T2;
}

bool is_T1(strategy_pair ch)
{
    switch (ch)
    {
        case PATH_STRATEGY_LEFT_T1:
        case PATH_STRATEGY_RIGHT_T1:
        case PATH_STRATEGY_HEAVY_T1:
            return true;
        default:
            return false;
    }
}
bool is_T2(strategy_pair ch)
{
    switch (ch)
    {
        case PATH_STRATEGY_LEFT_T2:
        case PATH_STRATEGY_RIGHT_T2:
        case PATH_STRATEGY_HEAVY_T2:
            return true;
        default:
            return false;
    }
}


#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PatternLayout.hh>

// getpid():
#include <sys/types.h>
#include <unistd.h>

log4cpp::Category& init_logger()
{
    log4cpp::Appender* console_appender;
    log4cpp::Appender* file_appender;
    log4cpp::PatternLayout* console_layout;
    log4cpp::PatternLayout* file_layout;
    string logfile = "build/program-" + to_string(getpid()) + ".log";
    string pattern = "%d{%H:%M:%S:%l} %u:\t[%p] %m%n";

    console_appender = new log4cpp::OstreamAppender("console", &std::cout);
    file_appender = new log4cpp::FileAppender("default", logfile, false);   // append=false=>truncate
    
    file_layout = new log4cpp::PatternLayout();
    console_layout = new log4cpp::PatternLayout();
    console_layout->setConversionPattern(pattern);
    file_layout->setConversionPattern(pattern);

    file_appender->setLayout(file_layout);
    console_appender->setLayout(console_layout);

    log4cpp::Category& log = log4cpp::Category::getRoot();
    log.setPriority(log4cpp::Priority::DEBUG);

    log.addAppender(console_appender);
    log.addAppender(file_appender);

    return log;
}

log4cpp::Category& logger = init_logger();




