#include "json_writer.hpp"
#include <iostream>

using namespace std;
using json = nlohmann::json;

void json_writer::init(const string& filename, rna_tree& rna, bool labels_template)
{
    document_writer::init(filename, ".json", rna);
}

streampos json_writer::print(const string& text)
{
    if (text.empty())
        return -1;

    //streampos pos = get_pos();

    print_to_stream(text);

    return get_pos();
}

std::string json_writer::get_rna_formatted(
        rna_tree rna,
        const numbering_def& numbering,
        pseudoknots pn) const
{
    rna.update_labels_seq_ix(); //set indexes for the individual labels which is needed for outputing base pair indexes (at least in the traveler writer)
    return get_rna_subtree_formatted(rna, numbering); //+ get_rna_background_formatted(rna.begin_pre_post(), rna.end_pre_post());
}

vector<string> split_clazz(string clazz) {
    vector<string> tokens;
    string token;

    for (const auto& c: clazz) {
        if (!isspace(c))
            token += c;
        else {
            if (token.length()) tokens.push_back(token);
            token.clear();
        }
    }

    if (token.length()) tokens.push_back(token);

    return tokens;
}

vector<pair<int, int>> get_bps(rna_tree &rna){

    vector<pair<int, int>> bps;
    auto extract_line =
            [&bps](rna_tree::pre_post_order_iterator it)
            {
                if (it->paired()) {
                    bps.push_back(make_pair(it->at(0).seq_ix, it->at(1).seq_ix));
                }

            };

    rna_tree::for_each_in_subtree(rna.begin(), extract_line);
    return bps;
}

json json_writer::get_classes() const {
    return json({
            {
                    {"name", "font"},
                    {"font-size", to_string(get_font_size()) + "px"},
                    {"font-weight", "bold"},
                    {"font-family", "Helvetica"}
            },
            {
                    {"name", "numbering-line"},
                    {"stroke-width", ceil(get_font_size() / 16 * 10000.0) / 10000.0},
                    {"stroke", "rgb(204, 204, 204)"}
            },
            {
                    {"name", "bp-line"},
                    {"stroke-width", ceil(get_font_size() / 8 * 10000.0) / 10000.0}
            }
    });
}

/***
 * The positions are a bit shifted due to the fact that the map_point of document writer works with dimensions
 * of the original image which actually might have had some padding.
 * @param json_sequence
 * @param json_labels
 */
void remove_margins(json &json_sequence, json &json_labels, const point &dim_min, const point &dim_max) {

    for (int i =0; i < json_sequence.size(); ++i){
        json &r = json_sequence[i];
        r["x"] = (double)r["x"] - dim_min.x;
        r["y"] = (double)r["y"] - dim_min.y;
    }
    for (int i =0; i < json_labels.size(); ++i){
        json &l = json_labels[i];
        l["labelContent"]["x"] = (double)l["labelContent"]["x"] - dim_min.x;
        l["labelContent"]["y"] = (double)l["labelContent"]["y"] - dim_min.y;
        l["labelLine"]["x1"] = (double)l["labelLine"]["x1"] - dim_min.x;
        l["labelLine"]["y1"] = (double)l["labelLine"]["y1"] - dim_min.y;
        l["labelLine"]["x2"] = (double)l["labelLine"]["x2"] - dim_min.x;
        l["labelLine"]["y2"] = (double)l["labelLine"]["y2"] - dim_min.y;
    }

}

void update_dim(point &dim_min, point &dim_max, const double x, const double y){
    if (x < dim_min.x) dim_min.x = x;
    if (y < dim_min.y) dim_min.y = y;
    if (x > dim_max.x) dim_max.x = x;
    if (y > dim_max.y) dim_max.y = y;
}

