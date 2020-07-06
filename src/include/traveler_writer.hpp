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
                      rna_tree& rna);
    virtual streampos print(
                            const std::string& text);
    
public: // formatters
    virtual std::string get_circle_formatted(
                                             point centre,
                                             double radius) const;
    virtual std::string get_label_formatted(
                                            const rna_label& label,
                                            const RGB& color,
                                            const label_info li) const;
    virtual std::string get_label_formatted(
            const rna_label& label,
            const std::string& clazz,
            const label_info li) const;
    
protected:
    virtual std::string get_line_formatted(
                                           point from,
                                           point to,
                                           const RGB& color) const;
    virtual std::string get_line_formatted(
            point from,
            point to,
            const std::string& clazz) const;
        

};


#endif
