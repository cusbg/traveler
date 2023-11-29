#include "traveler_writer.hpp"

using namespace std;

void traveler_writer::init(const string& filename, rna_tree& rna, bool labels_template)
{
    document_writer::init(filename, ".xml", rna);
}

streampos traveler_writer::print(const string& text)
{
    if (text.empty())
        return -1;

    streampos pos = get_pos();
    
    fill();
    print_to_stream(text);
    seek_from_current_pos(sizeof("</structure>\n") -1);
    
    return pos;
}

string traveler_writer::get_circle_formatted(point centre, double radius, const shape_options opts) const
{
    return "";
}

string traveler_writer::get_label_formatted(const rna_label& label, const RGB& color, const rna_pair_label::status_type status, const label_info li, const shape_options opts) const
{

    return get_label_formatted(label, color.get_name(), status, li, opts);
}

string traveler_writer::get_label_formatted(const rna_label& label, const std::string& clazz, const rna_pair_label::status_type status, const label_info li, const shape_options opts) const
{
    ostringstream out;

    out << "<point x=\"" << label.p.x << "\" y=\"" << label.p.y << "\" b=\"" << label.label
        << "\" status=\"" << get_status_name(status) << "\" color=\"" << clazz << "\"";// nt=\"" << li.is_nt << "\"";


    if (li.is_nt == 0) {
        out << " associatedId=\"" << li.ix <<  "\"";

    } else {
        out << " id=\"" << li.ix <<  "\"";

    }
    out << "/>"
        << endl;

    return out.str();
}

string traveler_writer::get_line_formatted(point from, point to, int ix_from, int ix_to, const bool is_base_pair, bool is_predicted, const RGB& color, const shape_options opts) const
{
    return get_line_formatted(from, to, ix_from, ix_to, is_base_pair, is_predicted, color.get_name(), opts);
}

string traveler_writer::get_line_formatted(point from, point to, const int ix_from, const int ix_to, const bool is_base_pair, bool is_predicted, const std::string& clazz, const shape_options opts) const
{
    ostringstream out;

    if (from.bad() || to.bad()) return "";

    out << "<line fromX=\"" << from.x << "\" fromY=\"" << from.y << "\" toX=\"" << to.x << "\" toY=\""
        << to.y << "\"";// bp=\"" << int(is_base_pair) << "\"";
    if (is_base_pair) {
        out << " fromIx=\"" << ix_from << "\" toIx=\"" << ix_to << "\" isPredicted=\"" << is_predicted << "\"";
    } else {
        out << " associatedId=\"" << ix_to << "\"";

    }
    out << "/>" << endl;

    return out.str();
}

std::string traveler_writer::get_polyline_formatted(
        std::vector<point> &points,
        const RGB& color,
        const shape_options opts) const {

    //TODO implement if needed
    return "";
};

std::string traveler_writer::get_circle_formatted(
        point centre,
        double radius) const {
    // TODO implement if needed
    return "";
}
