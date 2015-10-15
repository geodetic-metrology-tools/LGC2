//TLGCObsLSContributionGenerator.cpp : implementation file
//Class for a LGC Observation LS contrib generator
//
// Copyright 2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include "TLGCObsLSContributionGenerator.h"
#include "TDist.h"
#include "TARefFrameTransformation.h"
#include "TLSInputMatrices.h"
#include "TModifiedLocalAstronomicalRF.h"
#include "TARefFrameTransformation.h"
#include "TGC2MLATransformation.h"
#include "TMLA2GCTransformation.h"
#include "TReferenceEllipsoid.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////////////
TLGCObsLSContributionGenerator::TLGCObsLSContributionGenerator(TRefSystemFactory::ERefFrame refSur)
{
	fLAName = "NULL";
	fMLAName = "NULL";
	fRefSystem = refSur;

	// initialise the geodetic reference systems and transformations as a function
	// of the reference surface for the calculations
	if(fRefSystem == TRefSystemFactory::kNotInGraph)/*OLOC*/
	{
		fEllipsoid = 0;
		fGeoid = TRefSystemFactory::kNoGeoid;
		fGeoRefFrame = TRefSystemFactory::kNotInGraph;
		
		fTransfoMLAtoCGRF = 0;
		fTransfoCGRFtoMLA = 0;
		fLocalRF = 0;
		
		fCCStoMLABegin = fTransfoCCStoMLA.end();
		fCCStoMLAEnd = fTransfoCCStoMLA.end();

		fMLAtoCCSBegin = fTransfoMLAtoCCS.end();
		fMLAtoCCSEnd = fTransfoMLAtoCCS.end();

		fCCStoCGRFBegin = fTransfoCCStoCGRF.end();
		fCCStoCGRFEnd = fTransfoCCStoCGRF.end();

		fCCStoXYHBegin = fTransfoCCStoXYH.end();
		fCCStoXYHEnd = fTransfoCCStoXYH.end();
	}
	else
	{
		TRefFrameWrapper ccs, cgrf, xyh;

		//define the required CGRF (LEP or SPS) and geoid
		if(fRefSystem == TRefSystemFactory::kCERNXYHsSphereSPS)/*SPHE*/
		{
			xyh = TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCERNXYHsSphereSPS);
			cgrf.setFrame(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCGRFSphere));
			fEllipsoid = TRefSystemFactory::getRefSystemFactory()->getEllipsoid(TRefSystemFactory::kSphere);
			fGeoid = TRefSystemFactory::kCGSphere;
			fGeoRefFrame = TRefSystemFactory::kCGRFSphere;
		}

		if(fRefSystem == TRefSystemFactory::kCernXYHg85Machine)/*LEP*/
		{
			xyh = TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHg85Machine);
			cgrf.setFrame(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCGRF));
			fEllipsoid = TRefSystemFactory::getRefSystemFactory()->getEllipsoid(TRefSystemFactory::kGRS80);
			fGeoid = TRefSystemFactory::kCG1985Machine;
			fGeoRefFrame = TRefSystemFactory::kCGRF;
		}

		if(fRefSystem == TRefSystemFactory::kCernXYHg00Machine)/*RS2K*/
		{
			xyh = TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHg00Machine);
			cgrf.setFrame(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCGRF));
			fEllipsoid = TRefSystemFactory::getRefSystemFactory()->getEllipsoid(TRefSystemFactory::kGRS80);
			fGeoid = TRefSystemFactory::kCG2000Machine;	
			fGeoRefFrame = TRefSystemFactory::kCGRF;
		}

		//set the CCS reference frame wrapper
		ccs.setFrame(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));


		//initialise the local astronomical reference frame to be a Modified Local Reference Frame with P0 for the origin
		TFreeVector falseOrigin(LITERAL(0.0), LITERAL(0.0), LITERAL(0.0), TCoordSysFactory::k3DCartesian);
		TSpatialPosition origin(TRefSystemFactory::getRefSystemFactory()->getRefFrame(fGeoRefFrame));
		origin.setCoordinates(TPositionVector(LITERAL(4395400.363782), LITERAL(465785.056736), LITERAL(4583458.226014), TCoordSysFactory::k3DCartesian));
		// the reference frame of the instrument station in geodetic system calcs
		fLocalRF = new TModifiedLocalAstronomicalRF("Local reference frame", fGeoid, origin, falseOrigin, TAngle(LITERAL(0.0)), TAngle(LITERAL(0.0)));


		//define the transformation from the CCS to the CGRF
		fTransfoCCStoCGRF = TGraph::getGraph()->getTransform(ccs, cgrf);
		// set iterators to the start and end of the sequence of transformations
		fCCStoCGRFBegin = fTransfoCCStoCGRF.begin();
		fCCStoCGRFEnd = fTransfoCCStoCGRF.end();
		
		//define the transformation from the CGRF to the CCS
		fTransfoCGRFtoCCS = TGraph::getGraph()->getTransform(cgrf, ccs);
		// set iterators to the start and end of the sequence of transformations
		fCGRFtoCCSBegin = fTransfoCGRFtoCCS.begin();
		fCGRFtoCCSEnd = fTransfoCGRFtoCCS.end();

		// define the transformations between the CGRF and the local astronomical reference frame
		fTransfoCGRFtoMLA = new TGC2MLATransformation( fLocalRF, TRefSystemFactory::getRefSystemFactory()->getGeoid(fGeoid) );
		fTransfoMLAtoCGRF = new TMLA2GCTransformation( fLocalRF, TRefSystemFactory::getRefSystemFactory()->getGeoid(fGeoid) );


		//define the transformation from the CCS to the MLA
		fTransfoCCStoMLA = fTransfoCCStoCGRF;
		fTransfoCCStoMLA.insert(fTransfoCCStoMLA.end(),fTransfoCGRFtoMLA);
		// set iterators to the start and end of the sequence of transformations
		fCCStoMLABegin = fTransfoCCStoMLA.begin();
		fCCStoMLAEnd = fTransfoCCStoMLA.end();


		//define the transformation from the MLA to the CCS
		fTransfoMLAtoCCS = TGraph::getGraph()->getTransform(cgrf, ccs);
		fTransfoMLAtoCCS.insert(fTransfoMLAtoCCS.begin(), fTransfoMLAtoCGRF);
		// set iterators to the start and end of the sequence of transformations
		fMLAtoCCSBegin = fTransfoMLAtoCCS.begin();
		fMLAtoCCSEnd = fTransfoMLAtoCCS.end();


		//define the transformation from the CCS to the XYH projection
		fTransfoCCStoXYH = TGraph::getGraph()->getTransform(ccs, xyh);
		// set iterators to the start and end of the sequence of transformations
		fCCStoXYHBegin = fTransfoCCStoXYH.begin();
		fCCStoXYHEnd = fTransfoCCStoXYH.end();
	}
}


