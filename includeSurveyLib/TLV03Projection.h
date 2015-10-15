#ifndef SURVEYLIB_TLV03_PROJECTION_3d99526b_0d8f_42a8_865a_32180bf2365c
#define SURVEYLIB_TLV03_PROJECTION_3d99526b_0d8f_42a8_865a_32180bf2365c

#include <TA2DPlus1MapProjection.h>
#include <string>

class TLV03Projection : public TA2DPlus1MapProjection
{
public:
    TLV03Projection(const std::string & name) : TA2DPlus1MapProjection(name) {}
};

#endif
