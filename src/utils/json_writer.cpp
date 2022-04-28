#include "json_writer.hpp"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

void json_writer::init(const string& filename, rna_tree& rna)
{
    document_writer::init(filename, ".json");
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
        const numbering_def& numbering) const
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

    auto jsonize =
            [&rna, &json_sequence, &json_labels, &seq_ix, &residues_positions, &lines, &numbering, this](rna_tree::pre_post_order_iterator it)
            {
                json residue;
                residue["residueIndex"] = seq_ix;
                residue["residueName"] = it->at(it.label_index()).label;
                residue["templateResidueName"] = it->at(it.label_index()).tmp_label;
                residue["templateResidueIndex"] = it->at(it.label_index()).tmp_ix;
                residue["templateNumberingLabel"] = it->at(it.label_index()).tmp_numbering_label;
                json_sequence.push_back(residue);

                labels_lines_def lld = create_numbering_formatted(it, seq_ix, rna.get_seq_distance_median(), residues_positions, lines, numbering);

                assert(lld.line_defs.size() == lld.label_defs.size());

                for (int i =0; i < lld.line_defs.size(); ++i) {
                    json label;
                    label_def label_def = lld.label_defs[i];
                    line_def line_def = lld.line_defs[i];
                    label["residueIndex"] = line_def.ix_to;
                    label["labelLine"] = {
                            {"x1", line_def.from.x},
                            {"y1", line_def.from.y},
                            {"x2", line_def.to.x},
                            {"y2", line_def.to.y},
                    };
                    label["labelLine"]["classes"] = json::array();
                    if (line_def.is_predicted) {
                        label["labelLine"]["classes"].push_back("predicted");
                    }
                    label["labelContent"] = {
                            {"label", label_def.label.label},
                            {"x", label_def.label.p.x},
                            {"y", label_def.label.p.y},
                            {"classes", json::array()}
                    };
                    if (!label_def.clazz.empty()) {
                        for (const string& cls: split_clazz(label_def.clazz)){
                            label["labelContent"]["classes"].push_back(cls);
                        }

                    }
                    json_labels.push_back(label);
                }

                seq_ix++;
            };

    rna_tree::for_each_in_subtree(rna.begin_pre_post(), jsonize);

    json &json_mol = structure["rnaComplexes"][0]["rnaMolecules"][0];
    json_mol["sequence"] = json_sequence;
    json_mol["labels"] = json_labels;

    return structure.dump(2);
}

/* virtual */ std::string json_writer::get_circle_formatted(
        point centre,
        double radius) const
{

    return "";
}

/* virtual */ std::string json_writer::get_label_formatted(
        const rna_label& label,
        const RGB& color,
        const rna_pair_label::status_type status,
        const label_info li) const
{
    return "";
}

/* virtual */ std::string json_writer::get_label_formatted(
        const rna_label& label,
        const std::string& clazz,
        const rna_pair_label::status_type status,
        const label_info li) const
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
        const RGB& color) const
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
        const std::string& clazz) const
{
    return "";
}


