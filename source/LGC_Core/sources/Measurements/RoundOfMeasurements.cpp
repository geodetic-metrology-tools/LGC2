// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RoundOfMeasurements.h"

#include <LGCAdjustablePoint.h>

#include "tree.hh"

int TECHOROM::romCounter_ = 0;
int TECSPROM::romCounter_ = 0;
int TECVEROM::romCounter_ = 0;
int TORIEROM::romCounter_ = 0;
int TINCLYROM::romCounter_ = 0;
int TROLLYROM::romCounter_ = 0;
int TECWSROM::romCounter_ = 0;
int TECWIROM::romCounter_ = 0;

void TECHOROM::initialiseObsSummaries()
{
	// First clear the old contents away
	echoSummary_.clear();

	// Add the residuals of each measurement and initialise the obsSummaries:
	if (measECHO.size() != 0)
	{
		for (auto const &ItECHOROM : measECHO)
			echoSummary_.addNewResidual(ItECHOROM.getDistanceResidual().getMMetresValue());

		echoSummary_.initialise();
	}
}

void TECSPROM::initialiseObsSummaries()
{
	// First clear the old contents away
	ecspSummary_.clear();

	// Add the residuals of each measurement and initialise the obsSummaries:
	if (measECSP.size() != 0)
	{
		for (auto const &ItECSPROM : measECSP)
			ecspSummary_.addNewResidual(ItECSPROM.getDistanceResidual().getMMetresValue());

		ecspSummary_.initialise();
	}
}

void TECVEROM::initialiseObsSummaries()
{
	// First clear the old contents away
	ecveSummary_.clear();

	// Add the residuals of each measurement and initialise the obsSummaries:
	if (measECVE.size() != 0)
	{
		for (auto const &ItECVEROM : measECVE)
			ecveSummary_.addNewResidual(ItECVEROM.getDistanceResidual().getMMetresValue());

		ecveSummary_.initialise();
	}
}

void TORIEROM::initialiseObsSummaries()
{
	// First clear the old contents away
	orieSummary_.clear();

	// Add the residuals of each measurement and initialise the obsSummaries:
	if (measORIE.size() != 0)
	{
		for (auto const &ItORIE : measORIE)
			orieSummary_.addNewResidual(ItORIE.getAngleResidual().getSignedCCValue());

		orieSummary_.initialise();
	}
}

void TINCLYROM::initialiseObsSummaries()
{
	// First clear the old contents away
	inclySummary_.clear();

	// Add the residuals of each measurement and initialise the obsSummaries:
	if (measINCLY.size() != 0)
	{
		for (auto const &ItINCLY : measINCLY)
		{
			inclySummary_.addNewResidual(ItINCLY.getAngleResidual().getSignedCCValue());
		}
		inclySummary_.initialise();
	}
}

/*
 * ROLLY ROM Observation Summaries Initializer
 *
 * Initializes and populates the observation summary statistics for ROLLY inclinometer
 * measurements by processing computed residuals from the least squares adjustment.
 * This function is a critical post-adjustment step that prepares statistical summaries
 * for quality assessment, reliability analysis, and output reporting.
 *
 * The function performs the following operations:
 * - Clears any existing summary data to ensure fresh statistics
 * - Processes each ROLLY measurement's computed residual
 * - Converts residuals to centesimal seconds [cc] for consistent unit representation
 * - Populates the observation summary with residual data
 * - Initializes statistical calculations for quality metrics
 *
 * @note This function is called after least squares adjustment completion and residual
 *       extraction to prepare comprehensive statistical summaries. The observation
 *       summaries provide essential data for:
 *       - Quality assessment and outlier detection
 *       - Reliability analysis and measurement validation
 *       - Statistical reporting and histogram generation
 *       - Performance evaluation of the adjustment process
 *
 * @note Residuals are converted to centesimal seconds (cc) for consistency with
 *       traditional surveying units and to match the precision requirements of
 *       quality assessment procedures. This unit conversion ensures compatibility
 *       with industry standards and reporting formats.
 */