TLGCObsLSContributionGenerator::~TLGCObsLSContributionGenerator()
{//destructor
	if (fLocalRF != 0)
	{
		
		delete fTransfoCGRFtoMLA;
		delete fTransfoMLAtoCGRF;
		delete fLocalRF;
		fTransfoCCStoMLA.clear();
		fTransfoMLAtoCCS.clear();
		fTransfoCCStoCGRF.clear();
		fTransfoCCStoXYH.clear();

	}
}


/////////////////////////////////////////////////////////////////
// Reference Frame TRANSFORMATION ROUTINES
/////////////////////////////////////////////////////////////////
// Set the local reference frame to be a Local Astronomical system and re-initialise the transformation
void	TLGCObsLSContributionGenerator::setToLATransformation(TPositionVector station , string laName)
{
	// reset the geodetic transformations if the calculation is not in a Local System (OLOC Keyword)
	if(fLocalRF !=0)
	{
		// transform the vector for the origin to the CGRF system
		vector<TARefFrameTransformation*>::iterator i = fCCStoCGRFBegin;
		while (i != fCCStoCGRFEnd )
		{
			(*i)->transform(station);
			i++;
		}

		//Set the new origin of the local reference frame
		TSpatialPosition origin(TRefSystemFactory::getRefSystemFactory()->getRefFrame(fGeoRefFrame));
		origin.setCoordinates(station);
		
		// set the local reference frame to be a Local Astronomical system, and set it's origin
		fLocalRF->initialiseLA(origin);

		//initialise new transformations between LA and CGRF
		fTransfoCGRFtoMLA->initialise();
		fTransfoMLAtoCGRF->initialise();
	}

	// note the system name to identify changes of instrument station
	fLAName = laName;
	fMLAName = "NULL";

	return;
}


// Set the local reference frame to be a Modified Local Astronomical system and re-initialise 
// the transformation
void	TLGCObsLSContributionGenerator::setToMLATransformation(TPositionVector station , string mlaName)
{
	// reset the geodetic transformations if the calculation is not in a Local System (OLOC Keyword)
	if(fLocalRF !=0)
	{
		// transform the vector for the origin to the CGRF system
		vector<TARefFrameTransformation*>::iterator i = fCCStoCGRFBegin;
		while (i != fCCStoCGRFEnd )
		{
			(*i)->transform(station);
			i++;
		}

		//Set the new origin of the local reference frame
		TSpatialPosition origin(TRefSystemFactory::getRefSystemFactory()->getRefFrame(fGeoRefFrame));
		origin.setCoordinates(station);
		
		// set the local reference frame to be a Modified Local Astronomical system, and set it's origin
		fLocalRF->initialiseMLA(origin);

		//initialise new transformations between MLA and CGRF
		fTransfoCGRFtoMLA->initialise();
		fTransfoMLAtoCGRF->initialise();
	}

	// note the system name to identify changes of instrument station
	fMLAName = mlaName;
	fLAName = "NULL";

	return;
}




/////////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS FOR CONTRIBUTIONS
/////////////////////////////////////////////////////////////////

//  return the calculated observation value and design matrix contributions 
//	for the given Horizontal Angle observation with the current parameters 
TheoStnContrib	TLGCObsLSContributionGenerator::getHorAngDsgnMxContributions(const LSHorAngConstIter obsIt)
{
	TReal a,b;//station position coefficients (negative values give target coefficients)
	TAngle calcMeas;
	TFreeVector coordContrib(TCoordSysFactory::k3DCartesian);
	TReal hiContrib;
	TReal v0Contrib;
	TPositionVector station = obsIt->getStEstCoordinate();
	TPositionVector target = obsIt->getTgEstCoordinate();
	vector<TARefFrameTransformation*>::iterator currentIter;
	string mlaName = obsIt->getStPointName();

	// If a new station is being processed initialise the transformation to the MLA system
	// corresponding to the station position
	if(fMLAName != mlaName)
	{
		setToMLATransformation(obsIt->getStEstCoordinate(), mlaName);
	}

	//TRANSFORM OBSERVATION PARAMETERS IN LOCAL INSTRUMENT SYSTEM IF NECESSARY
	currentIter = this->fCCStoMLABegin;
	while (currentIter != this->fCCStoMLAEnd)
	{
		(*currentIter)->transform(station);
		(*currentIter)->transform(target);
		currentIter++;
	}

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = station.getX().getMetresValue();
	TReal ySt = station.getY().getMetresValue();

	TReal xTg = target.getX().getMetresValue();
	TReal yTg = target.getY().getMetresValue();

	TReal distance = powq(dist(xSt, ySt, xTg, yTg), 2);

	//CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
	calcMeas = TAngle::aTan2((xTg - xSt),(yTg - ySt)) - obsIt->getEstConstAngle();

	if (distance < kSmallValue)
		throw std::logic_error("Horizontal angle error: Station " + mlaName + 
		                       " and target " + obsIt->getTgPointName() + " are identical.");

	//CALCULATION OF THE CONTRIBUTION IN INSTRUMENT LOCAL SYSTEM	
	// contribution for the station coordinates
	a = -LITERAL(1.0) * (yTg - ySt)/distance;//xSt coefficient
	b = (xTg - xSt)/distance;//ySt coefficient
	coordContrib = TFreeVector(a,b,LITERAL(0.0), TCoordSysFactory::k3DCartesian);

	//contribution for the V0 parameter
	v0Contrib = -1;
	// no contribution for the instrument height
	hiContrib = 0;

	//TRANSFORM CONTRIBUTIONS FROM LOCAL INSTRUMENT SYSTEM TO CCS, IF NECESSARY
	currentIter = this->fMLAtoCCSBegin;
	while (currentIter != this->fMLAtoCCSEnd )
	{
		(*currentIter)->transform(coordContrib);
		currentIter++;
	}

	TheoStnContrib  contrib = {calcMeas, coordContrib, hiContrib, v0Contrib};
	return contrib;
}


