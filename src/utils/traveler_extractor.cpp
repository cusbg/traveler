#include <fstream>
#include "traveler_extractor.hpp"
#include "types.hpp"

using namespace std;

void traveler_extractor::extract(const string& filename)
{
    points.clear();
    labels.clear();
    numbering_labels.clear();
    
    ifstream in(filename);
    regex base_line("\\s*<point\\s+x=\"(.+)\"\\s+y=\"(.+)\"\\s+b=\"([^\"]+)\"(\\s*numbering-label=\"([^\"])+\")?\\s*/>");
//    regex base_line("\\s*<point\\s+x=\"(.+)\"\\s+y=\"(.+)\"\\s+b=\"(.+)\"\\s*/>");
    string line;
    smatch match;
    point p;
    string base;
    string numbering_label;
    
    while(getline(in, line))
    {
        if(regex_search(line, match, base_line))
        {
            stringstream s;
            s << match[1] << " " << match[2] << " " << match[3] << " " << match[5];
            if (match[5].matched) {
            s >> p.x >> p.y >> base >> numbering_label;
            } else {
                s >> p.x >> p.y >> base;
            }

            assert(!s.fail() && base.size() == 1);
//            assert(!s.fail() && s.eof() && base.size() == 1);
            
            points.push_back(p);
            labels.push_back(base[0]);
            numbering_labels.push_back(numbering_label);
        }
    }

    mirror_y();
    
}
