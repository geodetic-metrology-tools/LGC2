// TWorkingGyroOrientationObs.h

#ifndef SU_WORK_OBS
#define SU_WORK_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <hash_set>

using namespace std;
using namespace stdext;

template <typename Observation>
class TWorkingObservation {

public:

	/*! Constructor */
	TWorkingObservation() {	fLastElNbr = 0;	fListError = false;	}
	/*! Destructor */
	~TWorkingObservation() { }

	/*! Returns an iterator to the first element in the list */
	typename list<Observation>::iterator		begin() { return observationsList.begin(); }

	/*! Returns a const iterator to the first element in the list */
	typename list<Observation>::const_iterator	begin() const { return observationsList.begin(); }

	/*! Returns an iterator pointing after the last element in the list */
	typename list<Observation>::iterator		end() { return observationsList.end(); }

	/*! Returns a const iterator pointing after the last element in the list */
	typename list<Observation>::const_iterator	end() const { return observationsList.end(); }

	/*! Adds a new Observation at the end of the list, if it is not done yet (tests first if the object is
	already in the list or not)
	\returns an iterator to the Observation wether it was already inserted or just being inserted */
	void										push_back(const Observation& obs, bool noDuplicates)
	{
		if(!fListError)
		{
			// Checks if the observation is already in the container or not
			if (noDuplicates)
			{
				if (observationsSet.find(obs) == observationsSet.end())
				{
					observationsList.push_back(obs);
					fLastElNbr++;
					observationsSet.insert(obs);
				}
				else
				{
					fListError = true;
				}
			}
			else
			{
				observationsList.push_back(obs);
			}
		}
	}

	/*! Erases the selected Observation and deletes its corresponding pointer from the list */
	bool				erase(Observation& obs) // TODO: this doesn't work when duplicates allowed!
	{
		int id = obs.getObservationID();
		hash_map<int, list<Observation>::iterator> i = observationsSet.find(id);
		if (i == observationsSet.end())
		{
			return false;
		}
		observationsList.erase(i->second);
		observationsSet.erase(id);
		return true;
	}

	/*! Returns the size of the list*/
	int	size() const { return observationsList.size(); }

	bool getListError() const { return fListError; }

private:

	class hash_function_for_observations
	{
	public:
		static const size_t bucket_size = 4;
		static const size_t min_buckets = 8;

		size_t operator() (const Observation& o) const
		{
			return o.getObservationID();
		}
		
		bool operator() (const Observation& o1, const Observation& o2) const
		{
			return o1 == o2;
		} 
	};

	typename hash_set<Observation, hash_function_for_observations> observationsSet;
	typename list<Observation> observationsList; /*!< list of observations */
	int	fLastElNbr;	/*!< last number of elements in the list */
	bool fListError; /*!< handling error attribute: false if no errors */
};

#endif //SU_WORK_OBS