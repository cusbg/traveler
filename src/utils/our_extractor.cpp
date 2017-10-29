#include <fstream>
#include "our_extractor.hpp"
#include "types.hpp"

using namespace std;

void our_extractor::extract(const string& filename)
{
    points.clear();
    labels.clear();
    
    ifstream in(filename);
    regex base_line("\\s*<point\\s+x=\"(.+)\"\\s+y=\"(.+)\"\\s+b=\"(.+)\"\\s*/>");
    string line;
    smatch match;
    point p;
    string base;
    
    while(getline(in, line))
    {
        if(regex_search(line, match, base_line))
        {
            stringstream s;
            s << match[1] << " " << match[2] << " " << match[3];
            s >> p.x >> p.y >> base;
            
            assert(!s.fail() && s.eof() && base.size() == 1);
            
            points.push_back(p);
            labels.push_back(base[0]);
        }
    }
    
}