void TROLLYROM::initialiseObsSummaries()
{
	// Clear any existing summary data to ensure fresh statistics
	// This prevents mixing of residuals from different adjustment runs
	rollySummary_.clear();

	// Process ROLLY measurements and populate observation summaries with residuals
	// Only proceed if there are actual measurements to process
	if (measROLLY.size() != 0)
	{
		// Iterate through each ROLLY measurement to collect residual data
		for (auto const &ItROLLY : measROLLY)
		{
			// Extract the computed residual for this measurement and convert to centesimal seconds
			// The residual represents the difference between observed and estimated values
			// Converting to cc units ensures consistency with surveying standards and precision
			rollySummary_.addNewResidual(ItROLLY.getAngleResidual().getSignedCCValue());
		}

		// Initialize statistical calculations for the collected residual data
		// This includes computing mean, standard deviation, quality indicators,
		// and preparing data structures for histogram generation and reliability analysis
		rollySummary_.initialise();
	}
}

void TECWSROM::initialiseObsSummaries()
{
	// First clear the old contents away
	ecwsSummary_.clear();

	// Add the residuals of each measurement and initialise the obsSummaries:
	if (measECWS.size() != 0)
	{
		for (auto const &ItECWSROM : measECWS)
			ecwsSummary_.addNewResidual(ItECWSROM.getDistanceResidual().getMMetresValue());

		ecwsSummary_.initialise();
	}
}

void TECWIROM::initialiseObsSummaries()
{
	// First clear the old contents away
	ecwiSummary_.xObsSum.clear();
	ecwiSummary_.zObsSum.clear();

	// Add the residuals of each measurement and initialise the obsSummaries:
	if (measECWI.size() != 0)
	{
		for (auto const &ItECWIROM : measECWI)
		{
			ecwiSummary_.xObsSum.addNewResidual(ItECWIROM.getDistanceResidual(EECWIDistances::kX).getMMetresValue());
			ecwiSummary_.zObsSum.addNewResidual(ItECWIROM.getDistanceResidual(EECWIDistances::kZ).getMMetresValue());
		}

		ecwiSummary_.xObsSum.initialise();
		ecwiSummary_.zObsSum.initialise();
	}
}

const TLGCObsSummary &TECHOROM::getECHOObsSummary() const
{
	return echoSummary_;
}

const TLGCObsSummary &TECHOROM::getECHOObsSummary(std::string text) noexcept
{
	echoSummary_.setObsText(text);
	return echoSummary_;
}

const TLGCObsSummary &TECSPROM::getECSPObsSummary() const
{
	return ecspSummary_;
}

const TLGCObsSummary &TECSPROM::getECSPObsSummary(std::string text) noexcept
{
	ecspSummary_.setObsText(text);
	return ecspSummary_;
}

const TLGCObsSummary &TECVEROM::getECVEObsSummary() const
{
	return ecveSummary_;
}

const TLGCObsSummary &TECVEROM::getECVEObsSummary(std::string text) noexcept
{
	ecveSummary_.setObsText(text);
	return ecveSummary_;
}

const TLGCObsSummary &TORIEROM::getORIEObsSummary() const
{
	return orieSummary_;
}

const TLGCObsSummary &TORIEROM::getORIEObsSummary(std::string text) noexcept
{
	orieSummary_.setObsText(text);
	return orieSummary_;
}

const TLGCObsSummary &TINCLYROM::getINCLYObsSummary() const
{
	return inclySummary_;
}

const TLGCObsSummary &TINCLYROM::getINCLYObsSummary(std::string text) noexcept
{
	inclySummary_.setObsText(text);
	return inclySummary_;
}

const TLGCObsSummary &TROLLYROM::getROLLYObsSummary() const
{
	return rollySummary_;
}

const TLGCObsSummary &TROLLYROM::getROLLYObsSummary(std::string text) noexcept
{
	rollySummary_.setObsText(text);
	return rollySummary_;
}

