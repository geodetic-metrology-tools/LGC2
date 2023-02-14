#include "RoundOfMeasurements.h"
#include <LGCAdjustablePoint.h>
#include "tree.h"

int TECHOROM::romCounter_ = 0;
int TECSPROM::romCounter_ = 0;
int TECVEROM::romCounter_ = 0;
int TORIEROM::romCounter_ = 0;
int TINCLYROM::romCounter_ = 0;
int TECWSROM::romCounter_ = 0;

void TECHOROM::initialiseObsSummaries() {
    // First clear the old contents away
    echoSummary_.clear();

    // Add the residuals of each measurement and initialise the obsSummaries:
    if(measECHO.size() != 0) {
        for(auto const& ItECHOROM : measECHO)
            echoSummary_.addNewResidual(ItECHOROM.getDistanceResidual().getMMetresValue());

        echoSummary_.initialise();
    }
}

void TECSPROM::initialiseObsSummaries() {
    // First clear the old contents away
    ecspSummary_.clear();

    // Add the residuals of each measurement and initialise the obsSummaries:
    if(measECSP.size() != 0) {
        for(auto const& ItECSPROM : measECSP)
            ecspSummary_.addNewResidual(ItECSPROM.getDistanceResidual().getMMetresValue());

        ecspSummary_.initialise();
    }
}

void TECVEROM::initialiseObsSummaries() {
    // First clear the old contents away
    ecveSummary_.clear();

    // Add the residuals of each measurement and initialise the obsSummaries:
    if(measECVE.size() != 0) {
        for(auto const& ItECVEROM : measECVE)
            ecveSummary_.addNewResidual(ItECVEROM.getDistanceResidual().getMMetresValue());

        ecveSummary_.initialise();
    }
}

void TORIEROM::initialiseObsSummaries() {
    // First clear the old contents away
    orieSummary_.clear();

    // Add the residuals of each measurement and initialise the obsSummaries:
    if(measORIE.size() != 0) {
        for(auto const& ItORIE : measORIE)
            orieSummary_.addNewResidual(ItORIE.getAngleResidual().getSignedCCValue());

        orieSummary_.initialise();
    }
}

void TINCLYROM::initialiseObsSummaries() {
	// First clear the old contents away
	inclySummary_.clear();

	// Add the residuals of each measurement and initialise the obsSummaries:
	if (measINCLY.size() != 0) {
		for (auto const& ItINCLY : measINCLY) {
			inclySummary_.addNewResidual(ItINCLY.getAngleResidual().getSignedCCValue());
		}
		inclySummary_.initialise();
	}
}

void TECWSROM::initialiseObsSummaries() {
    // First clear the old contents away
    ecwsSummary_.clear();

    // Add the residuals of each measurement and initialise the obsSummaries:
    if (measECWS.size() != 0) {
        for (auto const& ItECWSROM : measECWS)
            ecwsSummary_.addNewResidual(ItECWSROM.getDistanceResidual().getMMetresValue());

        ecwsSummary_.initialise();
    }
}

const TLGCObsSummary&  TECHOROM::getECHOObsSummary() const { return echoSummary_; }

const TLGCObsSummary& TECHOROM::getECHOObsSummary(std::string text)  noexcept {
	echoSummary_.setObsText(text);
	return echoSummary_;
}

const TLGCObsSummary&  TECSPROM::getECSPObsSummary() const { return ecspSummary_; }

const TLGCObsSummary& TECSPROM::getECSPObsSummary(std::string text) noexcept {
	ecspSummary_.setObsText(text);
	return ecspSummary_;
}

const TLGCObsSummary&  TECVEROM::getECVEObsSummary() const { return ecveSummary_; }

const TLGCObsSummary& TECVEROM::getECVEObsSummary(std::string text) noexcept {
	ecveSummary_.setObsText(text);
	return ecveSummary_;
}

const TLGCObsSummary&  TORIEROM::getORIEObsSummary() const { return orieSummary_; }

const TLGCObsSummary& TORIEROM::getORIEObsSummary(std::string text) noexcept {
	orieSummary_.setObsText(text);
	return orieSummary_;
}

const TLGCObsSummary& TINCLYROM::getINCLYObsSummary() const { return inclySummary_; }

const TLGCObsSummary& TINCLYROM::getINCLYObsSummary(std::string text) noexcept {
	inclySummary_.setObsText(text);
	return inclySummary_; 
}

const TLGCObsSummary& TECWSROM::getECWSObsSummary() const { return ecwsSummary_; }

const TLGCObsSummary& TECWSROM::getECWSObsSummary(std::string text)  noexcept {
    ecwsSummary_.setObsText(text);
    return ecwsSummary_;
}

#ifdef USE_SERIALIZER
// Inherited via Serializable

void TECHOROM::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("echoSummary_", echoSummary_);
	obj.addProperty("fMeasuredPlane", fMeasuredPlane);
	obj.addProperty("line", line);
	obj.addProperty("measECHO", measECHO);
	obj.addProperty("romCounter_", romCounter_);
	obj.addProperty("romId", romId);
}

void TECSPROM::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("ecspSummary_", ecspSummary_);
	obj.addProperty("line", line);
	obj.addProperty("line", measECSP);
	obj.addProperty("p1", p1);
	obj.addProperty("p2", p2);
	obj.addProperty("romCounter_", romCounter_);
	obj.addProperty("romId", romId);
	obj.addProperty("romName", romName);
}

void TECVEROM::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("ecveSummary_", ecveSummary_);
	obj.addProperty("fMeasuredLine", fMeasuredLine);
	obj.addProperty("fPtLine", fPtLine);
	obj.addProperty("line", line);
	obj.addProperty("measECVE", measECVE);
	obj.addProperty("romCounter_", romCounter_);
	obj.addProperty("romId", romId);
}

void TORIEROM::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("fConstantAngle", fConstantAngle.getRadiansValue());
	obj.addProperty("instrument", instrument);
	obj.addProperty("instrumentPos", instrumentPos);
	obj.addProperty("line", line);
	obj.addProperty("measORIE", measORIE);
	obj.addProperty("orieSummary_", orieSummary_);
	obj.addProperty("romCounter_", romCounter_);
	obj.addProperty("romId", romId);
}

void TINCLYROM::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("fConstantAngle", fConstantAngle.getRadiansValue());
	obj.addProperty("inclySummary_", inclySummary_);
	obj.addProperty("instrument", instrument);
	obj.addProperty("line", line);
	obj.addProperty("measINCLY", measINCLY);
	//obj.addProperty("positionInTree", positionInTree);
	obj.addProperty("romCounter_", romCounter_);
	obj.addProperty("romId", romId);
}

void TECWSROM::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("ecwsSummary_", ecwsSummary_);
	obj.addProperty("fMeasuredWSHeight", fMeasuredWSHeight);
	obj.addProperty("instrument", instrument);
	obj.addProperty("line", line);
	obj.addProperty("measECWS", measECWS);
	obj.addProperty("romCounter_", romCounter_);
	obj.addProperty("romId", romId);
	obj.addProperty("romName", romName);
	obj.addProperty("sigmaWS", sigmaWS.getMetresValue());
}
#endif