// return the calculated observation value and design matrix contributions 
// for the given Zenith Distance observation with the current parameters 
TheoStnContrib  TLGCObsLSContributionGenerator::getZenDistDsgnMxContributions(const LSZenDistConstIter obsIt)
{
	TReal a,b,c;
	TAngle calcMeas;
	TFreeVector coordContrib(TCoordSysFactory::k3DCartesian);
	TReal hiContrib;
	TReal v0Contrib;
	TPositionVector station = obsIt->getStEstCoordinate();
	TPositionVector target = obsIt->getTgEstCoordinate();
	vector<TARefFrameTransformation*>::iterator currentIter;
	string mlaName = obsIt->getStPointName();

	// If a new station is being processed initialise the transformation to the MLA system
	// corresponding to the station position
	if(fMLAName != mlaName)
	{
		setToMLATransformation(obsIt->getStEstCoordinate(), mlaName);
	}

	//TRANSFORM OBSERVATION PARAMETERS IN LOCAL INSTRUMENT SYSTEM IF NECESSARY
	currentIter = this->fCCStoMLABegin;
	while (currentIter != this->fCCStoMLAEnd)
	{
		(*currentIter)->transform(station);
		(*currentIter)->transform(target);
		currentIter++;
	}

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = station.getX().getMetresValue();
	TReal ySt = station.getY().getMetresValue();
	TReal zSt = station.getZ().getMetresValue();

	TReal xTg = target.getX().getMetresValue();
	TReal yTg = target.getY().getMetresValue();
	TReal zTg = target.getZ().getMetresValue();

	TReal hi = obsIt->getEstimatedHInstr().getMetresValue();
	TReal hp = obsIt->getHPrism().getMetresValue();

	TReal dx,dy,dz, dist, sinV;
	dx = xTg-xSt;
	dy = yTg-ySt;
	dz = zTg-zSt-hi+hp;
	dist = sqrtq(powq(dx,2)+powq(dy,2)+powq(dz,2));
	if (dist < kSmallValue)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistDsgnMxContributions: Division by zero because observation points have identical coordinates (dist).");

	TReal distance3D = dist3D(xSt, ySt, zSt+hi, xTg, yTg, zTg+hp);
	if (distance3D < kSmallValue)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistDsgnMxContributions: Division by zero because observation points have identical coordinates (distance3D).");

	//CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
//	calcMeas = TAngle::aCos((zTg - zSt - hi + hp)/
									//dist3D(xSt, ySt, zSt+hi, xTg, yTg, zTg+hp));

	calcMeas = TAngle::aCos((zTg - zSt - hi + hp)/distance3D);

	//CALCULATION OF THE CONTRIBUTION IN INSTRUMENT LOCAL SYSTEM	
	sinV = sinq(calcMeas.getRadiansValue());
	if (sinV < kSmallValue)
		throw std::logic_error("TLGCObsLSContributionGenerator::getZenDistDsgnMxContributions: Division by zero because observation points have identical coordinates (sinV).");
	
	a = -1 * dz * dx / (powq(dist,3) * sinV);//xSt coefficient
	b = -1 * dz * dy / (powq(dist,3) * sinV);//ySt coefficient
	c = 1 / (dist * sinV) - powq(dz,2)/(powq(dist,3) * sinV);//zSt coefficient
	
	// contribution for the station coordinates
	coordContrib = TFreeVector(a, b, c, TCoordSysFactory::k3DCartesian);

	// no contribution for the v0 parameter
	//hiContrib = c;
	v0Contrib = LITERAL(0.0);

	//TRANSFORM CONTRIBUTIONS FROM LOCAL INSTRUMENT SYSTEM TO CCS, IF NECESSARY
	currentIter = this->fMLAtoCCSBegin;
	while (currentIter != this->fMLAtoCCSEnd )
	{
		(*currentIter)->transform(coordContrib);
		currentIter++;
	}

	// contribution for the instrument height
	hiContrib = coordContrib.getZ().getMetresValue();

	TheoStnContrib  contrib = {calcMeas, coordContrib, hiContrib, v0Contrib};
	return contrib;
}


