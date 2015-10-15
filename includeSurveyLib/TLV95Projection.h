#ifndef SURVEYLIB_TLV95_PROJECTION_fbf774f0_9fc2_4c4c_ae17_4c2a804516b8
#define SURVEYLIB_TLV95_PROJECTION_fbf774f0_9fc2_4c4c_ae17_4c2a804516b8

#include <TA2DPlus1MapProjection.h>
#include <string>

class TLV95Projection : public TA2DPlus1MapProjection
{
public:
    TLV95Projection(const std::string & name) : TA2DPlus1MapProjection(name) {}
};

#endif
