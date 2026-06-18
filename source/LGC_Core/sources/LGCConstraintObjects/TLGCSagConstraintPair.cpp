// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TLGCSagConstraintPair.h"

#include "LGCAdjustablePoint.h"
#include "TLGCData.h"

SagPairBaseFrame TLGCSagConstraintPair::transformToBaseFrame(const TLGCData &data) const
{
	const LGCAdjustablePoint &refPt = data.getPoints().getObject(refPoint);
	const LGCAdjustablePoint &assocPt = data.getPoints().getObject(assocPoint);
	TDataTreeIterator sagFramePos = data.locateNode(fSag.getBaseFrame());

	return SagPairBaseFrame{
		TLOR2LOR(refPt.getFrameTreePosition(), sagFramePos, "ref2Sag"),
		TLOR2LOR(assocPt.getFrameTreePosition(), sagFramePos, "assoc2Sag"),
		refPt.getEstimatedValue(),
		assocPt.getEstimatedValue()};
}

#if USE_SERIALIZER
void TLGCSagConstraintPair::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("refPoint", refPoint);
	obj.addProperty("assocPoint", assocPoint);
	obj.addProperty("sagElementName", fSag.getName());
	obj.addProperty("firstCIndex", firstCIndex);
	obj.addProperty("fDy", fDy);
	obj.addProperty("fXOffset", fXOffset);
	obj.addProperty("fZOffset", fZOffset);
}
#endif // USE_SERIALIZER