// return the calculated observation value and design matrix contributions 
// for the given Spatial Distrance observation with the current parameters 
DistStnContrib  TLGCObsLSContributionGenerator::getSpaDistDsgnMxContributions(const LSSpaDistConstIter obsIt)
{
	TReal a,b,c;
	TLength calcMeas;
	TFreeVector coordContrib(TCoordSysFactory::k3DCartesian);
	TReal hiContrib;
	TReal constContrib;
	TPositionVector station = obsIt->getStEstCoordinate();
	TPositionVector target = obsIt->getTgEstCoordinate();
	vector<TARefFrameTransformation*>::iterator currentIter;
	string mlaName = obsIt->getStPointName();

	// If a new station is being processed initialise the transformation to the MLA system
	// corresponding to the station position
	if(fMLAName != mlaName)
	{
		setToMLATransformation(obsIt->getStEstCoordinate(), mlaName);
	}

	//TRANSFORM COORDINATES INTO LOCAL INSTRUMENT SYSTEM IF NECESSARY
	currentIter = this->fCCStoMLABegin;
	while (currentIter != this->fCCStoMLAEnd)
	{
		(*currentIter)->transform(station);
		(*currentIter)->transform(target);
		currentIter++;
	}

	//get calculate distance in the local system
	TReal xSt = station.getX().getMetresValue();
	TReal ySt = station.getY().getMetresValue();
	TReal zSt = station.getZ().getMetresValue();

	TReal xTg = target.getX().getMetresValue();
	TReal yTg = target.getY().getMetresValue();
	TReal zTg = target.getZ().getMetresValue();

	TReal hi = obsIt->getEstHInst().getMetresValue();
	TReal hp = obsIt->getHPrism().getMetresValue();

	TReal cst = obsIt->getEstDistConst().getMetresValue();

	//CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
	calcMeas = TLength( dist3D(xSt, ySt, (zSt + hi), xTg, yTg, (zTg + hp)) - cst );

	if (calcMeas.getMetresValue() < kSmallValue)
		throw std::logic_error("TLGCObsLSContributionGenerator::getSpaDistDsgnMxContributions: Division by zero because observation points have identical coordinates.");
			
	// calculation of the constribution in the local system
	a = -LITERAL(1.0) * (xTg - xSt)/calcMeas.getMetresValue();// xSt coefficient
	b = -LITERAL(1.0) * (yTg - ySt)/calcMeas.getMetresValue();//ySt coefficient
	c = -LITERAL(1.0) * (zTg - zSt + hp - hi)/calcMeas.getMetresValue();//zSt coefficient
	coordContrib = TFreeVector(a,b,c, TCoordSysFactory::k3DCartesian);

	//hiContrib =  c;//hi coefficient
	constContrib =  -1;//c coefficient

	//TRANSFORM CONTRIBUTIONS FROM LOCAL INSTRUMENT SYSTEM TO CCS, IF NECESSARY
	currentIter = this->fMLAtoCCSBegin;
	while (currentIter != this->fMLAtoCCSEnd )
	{
		(*currentIter)->transform(coordContrib);
		currentIter++;
	}
	hiContrib =  coordContrib.getZ().getMetresValue();//hi coefficient

	DistStnContrib  contrib = {calcMeas, coordContrib, hiContrib, constContrib};
	return contrib;
}



// return the calculated observation value and design matrix contributions 
// for the given Horizontal Distance observation with the current parameters 
pair<TLength, TFreeVector>	TLGCObsLSContributionGenerator::getHorDistDsgnMxContributions(const LSHorDistConstIter obsIt)
{
	TReal a,b;
	TLength calcMeas;
	TFreeVector coordContrib(TCoordSysFactory::k3DCartesian);
	TPositionVector station = obsIt->getStEstCoordinate();
	TPositionVector target = obsIt->getTgEstCoordinate();

	//
	// this observation is curently only valid in a local reference frame (OLOC)
	//

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = station.getX().getMetresValue();
	TReal ySt =  station.getY().getMetresValue();
	TReal xTg = target.getX().getMetresValue();
	TReal yTg = target.getY().getMetresValue();
	TReal cte = obsIt->getEstDistConst().getMetresValue();

	//CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
	calcMeas = TLength( dist(xSt, ySt, xTg, yTg) - cte );

	if (calcMeas.getMetresValue() < kSmallValue)
		throw std::logic_error("TLGCObsLSContributionGenerator::getHorDistDsgnMxContributions: Division by zero because observation points have identical coordinates.");
	
	//CALCULATION OF THE CONTRIBUTION IN INSTRUMENT LOCAL SYSTEM
	a = -1 * (xTg - xSt)/calcMeas.getMetresValue();// xSt coefficient
	b = -1 * (yTg - ySt)/calcMeas.getMetresValue();//ySt coefficient
	coordContrib = TFreeVector(a, b, LITERAL(0.0), TCoordSysFactory::k3DCartesian);

	return make_pair(calcMeas, coordContrib);
}


LevelStnContrib	TLGCObsLSContributionGenerator::getVertDistDsgnMxContributions(const LSVertDistConstIter obsIt)
{
	TPositionVector ref(obsIt->getRefEstCoordinate());
	TPositionVector target(obsIt->getTgEstCoordinate());
	auto k3D = TCoordSysFactory::k3DCartesian;
	// In the local case the height difference is the difference in Z
	TLength dh(target.getZ() - ref.getZ() - obsIt->getEstDistConst());

	// Define local contributions in the astronomic systems of station and target.
	TFreeVector contRef(0, 0, -1, k3D);
	TFreeVector contTgt(0, 0,  1, k3D);
	
	// Using a geodetic system
	if( fGeoRefFrame != TRefSystemFactory::kNotInGraph )
	{
		// set the MLA to the current station
		setToMLATransformation(ref, obsIt->getRefPointName());
		// transform the station contribution to the CCS
		for (auto i = fMLAtoCCSBegin; i != fMLAtoCCSEnd; i++)
			(*i)->transform(contRef);

		// set the MLA to the target position
		setToMLATransformation(target, obsIt->getTgPointName());
		// transform the target contribution to the CCS
		for (auto i = fMLAtoCCSBegin; i != fMLAtoCCSEnd; i++)
			(*i)->transform(contTgt);

		// transform station and target position to 2DH to calculate the true height difference
		for (auto i = fCCStoXYHBegin; i != fCCStoXYHEnd; i++) {
			(*i)->transform(ref);
			(*i)->transform(target);
		}

		// update the height difference to use orthometric heights
		dh = target.getH() - ref.getH() - obsIt->getEstDistConst();
	}
			
	LevelStnContrib  contrib = { dh, contRef, contTgt };

	return contrib;
}


