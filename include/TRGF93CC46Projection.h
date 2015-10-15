#ifndef SURVEYLIB_TRGF93_PROJECTION
#define SURVEYLIB_TRGF93_PROJECTION

#include <TA2DPlus1MapProjection.h>
#include <string>

class TRGF93CC46Projection : public TA2DPlus1MapProjection
{
public:
    TRGF93CC46Projection(const std::string & name) : TA2DPlus1MapProjection(name) {}
};

#endif