std::string json_writer::get_rna_subtree_formatted(
        rna_tree &rna,
        const numbering_def& numbering) const
{
    json structure = json::parse(R"({"rnaComplexes": [{"name": "complex","rnaMolecules": [{"name": "molecule","sequence": [],"basePairs": [], "labels":[] }]}]})");

    vector<point> residues_positions = get_residues_positions(rna);
    vector<pair<point, point>> lines = get_lines(rna);

    json json_sequence;
    json json_labels;
    int seq_ix = 0;

    //Write sequence and labels information
    point dim_min = point(point(numeric_limits<double>::max(), numeric_limits<double>::max()));
    point dim_max = point(point(numeric_limits<double>::min(), numeric_limits<double>::min()));

    auto jsonize =
            [&rna, &json_sequence, &json_labels, &seq_ix, &residues_positions, &lines, &numbering, &dim_min, &dim_max, this](rna_tree::pre_post_order_iterator it)
            {
                point p = map_point(it->at(it.label_index()).p, false);
                json residue;
                residue["residueIndex"] = seq_ix;
                residue["residueName"] = it->at(it.label_index()).label;
                residue["x"] = p.x;
                residue["y"] = p.y;
                residue["classes"] = {msprintf("text-%s", get_default_color(it->status).get_name()), "font"};

                json info;
                info["templateResidueName"] = it->at(it.label_index()).tmp_label;
                info["templateResidueIndex"] = it->at(it.label_index()).tmp_ix;
                info["templateNumberingLabel"] = it->at(it.label_index()).tmp_numbering_label;
                residue["info"] = info;

                json_sequence.push_back(residue);

                update_dim(dim_min, dim_max, p.x, p.y);

                labels_lines_def lld = create_numbering_formatted(it, seq_ix, rna.get_seq_distance_median(), residues_positions, lines, numbering);

                assert(lld.line_defs.size() == lld.label_defs.size());

                for (int i =0; i < lld.line_defs.size(); ++i) {
                    json label;
                    label_def label_def = lld.label_defs[i];
                    line_def line_def = lld.line_defs[i];
                    point p_from = map_point(line_def.from, false);
                    point p_to = map_point(line_def.to, false);
                    label["residueIndex"] = line_def.ix_to;
                    label["labelLine"] = {
                            {"x1", p_from.x},
                            {"y1", p_from.y},
                            {"x2", p_to.x},
                            {"y2", p_to.y},
                    };
                    label["labelLine"]["classes"] = json::array();
                    if (line_def.is_predicted) {
                        label["labelLine"]["classes"].push_back("predicted");
                    }
                    if (!line_def.clazz.empty()) {
                        for (const string& cls: split_clazz(line_def.clazz)){
                            label["labelLine"]["classes"].push_back(cls);
                        }
                    }
                    point p = map_point(label_def.label.p, false);
                    label["labelContent"] = {
                            {"label", label_def.label.label},
                            {"x", p.x},
                            {"y", p.y},
                            {"classes", {"font"}}
                    };
                    if (!label_def.clazz.empty()) {
                        for (const string& cls: split_clazz(label_def.clazz)){
                            label["labelContent"]["classes"].push_back(cls);
                        }
                    }
                    json_labels.push_back(label);

                    update_dim(dim_min, dim_max, p_from.x, p_from.y);
                    update_dim(dim_min, dim_max, p_to.x, p_to.y);
                    update_dim(dim_min, dim_max, p.x, p.y);
                }

                seq_ix++;
            };

    rna_tree::for_each_in_subtree(rna.begin_pre_post(), jsonize);

    remove_margins(json_sequence, json_labels, dim_min, dim_max);

    //Write base pairing information

    json json_bps;

    for (const pair<int, int> bp: get_bps(rna)) {
        if (bp.first == -1 || bp.second == -1) continue; //5' end
        json json_bp;
        json_bp["residueIndex1"] = bp.first + 1;
        json_bp["residueIndex2"] = bp.second + 1;
        json_bp["basePairType"] = "canonical";
        json_bp["classes"] = {"bp-line"};
        json_bps.push_back(json_bp);
    }

    json &json_mol = structure["rnaComplexes"][0]["rnaMolecules"][0];
    json_mol["sequence"] = json_sequence;
    json_mol["basePairs"] = json_bps;
    json_mol["labels"] = json_labels;

    structure["classes"] = this->get_classes();

    return structure.dump(2);
}

std::string json_writer::get_polyline_formatted(
        std::vector<point> &points,
        const RGB& color,
        shape_options opts) const {
    return "";
}

/* virtual */ std::string json_writer::get_circle_formatted(
        point centre,
        double radius,
        shape_options opts) const
{

    return "";
}

/* virtual */ std::string json_writer::get_label_formatted(
        const rna_label& label,
        const RGB& color,
        const rna_pair_label::status_type status,
        const label_info li,
        const shape_options opts) const
{
    return "";
}

/* virtual */ std::string json_writer::get_label_formatted(
        const rna_label& label,
        const std::string& clazz,
        const rna_pair_label::status_type status,
        const label_info li,
        const shape_options opts) const
{
    return "";
}


/* virtual */ std::string json_writer::get_line_formatted(
        point from,
        point to,
        int ix_from,
        int ix_to,
        bool is_base_pair,
        bool is_predicted,
        const RGB& color,
        const shape_options opts) const
{

    return "";
}

/* virtual */ std::string json_writer::get_line_formatted(
        point from,
        point to,
        int ix_from,
        int ix_to,
        bool is_base_pair,
        bool is_predicted,
        const std::string& clazz,
        const shape_options opts) const
{
    return "";
}