const TLGCObsSummary &TECWSROM::getECWSObsSummary() const
{
	return ecwsSummary_;
}

const TLGCObsSummary &TECWSROM::getECWSObsSummary(std::string text) noexcept
{
	ecwsSummary_.setObsText(text);
	return ecwsSummary_;
}

const TECWIObsSummary &TECWIROM::getECWIObsSummary() const
{
	return ecwiSummary_;
}

const TECWIObsSummary &TECWIROM::getECWIObsSummary(std::string text) noexcept
{
	ecwiSummary_.xObsSum.setObsText(text);
	ecwiSummary_.zObsSum.setObsText(text);
	return ecwiSummary_;
}

#if USE_SERIALIZER
// Inherited via Serializable

void TECHOROM::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("echoSummary_", echoSummary_);
	obj.addProperty("fMeasuredPlane", fMeasuredPlane);
	obj.addProperty("fReferencePoint", fReferencePoint);
	obj.addProperty("line", line);
	obj.addProperty("measECHO", measECHO);
	obj.addProperty("romId", romId);
}

void TECSPROM::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("ecspSummary_", ecspSummary_);
	obj.addProperty("line", line);
	obj.addProperty("measECSP", measECSP);
	obj.addProperty("p1", p1);
	obj.addProperty("p2", p2);
	obj.addProperty("romId", romId);
	obj.addProperty("romName", romName);
}

void TECVEROM::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("ecveSummary_", ecveSummary_);
	obj.addProperty("fMeasuredLine", fMeasuredLine);
	obj.addProperty("fPtLine", fPtLine);
	obj.addProperty("line", line);
	obj.addProperty("measECVE", measECVE);
	obj.addProperty("romId", romId);
}

void TORIEROM::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("fConstantAngle", fConstantAngle.getRadiansValue());
	obj.addProperty("instrument", instrument);
	if (instrumentPos)
		obj.addProperty("instrumentPos", instrumentPos->getName());
	obj.addProperty("line", line);
	obj.addProperty("measORIE", measORIE);
	obj.addProperty("orieSummary_", orieSummary_);
	obj.addProperty("romId", romId);
}

void TINCLYROM::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("inclySummary_", inclySummary_);
	obj.addProperty("instrument", instrument);
	obj.addProperty("line", line);
	obj.addProperty("measINCLY", measINCLY);
	obj.addProperty("romId", romId);
}

void TROLLYROM::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("rollySummary_", rollySummary_);
	obj.addProperty("instrument", instrument);
	obj.addProperty("line", line);
	obj.addProperty("measROLLY", measROLLY);
	obj.addProperty("romId", romId);
}

void TECWSROM::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("ecwsSummary_", ecwsSummary_);
	obj.addProperty("fMeasuredWSHeight", fMeasuredWSHeight);
	obj.addProperty("instrument", instrument);
	obj.addProperty("line", line);
	obj.addProperty("measECWS", measECWS);
	obj.addProperty("romId", romId);
	obj.addProperty("romName", romName);
	obj.addProperty("sigmaWS", sigmaWS.getMetresValue());
}

void TECWIROM::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("ecwiSummary_", ecwiSummary_);
	obj.addProperty("instrument", instrument);
	obj.addProperty("line", line);
	obj.addProperty("referencePoint", referencePoint);
	obj.addProperty("fWireDx", fWireDx);
	obj.addProperty("fWireDz", fWireDz);
	obj.addProperty("fWireBearing", fWireBearing);
	obj.addProperty("fWireSlope", fWireSlope);
	obj.addProperty("sagfix", sagfix);
	obj.addProperty("sagAdjustable", sagAdjustable);
	obj.addProperty("sigmaWire", sigmaWire.getMetresValue());
	obj.addProperty("anchorPtFirst", anchorPtFirst->getName());
	obj.addProperty("anchorPtSecond", anchorPtSecond->getName());
	obj.addProperty("romId", romId);
	obj.addProperty("romName", romName);
	obj.addProperty("measECWI", measECWI);
}
#endif