// return the calculated observation value and design matrix contributions 
// for the given Vertical Distance observation with the current parameters 
LevelStnContrib	TLGCObsLSContributionGenerator::getDLEVDsgnMxContributions(const LSVertDistConstIter obsIt)
{
	TPositionVector ref(obsIt->getRefEstCoordinate());
	TPositionVector target(obsIt->getTgEstCoordinate());
	auto k3D = TCoordSysFactory::k3DCartesian;

	// Define local contributions
	TFreeVector contRef(0, 0, -1, k3D);
	TFreeVector contTgt(0, 0,  1, k3D);

	// Using a geodetic system
	if( fGeoRefFrame != TRefSystemFactory::kNotInGraph )
	{
		// set the MLA to the current station
		if (fMLAName != obsIt->getRefPointName())
			setToMLATransformation(ref, obsIt->getRefPointName());
	
		// transform the contributions to the CCS
		for (auto i = fMLAtoCCSBegin; i != fMLAtoCCSEnd; i++) {
			(*i)->transform(contRef);
			(*i)->transform(contTgt);
		}
		// transform station and target into the target MLA
		for (auto i = fCCStoMLABegin; i != fCCStoMLAEnd; i++) {
			(*i)->transform(ref);
			(*i)->transform(target);
		}
	}
			
	LevelStnContrib  contrib = {
		target.getZ() - ref.getZ() - obsIt->getEstDistConst(), contRef, contTgt
	};

	return contrib;
}



// return the calculated observation value and design matrix contributions 
// for the given Offset to Vertical Line observation with the current parameters 
pair<TLength, TFreeVector>	TLGCObsLSContributionGenerator::getOffsetToVerLineDsgnMxContributions(const LSOffsetToVerLineConstIter obsIt)
{
	TReal a,b,c;
	TLength calcMeas;
	TFreeVector coordContrib(TCoordSysFactory::k3DCartesian);
	TPositionVector station = obsIt->getStEstCoordinate();
	TPositionVector target = obsIt->getFirstTgEstCoordinate();
	vector<TARefFrameTransformation*>::iterator currentIter;
	string mlaName = obsIt->getStPointName();

	// If a new station is being processed initialise the transformation to the MLA system
	// corresponding to the station position
	if(fMLAName != mlaName)
	{
		setToMLATransformation(obsIt->getStEstCoordinate(), mlaName);
	}

	//TRANSFORM COORDINATES INTO LOCAL INSTRUMENT SYSTEM IF NECESSARY
	currentIter = this->fCCStoMLABegin;
	while (currentIter != this->fCCStoMLAEnd)
	{
		(*currentIter)->transform(station);
		(*currentIter)->transform(target);
		currentIter++;
	}

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = station.getX().getMetresValue();
	TReal ySt = station.getY().getMetresValue();

	TReal xTg = target.getX().getMetresValue();
	TReal yTg = target.getY().getMetresValue();

	TReal cte = obsIt->getEstDistConst().getMetresValue();

	//CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
	calcMeas = TLength( sqrtq( powq ((xSt - xTg), 2)+ powq((ySt - yTg), 2) ) - cte );

	if(calcMeas.getMetresValue() < kSmallValue )
	{
		calcMeas.setMetresValue(kSmallValue );
	}
	//CALCULATION OF THE CONTRIBUTION IN INSTRUMENT LOCAL SYSTEM	
	a = -1*(xTg - xSt)/calcMeas.getMetresValue();// xSt coefficient
	b = -1*(yTg - ySt)/calcMeas.getMetresValue();//ySt coefficient
	c = LITERAL(0.0);
	coordContrib = TFreeVector(a, b, c, TCoordSysFactory::k3DCartesian); /*contribution for station*/


	//TRANSFORM CONTRIBUTIONS FROM LOCAL INSTRUMENT SYSTEM TO CCS, IF NECESSARY
	currentIter = this->fMLAtoCCSBegin;
	while (currentIter != this->fMLAtoCCSEnd )
	{
		(*currentIter)->transform(coordContrib);
		currentIter++;
	}

	return make_pair(calcMeas, coordContrib);
}



