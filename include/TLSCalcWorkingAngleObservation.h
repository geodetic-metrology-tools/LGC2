
#ifndef SU_LS_WORK_AOBS
#define SU_LS_WORK_AOBS

#pragma once

#include <list>
#include "TLSCalcWorkingObservation.h"
#include "TLGCObsSummary.h"
#include "TLSCalcHorAngleObservation.h"
#include "TLSCalcGyroOrientationObservation.h"

using namespace std;

template <typename Observation>
class TLSCalcWorkingAngleObservation : public TLSCalcWorkingObservation<Observation>
{
public:
	TLSCalcWorkingAngleObservation()
	{
		fStatisticsOK = false;
		fObsSummary.clear();
		fObsSummary.defineAngleObservation();
	}

	~TLSCalcWorkingAngleObservation() { }

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
				fObsSummary.addNewResidual(iter->getResidue().getSignedCCValue());
				iter++;
			}

			fStatisticsOK = true;
		}

		return;
	}

	bool				fStatisticsOK; /*! flags if statistics are up to date or not */
	TLGCObsSummary		fObsSummary;
};

typedef TLSCalcWorkingAngleObservation<TLSCalcHorAngleObservation> TLSCalcWorkingHorAngObs;
typedef TLSCalcWorkingAngleObservation<TLSCalcGyroOrientationObservation> TLSCalcWorkingGyroOrientationObs;

#endif