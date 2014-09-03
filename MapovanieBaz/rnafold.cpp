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
   Run();
   //assert(seq.size() != brackets.size());
}

RNAfold::RNAfold(vector<Base> vec)
{
    for (const auto& val: vec)
        seq.push_back(val.getBase());
    Run();
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
    cout << "RUN" << endl;
    char filename[L_tmpnam];
    tmpnam(filename);
    
    string outputToPS = ">build/output.ps\n";
    outputToPS = "";
    string command = "echo \'" + outputToPS + seq + "\' | RNAfold >> " + filename;
    system(command.c_str());

    ifstream input(filename);

    cout << "reading : " << filename << endl;
    getline(input, brackets);  // == seq
    cout << brackets << endl;
    getline(input, brackets);  // == output
    cout << brackets << endl;
    cout << "END read" << endl;

    brackets = brackets.substr(0, brackets.find(' '));

    remove(filename);
}

std::vector<BasePair> RNAfold::makePairs()
{
    //cout << seq << endl;
    //cout << brackets << endl;

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

