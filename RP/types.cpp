/*
 * File: types.cpp
 * Created: 2014-09-01
 * By: eliasr <eliasr@u-pl22>
 */

#include "types.hpp"

using namespace std;

bool Base::isBase(char ch)
{
    string s = "ACGUacgu";
    return s.find(ch) != s.npos;
}

void Base::setBase(char ch)
{
    switch (ch)
    {
        case 'A':
        case 'a':
            b = _base::Adenin;
            break;
        case 'C':
        case 'c':
            b = _base::Cytosin;
            break;
        case 'G':
        case 'g':
            b = _base::Guanin;
            break;
        case 'U':
        case 'u':
            b = _base::Uracil;
            break;
        default:
            throw "ERROR: not a base";
    }
}

char Base::getBase() const
{
    switch (b)
    {
        case Adenin:
            return 'A';
        case Guanin:
            return 'G';
        case Uracil:
            return 'U';
        case Cytosin:
            return 'C';
    }
    throw "";
}

Base::Base(char ch)
{
    setBase(ch);
}


std::string Global::HS_seq = "../InFiles/seq";
std::string Global::HS_db = "../InFiles/homo_sapiens.ps";
std::string Global::HS_bpseq = "../InFiles/homo_sapiens.bpseq";
std::string Global::HS_rnafold = "../InFiles/homo_sapiens_RNAfold.ps";
std::string Global::HS_zatvorky = "../InFiles/zatvorky";

std::string Global::rnaseq =    "AACGTTuGvCATU";
std::string Global::zatvorky =  "((...).(.).).";




#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PatternLayout.hh>

log4cpp::Category& init_logger()
{
    log4cpp::Appender *appender = new log4cpp::OstreamAppender("console", &std::cout);
    log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
    layout->setConversionPattern("%d{%H:%M:%S:%m}\t[%p] %m%n");
    appender->setLayout(layout);


    log4cpp::Category& logger = log4cpp::Category::getRoot();
    logger.setPriority(log4cpp::Priority::DEBUG);

    logger.addAppender(appender);
    return logger;
}

log4cpp::Category& logger = init_logger();






