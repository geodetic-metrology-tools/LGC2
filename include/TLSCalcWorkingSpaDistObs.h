////////////////////////////////////////////////////////////////////
// TLSCalcWorkingSpaDistObs.h
/*!
Container for the least squares calculation spatial distance observations

Patterns:
 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/

#ifndef SU_LS_WORK_SPADIST_OBS
#define SU_LS_WORK_SPADIST_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TLSCalcSpatialDistObservation.h"
#include "TLSCalcWorkingObservation.h"
#include "TAMeasurement.h"
#include "TLGCObsSummary.h"

#include <list>
using namespace std;

class TLSCalcWorkingSpaDistObs : public TLSCalcWorkingObservation<TLSCalcSpatialDistObservation> {

public:

	/*!@name Constructor / Destructor */
	//@{
		/*! Default constructor */
		TLSCalcWorkingSpaDistObs();

		/*! Destructor */
		~TLSCalcWorkingSpaDistObs();
	//@}
		
		/*! Returns the number of spatial distance measurements where the instrument height is fixed */
		int					numDMES() const {return fNumDMES;}
		/*! Returns the number of spatial distance measurements where the instrument height is unknown */
		int					numDTHE() const {return fNumDTHE;}
		/*! Returns the observations' summary statistics here the instrument height is fixed */
		TLGCObsSummary	getDMESObsSummary();
		/*! Returns the observations' summary statistics here the instrument height is unknown */
		TLGCObsSummary	getDTHEObsSummary();

		list<TLSCalcSpatialDistObservation>::iterator push_back(const TLSCalcSpatialDistObservation& sdObs)
		{
			if(sdObs.getHInstStatus() == TALSCalcParameter::kFixed)
				fNumDMES++;
			else
				fNumDTHE++;

			return TLSCalcWorkingObservation<TLSCalcSpatialDistObservation>::push_back(sdObs);
		}

		/*! Resets the flag to indicate that the observations' summary statistics are not up to date */
		void		statisticsBad() {fDMESStatisticsOK = false; fDTHEStatisticsOK = false; return;}
	//@}

private:

	/*! Indicates if the current statistics for the DMES observations are up to date or not */
	bool			dmesStatisticsOK() const {return fDMESStatisticsOK;}
	/*! Compiles the observations' summary statistics */
	void			compileDMESObsSummary();

	/*! Indicates if the current statistics for the DTHE observations are up to date or not */
	bool			dtheStatisticsOK() const {return fDTHEStatisticsOK;}
	/*! Compiles the observations' summary statistics */
	void			compileDTHEObsSummary();

	int					fNumDMES;
	bool				fDMESStatisticsOK; /*! flags if statistics are up to date or not */
	TLGCObsSummary		fDMESObsSummary;

	int					fNumDTHE;
	bool				fDTHEStatisticsOK; /*! flags if statistics are up to date or not */
	TLGCObsSummary		fDTHEObsSummary;

};

#endif //SU_LS_WORK_SPADIST_OBS