/*
 * File: rnafold.cpp
 * Created: 2014-07-23
 * By: eliasr <eliasr@u-pl22>
 */


#include "rnafold.hpp"

using namespace std;

string RNAfold::getSeq()
{
    return seq;
}

string RNAfold::getBrackets()
{
    return brackets;
}

RNAfold::RNAfold(string _seq)
    : seq(_seq)
{
   Run2();
   //assert(seq.size() != brackets.size());
}

void RNAfold::Run2()
{
    ifstream Binput("../InFiles/zatvorky");
    getline(Binput, brackets);

    ifstream Sinput("../InFiles/seq");
    getline(Sinput, seq);
}

void RNAfold::Run()
{
    char filename[L_tmpnam];
    tmpnam(filename);
    
    string command = "echo \'" + seq + "\' | RNAfold ---noPS >> " + filename;
    system(command.c_str());

    ifstream input(filename);

    getline(input, brackets);  // == seq
    getline(input, brackets);  // == output

    brackets = brackets.substr(brackets.find(' '));
}

std::vector<BasePair> RNAfold::makePairs()
{
    //cout << seq << endl;
    //cout << brackets << endl;

    brackets = ".......";
    vector<BasePair> vec(brackets.size(), make_pair<int,int>(-1, -1));

    for (int i=0; i< brackets.size(); i++)
    {
        if (brackets[i] == ')')  // already done
            continue;
        else if (brackets[i] == '.')
        {
            vec[i].first = i + 1;
            vec[i].second = 0;
        }
        else
        {
            assert(brackets[i] == '(');
            int zanorenie = 1;
            int j = i + 1;
            while(zanorenie)
            {
                assert(j < brackets.size());
                if (brackets[j] == ')')
                    zanorenie--;
                else if (brackets[j] == '(')
                    zanorenie++;
                ++j;
            }
            --j;
            vec[i].first    = i + 1;
            vec[i].second   = j + 1;
            vec[j].first    = j + 1;
            vec[j].second   = i + 1;
        }
    }
    for (const auto& val: vec)
    {
        assert (val.first >= 0 && val.second >= 0);
        //cout << val.first << " " << val.second << endl;
    }
    return vec;
}

