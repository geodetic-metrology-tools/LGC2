/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _TLGC_SAGCONSTRAINTPAIR_H
#	define _TLGC_SAGCONSTRAINTPAIR_H

#include <LGCAdjustableSag.h>
#include <TLOR2LOR.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER

class TLGCData;

/*!
	\brief Reference and associated point coordinates of a sag pair, expressed
	both in their own subframes and in the sag element's base frame, together
	with the LOR-to-LOR transforms used to obtain the latter.

	The transforms are kept so callers that need partial derivatives (the
	contributions generator) can reuse them instead of rebuilding them.
*/
struct SagPairBaseFrame
{
	SagPairBaseFrame(TLOR2LOR r2s, TLOR2LOR a2s,
		TPositionVector refPos, TPositionVector assocPos) :
		ref2Sag(std::move(r2s)), assoc2Sag(std::move(a2s)),
		refSub(refPos), assocSub(assocPos),
		refSag(refPos), assocSag(assocPos)
	{
		ref2Sag.transform(refSag);
		assoc2Sag.transform(assocSag);
	}

	TLOR2LOR ref2Sag;
	TLOR2LOR assoc2Sag;
	TPositionVector refSub;
	TPositionVector assocSub;
	TPositionVector refSag;
	TPositionVector assocSag;
};

#	if USE_SERIALIZER
class TLGCSagConstraintPair : public Serializable
#	else
class TLGCSagConstraintPair
#	endif // USE_SERIALIZER
{
public:
	// constructor taking two point names: reference and associated (sagged) point
	TLGCSagConstraintPair(std::string ref, std::string assoc, const LGCAdjustableSag &sag) :
		refPoint(std::move(ref)), assocPoint(std::move(assoc)), fSag(sag){};

	/*!
		\brief Look up the two points and apply the ref->sag and assoc->sag
		transforms, returning both the transforms and the four position vectors.
	*/
	SagPairBaseFrame transformToBaseFrame(const TLGCData &data) const;

	// reference / associated point names and the sag element
	const std::string &getRefPoint() const { return refPoint; }
	const std::string &getAssocPoint() const { return assocPoint; }
	const LGCAdjustableSag &getSag() const { return fSag; }

	// indices for the constraints (same API as TLGCFrameConstraintGroup / TLGCPointConstraintGroup)
	int getConstraintDimension() const { return kConstraintDim; }
	int getFirstCIndex() const { return firstCIndex; }
	void setFirstCIndex(int j) { firstCIndex = j; }

	// post-LS cached base-frame view, populated by TLSResultsMatricesExtractor
	const TLength &getDy() const { return fDy; }
	const TLength &getXOffset() const { return fXOffset; }
	const TLength &getZOffset() const { return fZOffset; }
	void setDy(const TLength &dy) { fDy = dy; }
	void setXOffset(const TLength &xOffset) { fXOffset = xOffset; }
	void setZOffset(const TLength &zOffset) { fZOffset = zOffset; }

#	if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#	endif

private:
	std::string refPoint;
	std::string assocPoint;
	const LGCAdjustableSag &fSag;

	int firstCIndex{-1};
	// the constraint refSag + offset - assocSag = 0 is structurally a 3-vector equation
	static constexpr int kConstraintDim = 3;

	// Post-LS cached base-frame view, populated by TLSResultsMatricesExtractor.
	TLength fDy;
	TLength fXOffset;
	TLength fZOffset;
};

#endif
