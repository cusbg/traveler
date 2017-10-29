#ifndef our_extractor_hpp
#define our_extractor_hpp

#include "extractor.hpp"

class our_extractor: public extractor
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