// return the calculated observation value and design matrix contributions 
// for the given Offset to Spatial Line observation with the current parameters 
OffsetStnContrib  TLGCObsLSContributionGenerator::getOffsetToSpaLineDsgnMxContributions(const LSOffsetToSpaLineConstIter obsIt)
{
	TReal xStC, yStC, zStC, xFTgC, yFTgC, zFTgC, xSTgC, ySTgC, zSTgC;
	TLength calcMeas;
	TFreeVector stnCoordContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector tgt1CoordContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector tgt2CoordContrib(TCoordSysFactory::k3DCartesian);
	TPositionVector station = obsIt->getStEstCoordinate();
	TPositionVector firstTarget = obsIt->getFirstTgEstCoordinate();
	TPositionVector secondTarget = obsIt->getSecondTgEstCoordinate();

	// contributions can be calculated simply in both the CCS and a local 
	// reference frame since no instrument or target heights are involved
	// and the measurement is independent of the local vertical
	// Therefore there are no transformations necessary
	//PARAMETERS IN CARTESIAN REFERENCE SYSTEM
	TReal xSt = station.getX().getMetresValue();
	TReal ySt = station.getY().getMetresValue();
	TReal zSt = station.getZ().getMetresValue();

	TReal xFTg = firstTarget.getX().getMetresValue();
	TReal yFTg = firstTarget.getY().getMetresValue();
	TReal zFTg = firstTarget.getZ().getMetresValue();

	TReal xSTg = secondTarget.getX().getMetresValue();
	TReal ySTg = secondTarget.getY().getMetresValue();
	TReal zSTg = secondTarget.getZ().getMetresValue();

	TReal cte = obsIt->getEstDistConst().getMetresValue();

	TReal d = dist3D(xSt, ySt, zSt, xFTg, yFTg, zFTg);
	TReal D = dist3D(xFTg, yFTg, zFTg, xSTg, ySTg, zSTg);
	TReal DXAB = xSt - xFTg;
	TReal DYAB = ySt - yFTg;
	TReal DZAB = zSt - zFTg;
	TReal DXAC = xSTg - xFTg;
	TReal DYAC = ySTg - yFTg;
	TReal DZAC = zSTg - zFTg;

	TReal p = DXAB * DXAC + DYAB * DYAC + DZAB * DZAC;

	TReal div = sqrtq(D*D*d*d - p*p);

	if(D < kSmallValue)
	{
		D = kSmallValue ;
	}

	if(div < kSmallValue)
	{
		div = kSmallValue ;
	}

	//GET CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
	calcMeas = TLength( sqrtq( powq(xSt - xFTg, 2) + powq (ySt - yFTg, 2) + powq (zSt - zFTg, 2)
								- powq( (xSt - xFTg) * (xSTg - xFTg) / D
										+ (ySt - yFTg) * (ySTg - yFTg) / D
										+( zSt - zFTg) * (zSTg - zFTg) / D, 2) ) - cte );

	// calculation of the constribution in the Cartesian Reference System	
	//calculated contributions
	xStC = ( D*(DXAB) - (p/D)*(DXAC) ) / div;// xSt coefficient
	yStC = ( D*(DYAB) - (p/D)*(DYAC) ) / div;//ySt coefficient
	zStC = ( D*(DZAB) - (p/D)*(DZAC) ) / div;//zSt coefficient
	xFTgC = ( D*(-1*(DXAB))	+ (p/D)*(DXAC+DXAB)	+ (p*p)/(D*D*D)*(-1*DXAC) ) / div;//xFTg coefficient
	yFTgC = ( -1*D*DYAB	+ (p/D)*(DYAC+DYAB) - (p*p)/(D*D*D)*(DYAC) ) / div;//yFTg coefficient		
	zFTgC = ( -1*D*(DZAB) + (p/D)*(DZAC+DZAB) - (p*p)/(D*D*D)*(DZAC) ) / div;//zFTg coefficient
	xSTgC = ( -1*(p/D)*(DXAB) + (p*p)/(D*D*D)*(DXAC) ) / div;//xSTg coefficient
	ySTgC = ( -1*(p/D)*(DYAB) + (p*p)/(D*D*D)*(DYAC) ) / div;//ySTg coefficient
	zSTgC = ( -1*(p/D)*(DZAB) + (p*p)/(D*D*D)*(DZAC) ) / div;//zSTg coefficient

	stnCoordContrib = TFreeVector(xStC, yStC, zStC, TCoordSysFactory::k3DCartesian);
	tgt1CoordContrib = TFreeVector(xFTgC, yFTgC, zFTgC, TCoordSysFactory::k3DCartesian);
	tgt2CoordContrib = TFreeVector(xSTgC, ySTgC, zSTgC, TCoordSysFactory::k3DCartesian);

	OffsetStnContrib  contrib = {calcMeas, stnCoordContrib, tgt1CoordContrib, tgt2CoordContrib};
	return contrib;
}



