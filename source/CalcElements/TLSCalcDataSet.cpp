//TLSCalcDataSet.cpp : implementation file
//Class for a data holding all the Ls calc data


#include "TLSInputMatrices.h"
#include "TLGCDataSet.h"

#include "TLSCalcDataSet.h"

//////////////////////////
// no argument constructor
//////////////////////////
TLSCalcDataSet::TLSCalcDataSet(){

	fUEOIndices.UIndex = 1;
	fUEOIndices.EIndex = 1;
	fUEOIndices.OIndex = 1;

	fWorkingStations = new TLSCalcWorkingStations();
	fWorkingPoints = new TLSCalcWorkingPoints();
	fWorkingConstants = new TLSCalcWorkingConstants();
	fWorkingTargetObjects = new TLSCalcWorkingTargetObjects();
	fWorkingConstraints = new TLSCalcWorkingConstraints();
	
}

//////////////////////////////////////////////////////
// constructor initializing from the passed LGCDataSet
//////////////////////////////////////////////////////
TLSCalcDataSet::TLSCalcDataSet(const TLGCDataSet& lgcDS){

	fUEOIndices.UIndex = 1;
	fUEOIndices.EIndex = 1;
	fUEOIndices.OIndex = 1;

	fWorkingStations = new TLSCalcWorkingStations();
	fWorkingPoints = new TLSCalcWorkingPoints();
	fWorkingConstants = new TLSCalcWorkingConstants();
	fWorkingTargetObjects  =new TLSCalcWorkingTargetObjects();
	fWorkingConstraints = new TLSCalcWorkingConstraints();

	fWorkingPoints->initialize(lgcDS.getWorkingPoints());

	fWorkingTargetObjects->initialize(lgcDS.getWorkingTargetObjects(), fWorkingPoints);

	fWorkingConstants->initialize(lgcDS.getWorkingConstants());

	fWorkingStations->initialize(lgcDS.getWorkingStations(), fWorkingPoints, fWorkingTargetObjects);

	fWorkingConstraints->initialize(lgcDS.getWorkingConstraints(), fWorkingPoints);
}
/////////////
// Destructor
/////////////
TLSCalcDataSet::~TLSCalcDataSet(){

	delete fWorkingPoints;
	delete fWorkingStations;
	delete fWorkingConstants;
	delete fWorkingTargetObjects;
	delete fWorkingConstraints;

}

////////////////////////////////////
// sets the indices for all the data
////////////////////////////////////
void TLSCalcDataSet::setIndices(){

	fUEOIndices.UIndex = 1;
	fUEOIndices.EIndex = 1;
	fUEOIndices.OIndex = 1;

	fUEOIndices = fWorkingPoints->setPointIndices(fUEOIndices);
	fUEOIndices = fWorkingStations->setStationIndices(fUEOIndices);
	fUEOIndices = fWorkingConstants->setConstIndices(fUEOIndices);
	fUEOIndices = fWorkingTargetObjects->setTargetObjectsIndices(fUEOIndices);
	fUEOIndices = fWorkingConstraints->setConstraintsIndices(fUEOIndices);

}


///////////////////////////////////////////////////////
// gets the number of unknowns, equations, observations
///////////////////////////////////////////////////////
UEOIndices TLSCalcDataSet::getDimensions() const{

	UEOIndices ueoi = {(fUEOIndices.UIndex-1), (fUEOIndices.EIndex-1), (fUEOIndices.OIndex-1)};
	return ueoi;
}

///////////////////////////////////////////
//returns a pointer to the Working Stations
///////////////////////////////////////////
TLSCalcWorkingStations* TLSCalcDataSet::getWorkingStations() const{

	return fWorkingStations;
}

/////////////////////////////////////////
//returns a pointer to the Working Points
/////////////////////////////////////////
TLSCalcWorkingPoints* TLSCalcDataSet::getWorkingPoints() const{

	return fWorkingPoints;
}

/////////////////////////////////////////
//returns a pointer to the Working Constants
/////////////////////////////////////////
TLSCalcWorkingConstants* TLSCalcDataSet::getWorkingConstants() const{

	return fWorkingConstants;
}

/////////////////////////////////////////////////
//returns a pointer to the Working Target Objects
/////////////////////////////////////////////////
TLSCalcWorkingTargetObjects* TLSCalcDataSet::getWorkingTargetObjects() const{

	return fWorkingTargetObjects;
}

/////////////////////////////////////////
//returns a pointer to the Working Constraints
/////////////////////////////////////////
TLSCalcWorkingConstraints* TLSCalcDataSet::getWorkingConstraints() const{

	return fWorkingConstraints;
}


//////////////////////////////////////////////
// makes the data ready for the next iteration
//////////////////////////////////////////////
void TLSCalcDataSet::prepareNextIteration(){

	fWorkingPoints->prepareNextIteration();
	fWorkingStations->prepareNextIteration();
	fWorkingConstants->prepareNextIteration();
}

//////////////////////////////////////////////
// makes the data ready for the next simulation
//////////////////////////////////////////////
void TLSCalcDataSet::prepareNextSimulation(){

	fWorkingPoints->prepareNextSimulation();
	fWorkingStations->prepareNextSimulation();
}