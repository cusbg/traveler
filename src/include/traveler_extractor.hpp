#ifndef traveler_extractor_hpp
#define traveler_extractor_hpp

#include "extractor.hpp"

class traveler_extractor: public extractor
{
protected:
    virtual void extract(
                         const std::string& filename);
    virtual std::string get_type() const
    {
        return "traveler";
    }
};


#endif
