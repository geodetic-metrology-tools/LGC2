////////////////////////////////////////////////////////////////////
// TSimFrameSummary.cpp : implementation file
// Class containing all information concerning a point of the TLGCData
// that are necessary to write the summary of the simulations results file.
//
// Copyright 2015 Surveying section, CERN, MEF/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////
#include  "TSimFrameSummary.h"

//////////////////////
// Default Constructor
//////////////////////
TSimFrameSummary::TSimFrameSummary()
{
	fFirstSim = true;
	fSumRes.scale = 0.0;
	fResMin.scale = 0.0;
	fResMax.scale = 0.0;
	fSumRes2.scale = 0.0;
}


//////////////////////
// Constructor taking a LSCPosVecIterator
//////////////////////
TSimFrameSummary::TSimFrameSummary(const TAdjustableHelmertTransformation& frame) :  fFrame(&frame)
{
	fFirstSim = true;
	fSumRes.scale = 0.0;
	fResMin.scale = 0.0;
	fResMax.scale = 0.0;
	fSumRes2.scale = 0.0;
}


///////////////////
// copy constructor
///////////////////
///////////////////
// copy constructor
///////////////////
TSimFrameSummary::TSimFrameSummary(const TSimFrameSummary& source)
{
	*this = source;
}


/////////////
// destructor
/////////////
TSimFrameSummary::~TSimFrameSummary()
{
}


///////////////////////////
// copy assignment operator
///////////////////////////
TSimFrameSummary& TSimFrameSummary::operator=(const TSimFrameSummary& right)
{
	if (this != &right)
	{
		fFrame = right.fFrame;

		fSumRes = right.fSumRes;
		fResMin = right.fResMin;
		fResMax = right.fResMax;

		fSumRes2 = right.fSumRes2;

		fFirstSim = right.fFirstSim;
	}
	return *this;
}

////////////////////////////////////////////
// add a new value in the sum of the residus
////////////////////////////////////////////
void	TSimFrameSummary::addNewResValue(const TransformParameters& res)
{
	fSumRes += res;
	
	/*Because it is not an angle, but a square angle, we use double. Need to manage manually the units*/
	fSumRes2.omega = fSumRes2.omega + res.omega.getRadiansValue()  *res.omega.getRadiansValue();
	fSumRes2.phi = (fSumRes2.phi + res.phi.getRadiansValue() * res.phi.getRadiansValue());
	fSumRes2.kappa = (fSumRes2.kappa + res.kappa.getRadiansValue()  *res.kappa.getRadiansValue());
	fSumRes2.tX += (res.tX * res.tX);
	fSumRes2.tY += (res.tY * res.tY);
	fSumRes2.tZ += (res.tZ * res.tZ);
	fSumRes2.scale += (res.scale * res.scale);

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
void	TSimFrameSummary::ifNecessarySetMin(const TransformParameters& res)
{
	if (res.omega < fResMin.omega)
		fResMin.omega = res.omega;
	if (res.phi < fResMin.phi)
		fResMin.phi = res.phi;
	if (res.kappa < fResMin.kappa)
		fResMin.kappa = res.kappa;

	if (res.tX < fResMin.tX)
		fResMin.tX = res.tX;
	if (res.tY < fResMin.tY)
		fResMin.tY = res.tY;
	if (res.tZ < fResMin.tZ)
		fResMin.tZ = res.tZ;

	if (res.scale < fResMin.scale)
		fResMin.scale = res.scale;
}

////////////////////////////////////////////////////////////
// update, if necessary, the value of the minimum of residus
////////////////////////////////////////////////////////////
void	TSimFrameSummary::ifNecessarySetMax(const TransformParameters& res)
{
	if (res.omega > fResMax.omega)
		fResMax.omega = res.omega;
	if (res.phi > fResMax.phi)
		fResMax.phi = res.phi;
	if (res.kappa > fResMax.kappa)
		fResMax.kappa = res.kappa;

	if (res.tX > fResMax.tX)
		fResMax.tX = res.tX;
	if (res.tY > fResMax.tY)
		fResMax.tY = res.tY;
	if (res.tZ > fResMax.tZ)
		fResMax.tZ = res.tZ;

	if (res.scale > fResMax.scale)
		fResMax.scale = res.scale;
}