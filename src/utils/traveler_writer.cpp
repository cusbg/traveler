#include "traveler_writer.hpp"

using namespace std;

void traveler_writer::init(const string& filename, rna_tree& rna)
{
    document_writer::init(filename, ".xml");
}

streampos traveler_writer::print(const string& text)
{
    if (text.empty())
        return -1;

    streampos pos = get_pos();
    
    fill();
    
    print_to_stream("<structure>\n");
    print_to_stream(text);
    print_to_stream("</structure>\n");
    
    seek_from_current_pos(sizeof("</structure>\n") -1);
    
    return pos;
}

string traveler_writer::get_circle_formatted(point centre, double radius) const
{
    return "";
}

string traveler_writer::get_label_formatted(const rna_label& label, const RGB& color, const label_info li) const
{
    ostringstream out;
    
    out << "<point x=\"" << label.p.x << "\" y=\"" << label.p.y << "\" b=\"" << label.label << "\"/>"
    << endl;
    
    return out.str();
}

string traveler_writer::get_line_formatted(point from, point to, const RGB& color) const
{
    ostringstream out;
    
    if (from.bad() || to.bad()) return "";
    
    out << "<line fromX=\"" << from.x << "\" fromY=\"" << from.y << "\" toX=\"" << to.x << "\" toY=\""
    << to.y << "\"/>" << endl;
    
    return out.str();
}
