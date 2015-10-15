#ifndef SU_THEODOLITE_STATION_CONVERTER
#define SU_THEODOLITE_STATION_CONVERTER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include "TALGCConverter.h"
#include "TLGCProject.h"
#include "TheodoliteStationROM.h"
#include "TTheodolite.h"

#include <hash_set>
using namespace stdext;

template <typename Observation>
class hash_function_for_observations
{
public:
	static const size_t bucket_size = 4;
	static const size_t min_buckets = 8;

	size_t operator() (const Observation& o) const
	{
		std::tr1::hash<string> H;
		return H(o.getTargetPoint()->getName().getName());
	}
	
	bool operator() (const Observation& o1, const Observation& o2) const
	{
		return o1 == o2;
	}
};

class TheodoliteStationConverter : public TALGCConverter
{
public:

	TheodoliteStationConverter(TAStreamFormatter& s) : TALGCConverter(s) { }

	bool readTheodoliteStation(int& lineNumber, TLGCProject* project);

	template <typename ObservationROM, typename Observation, typename ObservationConverter, typename T, typename Instrument>
	bool readObservations(int& lineNumber, TLGCProject* project, ObservationROM* rom, const T* target, Instrument* instr)
	{
		ObservationConverter conv(*fStream);

		bool disallowDuplicates = project->getDataSet()->disallowsDuplicates();

		hash_set<Observation, hash_function_for_observations<Observation> > observationsSet;

		while (fStream->peek() != '*')
		{
			if (isComment())
			{
				fStream->readLine();
			}
			else
			{
				Observation* meas = conv.readObservation(project, target, instr, lineNumber);
				if (meas == NULL)
				{
					return false;
				}
				target = meas->getTarget();
				lineNumber++;

				if (disallowDuplicates)
				{
					if (observationsSet.find(*meas) == observationsSet.end())
					{
						observationsSet.insert(*meas);
					}
					else
					{
						char lineNum[10];
						_itoa(lineNumber, lineNum, 10);
						project->setError(project->getError() + "Line: " + lineNum + " - Duplicate observation!\n");
					}
				}

				rom->addMeasurement(meas);
				fStream->skipWhiteSpace();
			}
		}

		return true;
	}

	template <typename ObservationROM, typename Observation, typename ObservationConverter, typename T>
	bool readObservationROMWithScale(int& lineNumber, TLGCProject* project, ObservationROM* oROM, const Scale* scale, T* angle)
	{
		ObservationConverter conv(*fStream);

		bool disallowDuplicates = project->getDataSet()->disallowsDuplicates();

		hash_set<Observation, hash_function_for_observations<Observation> > observationsSet;

		while (fStream->peek() != '*')
		{
			if (isComment())
			{
				fStream->readLine();
			}
			else
			{
				Observation* meas = conv.readObservation(project, scale, angle, lineNumber);
				if (meas == NULL)
				{
					return false;
				}
				scale = meas->getScale();
				lineNumber++;

				if (disallowDuplicates)
				{
					if (observationsSet.find(*meas) == observationsSet.end())
					{
						observationsSet.insert(*meas);
					}
					else
					{
						char lineNum[10];
						_itoa(lineNumber, lineNum, 10);
						project->setError(project->getError() + "Line: " + lineNum + " - Duplicate observation!\n");
					}
				}

				oROM->addMeasurement(meas);
				fStream->skipWhiteSpace();
			}
		}

		return true;
	}

private:
	
	bool readTheodoliteStationROM(int& lineNumber, TLGCProject* project, TheodoliteStationROM* rom, TTheodolite* theo);
	
	template <typename ObservationROM, typename Observation, typename ObservationConverter>
	ObservationROM* readObservationROM(int& lineNumber, TLGCProject* project, TheodoliteStationROM* rom, TTheodolite* theo)
	{
		string trgt = readOptional("TRGT");

		const TheodoliteTarget* target;

		if (trgt != "")
		{
			target = theo->getTargetNamed(trgt);

			if (target == NULL)
			{
				char lineNum[10];
				_itoa(lineNumber, lineNum, 10);
				project->setError(project->getError() + "Line: " + lineNum + " - Unknown target " + trgt + "\n");
				return NULL;
			}
		}
		else
		{
			target = rom->getDefaultTarget();
		}

		ObservationROM* oROM = new ObservationROM();
		lineNumber++;
		fStream->readLine();

		if (!readObservations<ObservationROM, Observation, ObservationConverter, TheodoliteTarget, TTheodolite>(lineNumber, project, oROM, target, theo))
		{
			delete oROM;
			return NULL;
		}

		return oROM;
	}

};

#endif
