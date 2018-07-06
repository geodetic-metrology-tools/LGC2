/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef MEASUREMENTS_H_
#define MEASUREMENTS_H_


//LGC
#include <TEDM.h>
#include <TTSTN.h>
#include <TLEVEL.h>
#include <TCAM.h>
#include <RoundOfMeasurements.h>

/*!
	\ingroup Measurements
	The measurement class bundles all possible measurements together. Every node of the tree of local frames has one class of this type.
*/
struct TMeasurements {
	/// All total station measurements in the order in which they appeared in the input file
	std::list<std::shared_ptr<TTSTN>>  fTSTN;

	/// All electronic distance measurements in the order in which they appeared in the input file
	std::list<TEDM>   fEDM;

	/// All leveling measurements in the order in which they appeared in the input file
	std::list<TLEVEL> fLEVEL;

	/// All measurements made by cameras in the order in which they appeared in the input file
	std::list<TCAM> fCAM;

	/// All DVER measurements in the order in which they appeared in the input file
	std::list<TDVER> fDVER;

	/// All ORIE (and PDOR if used) measurements in the order in which they appeared in the input file
	std::list<TORIEROM> fORIE;

	/// All ECHO measurements in the order in which they appeared in the input file
	std::list<TECHOROM> fECHO;

	/// All ECVE measurements in the order in which they appeared in the input file
	std::list<TECVEROM> fECVE;

	/// All ECSP measurements in the order in which they appeared in the input file
	std::list<TECSPROM> fECSP;

	///pdor measurements in the order in which they appeared in the input file
	TPdorObs fPDOR;

	/// All RADI measurements in the order in which they appeared in the input file
	std::list<TRADI> fRADI;

    /// All OBSXYZ measurements in the order in which they appeared in the input file
    std::list<TOBSXYZ> fOBSXYZ;

    bool dverActive{ true }; ///< activation status of the DVER rom
    bool radiActive{ true }; ///< activation status of the RADI rom
    bool obsxyzActive{ true }; ///< activation status of the OBSXYZ rom

    /// Initialise all observation summaries
    void initialiseObsSummaries();

    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getDVERObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getRADIObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TOBSXYZObsSummary& getOBSXYZObsSummary() const;

    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TPOLARObsSummary& getPOLARGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getANGLGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getZENDGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getDISTGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getDHORGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getECTHGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getECDIRGlobalObsSummary() const;
    
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TUVDObsSummary& getUVDGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TUVECObsSummary& getUVECGlobalObsSummary() const;

    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getDSPTGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getDLEVGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getDlevDHORGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getORIEGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getECHOGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getECVEGlobalObsSummary() const;
    /// \note This function can be called only when the calculation is finished and the residuals of the observations are already filled.
    const TLGCObsSummary& getECSPGlobalObsSummary() const;

private:

    TLGCObsSummary dverSummary_;
    TLGCObsSummary radiSummary_;
    TOBSXYZObsSummary obsxyzSummary_;

    // Compound summaries:
    // TSTN:
    TPOLARObsSummary plrGlobalSummary_;
    TLGCObsSummary anglGlobalSummary_;
    TLGCObsSummary zendGlobalSummary_;
    TLGCObsSummary distGlobalSummary_;
    TLGCObsSummary dhorGlobalSummary_;
    TLGCObsSummary ecthGlobalSummary_;
    TLGCObsSummary ecdirGlobalSummary_;

    // CAM:
    TUVDObsSummary uvdGlobalSummary_;
    TUVECObsSummary uvecGlobalSummary_;

    // Other:
    TLGCObsSummary dsptGlobalSummary_;
    TLGCObsSummary dlevGlobalSummary_;
    TLGCObsSummary dlevDHORGlobalSummary_;
    TLGCObsSummary orieGlobalSummary_;
    TLGCObsSummary echoGlobalSummary_;
    TLGCObsSummary ecveGlobalSummary_;
    TLGCObsSummary ecspGlobalSummary_;

};

#endif // MEASUREMENTS_H_
