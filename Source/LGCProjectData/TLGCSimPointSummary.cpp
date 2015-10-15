////////////////////////////////////////////////////////////////////
// TLGCSimPointSummary.cpp : implementation file
// Class containing all information concerning a point of the DataSet
// that are necessary to write the summary of the simulations' results file.
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////


#include  "TVNumericValue.h"

#include  "TLGCSimPointSummary.h"

//////////////////////
// Default Constructor
//////////////////////
TLGCSimPointSummary::TLGCSimPointSummary() : fSumRes(TCoordSysFactory::k3DCartesian),
fResMin(TCoordSysFactory::k3DCartesian), fResMax(TCoordSysFactory::k3DCartesian),
fSumRes2(TCoordSysFactory::k3DCartesian)
{
	fFirstSim = true;
}


//////////////////////
// Constructor taking a LSCPosVecIterator
//////////////////////
TLGCSimPointSummary::TLGCSimPointSummary(LSPosVecIter point) :  fSumRes(0,0,0,TCoordSysFactory::k3DCartesian),
fResMin(0,0,0,TCoordSysFactory::k3DCartesian), fResMax(0,0,0,TCoordSysFactory::k3DCartesian),
fSumRes2(0,0,0,TCoordSysFactory::k3DCartesian)
{
	fPoint = point;
	fFirstSim = true;
}


///////////////////
// copy constructor
///////////////////
TLGCSimPointSummary::TLGCSimPointSummary(const TLGCSimPointSummary& source) :  fSumRes(TCoordSysFactory::k3DCartesian),
fResMin(TCoordSysFactory::k3DCartesian), fResMax(TCoordSysFactory::k3DCartesian),
fSumRes2(TCoordSysFactory::k3DCartesian)
{
	*this = source;
}

/////////////
// destructor
/////////////
TLGCSimPointSummary::~TLGCSimPointSummary()
{
}


///////////////////////////
// copy assignment operator
///////////////////////////
TLGCSimPointSummary& TLGCSimPointSummary::operator=(const TLGCSimPointSummary& right)
{
	if (this != &right)
	{
		fPoint = right.fPoint;

		fSumRes = right.fSumRes;
		fResMin = right.fResMin;
		fResMax = right.fResMax;

		fSumRes2 = right.fSumRes2;

		fFirstSim = right.fFirstSim;
	}
	return *this;
}


///////////////////////
// equivalence operator
///////////////////////
bool	TLGCSimPointSummary::operator==(const TLGCSimPointSummary& right) const
{
	return fPoint->getName() == right.fPoint->getName();
	/*return (fPoint == right.fPoint &&
		fSumRes == right.fSumRes &&
		fResMin == right.fResMin &&
		fResMax == right.fResMax &&
		fSumRes2 == right.fSumRes2);*/
}


////////////////////////////////////////////
// add a new value in the sum of the residus
////////////////////////////////////////////
void	TLGCSimPointSummary::addNewResValue(const TFreeVector& res)
{
	fSumRes += res;
	
	fSumRes2.setX( fSumRes2.getX() + TLength(powq(res.getX().getMetresValue(),2)) );
	fSumRes2.setY( fSumRes2.getY() + TLength(powq(res.getY().getMetresValue(),2)) );
	fSumRes2.setZ( fSumRes2.getZ() + TLength(powq(res.getZ().getMetresValue(),2)) );

	if (fFirstSim == true)
	{
		fResMin = fSumRes;
		fResMax = fSumRes;
		fFirstSim = false;
	}
	else
	{
		ifNecessarySetMin(res);
		ifNecessarySetMax(res);
	}

	return;
}

////////////////////////////////////////////////////////////
// update, if necessary, the value of the minimum of residus
////////////////////////////////////////////////////////////
void	TLGCSimPointSummary::ifNecessarySetMin(const TFreeVector& res)
{
	if (res.getX() < fResMin.getX())
	{fResMin.setX(res.getX());}
	if (res.getY() < fResMin.getY())
	{fResMin.setY(res.getY());}
	if (res.getZ() < fResMin.getZ())
	{fResMin.setZ(res.getZ());}
	

	return;
}

////////////////////////////////////////////////////////////
// update, if necessary, the value of the minimum of residus
////////////////////////////////////////////////////////////
void	TLGCSimPointSummary::ifNecessarySetMax(const TFreeVector& res)
{
	if (fResMax.getX() < res.getX())
	{fResMax.setX(res.getX());}
	if (fResMax.getY() < res.getY())
	{fResMax.setY(res.getY());}
	if (fResMax.getZ() < res.getZ())
	{fResMax.setZ(res.getZ());}
	
	return;
}