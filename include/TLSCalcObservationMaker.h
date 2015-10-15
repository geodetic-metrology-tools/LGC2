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

#include "ObservationSet.h"
#include "LSCalcDataSet.h"


class TLSCalcObservationMaker {

public:

	/*!@name Constructors / Destructor*/
	//@{
	/*! Default constructor */
	TLSCalcObservationMaker();
	TLSCalcObservationMaker(bool isSimulation);
	//!Destructor
	virtual ~TLSCalcObservationMaker();
	//@}

	/*! Creates the LSCalc observations and LSCalc parameters from the Observation set and return a boolean */
	bool		processData(const ObservationSet&, LSCalcDataSet&);
	
	/*! Returns the LSCalc data set's error*/
	string	getError() const { return fError; }

private:
	/*! Copy constructor : not implemented */
	TLSCalcObservationMaker(const TLSCalcObservationMaker& source);
	/* Assignment operator: not implemented */
	TLSCalcObservationMaker& operator=(const TLSCalcObservationMaker& right);

	/*!@name Methods processing each type of observations */
	//@{
		/*! Processing of the horizontal angle observations */
		void		processHorAngleObs(const ObservationSet&,LSCalcDataSet&);
		/*! Processing of the zenithal angle observations */
		void		processZenDistObs(const ObservationSet&,LSCalcDataSet&);
		/*! Processing of the spatial dist observations */
		void		processSpaDistObs(const ObservationSet&,LSCalcDataSet&);
		/*! Processing of the horizontal dist observations */
		void		processHorDistObs(const ObservationSet&,LSCalcDataSet&);
		/*! Processing of the vertical dist observations */
		void		processVertDistObs(const ObservationSet&,LSCalcDataSet&,bool isDLEV);

		/*! Processing of the offset to vertical line observations */
		void		processOffsetToVerLineObs(const ObservationSet&, LSCalcDataSet&);
		/*! Processing of the offset to spatial line observations */
		void		processOffsetToSpaLineObs(const ObservationSet&, LSCalcDataSet&);
		/*! Processing of the offset to vertical plane observations */
		void		processOffsetToVerPlaneObs(const ObservationSet&, LSCalcDataSet&);
		/*! Processing of the offset to theodolite plane observations */
		void		processOffsetToTheoPlaneObs(const ObservationSet&, LSCalcDataSet&);

		/*! Processing of the gyro. orientation observations */
		void		processGyroOrieObs(const ObservationSet&, LSCalcDataSet&);

		/*! Processing of the radial offset constraint observations */
		void		processRadOffCnstrObs(const ObservationSet&, LSCalcDataSet&);
		/*! Processing of the radial offset constraint observations */
		void		processOrieCnstrObs(const ObservationSet&, LSCalcDataSet&);

	//@}
	
	/*!@name Methods converting the original parameters status to a struct LSParaStatus */ 
	//@{
		/*! Converts length's status to ls calc length's status */
		TALSCalcParameter::ELSStatus	convertLengthStatus(TAMeasurement::ECalcStatus);
		/*! Converts V0's status to ls calc orientation's status */
		struct LSParaStatus				convertV0Status(TAMeasurement::ECalcStatus ecs );
		/*! Converts position vector's status to ls calc position vector's status */
		struct LSParaStatus				convertPVStatus(TSpatialStatus::ESpatialStatus ess);
	//@}

	
	//*!Attributes

	string				fError;

};

#endif //SU_LS_OBS_MAKER