// return the calculated observation value and design matrix contributions 
// for the given Offset to Vertical Plane observation with the current parameters 
OffsetStnContrib  TLGCObsLSContributionGenerator::getOffsetToVerPlaneDsgnMxContributions(const LSOffsetToVerPlaneConstIter obsIt)
{
	TReal a,b,c,d, e, f, g, h, i;
	TLength calcMeas;
	TFreeVector stnCoordContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector tgt1CoordContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector tgt2CoordContrib(TCoordSysFactory::k3DCartesian);
	TPositionVector station = obsIt->getStEstCoordinate();
	TPositionVector firstTarget = obsIt->getFirstTgEstCoordinate();
	TPositionVector secondTarget = obsIt->getSecondTgEstCoordinate();
	vector<TARefFrameTransformation*>::iterator currentIter;
	string mlaName = obsIt->getStPointName();

	// If a new station is being processed initialise the transformation to the MLA system
	// corresponding to the station position
	if(fMLAName != mlaName)
	{
		setToMLATransformation(obsIt->getStEstCoordinate(), mlaName);
	}

	//TRANSFORM COORDINATES INTO LOCAL INSTRUMENT SYSTEM IF NECESSARY
	currentIter = this->fCCStoMLABegin;
	while (currentIter != this->fCCStoMLAEnd)
	{
		(*currentIter)->transform(station);
		(*currentIter)->transform(firstTarget);
		(*currentIter)->transform(secondTarget);
		currentIter++;
	}

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = station.getX().getMetresValue();
	TReal ySt = station.getY().getMetresValue();

	TReal xFTg = firstTarget.getX().getMetresValue();
	TReal yFTg = firstTarget.getY().getMetresValue();

	TReal xSTg = secondTarget.getX().getMetresValue();
	TReal ySTg = secondTarget.getY().getMetresValue();

	TReal cte = obsIt->getEstDistConst().getMetresValue();

	TReal p = dist(xSt, ySt, xFTg, yFTg);
	TReal D = dist(xFTg, yFTg, xSTg, ySTg);

	if (D < kSmallValue)
		throw std::logic_error("TLGCObsLSContributionGenerator::getOffsetToVerPlaneDsgnMxContributions: Division by zero because observation points have identical coordinates (D).");

	TReal l = (TAngle::aTan2( (xSt-xFTg), (ySt-yFTg) ) ).getRadiansValue();
	TReal L = (TAngle::aTan2((xSTg-xFTg), (ySTg-yFTg) )).getRadiansValue();
	TAngle angDif(L-l);
	TReal K = p*cosq( angDif.getRadiansValue() )/D; 

	//GET CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
	calcMeas = TLength( (sinq(L)*(ySt - yFTg) - cosq(L)*(xSt - xFTg)) - cte );

//	if (calcMeas.getMetresValue() < kSmallValue)
//		throw std::logic_error("TLGCObsLSContributionGenerator::getOffsetToVerPlaneDsgnMxContributions: Division by zero because observation points have identical coordinates (calcMeas).");

	//CALCULATION OF THE CONTRIBUTION IN LOCAL INSTRUMENT SYSTEM	
	a = -cosq(L);		//xSt coefficient
	b = sinq(L);			//ySt coefficient
	c = 0;				//zSt coefficient
	d = (1-K)*cosq(L);	//xFTg coefficient
	e = (K-1)*sinq(L);	//yFTg coefficient
	f = 0;				//zFTg coefficient
	g = K*cosq(L);		//xSTg coefficient
	h = -K*sinq(L);		//ySTg coefficient
	i = 0;				//zSTg coefficient

	// contributions for the coordinates
	stnCoordContrib = TFreeVector(a, b, c, TCoordSysFactory::k3DCartesian);//contribution for station
	tgt1CoordContrib = TFreeVector(d, e, f, TCoordSysFactory::k3DCartesian);//contribution for first target point
	tgt2CoordContrib = TFreeVector(g, h, i, TCoordSysFactory::k3DCartesian);//contribution for second target point

	//TRANSFORM CONTRIBUTIONS FROM LOCAL INSTRUMENT SYSTEM TO CCS, IF NECESSARY
	currentIter = this->fMLAtoCCSBegin;
	while (currentIter != this->fMLAtoCCSEnd )
	{
		(*currentIter)->transform(stnCoordContrib);
		(*currentIter)->transform(tgt1CoordContrib);
		(*currentIter)->transform(tgt2CoordContrib);
		currentIter++;
	}

	OffsetStnContrib  contrib = {calcMeas, stnCoordContrib, tgt1CoordContrib, tgt2CoordContrib};
	return contrib;
}



// return the calculated observation value and design matrix contributions 
// for the given Offset to Theodolite Plane observation with the current parameters 
TheoOffsetStnContrib  TLGCObsLSContributionGenerator::getOffsetToTheoPlaneDsgnMxContributions(const LSOffsetToTheoPlaneConstIter obsIt)
{
	TReal a,b,c;
	TLength calcMeas;
	TFreeVector coordContrib(TCoordSysFactory::k3DCartesian);
	TReal v0Contrib;
	TPositionVector station = obsIt->getStEstCoordinate();
	TPositionVector target = obsIt->getFirstTgEstCoordinate();
	vector<TARefFrameTransformation*>::iterator currentIter;
	string mlaName = obsIt->getStPointName();

	// If a new station is being processed initialise the transformation to the MLA system
	// corresponding to the station position
	if(fMLAName != mlaName)
	{
		setToMLATransformation(obsIt->getStEstCoordinate(), mlaName);
	}

	//TRANSFORM COORDINATES INTO LOCAL INSTRUMENT SYSTEM IF NECESSARY
	currentIter = this->fCCStoMLABegin;
	while (currentIter != this->fCCStoMLAEnd)
	{
		(*currentIter)->transform(station);
		(*currentIter)->transform(target);
		currentIter++;
	}

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = station.getX().getMetresValue();
	TReal ySt = station.getY().getMetresValue();

	TReal xTg = target.getX().getMetresValue();
	TReal yTg = target.getY().getMetresValue();

	TReal cte = obsIt->getEstDistConst().getMetresValue();

	TAngle gis = obsIt->getEstBearing();
	TReal L = gis.getRadiansValue();

	//CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
	calcMeas = TLength((sinq(L)*(ySt-yTg) - cosq(L)*(xSt-xTg)) - cte);	
	//CALCULATION OF THE CONTRIBUTION IN LOCAL INSTRUMENT SYSTEM
	// contributions for the station coordinates
	a = -1*cosq(L);// xSt coefficient
	b = sinq(L);//ySt coefficient
	c = 0;//zSt coefficient
	coordContrib = TFreeVector(a, b, c, TCoordSysFactory::k3DCartesian); //contribution for station
	
	//v0 coefficient
	v0Contrib = cosq(L)*(ySt-yTg) + sinq(L)*(xSt-xTg);


	//TRANSFORM CONTRIBUTIONS FROM LOCAL INSTRUMENT SYSTEM TO CCS, IF NECESSARY
	currentIter = this->fMLAtoCCSBegin;
	while (currentIter != this->fMLAtoCCSEnd )
	{
		(*currentIter)->transform(coordContrib);
		currentIter++;
	}

	TheoOffsetStnContrib  contrib = {calcMeas, coordContrib, v0Contrib};
	return contrib;
}



