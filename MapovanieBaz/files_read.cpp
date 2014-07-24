/*
 * File: files_read.cpp
 * Created: 2014-07-23
 * By: eliasr <eliasr@u-pl22>
 */

#include "files_read.hpp"

using namespace std;

std::pair<std::vector<Base>, std::vector<BasePair> > Reader::readBPSEQ(string file)
{
    ifstream input(file);
    string line;
    int n1, n2;
    char b;
    vector<BasePair> vecBP;
    vector<Base> vecB;

    while (true)
    {
        getline(input, line);
        if (input.fail())
            break;
        
        stringstream other(line);
        other >> n1 >> b >> n2;

        if (other.fail() || !Base::isBase(b))
            continue;

        vecBP.emplace_back(n1, n2);
        vecB.emplace_back(b);
    }
    return make_pair<vector<Base>, vector<BasePair> >(move(vecB), move(vecBP));
}

std::pair<vector<Base>, vector<Point> > Reader::readPS_DB_format(std::string file)
{
    ifstream input(file);
    string line, x;
    string bs;
    float n1, n2;

    vector<Point> vecP;
    vector<Base> vecB;

    while(true)
    {
        getline(input, line);
        if (input.fail())
            break;

        stringstream other(line);
        other >> bs >> n1 >> n2 >> x;

        if (other.fail() || bs.size() != 3 || !Base::isBase(bs[1]))
            continue;

         vecP.push_back({n1, n2});
         vecB.emplace_back(bs[1]);
    }
    return make_pair<vector<Base>, vector<Point> >(move(vecB), move(vecP));
}

std::pair<std::vector<Base>, std::vector<Point> > Reader::readPS_RNAfold_format(std::string file)
{
    ifstream input(file);
    string line;
    float n1, n2;

    vector<Point> vecP;
    vector<Base> vecB;

    while(true)
    {
        getline(input, line);
        if (input.fail())
            break;

        if (line == "/sequence (\\")
        {
            while (true)
            {
                getline(input, line);
                if (input.fail() || line == ") def")
                    break;
                for (size_t i=0; i< line.size(); ++i)
                    if (Base::isBase(line[i]))
                        vecB.emplace_back(line[i]);
            }
        }
        if (line == "/coor [")
        {
            char ch;
            while(true)
            {
                getline(input, line);
                if (input.fail() || line == "] def")
                    break;
                stringstream other(line);
                other >> ch >> n1 >> n2 >> ch;

                if (other.fail())
                    continue;
                vecP.push_back({n1, n2});
            }
        }
    }
    return make_pair<vector<Base>, vector<Point> >(move(vecB), move(vecP));
}


