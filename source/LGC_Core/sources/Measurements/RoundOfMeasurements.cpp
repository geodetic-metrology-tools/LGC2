#include "RoundOfMeasurements.h"

int TECHOROM::romCounter_ = 0;
int TECSPROM::romCounter_ = 0;
int TECVEROM::romCounter_ = 0;
int TORIEROM::romCounter_ = 0;

void TECHOROM::initialiseObsSummaries() {
    // First clear the old contents away
    echoSummary_.clear();

    // Add the residuals of each measurement:
    for(auto const& ItECHOROM : measECHO)
        echoSummary_.addNewResidual(ItECHOROM.getDistanceResidual().getMMetresValue());
}

void TECSPROM::initialiseObsSummaries() {
    // First clear the old contents away
    ecspSummary_.clear();

    // Add the residuals of each measurement:
    for(auto const& ItECSPROM : measECSP)
        ecspSummary_.addNewResidual(ItECSPROM.getDistanceResidual().getMMetresValue());
}

void TECVEROM::initialiseObsSummaries() {
    // First clear the old contents away
    ecveSummary_.clear();

    // Add the residuals of each measurement:
    for(auto const& ItECVEROM : measECVE)
        ecveSummary_.addNewResidual(ItECVEROM.getDistanceResidual().getMMetresValue());
}

void TORIEROM::initialiseObsSummaries() {
    // First clear the old contents away
    orieSummary_.clear();

    // Add the residuals of each measurement:
    for(auto const& ItORIE : measORIE)
        orieSummary_.addNewResidual(ItORIE.getAngleResidual().getSignedCCValue());
}

TLGCObsSummary  TECHOROM::getECHOObsSummary() const { return echoSummary_; }

TLGCObsSummary  TECSPROM::getECSPObsSummary() const { return ecspSummary_; }

TLGCObsSummary  TECVEROM::getECVEObsSummary() const { return ecveSummary_; }

TLGCObsSummary  TORIEROM::getORIEObsSummary() const { return orieSummary_; }

