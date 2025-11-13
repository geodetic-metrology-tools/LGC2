// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TLEVEL.h"

#include <LGCAdjustablePoint.h>

int TLEVEL::stnCounter_ = 0;

void TLEVEL::initialiseObsSummaries()
{
	// First clear the old contents away
	dlevSummary_.clear();
	dlevDhorSummary_.clear();

	if (!measDLEV.empty())
	{
		// Add the residuals of each measurement:
		for (auto const &ItDLEV : measDLEV)
		{
			dlevSummary_.addNewResidual(ItDLEV.getDistanceResidual().getMMetresValue());
			if (ItDLEV.dhor)
				dlevDhorSummary_.addNewResidual(ItDLEV.dhor->getDistanceResidual().getMMetresValue());
		}

		// Initialise the obsSummaries:
		dlevSummary_.initialise();
		if (dlevDhorSummary_.getNumberOfObs() != 0)
			dlevDhorSummary_.initialise();
	}
}

const TLGCObsSummary &TLEVEL::getDLEVObsSummary() const
{
	return dlevSummary_;
}

const TLGCObsSummary &TLEVEL::getDLEVObsSummary(std::string text) noexcept
{
	dlevSummary_.setObsText(text);
	return dlevSummary_;
}

const TLGCObsSummary &TLEVEL::getDHORObsSummary() const
{
	return dlevDhorSummary_;
}

const TLGCObsSummary &TLEVEL::getDHORObsSummary(std::string text) noexcept
{
	dlevDhorSummary_.setObsText(text);
	return dlevDhorSummary_;
}

#if USE_SERIALIZER
// Inherited via Serializable
void TLEVEL::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("dlevDhorSummary_", dlevDhorSummary_);
	obj.addProperty("dlevSummary_", dlevSummary_);
	obj.addProperty("fMeasuredPlane", fMeasuredPlane);
	obj.addProperty("fRefPt", fRefPt);
	obj.addProperty("hasDHOR", hasDHOR);
	obj.addProperty("nbDHOR", nbDHOR);
	obj.addProperty("instrument", instrument);
	obj.addProperty("line", line);
	obj.addProperty("measDLEV", measDLEV);
	obj.addProperty("stnId", stnId);
}
#endif
