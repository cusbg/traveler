/*
 * File: main.cpp
 * Created: 2014-09-01
 * By: eliasr <eliasr@u-pl22>
 */

#include <iostream>
#include "trees.hpp"

using namespace std;

    typedef vector<int> vint;

                    //  {1, 2, 3, 4, 5, 6}
    vint left1 =        {1, 1, 3, 4, 1, 1};
    vint left2 =        {1, 1, 3, 3, 5, 1};
    vint LRkeyroots1 =  {0, 0, 1, 1, 0, 1};
    vint LRkeyroots2 =  {0, 0, 0, 1, 1, 1};

    vector<vector<int> > cost= 
                        {{0, 1, 1, 1, 1, 1, 1},
                         {1, 0, 1, 0, 1, 0, 1},
                         {1, 1, 0, 1, 0, 1, 1},
                         {1, 0, 1, 0, 1, 0, 1},
                         {1, 0, 1, 0, 1, 0, 1}, 
                         {1, 1, 0, 1, 0, 1, 1}, 
                         {1, 1, 0, 1, 0, 1, 1}};

    vector<vector<int> > TED(left1.size() + 1, vector<int>(left2.size() + 1, 0xBADF00D));

int min(int x, int y, int z)
{
    if (x <= y && x <= z)
        return x;
    if (y <= x && y <= z)
        return y;
    return z;
}

void treedist(int pos1, int pos2)
{
    int b1 = pos1 - left1[pos1-1]+2;
    int b2 = pos2 - left2[pos2-1]+2;

    vector<vector<int> > Fdist(b1, vector<int>(b2, 0xBADF00D));
    //cout << Fdist.size() << endl;
    //cout << Fdist.at(0).size() << endl;
    
    Fdist[0][0] = 0;
    for (int i = 1; i < b1; ++i)
        Fdist[i][0] = Fdist[i - 1][0] + cost[i][0];
    for (int i = 1; i < b2; ++i)
        Fdist[0][i] = Fdist[0][i - 1] + cost[0][i];

    int i(1), j(1), k, l;
    for (k = left1[pos1]; k != pos1; ++k, ++i)
    {
        for (l = left2[pos2]; l != pos2; ++l, ++j)
        {
            if (left1[k] == left1[pos1] && left2[l] == left2[pos2])
            {
                Fdist[i][j] = min(
                            Fdist[i-1][j] + cost[0][l],
                            Fdist[i][j-1] + cost[k][0],
                            Fdist[i-1][j-1] + cost[k][l]
                        );
                TED[k][l] = Fdist [i][j];
            }
            else
            {
                int M = left1[k] - left1[pos1];
                int N = left2[l] - left2[pos2];

                Fdist[i][j] = min(
                        Fdist[i-1][j] + cost[0][l],
                        Fdist[i][j - 1] + cost[k][0],
                        Fdist[M][N] + TED[k][l]
                     );
            }
        }
    }
}

int main()
{
    treedist(3, 4);

    //vint left1 =        {1, 2, 2, 1, 5, 1}
    //vint left2 =        {1, 2, 1, 1, 5, 1}
    //vint LRkeyroots1 =  {0, 0, 1, 0, 1, 1}
    //vint LRkeyroots2 =  {0, 1, 0, 0, 1, 1}
    //int cost[X][Y];

    return 0;
}




//string zatvorky =   "(.).((.).)";
//string rna =        "UAACGiAjAC";


//int main(int argc, char** argv)
//{
    //Node n;
    //n.createTree(rna, zatvorky,0);

    //cout << rna << endl << zatvorky << endl << endl;
    //cout << printLabels(n) << endl;
    
    //cout << "R_path"<<endl;

    //return 0;
//}

