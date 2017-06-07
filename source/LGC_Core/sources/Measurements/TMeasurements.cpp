
#include "TMeasurements.h"

void TMeasurements::initialiseObsSummaries() {
    // First clear the old contents away
    dverSummary_.clear();
    radiSummary_.clear();

    // Add the residuals of each measurement:

    // DVER
    for(auto &dver : fDVER)
        dverSummary_.addNewResidual(dver.getDistanceResidual().getMMetresValue());

    // RADI
    for(auto &radi : fRADI)
        radiSummary_.addNewResidual(radi.getResidual().getMMetresValue());

    // Initialise the residual summaries of
    // measurements that use an instrument:

    // TSTN
    for(auto &tstn : fTSTN)
        for(auto &rom : tstn->roms)
            rom->initialiseObsSummaries();

    // CAM
    for(auto &cam : fCAM)
        cam.initialiseObsSummaries();

    // DSPT
    for(auto &edm : fEDM)
        edm.initialiseObsSummaries();

    // DLEV
    for(auto &level : fLEVEL)
        level.initialiseObsSummaries();

    // ORIE
    for(auto &orierom : fORIE)
        orierom.initialiseObsSummaries();

    // ECHO
    for(auto &echorom : fECHO)
        echorom.initialiseObsSummaries();

    // ECVE
    for(auto &ecverom : fECVE)
        ecverom.initialiseObsSummaries();

    // ECSP
    for(auto &ecsprom : fECSP)
        ecsprom.initialiseObsSummaries();
}

TLGCObsSummary  TMeasurements::getDVERObsSummary() const { return dverSummary_; }

TLGCObsSummary  TMeasurements::getRADIObsSummary() const { return radiSummary_; }


