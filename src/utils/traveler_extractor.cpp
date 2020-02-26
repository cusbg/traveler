#include <fstream>
#include "traveler_extractor.hpp"
#include "types.hpp"

using namespace std;

void traveler_extractor::extract(const string& filename)
{
    points.clear();
    labels.clear();
    
    ifstream in(filename);
    regex base_line("\\s*<point\\s+x=\"(.+)\"\\s+y=\"(.+)\"\\s+b=\"([^\"]+)\"\\s*(numbering-label=\"([^\"])\")?");
//    regex base_line("\\s*<point\\s+x=\"(.+)\"\\s+y=\"(.+)\"\\s+b=\"(.+)\"\\s*/>");
    string line;
    smatch match;
    point p;
    string base;
    string ix_label;
    
    while(getline(in, line))
    {
        if(regex_search(line, match, base_line))
        {
            stringstream s;
            // If the string does not include numbering label, match[5] results in an empty string and so does the ix_label
            s << match[1] << " " << match[2] << " " << match[3] << " " << match[5];
            s >> p.x >> p.y >> base >> ix_label;
            
            assert(!s.fail() && s.eof() && base.size() == 1);
            
            points.push_back(p);
            labels.push_back(base[0]);
        }
    }

    mirror_y();
    
}
