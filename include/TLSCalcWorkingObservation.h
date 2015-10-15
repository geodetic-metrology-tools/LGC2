
#ifndef SU_LS_WORK_OBS
#define SU_LS_WORK_OBS

#pragma once

#include <list>
#include "TLSCalcOrientationCnstrObservation.h"

using namespace std;

template <typename Observation>
class TLSCalcWorkingObservation
{
public:
	TLSCalcWorkingObservation() { fLastElNbr = 0; fListError = false; }
	~TLSCalcWorkingObservation() { }

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
		return --observationsList.end();
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

	typename list<Observation>	observationsList; /*!< list of observations */
	int					fLastElNbr;	/*!< last number of elements in the list */
	bool				fListError; /*!< handling error attribute: false if no errors */
};

typedef TLSCalcWorkingObservation<TLSCalcOrientationCnstrObservation> TLSCalcWorkingOrientationCnstrObs;

#endif