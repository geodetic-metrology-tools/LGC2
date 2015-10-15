////////////////////////////////////////////////////////////////////
// TLSCalcObservationMaker
/*!
Creates the Least Squares calculation data set from the observations

Patterns:
 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/


#ifndef SU_LS_OBS_MAKER
#define SU_LS_OBS_MAKER

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000

#include "TLGCDataSet.h"
#include "LSCalcDataSet.h"

class TLSCalcObservationMaker {

public:

	TLSCalcObservationMaker() { }
	/*! Creates the LSCalc observations and LSCalc parameters from the Observation set and return a boolean */

	bool processData(const TLGCDataSet& data,LSCalcDataSet& lsobs);
	/*! Returns the LSCalc data set's error*/
	string	getError() const { return fError; }

private:
	/*! Copy constructor : not implemented */
	TLSCalcObservationMaker(const TLSCalcObservationMaker& source);
	/* Assignment operator: not implemented */
	TLSCalcObservationMaker& operator=(const TLSCalcObservationMaker& right);
	
	void processConstraints(const TLGCDataSet& data,LSCalcDataSet& lsobs);
	void processROMs(const TLGCDataSet& data,LSCalcDataSet& lsobs);
	void processOffsetToVerticalLineROMs(const TLGCDataSet& data,LSCalcDataSet& lsobs);
	void processOffsetToSpatialLineROMs(const TLGCDataSet& data,LSCalcDataSet& lsobs);
	void processOffsetToVerticalPlaneROMs(const TLGCDataSet& data,LSCalcDataSet& lsobs);
	void processEDMStations(const TLGCDataSet& data,LSCalcDataSet& lsobs);
	void processGyroscopeStations(const TLGCDataSet& data,LSCalcDataSet& lsobs);
	void processLevelStations(const TLGCDataSet& data,LSCalcDataSet& lsobs);
	void processTheodoliteStations(const TLGCDataSet& data,LSCalcDataSet& lsobs);
	void processTheoStationROMs(LSCalcDataSet& lsobs, const TheodoliteStation* theoStation, const LSLengthIter& lengthI, const LSPosVecIter& stPos, const LSOrientIter& cte);
	void processHAROMs(LSCalcDataSet& lsobs, const list<const HorizontalAngleROM*>& roms, const LSOrientIter& orientation, const LSPosVecIter& stPos, const LSOrientIter& cte, const TheodoliteStation* theoStation);
	void processZDROMs(LSCalcDataSet& lsobs, const list<const ZenithDistanceROM*>& roms, const LSLengthIter& lengthI, const LSPosVecIter& stPos, const TheodoliteStation* theoStation);
	void processSDROMs(LSCalcDataSet& lsobs, const list<const SpatialDistanceROM*>& roms, const LSLengthIter& lengthI, const LSPosVecIter& stPos, const TheodoliteStation* theoStation);
	void processHDROMs(LSCalcDataSet& lsobs, const list<const HorizontalDistanceROM*>& roms, const LSPosVecIter& stPos, const TheodoliteStation* theoStation);
	void processOTPROMs(LSCalcDataSet& lsobs, const list<const OffsetToTheodolitePlaneROM*>& roms, const LSOrientIter& orientation, const LSPosVecIter& stPos, const TheodoliteStation* theoStation);
	void processP3DROMs(LSCalcDataSet& lsobs, const list<const PolarROM*>& roms, const LSOrientIter& orientation, const LSLengthIter& lengthI, const LSPosVecIter& stPos, const LSOrientIter& cte, const TheodoliteStation* theoStation);
	void processVerticalDistanceROMs(const TLGCDataSet& data,LSCalcDataSet& lsobs);
	
		TPositionVector	get3DPosVec(const TSpatialPoint* pt) const;

	/*!@name Methods processing each type of observations */
	//@{

		/*! Processing of the radial offset constraint observations */
		void		processRadOffCnstrObs(const TLGCDataSet&, LSCalcDataSet&);
		/*! Processing of the radial offset constraint observations */
		void		processOrieCnstrObs(const TLGCDataSet&, LSCalcDataSet&);

	//@}
	
	/*!@name Methods converting the original parameters status to a struct LSParaStatus */ 
	//@{
		/*! Converts length's status to ls calc length's status */
		TALSCalcParameter::ELSStatus	convertLengthStatus(TAMeasurement::ECalcStatus) const;
		/*! Converts V0's status to ls calc orientation's status */
		struct LSParaStatus				convertV0Status(TAMeasurement::ECalcStatus ecs ) const;
		/*! Converts position vector's status to ls calc position vector's status */
		struct LSParaStatus				convertPVStatus(TSpatialStatus::ESpatialStatus ess) const;
	//@}

	
	//*!Attributes

	string				fError;

};

#endif //SU_LS_OBS_MAKER
