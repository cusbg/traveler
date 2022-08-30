#ifndef traveler_writer_hpp
#define traveler_writer_hpp

#include "document_writer.hpp"

class traveler_writer : public document_writer
{
private:
    struct properties;
    struct style;
    
public:
    virtual ~traveler_writer() = default;
    
public:
    virtual void init(
                      const std::string& filename,
                      rna_tree& rna,
                      bool labels_template);
    virtual streampos print(
                            const std::string& text);
    
public: // formatters
    virtual std::string get_circle_formatted(
                                             point centre,
                                             double radius) const;
    virtual std::string get_label_formatted(
                                            const rna_label& label,
                                            const RGB& color,
                                            const rna_pair_label::status_type status,
                                            const label_info li) const;
    virtual std::string get_label_formatted(
            const rna_label& label,
            const std::string& clazz,
            const rna_pair_label::status_type status,
            const label_info li) const;
    
protected:
    virtual std::string get_line_formatted(
                                           point from,
                                           point to,
                                           int ix_from,
                                           int ix_to,
                                           bool is_base_pair,
                                           bool is_predicted,
                                           const RGB& color) const;
    virtual std::string get_line_formatted(
            point from,
            point to,
            int ix_from,
            int ix_to,
            bool is_base_pair,
            bool is_predicted,
            const std::string& clazz) const;
        

};


#endif
