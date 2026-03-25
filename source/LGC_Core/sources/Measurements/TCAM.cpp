// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TCAM.h"

#include <LGCAdjustablePoint.h>

int TCAM::stnCounter_ = 0;

void TCAM::initialiseObsSummaries()
{
	// First clear the old contents away
	uvdSummary_.xVectorCompObsSum.clear();
	uvdSummary_.yVectorCompObsSum.clear();
	uvdSummary_.distObsSum.clear();
	uvecSummary_.xVectorCompObsSum.clear();
	uvecSummary_.yVectorCompObsSum.clear();

	// Add the residuals of each measurement and initialise the obs summaries:

	// UVD
	if (measUVD.size() != 0)
	{
		for (std::list<TUVD>::const_iterator ItUVD = measUVD.begin(); ItUVD != measUVD.end(); ItUVD++)
		{ // auto const& ItUVD: measUVD){
			uvdSummary_.xVectorCompObsSum.addNewResidual(ItUVD->getXCompVectorResidual() * LGC::VECCONVINV);
			uvdSummary_.yVectorCompObsSum.addNewResidual(ItUVD->getYCompVectorResidual() * LGC::VECCONVINV);
			uvdSummary_.distObsSum.addNewResidual(ItUVD->getDistanceResidual().getMMetresValue());
		}

		uvdSummary_.xVectorCompObsSum.initialise();
		uvdSummary_.yVectorCompObsSum.initialise();
		uvdSummary_.distObsSum.initialise();
	}

	// UVEC
	if (measUVEC.size() != 0)
	{
		for (std::list<TUVEC>::const_iterator ItUVEC = measUVEC.begin(); ItUVEC != measUVEC.end(); ItUVEC++)
		{ // auto const& ItUVEC: measUVEC){
			uvecSummary_.xVectorCompObsSum.addNewResidual(ItUVEC->getXCompVectorResidual() * LGC::VECCONVINV);
			uvecSummary_.yVectorCompObsSum.addNewResidual(ItUVEC->getYCompVectorResidual() * LGC::VECCONVINV);
		}

		uvecSummary_.xVectorCompObsSum.initialise();
		uvecSummary_.yVectorCompObsSum.initialise();
	}
}

const TUVDObsSummary &TCAM::getUVDObsSummary() const
{
	return uvdSummary_;
}

const TUVDObsSummary &TCAM::getUVDObsSummary(std::string text) noexcept
{
	uvdSummary_.xVectorCompObsSum.setObsText(text);
	uvdSummary_.yVectorCompObsSum.setObsText(text);
	uvdSummary_.distObsSum.setObsText(text);
	return uvdSummary_;
}

const TUVECObsSummary &TCAM::getUVECObsSummary() const
{
	return uvecSummary_;
}

const TUVECObsSummary &TCAM::getUVECObsSummary(std::string text) noexcept
{
	uvecSummary_.xVectorCompObsSum.setObsText(text);
	uvecSummary_.yVectorCompObsSum.setObsText(text);
	return uvecSummary_;
}

#if USE_SERIALIZER
// Inherited via Serializable
void TCAM::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("instrument", instrument);
	if (instrumentPos)
		obj.addProperty("instrumentPos", instrumentPos->getName());
	obj.addProperty("line", line);
	obj.addProperty("measUVD", measUVD);
	obj.addProperty("measUVEC", measUVEC);
	obj.addProperty("stnCounter_", stnCounter_);
	obj.addProperty("stnId", stnId);
	obj.addProperty("uvdActive", uvdActive);
	obj.addProperty("uvdSummary_", uvdSummary_);
	obj.addProperty("uvecActive", uvecActive);
	obj.addProperty("uvecSummary_", uvecSummary_);
}
#endif
