
#ifndef SU_LS_WORK_OBS
#define SU_LS_WORK_OBS

#pragma once

#include <list>
#include <hash_map>
#include "TLSCalcOrientationCnstrObservation.h"
#include "TLSCalcOrientationCnstrObservation.h"
#include "TLSCalcHorAngleObservation.h"
#include "TLSCalcGyroOrientationObservation.h"
#include "TLSCalcHorDistObservation.h"
#include "TLSCalcOffsetToVerticalLineObservation.h"
#include "TLSCalcOffsetToTheoPlaneObservation.h"
#include "TLSCalcOffsetToVerticalLineObservation.h"
#include "TLSCalcOffsetToSpatialLineObservation.h"
#include "TLSCalcOffsetToVerPlaneObservation.h"
#include "TLSCalcRadialOffsetCnstrObservation.h"
#include "TLSCalcVertDistObservation.h"
#include "TLSCalcLevelObservation.h"
#include "TLSCalcPolarObservation.h"
#include "TLSCalcSpatialDistObservation.h"
#include "TLSCalcZenithDistObservation.h"

using namespace std;
using namespace stdext;

template <typename Observation>
class TLSCalcWorkingObservation
{
public:
	TLSCalcWorkingObservation() { fLastElNbr = 0; fListError = false; }
	virtual ~TLSCalcWorkingObservation() { }

	/*! Returns an iterator to the first element in the list */
	typename list<Observation>::iterator		begin() { return observationsList.begin(); }

	/*! Returns a const iterator to the first element in the list */
	typename list<Observation>::const_iterator	begin() const { return observationsList.begin(); }

	/*! Returns an iterator pointing after the last element in the list */
	typename list<Observation>::iterator		end() { return observationsList.end(); }

	/*! Returns a const iterator pointing after the last element in the list */
	typename list<Observation>::const_iterator	end() const { return observationsList.end(); }

	/*! Adds a new Observation at the end of the list */
	typename list<Observation>::iterator push_back(const Observation& obs)
	{
		observationsList.push_back(obs);
		list<Observation>::iterator ret = --observationsList.end();
		observationsMap[obs.getObservationID()] = ret;
		return ret;
	}

	typename list<Observation>::iterator getObservationByID(int id)
	{
		return observationsMap[id];
	}

	/*! Erases the selected Observation and deletes its corresponding pointer from the list */
	bool erase(Observation& obs)
	{
		list<Observation>::iterator iter = observationsList.begin();
		while (iter != observationsList.end() && *iter != obs)
		{
			iter++;
		}
		if (iter == observationsList.end())
		{
			return false;
		}

		observationsList.erase(iter);
		return true;
	}

	/*! Returns the size of the list*/
	int	size() const { return observationsList.size(); }

private:

	hash_map<int, typename list<Observation>::iterator>	observationsMap; /*!< list of observations */
	typename list<Observation>	observationsList; /*!< list of observations */
	int					fLastElNbr;	/*!< last number of elements in the list */
	bool				fListError; /*!< handling error attribute: false if no errors */
};

typedef TLSCalcWorkingObservation<TLSCalcOrientationCnstrObservation> TLSCalcWorkingOrientationCnstrObs;
typedef TLSCalcWorkingObservation<TLSCalcHorAngleObservation> TLSCalcWorkingHorAngObs;
typedef TLSCalcWorkingObservation<TLSCalcOffsetToTheoPlaneObservation> TLSCalcWorkingOffsetToTheoPlaneObs;
typedef TLSCalcWorkingObservation<TLSCalcPolarObservation> TLSCalcWorkingPolarObs;
typedef TLSCalcWorkingObservation<TLSCalcZenithDistObservation> TLSCalcWorkingZenDistObs;
typedef TLSCalcWorkingObservation<TLSCalcGyroOrientationObservation> TLSCalcWorkingGyroOrientationObs;
typedef TLSCalcWorkingObservation<TLSCalcOffsetToSpatialLineObservation> TLSCalcWorkingOffsetToSpaLineObs;
typedef TLSCalcWorkingObservation<TLSCalcOffsetToVerticalLineObservation> TLSCalcWorkingOffsetToVerLineObs;
typedef TLSCalcWorkingObservation<TLSCalcOffsetToVerPlaneObservation> TLSCalcWorkingOffsetToVerPlaneObs;
typedef TLSCalcWorkingObservation<TLSCalcVertDistObservation> TLSCalcWorkingVertDistObs;
typedef TLSCalcWorkingObservation<TLSCalcLevelObservation> TLSCalcWorkingLevelObs;

template <typename T>
class TLSCalcWorkingSpaDistObs : public TLSCalcWorkingObservation<TLSCalcSpatialDistObservation<T> >
{
};

template <typename T>
class TLSCalcWorkingHorDistObs : public TLSCalcWorkingObservation<TLSCalcHorDistObservation<T> >
{
};

#endif
