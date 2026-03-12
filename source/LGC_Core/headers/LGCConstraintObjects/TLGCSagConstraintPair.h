/*
� Copyright CERN 2000-2024. All rigths reserved. This software is released under a CERN proprietary software licence.
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
	// constructor taking two point names: reference and associated (sagged) point
	TLGCSagConstraintPair(std::string ref, std::string assoc, const LGCAdjustableSag &sag) : refPoint(ref), assocPoint(assoc), fSag(sag){};

	std::string refPoint;
	std::string assocPoint;
	const LGCAdjustableSag &fSag;

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

