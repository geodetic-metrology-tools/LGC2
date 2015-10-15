#include <TLV03Transformation.h>
#include <TRefFrameInfo.h>

#include <iostream>
#include <sstream>



TLV03Transformation::TLV03Transformation(bool fFromLV95)
: fFromLV95(fFromLV95)
{
	std::cerr << "Warning! Using implementation stub for LV03 transformation!\n";
}

TLV03Transformation * TLV03Transformation::clone() const
{
	std::cerr << "Warning! Using implementation stub for LV03 transformation!\n";
    return new TLV03Transformation(*this);
}

TLV03Transformation * TLV03Transformation::inverse() const
{
	std::cerr << "Warning! Using implementation stub for LV03 transformation!\n";
    return new TLV03Transformation(!fFromLV95);
}

TAReferenceFrame * TLV03Transformation::getSourceFrame() const
{
	std::cerr << "Warning! Using implementation stub for LV03 transformation!\n";
    return TRefFrameInfo::getReferenceFrame(
        fFromLV95 ? TRefSystemFactory::kSwissLV95 : TRefSystemFactory::kSwissLV03);
}

TAReferenceFrame * TLV03Transformation::getDestinationFrame() const
{
	std::cerr << "Warning! Using implementation stub for LV03 transformation!\n";
    return TRefFrameInfo::getReferenceFrame(
        fFromLV95 ? TRefSystemFactory::kSwissLV03 : TRefSystemFactory::kSwissLV95);
}

bool TLV03Transformation::transform(TPositionVector&) const
{
	std::cerr << "Warning! Using implementation stub for LV03 transformation!\n";
    return false;
}

bool TLV03Transformation::computeSwissRefFrame(double & , double & , double & , int , int ) const
{
	std::cerr << "Warning! Using implementation stub for LV03 transformation!\n";
    return false;
}

