/*
© Copyright CERN 2000-2024. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/
#ifndef _TLGC_SAGCONSTRAINTPAIR_H
#	define _TLGC_SAGCONSTRAINTPAIR_H
#include <algorithm>
#include <iostream>
#include <list>

#include <LGCAdjustableSag.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER



#	if USE_SERIALIZER
class TLGCSagConstraintPair : public Serializable
#	else
class TLGCSagConstraintPair
#	endif // USE_SERIALIZER
{
public:
	// constructor
	// taking two point names
	TLGCSagConstraintPair(std::string ref, std::string assoc, const LGCAdjustableSag &sag) : refPoint(ref), assocPoint(assoc), fSag(sag){};
	// taking only one point name: in this case, a constraint will be added that identifies point with the sagged version of its provisional coordinates
	TLGCSagConstraintPair(std::string pointName, const LGCAdjustableSag &sag) : assocPoint(pointName), fSag(sag)
	{
		isAssociatedToProvisionalCoordinates = true;
		refPoint = pointName + "_provisional";
	};

	//everything public to begin with
	std::string refPoint;
	std::string assocPoint;
	const LGCAdjustableSag &fSag;
	// tells that this is a sag constraint pair between the point coordinates and the provisional coordinates of this point
	bool isAssociatedToProvisionalCoordinates{false};

	int firstCIndex{-1};
	// constraint dimension is always 3
	int constraintDim{3};

#	if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#	endif

//private:
};

#endif

