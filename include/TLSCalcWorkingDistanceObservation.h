
#ifndef SU_LS_WORK_DOBS
#define SU_LS_WORK_DOBS

#pragma once

#include <list>
#include "TLSCalcWorkingObservation.h"
#include "TLGCObsSummary.h"

#include "TLSCalcHorDistObservation.h"
#include "TLSCalcOffsetToVerticalLineObservation.h"
#include "TLSCalcOffsetToTheoPlaneObservation.h"
#include "TLSCalcOffsetToVerPlaneObservation.h"
#include "TLSCalcRadialOffsetCnstrObservation.h"
#include "TLSCalcVertDistObservation.h"

using namespace std;

template <typename Observation>
class TLSCalcWorkingDistanceObservation : public TLSCalcWorkingObservation<Observation>
{
public:
	TLSCalcWorkingDistanceObservation()
	{
		fStatisticsOK = false;
		fObsSummary.clear();
		fObsSummary.defineLengthObservation();
	}

	~TLSCalcWorkingDistanceObservation() { }

	/* Returns the observations' summary statistics*/
	TLGCObsSummary		getObsSummary()
	{
		compileObsSummary();

		return fObsSummary;
	}

	void				statisticsBad() { fStatisticsOK = false; }

private:
	
	/*! Indicates if the current statistics are up to date or not */
	bool				statisticsOK() const { return fStatisticsOK; }

	/*! Compiles the observations' summary statistics */
	void				compileObsSummary()
	{
		if( !statisticsOK() )  	
		{
			list<Observation>::iterator	iter = begin();
			list<Observation>::iterator	iterE = end();

			// clear the observation summary statistics
			fObsSummary.clear();

			// add the current observation results to the observation summary
			// iterate through all the observations
			while ( iter != iterE )
			{
				// add the observation residual to the observation summary
				fObsSummary.addNewResidual(iter->getResidue().getMMetresValue());
				iter++;
			}

			fStatisticsOK = true;
		}

		return;
	}

	bool				fStatisticsOK; /*! flags if statistics are up to date or not */
	TLGCObsSummary		fObsSummary;
};

typedef TLSCalcWorkingDistanceObservation<TLSCalcRadialOffsetCnstrObservation> TLSCalcWorkingRadialOffsetCnstrObs;

#endif
