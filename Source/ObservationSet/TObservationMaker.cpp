//  TObservationMaker.cpp
//
/*  A spatial point data set for geodetic transformation programs.  
  
    Patterns:
  
   
    Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
//
//
// other forward declarations
#include <list>
#include  "TObservationMaker.h"


////////////////////////////////////////////////////////////////


//ClassImp(TObservationMaker)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TObservationMaker::TObservationMaker() 
{	// default constructor
	fError ="";
	obsCount = 0;
}


TObservationMaker::TObservationMaker(const  TObservationMaker& original)
{	// copy constructor
	(*this) = original;
}


TObservationMaker::~TObservationMaker()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

void	TObservationMaker::operator = (const TObservationMaker& copy)
{
	fError =copy.fError;
	return;
}



bool	TObservationMaker::processData(TLGCDataSet& ds, ObservationSet& obs)
{

	bool retVal = true;

	//build Observations
	processTheoStations(ds, obs);
	processDistStations(ds, obs);
	processLevelStations(ds, obs);
	processConstraints(ds, obs);

	//if process is not ok
	if(fError!="")
	{
		retVal = false;
	}

	return retVal;
}


void	TObservationMaker::processTheoStations(const TLGCDataSet& ds, ObservationSet& obs)
{

	TheodStConstIter	theoIt =	ds.getWorkingStations()->getTheodStBeginIterator();
	TheodStConstIter	endTheoIt =	ds.getWorkingStations()->getTheodStEndIterator();

	while(theoIt != endTheoIt)
	{//process the ROM for all stations

		processHorAng(theoIt, ds, obs);

		processZenithDist(theoIt, ds, obs);

		processGyroOrie(theoIt, ds, obs);
		
		theoIt++;
	}
	return;
}


void	TObservationMaker::processDistStations(TLGCDataSet& ds, ObservationSet& obs)
{
	DistStIterator	distStIt =	ds.getWorkingStations()->getDistStBeginIterator();
	DistStIterator	endDistStIt =	ds.getWorkingStations()->getDistStEndIterator();

	while(distStIt != endDistStIt)
	{//process the ROM for all stations

		processSpatialDist(distStIt, ds, obs);

		processHorizontalDist(distStIt, ds, obs);

		processOffsetToVerLine(distStIt, ds, obs);

		processOffsetToSpaLine(distStIt, ds, obs);

		processOffsetToVerPlane(distStIt, ds, obs);

		processOffsetToTheoPlane(distStIt, ds, obs);

		distStIt++;
	}
	return;
}


void	TObservationMaker::processLevelStations(const TLGCDataSet& ds, ObservationSet& obs)
{
	// iterate over DLEV and DVER measurements, i corvers both caes for the
	// boolean arguments to distinct DLEV(1) and DVER(0)
	for (int i = 0; i < 2; i++) 
	{
		for (LevelStConstIter levelStIt = ds.getWorkingStations()->getLevelStBeginIterator(i==1);
			 levelStIt != ds.getWorkingStations()->getLevelStEndIterator(i==1); levelStIt++)
		{
			processVerticalDist(levelStIt, ds, obs, i==1);
		}
	}
}


void	TObservationMaker::processConstraints(const TLGCDataSet& ds, ObservationSet& obs)
{
	PointConstIter endPoint = ds.getWorkingPoints()->getPointsEndIterator();
	
	RadOffCnstrConstIter cnstrIt = ds.getWorkingConstraints()->getRadOffCnstrBeginIterator();
	RadOffCnstrConstIter endCnstrIt = ds.getWorkingConstraints()->getRadOffCnstrEndIterator();

	while(cnstrIt != endCnstrIt)
	{//build the constraint obs for all the measurement, and add it to the fObs list
		PointConstIter ptIt = ds.getWorkingPoints()->getPoint(cnstrIt->getPoint());

		if(ptIt !=endPoint)
		{
			TRadialOffsetCnstrObservation cnstr (ptIt, cnstrIt, obsCount++);
			obs.push_backRadOffCnstr(cnstr, ds.disallowsDuplicates());
		}
		else
		{
			string error = "Radial constraint on  point, ";
			error = error + ptIt->getName().getName();
			error = error + ", isn't known; ";
			setError(error);
		}
		cnstrIt++;
	}


	OrieCnstrConstIter cnstrOIt = ds.getWorkingConstraints()->getOrieCnstrBeginIterator();
	OrieCnstrConstIter endCnstrOIt = ds.getWorkingConstraints()->getOrieCnstrEndIterator();

	while(cnstrOIt != endCnstrOIt)
	{//build the constraint obs for all the measurement, and add it to the fObs list
		PointConstIter refPtIt = ds.getWorkingPoints()->getPoint(cnstrOIt->getPoint());

		PointConstIter firstPt = ds.getWorkingPoints()->getPointsBeginIterator();

		PointConstIter fixedPtIt;
		bool find = true;

		//search the first CALA point
		while(firstPt != endPoint && find == true)
		{
			if(firstPt->getPosition().getObjectStatus() == TSpatialStatus::kCala)
			{
				fixedPtIt = firstPt;
				find = false;
			}
			firstPt++;
		}

		//else search the first VXY point
		if(find == true)
		{
			//initialise first point for a second loop
			firstPt = ds.getWorkingPoints()->getPointsBeginIterator();

			while(firstPt != endPoint && find == true)
			{
				if(firstPt->getPosition().getObjectStatus() == TSpatialStatus::kVxy)
				{
					fixedPtIt = firstPt;
					find = false;
				}
				firstPt++;
			}
		}

		//else error
		if(find == true)
		{
			string error = "Orientation Constraint not defined, no 'CALA' or 'VXY' Point defined ";
			setError(error);
		}
		else
		{
			PointConstIter endPoint = ds.getWorkingPoints()->getPointsEndIterator();

			if(fixedPtIt != endPoint && refPtIt != endPoint)
			{
				TOrientationCnstrObservation cnstr (fixedPtIt, refPtIt, cnstrOIt, obsCount++);
				obs.push_backOrieCnstr(cnstr, ds.disallowsDuplicates());
			}
			else
			{
				if(fixedPtIt != endPoint)
				{
					string error = "Orientation constraint on fixed point, ";
					error = error + fixedPtIt->getName().getName();
					error = error + ", isn't known; ";
					setError(error);
				}
				if(refPtIt != endPoint)
				{
					string error = "Orientation constraint on ref point, ";
					error = error + refPtIt->getName().getName();
					error = error + ", isn't known; ";
					setError(error);
				}
			}
		}
		cnstrOIt++;
	}


	return;
}




////////////////////////////////////////////////////////////////////////////////////////////////
//Private functions
////////////////////////////////////////////////////////////////////////////////////////////////
void	TObservationMaker::processHorAng(TheodStConstIter theoIt, const TLGCDataSet& ds, ObservationSet& obs)
{
	HorAngROMConstIter HAROMIt = theoIt->getHorAngROMBeginIterator();
	HorAngROMConstIter endHAROMIt = theoIt->getHorAngROMEndIterator();

	PointConstIter endPoint = ds.getWorkingPoints()->getPointsEndIterator();
	AngleConstantConstIter endAng = ds.getWorkingConstants()->getAngleConstantsEndIterator();

	while(HAROMIt != endHAROMIt)
	{//get the HA measurement for all the ROM
		HorAngMeasConstIter HAMeasIt = HAROMIt->getHorAngMeasBeginIterator();
		HorAngMeasConstIter endHAMeasIt = HAROMIt->getHorAngMeasEndIterator();
	

		while(HAMeasIt != endHAMeasIt)
		{//build the observation for each HA measurement, and add it to the fObs list
			PointConstIter stationedIt = ds.getWorkingPoints()->getPoint(theoIt->getStationedPoint());
			PointConstIter targetIt = ds.getWorkingPoints()->getPoint(HAMeasIt->getTargetPoint());
			AngleConstantConstIter angCteIt = ds.getWorkingConstants()->getAngleConstant(theoIt->getAngleConst().getConstantName());

			if(stationedIt !=endPoint && targetIt != endPoint && angCteIt != endAng)
			{
				//THorAngleObservation HAObs (theoIt, HAROMIt, HAMeasIt, stationedIt, targetIt, angCteIt);
				//obs.push_backHorAng(HAObs);
				obs.push_backHorAng(THorAngleObservation(theoIt, HAROMIt, HAMeasIt, stationedIt, targetIt, angCteIt, obsCount++), ds.disallowsDuplicates());
			}
			else
			{
				if(stationedIt == endPoint)
				{
					string error = "Theodolite Station, ";
					error = error + theoIt->getStationedPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}

				if(targetIt == endPoint)
				{
					string error = "Horizontal Angle mesurement on target, ";
					error = error + HAMeasIt->getTargetPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}
				
				if(angCteIt == endAng)
				{
					string error = "Angle's constant on target, ";
					error = error + HAMeasIt->getTargetPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}
			}
			if (obs.getWorkingHorAngObsError())
			{
				setError("Cannot have duplicate observations!");
			}

			HAMeasIt++;
		}
		HAROMIt++;
	}
	return;
}
	

void	TObservationMaker::processZenithDist(TheodStConstIter theoIt, const TLGCDataSet& ds, ObservationSet& obs)
{
	ZenithDistROMConstIter DZROMIt = theoIt->getZenithDistROMBeginIterator();
	ZenithDistROMConstIter endDZROMIt = theoIt->getZenithDistROMEndIterator();
			
	PointConstIter end = ds.getWorkingPoints()->getPointsEndIterator();
	
	while(DZROMIt != endDZROMIt)
	{//get the DZ measurement for all the ROM
		ZenithDistMeasConstIter DZMeasIt = DZROMIt->getZenithDistMeasBeginIterator();
		ZenithDistMeasConstIter endDZMeasIt = DZROMIt->getZenithDistMeasEndIterator();

		while(DZMeasIt != endDZMeasIt)
		{//build the DZ obs for all the measurement, and add it to the fObs list
			PointConstIter stationedIt = ds.getWorkingPoints()->getPoint(theoIt->getStationedPoint());
			PointConstIter targetIt = ds.getWorkingPoints()->getPoint(DZMeasIt->getTargetPoint());

			if(stationedIt !=end && targetIt != end)
			{
				//TZenithDistObservation DZObs (theoIt, DZROMIt, DZMeasIt, stationedIt, targetIt);
				//obs.push_backZenDist(DZObs);
				obs.push_backZenDist(TZenithDistObservation (theoIt, DZROMIt, DZMeasIt, stationedIt, targetIt, obsCount++), ds.disallowsDuplicates());
			}
			else
			{
				if(stationedIt == end)
				{
					string error = "Theodolite Station, ";
					error = error + theoIt->getStationedPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}

				if(targetIt == end)
				{
					string error = "Zenithal distance mesurement on target, ";
					error = error + DZMeasIt->getTargetPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}			
			}
			if (obs.getWorkingZenDistObsError())
			{
				setError("Cannot have two observations to the same point!");
			}
			DZMeasIt++;

		}
		DZROMIt++;
	}
	return;
}



void	TObservationMaker::processSpatialDist(DistStIterator distStIt, TLGCDataSet& ds, ObservationSet& obs)
{
	SpatialDistROMConstIter spaDistROMIt = distStIt->getSpatialDistROMBeginIterator();
	SpatialDistROMConstIter endSpaDistROMIt = distStIt->getSpatialDistROMEndIterator();
			
	PointConstIter endPoint = ds.getWorkingPoints()->getPointsEndIterator();
	DistConstantConstIter endDist = ds.getWorkingConstants()->getDistConstantsEndIterator();

	while(spaDistROMIt != endSpaDistROMIt)
	{//get the Spatial distance measurement for all the ROM
		SpatialDistMeasConstIter spaDistMeasIt = spaDistROMIt->getDistMeasBeginIterator();
		SpatialDistMeasConstIter endSpaDistMeasIt = spaDistROMIt->getDistMeasEndIterator();
		
		while(spaDistMeasIt != endSpaDistMeasIt)
		{//build the Spatial distance obs for all the measurement, and add it to the fObs list
			PointConstIter stationedIt = ds.getWorkingPoints()->getPoint(distStIt->getStationedPoint());
			PointConstIter targetIt = ds.getWorkingPoints()->getPoint(spaDistMeasIt->getTargetPoint());
			DistConstantConstIter distCteIt = ds.getWorkingConstants()->getDistConstant(spaDistMeasIt->getMeasConst().getConstantName());


			if(stationedIt !=endPoint && targetIt != endPoint && distCteIt != endDist)
			{
				TSpatialDistObservation spaDistObs (distStIt, spaDistROMIt, spaDistMeasIt, stationedIt, targetIt, distCteIt, obsCount++);
				if (distStIt->getInstHeightStatus() == TAMeasurement::kVariable)
				{
					spaDistObs.setTheodStation(getAssociatedTheoStToSpaDist(distStIt, ds));
				}
				
				obs.push_backSpaDist(spaDistObs, ds.disallowsDuplicates());
			}
			else
			{
				if(stationedIt == endPoint)
				{
					string error = "Distance Station, ";
					error = error + distStIt->getStationedPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}

				if(targetIt == endPoint)
				{
					string error = "Spatial distance mesurement on target, ";
					error = error + spaDistMeasIt->getTargetPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}

				if(distCteIt == endDist)
				{
					string error = "Distance's constant on target, ";
					error = error + spaDistMeasIt->getTargetPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}
			}
			if (obs.getWorkingSpaDistObsError())
			{
				setError("Cannot have two observations to the same point!");
			}

			spaDistMeasIt++;
		}
		spaDistROMIt++;
	}
	return;
}


void	TObservationMaker::processHorizontalDist(DistStConstIter	distStIt, const TLGCDataSet& ds, ObservationSet& obs)
{
	HorDistROMConstIter horDistROMIt = distStIt->getHorDistROMBeginIterator();
	HorDistROMConstIter endHorDistROMIt = distStIt->getHorDistROMEndIterator();

	PointConstIter endPoint = ds.getWorkingPoints()->getPointsEndIterator();
	DistConstantConstIter endDist = ds.getWorkingConstants()->getDistConstantsEndIterator();

	while(horDistROMIt != endHorDistROMIt)
	{//get the Horizontal distance measurement for all the ROM
		HorDistMeasConstIter horDistMeasIt = horDistROMIt->getDistMeasBeginIterator();
		HorDistMeasConstIter endHorDistMeasIt = horDistROMIt->getDistMeasEndIterator();
		
		while(horDistMeasIt != endHorDistMeasIt)
		{//build the Horizontal distance obs for all the measurement, and add it to the fObs list
			PointConstIter stationedIt = ds.getWorkingPoints()->getPoint(distStIt->getStationedPoint());
			PointConstIter targetIt = ds.getWorkingPoints()->getPoint(horDistMeasIt->getTargetPoint());
			DistConstantConstIter distCteIt = ds.getWorkingConstants()->getDistConstant(horDistMeasIt->getMeasConst().getConstantName());

			if(stationedIt !=endPoint && targetIt != endPoint && distCteIt != endDist)
			{
//				THorizontalDistObservation horDistObs (distStIt, horDistROMIt, horDistMeasIt, stationedIt, targetIt, distCteIt);
//				obs.push_backHorDist(horDistObs);
				obs.push_backHorDist(THorizontalDistObservation (distStIt, horDistROMIt, horDistMeasIt, stationedIt, targetIt, distCteIt, obsCount++), ds.disallowsDuplicates());
			}
			else
			{
				if(stationedIt == endPoint)
				{
					string error = "Distance Station, ";
					error = error + distStIt->getStationedPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}

				if(targetIt == endPoint)
				{
					string error = "Horizontal distance mesurement on target, ";
					error = error + horDistMeasIt->getTargetPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}

				if(distCteIt == endDist)
				{
					string error = "Distance's constant on target, ";
					error = error + horDistMeasIt->getTargetPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}
			}
			if (obs.getWorkingHorDistObsError())
			{
				setError("Cannot have two observations to the same point!");
			}

			horDistMeasIt++;

		}
		horDistROMIt++;
	}
	return;
}



void	TObservationMaker::processVerticalDist(LevelStConstIter	levelStIt, const TLGCDataSet& ds, ObservationSet& obs, bool isDLEV)
{
	VertDistROMConstIter vertDistROMIt = levelStIt->getVertDistROMBeginIterator();
	VertDistROMConstIter endVertDistROMIt = levelStIt->getVertDistROMEndIterator();

	PointConstIter endPoint = ds.getWorkingPoints()->getPointsEndIterator();
	DistConstantConstIter endDist = ds.getWorkingConstants()->getDistConstantsEndIterator();
	
	while(vertDistROMIt != endVertDistROMIt)
	{//get the Vertical distance measurement for all the ROM

		VertDistMeasConstIter vertDistMeasIt = vertDistROMIt->getVertDistMeasBeginIterator();
		VertDistMeasConstIter endVertDistMeasIt = vertDistROMIt->getVertDistMeasEndIterator();

		while(vertDistMeasIt != endVertDistMeasIt)
		{//build the Vertical distance obs for all the measurement, and add it to the fObs list
			PointConstIter refIt = ds.getWorkingPoints()->getPoint(vertDistROMIt->getRefPtName());
			PointConstIter targetIt = ds.getWorkingPoints()->getPoint(vertDistMeasIt->getTargetPoint());
			DistConstantConstIter distCteIt = ds.getWorkingConstants()->getDistConstant(vertDistMeasIt->getMeasConst().getConstantName());

			if(refIt !=endPoint && targetIt != endPoint && distCteIt != endDist)
			{
				TVertDistObservation vertDistObs (levelStIt, vertDistROMIt, vertDistMeasIt, refIt, targetIt, distCteIt, obsCount++);
				obs.push_backVertDist(vertDistObs, ds.disallowsDuplicates(), isDLEV);				
			}
			else
			{
				if(refIt == endPoint)
				{
					string error = "Level Reference, ";
					error = error + vertDistROMIt->getRefPtName().getName();
					error = error + ", isn't known;";
					setError(error);
				}

				if(targetIt == endPoint)
				{
					string error = "Leveling mesurement on target, ";
					error = error + vertDistMeasIt->getTargetPoint().getName();
					error = error + ", isn't known;";
					setError(error);
				}

				if(distCteIt == endDist)
				{
					string error = "Distance's constant on target, ";
					error = error + vertDistMeasIt->getTargetPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}
			}
			if (obs.getWorkingVertDistObsError(isDLEV))
			{
				setError("Cannot have two observations to the same point!");
			}

			vertDistMeasIt++;
		}
		vertDistROMIt++;
	}
	return;
}


void	TObservationMaker::processOffsetToVerLine(DistStConstIter	distStIt, const TLGCDataSet& ds, ObservationSet& obs)
{//Offset to Ver. line

	OffsetToVerLineMeasConstIter measIt = distStIt->getOffsetToVerLineMeasBeginIterator();
	OffsetToVerLineMeasConstIter endMeasIt = distStIt->getOffsetToVerLineMeasEndIterator();

	PointConstIter endPoint = ds.getWorkingPoints()->getPointsEndIterator();
	DistConstantConstIter endDist = ds.getWorkingConstants()->getDistConstantsEndIterator();
		
	while(measIt != endMeasIt)
	{//build the Offset to Ver. line obs for all the measurement, and add it to the fObs list
		PointConstIter stationedIt = ds.getWorkingPoints()->getPoint(distStIt->getStationedPoint());
		PointConstIter targetIt = ds.getWorkingPoints()->getPoint(measIt->getTargetLineName().getFirstEndName());
		DistConstantConstIter distCteIt = ds.getWorkingConstants()->getDistConstant(measIt->getMeasConst().getConstantName());

		if(stationedIt !=endPoint && targetIt != endPoint && distCteIt != endDist)
		{
			TOffsetToVerLineObservation offset (distStIt, measIt, stationedIt, targetIt, distCteIt, obsCount++);
			obs.push_backOffsetToVerLine(offset, ds.disallowsDuplicates());
		}
		else
		{
			if(stationedIt == endPoint)
			{
				string error = "Distance Station, ";
				error = error + distStIt->getStationedPoint().getName();
				error = error + ", isn't known; ";
				setError(error);
			}

			if(targetIt == endPoint)
			{
				string error = "Offset mesurement on target line, ";
				error = error + measIt->getTargetLineName().getName();
				error = error + ", isn't known; ";
				setError(error);
			}

			if(distCteIt == endDist)
			{
				string error = "Distance's constant on target line, ";
				error = error + measIt->getTargetLineName().getName();
				error = error + ", isn't known; ";
				setError(error);
			}
		}
		if (obs.getWorkingOffsetToVerLineObsError())
		{
			setError("Cannot have two observations to the same point!");
		}

		measIt++;
	}
	return;
}


void	TObservationMaker::processOffsetToSpaLine(DistStConstIter	distStIt, const TLGCDataSet& ds, ObservationSet& obs)
{//Offset to spatial line

	OffsetToSpaLineMeasConstIter measIt = distStIt->getOffsetToSpaLineMeasBeginIterator();
	OffsetToSpaLineMeasConstIter endMeasIt = distStIt->getOffsetToSpaLineMeasEndIterator();

	PointConstIter endPoint = ds.getWorkingPoints()->getPointsEndIterator();
	DistConstantConstIter endDist = ds.getWorkingConstants()->getDistConstantsEndIterator();
		
	while(measIt != endMeasIt)
	{//build the Offset to Ver. line obs for all the measurement, and add it to the fObs list
		PointConstIter stationedIt = ds.getWorkingPoints()->getPoint(distStIt->getStationedPoint());
		PointConstIter firstTargetIt = ds.getWorkingPoints()->getPoint(measIt->getTargetLineName().getFirstEndName());
		PointConstIter secondTargetIt = ds.getWorkingPoints()->getPoint(measIt->getTargetLineName().getSecondEndName());
		DistConstantConstIter distCteIt = ds.getWorkingConstants()->getDistConstant(measIt->getMeasConst().getConstantName());

		if(stationedIt !=endPoint && firstTargetIt != endPoint && secondTargetIt != endPoint && distCteIt != endDist)
		{
			TOffsetToSpaLineObservation offset (distStIt, measIt, stationedIt, firstTargetIt, secondTargetIt, distCteIt, obsCount++);
			obs.push_backOffsetToSpaLine(offset, ds.disallowsDuplicates());
		}
		else
		{
			if(stationedIt == endPoint)
			{
				string error = "Distance Station, ";
				error = error + distStIt->getStationedPoint().getName();
				error = error + ", isn't known; ";
				setError(error);
			}

			if(firstTargetIt == endPoint || secondTargetIt == endPoint)
			{
				string error = "Offset mesurement on target line, ";
				error = error + measIt->getTargetLineName().getName();
				error = error + ", isn't known; ";
				setError(error);
			}

			if(distCteIt == endDist)
			{
				string error = "Distance's constant on target line, ";
				error = error + measIt->getTargetLineName().getName();
				error = error + ", isn't known; ";
				setError(error);
			}
		}
		if (obs.getWorkingOffsetToSpaLineObsError())
		{
			setError("Cannot have two observations to the same point!");
		}

		measIt++;
	}
	return;
}


void	TObservationMaker::processOffsetToVerPlane(DistStConstIter	distStIt, const TLGCDataSet& ds, ObservationSet& obs)
{//Offset to Ver. plane

	OffsetToVerPlaneMeasConstIter measIt = distStIt->getOffsetToVerPlaneMeasBeginIterator();
	OffsetToVerPlaneMeasConstIter endMeasIt = distStIt->getOffsetToVerPlaneMeasEndIterator();

	PointConstIter endPoint = ds.getWorkingPoints()->getPointsEndIterator();
	DistConstantConstIter endDist = ds.getWorkingConstants()->getDistConstantsEndIterator();
		
	while(measIt != endMeasIt)
	{//build the Offset to Ver. line obs for all the measurement, and add it to the fObs list
		PointConstIter stationedIt = ds.getWorkingPoints()->getPoint(distStIt->getStationedPoint());
		PointConstIter firstTargetIt = ds.getWorkingPoints()->getPoint(measIt->getTargetPlaneName().getFirstPointName());
		PointConstIter secondTargetIt = ds.getWorkingPoints()->getPoint(measIt->getTargetPlaneName().getSecondPointName());
		DistConstantConstIter distCteIt = ds.getWorkingConstants()->getDistConstant(measIt->getMeasConst().getConstantName());

		if(stationedIt !=endPoint && firstTargetIt != endPoint && secondTargetIt != endPoint && distCteIt != endDist)
		{
			TOffsetToVerPlaneObservation offset (distStIt, measIt, stationedIt, firstTargetIt, secondTargetIt, distCteIt, obsCount++);
			obs.push_backOffsetToVerPlane(offset, ds.disallowsDuplicates());
		}
		else
		{
			if(stationedIt == endPoint)
			{
				string error = "Distance Station, ";
				error = error + distStIt->getStationedPoint().getName();
				error = error + ", isn't known; ";
				setError(error);
			}

			if(firstTargetIt == endPoint || secondTargetIt == endPoint)
			{
				string error = "Offset mesurement on target line, ";
				error = error + measIt->getTargetPlaneName().getName();
				error = error + ", isn't known; ";
				setError(error);
			}

			if(distCteIt == endDist)
			{
				string error = "Distance's constant on target line, ";
				error = error + measIt->getTargetPlaneName().getName();
				error = error + ", isn't known; ";
				setError(error);
			}
		}
		if (obs.getWorkingOffsetToVerPlaneObsError())
		{
			setError("Cannot have two observations to the same point!");
		}

		measIt++;
	}
	return;
}


void	TObservationMaker::processOffsetToTheoPlane(DistStIterator	distStIt, TLGCDataSet& ds, ObservationSet& obs)
{//Offset to Theo Plane

	// initialise some iterators
	OffsetToTheoPlaneMeasConstIter measIt = distStIt->getOffsetToTheoPlaneMeasBeginIterator();
	OffsetToTheoPlaneMeasConstIter endMeasIt = distStIt->getOffsetToTheoPlaneMeasEndIterator();

	PointConstIter endPoint = ds.getWorkingPoints()->getPointsEndIterator();
	DistConstantConstIter endDist = ds.getWorkingConstants()->getDistConstantsEndIterator();
	TheodStIterator endTheoSt = ds.getWorkingStations()->getTheodStEndIterator();
	fLastTheoIt = endTheoSt;
	
	while(measIt != endMeasIt)
	{//build the Offset to Theo. plane obs for all the measurements, and add them to the fObs list
		// get iterators to the objects associated with the measurement
		PointConstIter stationedIt = ds.getWorkingPoints()->getPoint(distStIt->getStationedPoint());
		PointConstIter targetPtIt = ds.getWorkingPoints()->getPoint(measIt->getTargetPlaneName().getFirstPointName());
		TheodStIterator targetStIt = this->getAssociatedTheoStToOffset(distStIt, targetPtIt, ds, obs);
		DistConstantConstIter distCteIt = ds.getWorkingConstants()->getDistConstant(measIt->getMeasConst().getConstantName());

		// if all the object associated with the measurement have been found thn creat the observation
		// and add it to the rest
		if(stationedIt !=endPoint && targetPtIt != endPoint && targetStIt != endTheoSt && distCteIt != endDist)
		{
			TAngle angle = measIt->getTargetPlaneName().getAngle();
			TOffsetToTheoPlaneObservation offset (distStIt, measIt, stationedIt, targetPtIt, targetStIt, angle, distCteIt, obsCount++);
			obs.push_backOffsetToTheoPlane(offset, ds.disallowsDuplicates());
		}
		// if not all the objects have been found then create an appropriate error message		
		else
		{
			if(stationedIt == endPoint)
			{
				string error = "Distance Station, ";
				error = error + distStIt->getStationedPoint().getName();
				error = error + ", isn't known; ";
				setError(error);
			}

			if(targetPtIt == endPoint || targetStIt == endTheoSt)
			{
				string error = "Offset mesurement on target plane, ";
				error = error + measIt->getTargetPlaneName().getName();
				error = error + ", isn't known; ";
				setError(error);
			}

			if(distCteIt == endDist)
			{
				string error = "Distance's constant on target plane, ";
				error = error + measIt->getTargetPlaneName().getName();
				error = error + ", isn't known; ";
				setError(error);
			}
		}
		if (obs.getWorkingOffsetToTheoPlaneObsError())
		{
			setError("Cannot have two observations to the same point!");
		}
		measIt++;
	}
	return;
}


void	TObservationMaker::processGyroOrie(TheodStConstIter theoIt, const TLGCDataSet& ds, ObservationSet& obs)
{
	GyroOrieROMConstIter ROMIt = theoIt->getGyroOrieROMBeginIterator();
	GyroOrieROMConstIter endROMIt = theoIt->getGyroOrieROMEndIterator();

	PointConstIter endPoint = ds.getWorkingPoints()->getPointsEndIterator();
	AngleConstantConstIter endAng = ds.getWorkingConstants()->getAngleConstantsEndIterator();

	while(ROMIt != endROMIt)
	{//get the orientation measurement for all the ROM
		HorAngMeasConstIter measIt = ROMIt->getGyroOrieMeasBeginIterator();
		HorAngMeasConstIter endMeasIt = ROMIt->getGyroOrieMeasEndIterator();
		
		while(measIt != endMeasIt)
		{//build the observation for each HA measurement, and add it to the fObs list
			PointConstIter stationedIt = ds.getWorkingPoints()->getPoint(theoIt->getStationedPoint());
			PointConstIter targetIt = ds.getWorkingPoints()->getPoint(measIt->getTargetPoint());
			AngleConstantConstIter angCteIt = ds.getWorkingConstants()->getAngleConstant(theoIt->getAngleConst().getConstantName());


			if(stationedIt !=endPoint && targetIt != endPoint && angCteIt != endAng)
			{
				TGyroOrientationObservation orie (theoIt, ROMIt, measIt, stationedIt, targetIt, angCteIt, obsCount++);
				obs.push_backGyroOrie(orie, ds.disallowsDuplicates());
			}
			else
			{
				if(stationedIt == endPoint)
				{
					string error = "Theodolite Station, ";
					error = error + theoIt->getStationedPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}

				if(targetIt == endPoint)
				{
					string error = "Horizontal Angle mesurement on target, ";
					error = error + measIt->getTargetPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}
				
				if(angCteIt == endAng)
				{
					string error = "Angle's constant on target, ";
					error = error + measIt->getTargetPoint().getName();
					error = error + ", isn't known; ";
					setError(error);
				}
			}
			if (obs.getWorkingGyroOrieObsError())
			{
				setError("Cannot have two observations to the same point!");
			}

			measIt++;
		}
		ROMIt++;
	}
	return;
}

	
///////////////////////////////////////////////////////
//ERROR
///////////////////////////////////////////////////////
void	TObservationMaker::setError(string str)
{
	fError = fError + str + '\n';
	return;
}


string	TObservationMaker::getError() const
{
	return fError;
}

////////////////////////////////////////////////////////
//ASSOCIATED STATION
////////////////////////////////////////////////////////
TheodStIterator TObservationMaker::getAssociatedTheoStToSpaDist(DistStIterator distStIt, TLGCDataSet& ds)
{
	if(!(distStIt->isAssociatedTheoSt()))
	{
		TheodStIterator iter, iterEnd;
		iter = ds.getWorkingStations()->getTheodStBeginIterator();
		iterEnd = ds.getWorkingStations()->getTheodStEndIterator();

		while (iter != iterEnd)
		{
			if (iter->getStationedPoint() == distStIt->getStationedPoint()
				&& iter->getInstHeightStatus() == TAMeasurement::kVariable
				&& iter->getAssociatedDistStForSpaDist() == false)
			{
				distStIt->setAssociatedTheoSt(iter);
				iter->setAssociatedDistStForSpaDist();
				return iter;
			}
			iter++;
		}

		setError("Point " + distStIt->getStationedPoint().getName() + ": No associated theodolite station found");
		return iter;
	}
	else
	{
		return distStIt->getAssociatedTheoSt();
	}
}


TheodStIterator TObservationMaker::getAssociatedTheoStToOffset(DistStIterator distStIt, PointConstIter theo, TLGCDataSet& ds, ObservationSet& )
{
	TheodStIterator iter, iterEnd, selectedStn;
	iter = ds.getWorkingStations()->getTheodStBeginIterator();
	iterEnd = ds.getWorkingStations()->getTheodStEndIterator();

	// initialise the selected station iterator to the end of the container
	selectedStn = iterEnd;

	// iterate through the container of theodolite stations to find the station
	// given for the offset to a theo plane observation
	while (iter != iterEnd)
	{
		// check if the current theodolite station in the container could be that 
		// given for the offset to a theo plane observation
		if (iter->getStationedPoint() == theo->getName()
			&& iter->getAssociatedDistStForOffset() == false
			&& iter->getHorAngROMDimension() != 0)
		{
			selectedStn = iter;
		}
		iter++;
	}

	// set an error message if the theodolite station was not found
	if( selectedStn == iterEnd )
		setError("Point " + distStIt->getStationedPoint().getName() + ": No associated theodolite station found");

	// set the theodolite station to be associated with the distance station
	distStIt->setAssociatedTheoSt(selectedStn);
	
	// check if the theodolite station has changed
	if( selectedStn != fLastTheoIt)
	{
		// if the last theodolite station is not the end of the container flag that
		// a distance station for offset measurements has been linked to it
		//
		// Another offset distance station on the same point will therefore find a different 
		// theodolite station
		if( fLastTheoIt != iterEnd )   fLastTheoIt->setAssociatedDistStForOffset();

		// update the pointer to the last used theodolite station
		fLastTheoIt = selectedStn;
	}

	// return the selected theodolite station for the observation
	return selectedStn;
}


/*TheodStIterator TObservationMaker::getAssociatedTheoStToOffset(DistStIterator distStIt, PointConstIter theo, TLGCDataSet& ds, ObservationSet& obs)
{
	OffsetToTheoPlaneObsIter begObs = obs.beginOffsetToTheoPlane();
	OffsetToTheoPlaneObsIter endObs = obs.endOffsetToTheoPlane();


	TheodStIterator iter, iterEnd, lock;
	iter = ds.getWorkingStations()->getTheodStBeginIterator();
	iterEnd = ds.getWorkingStations()->getTheodStEndIterator();

	if(begObs == endObs) //it's the first observation, no observations have been added
	{
		while (iter != iterEnd)
		{
			if (iter->getStationedPoint() == theo->getName()
				&& iter->getAssociatedDistStForOffset() == false
				&& iter->getHorAngROMDimension() != 0)
			{
				distStIt->setAssociatedTheoSt(iter);
				return iter;
			}
			iter++;
		}
	}
	else
	{
		OffsetToTheoPlaneObsIter lastObs = --endObs;

		while (iter != iterEnd)
		{
			//it's the same theodolite target than the last observation
			if (iter->getStationedPoint() == theo->getName()
				&&  theo->getName().getName() == lastObs->getTargetTheoPosName()
				&& iter->getAssociatedDistStForOffset() == false
				&& iter->getHorAngROMDimension() != 0)
			{
				distStIt->setAssociatedTheoSt(iter);
				return iter;
			}

			//test
			string tgName = iter->getStationedPoint().getName();
			string theoName = theo->getName().getName();

			//it's not the same theodolite target than the last observation
			if (iter->getStationedPoint() == theo->getName()
				&&  theo->getName().getName() != lastObs->getTargetTheoPosName()
				&& iter->getAssociatedDistStForOffset() == false
				&& iter->getHorAngROMDimension() != 0)
			{
				distStIt->setAssociatedTheoSt(iter);
				//test
				string nameLast = fLastTheoIt->getStationedPoint().getName();
				fLastTheoIt->setAssociatedDistStForOffset();
				return iter;
			}
			iter++;
		}

	}
	
	setError("No associated theodolite station found");
	return iter;
}
*/

//////////////////////////////////////////////////////////////////////////
//end
//////////////////////////////////////////////////////////////////////////


