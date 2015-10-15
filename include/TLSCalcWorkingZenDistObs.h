////////////////////////////////////////////////////////////////////
// TLSCalcWorkingZenDistObs.h
/*!
Container for the least squares calculation zenith distance observations

Patterns:
 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/

#ifndef SU_LS_WORK_ZENDIST_OBS
#define SU_LS_WORK_ZENDIST_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TLSCalcZenithDistObservation.h"
#include "TLSCalcWorkingObservation.h"
#include "TAMeasurement.h"
#include "TLGCObsSummary.h"

#include <list>
using namespace std;

class TLSCalcWorkingZenDistObs : public TLSCalcWorkingObservation<TLSCalcZenithDistObservation> {

public:

	/*!@name Constructor / Destructor */
	//@{
	/*! Default constructor */
	TLSCalcWorkingZenDistObs();
	/*! Destructor */
	~TLSCalcWorkingZenDistObs();
	//@}

		/*! Returns the number of zenith distance measurements where the instrument height is unknown */
		int					numZENI() const {return fNumZeni;}
		/*! Returns the number of zenith distance measurements where the instrument height is fixed */
		int					numZENH() const {return fNumZenh;}
		/*! Returns the observations' summary statistics where the instrument height is unknown */
		TLGCObsSummary		getZENIObsSummary();
		/*! Returns the observations' summary statistics where the instrument height is fixed */
		TLGCObsSummary		getZENHObsSummary();

		list<TLSCalcZenithDistObservation>::iterator push_back(const TLSCalcZenithDistObservation& zdObs)
		{
			if(zdObs.getHInstrStatus() == TALSCalcParameter::kFixed)
				fNumZenh++;
			else
				fNumZeni++;

			return TLSCalcWorkingObservation<TLSCalcZenithDistObservation>::push_back(zdObs);
		}

		/*! Resets the flag to indicate that the observations' summary statistics are not up to date */
		void		statisticsBad() {fZENHStatisticsOK = false; fZENIStatisticsOK = false; return;}
	//@}

private:

	/*! Indicates if the current statistics for the ZENI observations are up to date or not */
	bool			zeniStatisticsOK() const {return fZENIStatisticsOK;}

	/*! Compiles the ZENI observations' summary statistics */
	void			compileZENIObsSummary();

	/*! Indicates if the current statistics ZENH observations are up to date or not */
	bool			zenhStatisticsOK() const {return fZENHStatisticsOK;}

	/*! Compiles the ZENH observations' summary statistics */
	void			compileZENHObsSummary();


	int					fNumZeni;
	bool				fZENIStatisticsOK; /*! flags if statistics are up to date or not */
	TLGCObsSummary		fZENIObsSummary;

	int					fNumZenh;
	bool				fZENHStatisticsOK; /*! flags if statistics are up to date or not */
	TLGCObsSummary		fZENHObsSummary;
};

#endif //SU_LS_WORK_ZENDIST_OBS