#include "RoundOfMeasurements.h"
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