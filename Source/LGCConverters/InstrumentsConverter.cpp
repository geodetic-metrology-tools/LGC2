////////////////////////////////////////////////////////////////////
// InstrumentsConverter.cpp

#include "InstrumentsConverter.h"
#include "TTheodolite.h"
#include "EDMConverter.h"
#include "ScaleConverter.h"
#include "GyroscopeConverter.h"
#include "LevelConverter.h"

bool InstrumentsConverter::readInstruments(int& lineNumber, TLGCProject* project)
{
	if (readOptionalExists("*POLAR"))
	{
		TheodoliteConverter converter(*fStream);
		int oldLineNumber = lineNumber;

		TTheodolite* theo = converter.readTheodolite(lineNumber, project->getDataSet()->getUnknownsCount());
		if (theo == NULL)
		{
			return false;
		}

		project->getDataSet()->getWorkingInstruments()->addTheodolite(theo);

		if (lineNumber == oldLineNumber + 1)
		{
			char lineNum[10];
			_itoa(lineNumber, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Theodolite must have at least one target!\n");
			return false;
		}

		return readInstruments(lineNumber, project);
	}
	else if (readOptionalExists("*EDM"))
	{
		EDMConverter converter(*fStream);
		int oldLineNumber = lineNumber;
		EDM* edm = converter.readEDM(lineNumber, project->getDataSet()->getUnknownsCount());
		if (edm == NULL)
		{
			return false;
		}

		project->getDataSet()->getWorkingInstruments()->addEDM(edm);

		if (lineNumber == oldLineNumber + 1)
		{
			char lineNum[10];
			_itoa(lineNumber, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - EDM must have at least one target!\n");
			return false;
		}

		return readInstruments(lineNumber, project);
	}
	else if (readOptionalExists("*SCALE"))
	{
		ScaleConverter converter(*fStream);
		Scale* scale = converter.readScale(project->getDataSet()->getUnknownsCount());
		if (scale == NULL)
		{
			return false;
		}

		project->getDataSet()->getWorkingInstruments()->addScale(scale);

		return readInstruments(++lineNumber, project);
	}
	else if (readOptionalExists("*GYRO"))
	{
		GyroscopeConverter converter(*fStream);
		Gyroscope* gyro = converter.readGyroscope();
		if (gyro == NULL)
		{
			return false;
		}

		project->getDataSet()->getWorkingInstruments()->addGyroscope(gyro);

		return readInstruments(++lineNumber, project);
	}
	else if (readOptionalExists("*LEVEL"))
	{
		LevelConverter converter(*fStream);
		Level* level = converter.readLevel(lineNumber, project->getDataSet()->getUnknownsCount());
		if (level == NULL)
		{
			return false;
		}

		project->getDataSet()->getWorkingInstruments()->addLevel(level);

		return readInstruments(++lineNumber, project);
	}
	return true;
}
