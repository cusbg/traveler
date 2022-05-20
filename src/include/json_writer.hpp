#ifndef json_writer_hpp
#define json_writer_hpp

#include "document_writer.hpp"
#include "json.hpp"

class json_writer : public document_writer
{
private:
    struct properties;
    struct style;
    
public:
    virtual ~json_writer() = default;
    
public:
    void init(
                      const std::string& filename,
                      rna_tree& rna) override;
    streampos print(
                            const std::string& text) override;

    std::string get_rna_formatted(
            rna_tree rna,
            const numbering_def& numbering) const override;
    
public: // formatters
    std::string get_circle_formatted(
                                             point centre,
                                             double radius) const override;
    std::string get_label_formatted(
                                            const rna_label& label,
                                            const RGB& color,
                                            const rna_pair_label::status_type status,
                                            const label_info li) const override;
    std::string get_label_formatted(
            const rna_label& label,
            const std::string& clazz,
            const rna_pair_label::status_type status,
            const label_info li) const override;
    
protected:
    std::string get_line_formatted(
                                           point from,
                                           point to,
                                           int ix_from,
                                           int ix_to,
                                           bool is_base_pair,
                                           bool is_predicted,
                                           const RGB& color) const override;
    virtual std::string get_line_formatted(
            point from,
            point to,
            int ix_from,
            int ix_to,
            bool is_base_pair,
            bool is_predicted,
            const std::string& clazz) const;


    std::string get_rna_subtree_formatted(rna_tree &rna, const numbering_def &numbering) const;

private:
    nlohmann::json get_classes() const;
};


#endif