#ifndef SURVEYLIB_TLV03TRANSFORMATION_95abd36b_d26e_463b_9931_6d0d3d756fab
#define SURVEYLIB_TLV03TRANSFORMATION_95abd36b_d26e_463b_9931_6d0d3d756fab


#include  <TARefFrameTransformation.h>
#include  <TSpatialPosition.h>

class TLV03Transformation : public TARefFrameTransformation
{
public:
    explicit TLV03Transformation(bool fFromLV95);

	TLV03Transformation * clone() const;
	TLV03Transformation * inverse() const;

    TAReferenceFrame * getSourceFrame() const;
	TAReferenceFrame * getDestinationFrame() const;

    bool transform( TPositionVector & pv) const;

private:
	TLV03Transformation & operator=(const TLV03Transformation &);  // non copyable
    bool computeSwissRefFrame(double & coordinate_x, double & coordinate_y, double & coordinate_z, int reframe_in, int reframe_out) const;

    const bool fFromLV95;

};

#endif