// return the calculated observation value and design matrix contributions 
// for the given Gyro Orientation observation with the current parameters 
pair<TAngle, TFreeVector>  TLGCObsLSContributionGenerator::getGyroOrieDsgnMxContributions(const LSGyroOrieConstIter obsIt)
{
	TReal a,b,c;
	TAngle calcMeas;
	TFreeVector coordContrib(TCoordSysFactory::k3DCartesian);
	TPositionVector station = obsIt->getStEstCoordinate();
	TPositionVector target = obsIt->getTgEstCoordinate();
	vector<TARefFrameTransformation*>::iterator currentIter;
	string laName = obsIt->getStPointName();

	// If a new station is being processed initialise the transformation to the MLA system
	// corresponding to the station position
	if(fLAName != laName)
	{
		setToLATransformation(obsIt->getStEstCoordinate(), laName);
	}

	//TRANSFORM COORDINATES INTO LOCAL INSTRUMENT SYSTEM IF NECESSARY
	currentIter = this->fCCStoMLABegin;
	while (currentIter != this->fCCStoMLAEnd)
	{
		(*currentIter)->transform(station);
		(*currentIter)->transform(target);
		currentIter++;
	}

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = station.getX().getMetresValue();
	TReal ySt = station.getY().getMetresValue();

	TReal xTg = target.getX().getMetresValue();
	TReal yTg = target.getY().getMetresValue();

	TReal Dist = powq(dist(xSt, ySt, xTg, yTg), 2);
	if (Dist < kSmallValue)
		throw std::logic_error("TLGCObsLSContributionGenerator::getGyroOrieDsgnMxContributions: Division by zero because observation points have identical coordinates.");


	TAngle cte = obsIt->getEstConstAngle();

	//CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
	calcMeas = TAngle::aTan2( (xTg - xSt),(yTg - ySt) ) - cte;

	//DETERMINE CONTRIBUTIONS IN LOCAL INSTRUMENT SYSTEM
	// station coordinate contributions
	a = -LITERAL(1.0) * (yTg - ySt)/Dist;//xSt coefficient
	b = (xTg - xSt)/Dist;//ySt coefficient
	c = 0;//zSt coefficient
 	coordContrib = TFreeVector(a, b, c, TCoordSysFactory::k3DCartesian);//station contribution


	//TRANSFORM CONTRIBUTIONS FROM LOCAL INSTRUMENT SYSTEM TO CCS, IF NECESSARY
	currentIter = this->fMLAtoCCSBegin;
	while (currentIter != this->fMLAtoCCSEnd )
	{
		(*currentIter)->transform(coordContrib);
		currentIter++;
	}

	return make_pair(calcMeas, coordContrib);
}



// return the calculated observation value and design matrix contributions 
// for the given Radial Offset Constraint observation with the current parameters 
pair<TLength, TFreeVector>  TLGCObsLSContributionGenerator::getRadOffCnstrDsgnMxContributions(const LSRadOffCnstrConstIter cnstrIt)
{
	TReal a,b,c;
	TLength calcMeas;
	TFreeVector coordContrib(TCoordSysFactory::k3DCartesian);
	TPositionVector point = cnstrIt->getPtEstCoordinate();
	TPositionVector prov = cnstrIt->getPtProvCoordinate();

	// contributions calculated simply in either the CCS or the local 
	// reference frame 
	// Therefore there are no transformations necessary
	TReal bear = cnstrIt->getBearing().getRadiansValue();

	//get calculate offset
	// gets the station's (estimated) coordinates
	TReal xe = point.getX().getMetresValue();
	TReal ye = point.getY().getMetresValue();

	TReal xp = prov.getX().getMetresValue();
	TReal yp = prov.getY().getMetresValue();
	
	calcMeas = TLength( -LITERAL(1.0) * sinq(bear) * (ye - yp) + cosq(bear) * (xe - xp) );


	//calculated contibutions in a local system
	a = -LITERAL(1.0) * cosq(bear);// xPt coefficient
	b = sinq(bear);//yPt coefficient
	c = LITERAL(0.0);//zPt coefficient
 	coordContrib = TFreeVector(a, b, c, TCoordSysFactory::k3DCartesian);//point contribution

	return make_pair(calcMeas, coordContrib);
}



// return the calculated constraint bearing value and design matrix contributions 
// for the given Orientation Constraint with the current parameters 
pair<TAngle, TFreeVector>  TLGCObsLSContributionGenerator::getOrientationCnstrDsgnMxContributions(const LSOrieCnstrConstIter obsIt)
{
	TReal a,b;
	TAngle calcBear;
	TFreeVector coordContrib(TCoordSysFactory::k3DCartesian);
	TPositionVector fixed = obsIt->getFixedEstCoordinate();
	TPositionVector ref = obsIt->getRefEstCoordinate();

	// contributions calculated simply in either the CCS or the local 
	// reference frame 
	// Therefore there are no transformations necessary

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xFix = fixed.getX().getMetresValue();
	TReal yFix = fixed.getY().getMetresValue();

	TReal xRef = ref.getX().getMetresValue();
	TReal yRef = ref.getY().getMetresValue();

	TReal D = dist(xFix, yFix, xRef, yRef);
	if (D < kSmallValue)
		throw std::logic_error("TLGCObsLSContributionGenerator::getOrientationCnstrDsgnMxContributions: Division by zero because observation points have identical coordinates.");

	//gets calc value and sigma
	calcBear = TAngle::aTan2( (xRef - xFix), (yRef - yFix) );

	//CALCULATION OF THE CONTRIBUTION IN LOCAL INSTRUMENT SYSTEM	
	a = -1 * (yRef - yFix) / powq(D, 2);//xFix coefficient
	b = (xRef - xFix) / powq(D, 2);//yFix coefficient
 	coordContrib = TFreeVector(a, b, LITERAL(0.0), TCoordSysFactory::k3DCartesian);//fixed point contribution
		
	return make_pair(calcBear, coordContrib);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////////////////////////////////////
