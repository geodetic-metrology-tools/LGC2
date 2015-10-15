//TLGCObsLSContributionGenerator.cpp : implementation file
//Class for a LGC Observation LS contrib generator
//
// Copyright 2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include "TLGCObsLSContributionGenerator.h"
#include "lsalgo/TLSInputMatrices.h"
#include "TModifiedLocalAstronomicalRF.h"
#include "TGC2MLATransformation.h"
#include "TMLA2GCTransformation.h"
#include "TReferenceEllipsoid.h"
#include "TRefSystemFactory.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////////////
TLGCObsLSContributionGenerator::TLGCObsLSContributionGenerator(TRefSystemFactory::ERefFrame refSur)
{
	fLAName = "NULL";
	fMLAName = "NULL";
	TRefSystemFactory::ERefFrame  fRefSystem = refSur;

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
		TRefSystemFactory::EGeoid fGeoid;
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


PolarContrib	TLGCObsLSContributionGenerator::getPolarDsgnMxContributions(const LSPolarConstIter obsIt)//, bool rot3D)
{
	TPositionVector station = obsIt->getHorizontalAngleObservation()->getStEstCoordinate();
	TPositionVector target = obsIt->getHorizontalAngleObservation()->getTgEstCoordinate();

	vector<TARefFrameTransformation*>::iterator currentIter;
	string mlaName = obsIt->getHorizontalAngleObservation()->getStPointName();

	// If a new station is being processed initialise the transformation to the MLA system
	// corresponding to the station position
	if(fMLAName != mlaName)
	{
		setToMLATransformation(station, mlaName);
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
	TReal dx = target.getX().getMetresValue() - station.getX().getMetresValue();
	TReal dy = target.getY().getMetresValue() - station.getY().getMetresValue();
	TReal dz = target.getZ().getMetresValue() - station.getZ().getMetresValue() -
		obsIt->getZenithDistanceObservation()->getHInstrument()->getEstimatedValue().getMetresValue() +
		obsIt->getZenithDistanceObservation()->getHPrism().getMetresValue();

	TReal r11, r12, r13, r21, r22, r23, r31, r32, r33;

	Angles rotAnglesContrib[3];
	TReal cosK = obsIt->getHorizontalAngleObservation()->getV0()->getEstimatedValue().kappa.cosine();
	TReal sinK = obsIt->getHorizontalAngleObservation()->getV0()->getEstimatedValue().kappa.sine();
	TReal cosF = obsIt->getHorizontalAngleObservation()->getV0()->getEstimatedValue().phi.cosine();
	TReal sinF = obsIt->getHorizontalAngleObservation()->getV0()->getEstimatedValue().phi.sine();
	TReal cosW = obsIt->getHorizontalAngleObservation()->getV0()->getEstimatedValue().omega.cosine();
	TReal sinW = obsIt->getHorizontalAngleObservation()->getV0()->getEstimatedValue().omega.sine();
	r11 = cosF * cosK;
	r12 = -cosF * sinK;
	r13 = sinF;
	r21 = cosW * sinK + sinW * sinF * cosK;
	r22 = cosW * cosK - sinW * sinF * sinK;
	r23 = -sinW * cosF;
	r31 = sinW * sinK - cosW * sinF * cosK;
	r32 = sinW * cosK + cosW * sinF * sinK;
	r33 = cosW * cosF;
	
	rotAnglesContrib[0].kappa = TAngle(-cosF * sinK * dx + (cosW * cosK - sinW * sinF * sinK) * dy +
		(sinW * cosK + cosW * sinF * sinK) * dz);
	rotAnglesContrib[1].kappa = TAngle(-cosF * cosK * dx - (sinW * sinF * cosK + cosW * sinK) * dy +
		(cosW * sinF * cosK - sinW * sinK) * dz);
	rotAnglesContrib[2].kappa = TAngle(0);
	rotAnglesContrib[0].phi = TAngle(-sinF * cosK * dx + sinW * cosF * cosK * dy - cosW * cosF * cosK * dz);
	rotAnglesContrib[1].phi = TAngle(sinF * sinK * dx - sinW * cosF * sinK * dy + cosW * cosF * sinK * dz);
	rotAnglesContrib[2].phi = TAngle(cosF * dx + sinW * sinF * dy - cosW * sinF * dz);
	rotAnglesContrib[0].omega = TAngle((cosW * sinF * cosK - sinW * sinK) * dy + (cosW * sinK + sinW
		* sinF * cosK) * dz);
	rotAnglesContrib[1].omega = TAngle(-(sinW * cosK + cosW * sinF * sinK) * dy + (cosW * cosK -
		sinW * sinF * sinK) * dz);
	rotAnglesContrib[2].omega = TAngle(-cosW * cosF * dy - sinW * cosF * dz);

	TFreeVector coordContrib1 = TFreeVector(r11, r21, r31, TCoordSysFactory::k3DCartesian);
	TFreeVector coordContrib2 = TFreeVector(r12, r22, r32, TCoordSysFactory::k3DCartesian);
	TFreeVector coordContrib3 = TFreeVector(r13, r23, r33, TCoordSysFactory::k3DCartesian);
	////TRANSFORM CONTRIBUTIONS FROM LOCAL INSTRUMENT SYSTEM TO CCS, IF NECESSARY
	currentIter = this->fMLAtoCCSBegin;
	while (currentIter != this->fMLAtoCCSEnd )
	{
		(*currentIter)->transform(coordContrib1);
		(*currentIter)->transform(coordContrib2);
		(*currentIter)->transform(coordContrib3);
		currentIter++;
	}

	const TAngle& ha = obsIt->getHorizontalAngleObservation()->getEstimatedAngle();
	const TAngle& zd = obsIt->getZenithDistanceObservation()->getEstimatedAngle();
	const TLength& sd = obsIt->getSpatialDistanceObservation()->getEstDist();

	TReal zdSine = zd.sine();
	TReal zdCos = zd.cosine();
	TReal haSine = ha.sine();
	TReal haCos = ha.cosine();

	TReal a = r11 * dx + r21 * dy + r31 * dz;
	TReal b = r12 * dx + r22 * dy + r32 * dz;
	TReal c = r13 * dx + r23 * dy + r33 * dz;

	TReal haCalc = -atan2q(b, a);
	TReal zdCalc = atan2q(a, cosq(haCalc) * c);
	TReal sdCalc = c / cosq(zdCalc);

	PolarContrib res = { haCalc, zdCalc, sdCalc,
						 { sd.getMetresValue() * zdSine * haSine, // haContrib
						   sd.getMetresValue() * zdSine * haCos,
						   0 },
						 { -sd.getMetresValue() * zdCos * haCos, // zdContrib
						   sd.getMetresValue() * zdCos * haSine,
						   sd.getMetresValue() * zdSine },
						 { -zdSine * haCos, // sdContrib
						   zdSine * haSine,
						   -zdCos },
						 { a - sd.getMetresValue() * zdSine * haCos, // miscVecElements
						   b + sd.getMetresValue() * zdSine * haSine,
						   c - sd.getMetresValue() * zdCos },
						 { coordContrib1, // fCoordContrib
						   coordContrib2,
						   coordContrib3 },
						 { rotAnglesContrib[0], // rotAnglesContrib
						   rotAnglesContrib[1],
						   rotAnglesContrib[2] } };
	return res;
}

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

	//CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
	calcMeas = TAngle::aCos((zTg - zSt - hi + hp)/
									dist3D(xSt, ySt, zSt+hi, xTg, yTg, zTg+hp));

	//CALCULATION OF THE CONTRIBUTION IN INSTRUMENT LOCAL SYSTEM	
	sinV = sinq(calcMeas.getRadiansValue());
	
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
// for the given Vertical Distance observation with the current parameters 
LevelStnContrib	TLGCObsLSContributionGenerator::getVertDistDsgnMxContributions(const LSVertDistConstIter obsIt)
{
	TReal a,b,c,d,e,f;
	TLength calcMeas;
	TFreeVector refCoordContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector tgtCoordContrib(TCoordSysFactory::k3DCartesian);
	TPositionVector ref = obsIt->getTg1EstCoordinate();
	TPositionVector target = obsIt->getTg2EstCoordinate();
	vector<TARefFrameTransformation*>::iterator currentIter;
	TReal cte = obsIt->getEstDistConst().getMetresValue();
	TReal zOrHRef, zOrHTg;
	TReal dz1, dz2;

	dz1 = ref.getZ().getMetresValue();
	dz2 = target.getZ().getMetresValue();

	//TRANSFORM COORDINATES INTO XYH SYSTEM IF NECESSARY
	// the transformations are applied from the end
	currentIter = this->fCCStoXYHEnd;
	while (currentIter != this->fCCStoXYHBegin)
	{
		currentIter--;
		(*currentIter)->transform(ref);
		(*currentIter)->transform(target);
	}

	// compute the contributions as a function of the type of reference frame for the
	// calculation (OLOC or Geodetic)
	if( fGeoRefFrame == TRefSystemFactory::kNotInGraph )
	{
		// OLOC System
		// heights given by Z-values
		zOrHRef = ref.getZ().getMetresValue();
		zOrHTg = target.getZ().getMetresValue();

		// Contributions
		a = 0;
		b = 0;
		c = -1;// zRef coefficient
		d = 0;
		e = 0;
		f = 1;//zTg coefficient
	}
	else
	{
		// Geodetic System
		//difference in Z incomparison with P0
		dz1 -= ZP0;
		dz2 -= ZP0;

		// height of points taken
		zOrHRef = ref.getH().getMetresValue();
		zOrHTg = target.getH().getMetresValue();

		//contributions for geodetic coordinate system 
		TAngle az1, az2, om1, om2, phi;
		TReal dx1, dy1, dx2, dy2, r1, r2, D01, D02, ak1, ak2, phiP0;

		phiP0 = PHIP0;
		phi.setGonsValue( phiP0 );

		dx1 = ref.getX().getMetresValue() - XP0;
		dx2 = target.getX().getMetresValue() - XP0;
		dy1 = ref.getY().getMetresValue() - YP0;
		dy2 = target.getY().getMetresValue() - YP0;

		if (dy1 == LITERAL(0.0))
		{
			az1.setGonsValue(LITERAL(137.77864));
		}
		else
		{
			az1.setGonsValue(LITERAL(37.77864));
			az1 += TAngle::aTan2(dx1,dy1);
		}

		//if (dx1 == LITERAL(0.0)) //????? was dshould be dy2
		if (dy2 == LITERAL(0.0))
		{
			az2.setGonsValue(LITERAL(137.77864));
		}
		else
		{
			az2.setGonsValue(LITERAL(37.77864));
			az2 += TAngle::aTan2(dx2,dy2);
		}

		r1 = this->fEllipsoid->getEuler(phi, az1);
		r2 = this->fEllipsoid->getEuler(phi, az2);

		D01 = sqrtq(powq(dx1,2) + powq(dy1,2));
		D02 = sqrtq(powq(dx2,2) + powq((dy2),2));

		//dz1 = zOrHRef- HP0; //????? these are really dZ in the Fortran program!
		//dz2 = zOrHTg - HP0; //????? what is the effect

		om1 = TAngle::aTan2(D01, (r1+dz1));  
		om2 = TAngle::aTan2(D02, (r2+dz2));  
		
		ak1 = r1*om1.cosine()/(r1+dz1);  
		ak2 = r2*om2.cosine()/(r2+dz2);

		if(D01 == LITERAL(0.0))
		{
			a = LITERAL(0.0);
			b = LITERAL(0.0);
		}
		else
		{
			a = (-ak1*(LITERAL(0.5)*om1).tangent()*dx1)/(D01*om1.cosine());
			b = (-ak1*(LITERAL(0.5)*om1).tangent()*dy1)/(D01*om1.cosine());
		}

		if(D02 == LITERAL(0.0))
		{
			d = LITERAL(0.0);
			e = LITERAL(0.0);
		}
		else
		{
			d = (ak2*(LITERAL(0.5)*om2).tangent()*dx2)/(D02*om2.cosine());
			e = (ak2*(LITERAL(0.5)*om2).tangent()*dy2)/(D02*om2.cosine());
		}
		c = -1/om1.cosine();
		f = 1/om2.cosine();
	}

	//GET CALCULATED VALUE
	calcMeas = TLength( zOrHTg - zOrHRef - cte );

	// contributions for the coordinates
	refCoordContrib = TFreeVector(a, b, c, TCoordSysFactory::k3DCartesian);
	tgtCoordContrib = TFreeVector(d, e, f, TCoordSysFactory::k3DCartesian);

	LevelStnContrib  contrib = {calcMeas, refCoordContrib, tgtCoordContrib};
	return contrib;
}

LevelStnContrib	TLGCObsLSContributionGenerator::getLevelObsDsgnMxContributions(const LSLevelConstIter obsIt)
{
	TLength calcMeas;
	TFreeVector refCoordContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector tgtCoordContrib(TCoordSysFactory::k3DCartesian);
	TPositionVector station = obsIt->getStEstCoordinate();
	TPositionVector target = obsIt->getTgEstCoordinate();
	TReal cte = obsIt->getEstDistConst().getMetresValue();
	TReal th = obsIt->getLevelObs()->getTargetHeight()->getMetresValue();
	vector<TARefFrameTransformation*>::iterator currentIter;
	string mlaName = obsIt->getStPointName();

	// If a new station is being processed initialise the transformation to the MLA system
	// corresponding to the station position
	if(fMLAName != mlaName)
	{
		setToMLATransformation(station, mlaName);
	}

	//TRANSFORM COORDINATES INTO LOCAL INSTRUMENT SYSTEM IF NECESSARY
	currentIter = this->fCCStoMLABegin;
	while (currentIter != this->fCCStoMLAEnd)
	{
		(*currentIter)->transform(target);
		(*currentIter)->transform(station);
		currentIter++;
	}

	//GET CALCULATED VALUE
	calcMeas = TLength( target.getZ().getMetresValue() - station.getZ().getMetresValue() - cte - th);

	// contributions for the coordinates
	refCoordContrib = TFreeVector(0, 0, -1, TCoordSysFactory::k3DCartesian);
	tgtCoordContrib = TFreeVector(0, 0, 1, TCoordSysFactory::k3DCartesian);
	
	//TRANSFORM CONTRIBUTIONS FROM LOCAL INSTRUMENT SYSTEM TO CCS, IF NECESSARY
	currentIter = this->fMLAtoCCSBegin;
	while (currentIter != this->fMLAtoCCSEnd )
	{
		(*currentIter)->transform(refCoordContrib);
		currentIter++;
	}

	LevelStnContrib  contrib = {calcMeas, refCoordContrib, tgtCoordContrib};
	return contrib;
}


// return the calculated observation value and design matrix contributions 
// for the given Offset to Vertical Line observation with the current parameters 
OffsetToVerLineContrib	TLGCObsLSContributionGenerator::getOffsetToVerLineDsgnMxContributions(const LSOffsetToVerLineConstIter obsIt)
{
	// the equation here (combined case) is just the horizontal distance between the target point
	// and the vertical line (which is represented by "ptOnLine"):
	// (targetX - ptOnLineX) ^ 2 + (targetY - ptOnLineY) ^ 2 - (meas + const + offset) ^ 2 = 0
	TReal a,b,c;
	TLength calcMeas;
	TFreeVector coordContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector ptOnLineContrib(TCoordSysFactory::k3DCartesian);
	TPositionVector target = obsIt->getTgEstCoordinate();
	TPositionVector ptOnLine = obsIt->getPointOnLine()->getEstimatedValue();
	vector<TARefFrameTransformation*>::iterator currentIter;
	string mlaName = obsIt->getPointOnLine()->getName();

	// If a new station is being processed initialise the transformation to the MLA system
	// corresponding to the station position
	if(fMLAName != mlaName)
	{
		setToMLATransformation(ptOnLine, mlaName);
	}

	//TRANSFORM COORDINATES INTO LOCAL INSTRUMENT SYSTEM IF NECESSARY
	currentIter = this->fCCStoMLABegin;
	while (currentIter != this->fCCStoMLAEnd)
	{
		(*currentIter)->transform(target);
		(*currentIter)->transform(ptOnLine);
		currentIter++;
	}

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xTg = target.getX().getMetresValue();
	TReal yTg = target.getY().getMetresValue();

	TReal xPtOnLine = ptOnLine.getX().getMetresValue();
	TReal yPtOnLine = ptOnLine.getY().getMetresValue();

	TReal cte = obsIt->getEstDistConst().getMetresValue();
	TReal tgOffset = obsIt->getOffsetToLineObs()->getScale()->getTargetOffset()->getMetresValue();

	TReal dx = xTg - xPtOnLine;
	TReal dy = yTg - yPtOnLine;

	TReal root = sqrtq(powq(dx, 2) + powq(dy, 2));
	//CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
	calcMeas = TLength(root - cte - tgOffset);

	//CALCULATION OF THE CONTRIBUTION IN INSTRUMENT LOCAL SYSTEM	
	a = 2 * dx;// xTg coefficient
	b = 2 * dy;// yTg coefficient
	c = LITERAL(0.0); 
	coordContrib = TFreeVector(a, b, c, TCoordSysFactory::k3DCartesian); /*contribution for target*/
	ptOnLineContrib = TFreeVector(-a, -b, -c, TCoordSysFactory::k3DCartesian);

	TReal temp = obsIt->getEstimatedDist().getMetresValue() + cte + tgOffset;
	TReal secondDesignMatrixContrib = -2 * temp;

	TReal miscVec = dx * dx + dy * dy - temp * temp;


	//TRANSFORM CONTRIBUTIONS FROM LOCAL INSTRUMENT SYSTEM TO CCS, IF NECESSARY
	currentIter = this->fMLAtoCCSBegin;
	while (currentIter != this->fMLAtoCCSEnd )
	{
		(*currentIter)->transform(coordContrib);
		(*currentIter)->transform(ptOnLineContrib);
		currentIter++;
	}

	OffsetToVerLineContrib contrib = { calcMeas, coordContrib, ptOnLineContrib, secondDesignMatrixContrib, miscVec };

	return contrib;
}



// return the calculated observation value and design matrix contributions 
// for the given Offset to Spatial Line observation with the current parameters 
OffsetStnContrib  TLGCObsLSContributionGenerator::getOffsetToSpaLineDsgnMxContributions(const LSOffsetToSpaLineConstIter obsIt)
{
	// the equation here (combined case) comes from a dot product between the vector between
	// the target and ptOnLine, and the unit vector on the line. After that, using Pythagorean theorem, comes:
	// |Target - ptOnLine| ^ 2 - ((Target - ptOnLine) * unitVector) ^ 2 = (meas + const + offset) ^ 2
	//
	// In the two points case (i.e. when you have two known points on the line), the equation becomes:
	// |Target - ptOnLine| ^ 2 - ((Target - ptOnLine) * (secondPtOnLine - ptOnLine) / |secondPtOnLine - ptOnLine|) ^ 2 = (meas + const + offset) ^ 2
	TReal xPtOnLineC, yPtOnLineC, zPtOnLineC, xTgC, yTgC, zTgC, xUnC, yUnC, zUnC, secondDesignMatrixContrib;
	TLength calcMeas;
	TFreeVector tgCoordContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector originCoordContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector unitVectorCoordContrib(TCoordSysFactory::k3DCartesian);
	TPositionVector target = obsIt->getTgEstCoordinate();
	TPositionVector pointOnLine = obsIt->getPointOnLine()->getEstimatedValue();
	if (obsIt->getUnitVector()->getName() == "two points")
	{
		TPositionVector f = obsIt->getPointOnLine()->getEstimatedValue();
		TPositionVector s = obsIt->getSecondPointOnLine()->getEstimatedValue();

		TFreeVector u = s - f;
		u *= 1 / u.length().getMetresValue();
		obsIt->getUnitVector()->setEstimatedValueVector(u);
	}
	else
	{
		TFreeVector u = obsIt->getUnitVector()->getEstimatedValue();
		u *= 1 / u.length().getMetresValue();
		obsIt->getUnitVector()->setEstimatedValueVector(u);
	}
	TFreeVector unitVector = obsIt->getUnitVector()->getEstimatedValue();

	// contributions can be calculated simply in both the CCS and a local 
	// reference frame since no instrument or target heights are involved
	// and the measurement is independent of the local vertical
	// Therefore there are no transformations necessary
	//PARAMETERS IN CARTESIAN REFERENCE SYSTEM
	TReal xTg = target.getX().getMetresValue();
	TReal yTg = target.getY().getMetresValue();
	TReal zTg = target.getZ().getMetresValue();

	TReal xPtOnLine = pointOnLine.getX().getMetresValue();
	TReal yPtOnLine = pointOnLine.getY().getMetresValue();
	TReal zPtOnLine = pointOnLine.getZ().getMetresValue();

	TReal xUn = unitVector.getX().getMetresValue();
	TReal yUn = unitVector.getY().getMetresValue();
	TReal zUn = unitVector.getZ().getMetresValue();

	TReal cte = obsIt->getEstDistConst().getMetresValue();
	TReal tgOffset = obsIt->getOffsetToLineObs()->getScale()->getTargetOffset()->getMetresValue();

	TReal tgXMinusPtOnLineX = xTg - xPtOnLine;
	TReal tgXMinusPtOnLineXSquared = tgXMinusPtOnLineX * tgXMinusPtOnLineX;
	TReal tgYMinusPtOnLineY = yTg - yPtOnLine;
	TReal tgYMinusPtOnLineYSquared = tgYMinusPtOnLineY * tgYMinusPtOnLineY;
	TReal tgZMinusPtOnLineZ = zTg - zPtOnLine;
	TReal tgZMinusPtOnLineZSquared = tgZMinusPtOnLineZ * tgZMinusPtOnLineZ;

	TReal tgMinusPtOnLineSquared = tgXMinusPtOnLineXSquared + tgYMinusPtOnLineYSquared + tgZMinusPtOnLineZSquared;
	TReal tgMinusPtOnLineTimesUn = tgXMinusPtOnLineX * xUn +
		tgYMinusPtOnLineY * yUn + tgZMinusPtOnLineZ * zUn; // dot product

	calcMeas = TLength(sqrtq(tgMinusPtOnLineSquared - tgMinusPtOnLineTimesUn * tgMinusPtOnLineTimesUn) - cte - tgOffset);

	if (obsIt->getUnitVector()->getName() == "two points")
	{
		TReal sX = obsIt->getSecondPointOnLine()->getEstimatedValue().getX().getMetresValue();
		TReal sY = obsIt->getSecondPointOnLine()->getEstimatedValue().getY().getMetresValue();
		TReal sZ = obsIt->getSecondPointOnLine()->getEstimatedValue().getZ().getMetresValue();
		TFreeVector sndMinusFst = obsIt->getSecondPointOnLine()->getEstimatedValue() - pointOnLine;
		TReal lenSquared = powq(sndMinusFst.length().getMetresValue(), 2);

		TReal dx = sX - xPtOnLine;
		TReal dy = sY - yPtOnLine;
		TReal dz = sZ - zPtOnLine;

		TReal tgMinusPtOnLineTimesSndMinusFirst = tgXMinusPtOnLineX * dx + tgYMinusPtOnLineY * dy + tgZMinusPtOnLineZ * dz;
		TReal mult = -2 * tgMinusPtOnLineTimesSndMinusFirst;

		TReal estDistSquared = powq(obsIt->getEstimatedDist().getMetresValue() + cte + tgOffset, 2);

		xUnC = 2 * dx * (tgMinusPtOnLineSquared - estDistSquared) +	mult * tgXMinusPtOnLineX; // second point's contributions
		yUnC = 2 * dy * (tgMinusPtOnLineSquared - estDistSquared) +	mult * tgYMinusPtOnLineY;
		zUnC = 2 * dz * (tgMinusPtOnLineSquared - estDistSquared) +	mult * tgZMinusPtOnLineZ;

		xTgC = 2 * tgXMinusPtOnLineX * lenSquared + mult * dx;
		yTgC = 2 * tgYMinusPtOnLineY * lenSquared + mult * dy;
		zTgC = 2 * tgZMinusPtOnLineZ * lenSquared + mult * dz;

		xPtOnLineC = -2 * tgXMinusPtOnLineX * lenSquared -
			2 * dx * (tgMinusPtOnLineSquared - estDistSquared) +
			mult * (2 * xPtOnLine - sX - xTg);
		yPtOnLineC = -2 * tgYMinusPtOnLineY * lenSquared -
			2 * dy * (tgMinusPtOnLineSquared - estDistSquared) +
			mult * (2 * yPtOnLine - sY - yTg);
		zPtOnLineC = -2 * tgZMinusPtOnLineZ * lenSquared -
			2 * dz * (tgMinusPtOnLineSquared - estDistSquared) +
			mult * (2 * zPtOnLine - sZ - zTg);

		secondDesignMatrixContrib = -2 * lenSquared * (obsIt->getEstimatedDist().getMetresValue() + cte + tgOffset);
	}
	else
	{
		TReal mult = -2 * tgMinusPtOnLineTimesUn;

		xUnC = mult * tgXMinusPtOnLineX;
		yUnC = mult * tgYMinusPtOnLineY;
		zUnC = mult * tgZMinusPtOnLineZ;

		xTgC = 2 * tgXMinusPtOnLineX + mult * xUn;
		yTgC = 2 * tgYMinusPtOnLineY + mult * yUn;
		zTgC = 2 * tgZMinusPtOnLineZ + mult * zUn;

		xPtOnLineC = -2 * tgXMinusPtOnLineX - mult * xUn;
		yPtOnLineC = -2 * tgYMinusPtOnLineY - mult * yUn;
		zPtOnLineC = -2 * tgZMinusPtOnLineZ - mult * zUn;

		secondDesignMatrixContrib = -2 * (obsIt->getEstimatedDist().getMetresValue() + cte + tgOffset);
	}

	TReal temp = obsIt->getEstimatedDist().getMetresValue() + cte + tgOffset;
	TReal miscVecContrib = tgMinusPtOnLineSquared - tgMinusPtOnLineTimesUn * tgMinusPtOnLineTimesUn - temp * temp;

	tgCoordContrib = TFreeVector(xTgC, yTgC, zTgC, TCoordSysFactory::k3DCartesian);
	originCoordContrib = TFreeVector(xPtOnLineC, yPtOnLineC, zPtOnLineC, TCoordSysFactory::k3DCartesian);
	unitVectorCoordContrib = TFreeVector(xUnC, yUnC, zUnC, TCoordSysFactory::k3DCartesian);

	OffsetStnContrib  contrib = {calcMeas, tgCoordContrib, originCoordContrib, unitVectorCoordContrib, TLength(secondDesignMatrixContrib), miscVecContrib};
	return contrib;
}



// return the calculated observation value and design matrix contributions 
// for the given Offset to Vertical Plane observation with the current parameters 
OffsetStnContrib  TLGCObsLSContributionGenerator::getOffsetToVerPlaneDsgnMxContributions(const LSOffsetToVerPlaneConstIter obsIt)
{
	// The equation (observation equation) comes from a dot product between the plane's unit vector
	// and the vector between the Target and the Origin. Subtracting the distance from the Origin
	// to the plane, we get the calculated value:
	// (Target - Origin) <dot_product> unit - distFromOrigin
	TReal a,b,c, d, g, h, i;
	TLength calcMeas;
	TFreeVector tgCoordContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector originCoordContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector planeCoordContrib(TCoordSysFactory::k3DCartesian);
	TPositionVector target = obsIt->getMeasuredPointEstCoordinate();
	TPositionVector origin = obsIt->getOriginPointCoordinates();
	TFreeVector normal = obsIt->getPlaneNormalVector()->getEstimatedValue();
	vector<TARefFrameTransformation*>::iterator currentIter;
	string mlaName = obsIt->getOriginPointDistanceFromPlaneUnknown()->getName();

	// If a new station is being processed initialise the transformation to the MLA system
	// corresponding to the station position
	if(fMLAName != mlaName)
	{
		setToMLATransformation(origin, mlaName);
	}

	//TRANSFORM COORDINATES INTO LOCAL INSTRUMENT SYSTEM IF NECESSARY
	currentIter = this->fCCStoMLABegin;
	while (currentIter != this->fCCStoMLAEnd)
	{
		(*currentIter)->transform(target);
		(*currentIter)->transform(normal);
		currentIter++;
	}

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xTg = target.getX().getMetresValue();
	TReal yTg = target.getY().getMetresValue();

	TReal aPl = normal.getX().getMetresValue();
	TReal bPl = normal.getY().getMetresValue();

	TReal cte = obsIt->getEstDistConst().getMetresValue();
	TReal tgOffset = obsIt->getOffsetToVerPlaneObs()->getScale()->getTargetOffset()->getMetresValue();


	//GET CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
	calcMeas = TLength(aPl * xTg + bPl * yTg - cte - tgOffset + obsIt->getOriginPointDistanceFromPlane().getMetresValue());


	//CALCULATION OF THE CONTRIBUTION IN LOCAL INSTRUMENT SYSTEM	
	a = aPl;			//xTg coefficient
	b = bPl;			//yTg coefficient
	c = 0;				//zTg coefficient
	g = xTg;		//xPl coefficient
	h = yTg;		//yPl coefficient
	i = 0;				//zPl coefficient
	d = 1;				//dist coefficient

	// contributions for the coordinates
	tgCoordContrib = TFreeVector(a, b, c, TCoordSysFactory::k3DCartesian);//contribution for target
	originCoordContrib = TFreeVector(0, 0, 0, TCoordSysFactory::k3DCartesian);
	planeCoordContrib = TFreeVector(g, h, i, TCoordSysFactory::k3DCartesian);

	//TRANSFORM CONTRIBUTIONS FROM LOCAL INSTRUMENT SYSTEM TO CCS, IF NECESSARY
	currentIter = this->fMLAtoCCSBegin;
	while (currentIter != this->fMLAtoCCSEnd )
	{
		(*currentIter)->transform(tgCoordContrib);
		(*currentIter)->transform(planeCoordContrib);
		currentIter++;
	}

	OffsetStnContrib  contrib = {calcMeas, tgCoordContrib, originCoordContrib, planeCoordContrib, TLength(d)};
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

	//PARAMETERS IN LOCAL INSTRUMENT SYTEM
	TReal xSt = station.getX().getMetresValue();
	TReal ySt = station.getY().getMetresValue();

	TReal xTg = target.getX().getMetresValue();
	TReal yTg = target.getY().getMetresValue();

	TReal cte = obsIt->getEstDistConst().getMetresValue();
	TReal tgOffset = obsIt->getOffsetToTheoPlaneObs()->getScale()->getTargetOffset()->getMetresValue();

	TAngle gis = obsIt->getEstBearing();
	TReal L = gis.getRadiansValue();

	//CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
	calcMeas = TLength((sinq(L)*(ySt-yTg) - cosq(L)*(xSt-xTg)) - cte - tgOffset);	


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

	//gets calc value and sigma
	calcBear = TAngle::aTan2( (xRef - xFix), (yRef - yFix) );

	//CALCULATION OF THE CONTRIBUTION IN LOCAL INSTRUMENT SYSTEM	
	a = -1 * (yRef - yFix) / powq(D, 2);//xFix coefficient
	b = (xRef - xFix) / powq(D, 2);//yFix coefficient
 	coordContrib = TFreeVector(a, b, LITERAL(0.0), TCoordSysFactory::k3DCartesian);//fixed point contribution
		
	return make_pair(calcBear, coordContrib);
}

OffsetSpatialLineConstraint TLGCObsLSContributionGenerator::getOffSpaLineCnstrDsgnMxContributions(const LSOffsetToSpaLineCstrConstIter obsIt)
{
	// there are two constraints:
	// that the point on the line is not "moving" away from the origin, that is:
	// (origin - point_on_line) <dot_product> unit_vector == 0
	// the second constraint is that the unit vector stays unit:
	// a ^ 2 + b ^ 2 + c ^ 2 == 1

	TFreeVector unitContrib1(TCoordSysFactory::k3DCartesian);
	TFreeVector unitContrib2(TCoordSysFactory::k3DCartesian);
	LSPosVecIter pointOnLine = obsIt->getPointOnLine();
	LSFreeVecIter unit = obsIt->getUnitVector();
	LSPosVecIter origin = obsIt->getOrigin();

	// contributions calculated simply in either the CCS or the local 
	// reference frame 
	// Therefore there are no transformations necessary

	TReal xUn = unit->getEstimatedValue().getX().getMetresValue();
	TReal yUn = unit->getEstimatedValue().getY().getMetresValue();
	TReal zUn = unit->getEstimatedValue().getZ().getMetresValue();

	TFreeVector orMinusPoint = origin->getEstimatedValue() - pointOnLine->getEstimatedValue();
	unitContrib1 = TFreeVector(orMinusPoint.getX().getMetresValue(),
		orMinusPoint.getY().getMetresValue(),
		orMinusPoint.getZ().getMetresValue(),
		TCoordSysFactory::k3DCartesian);

	TReal res1 = orMinusPoint.getX().getMetresValue() * xUn + 
				 orMinusPoint.getY().getMetresValue() * yUn +
				 orMinusPoint.getZ().getMetresValue() * zUn;

	// second constraint:

	unitContrib2 = TFreeVector(2 * xUn,	2 * yUn, 2 * zUn, TCoordSysFactory::k3DCartesian);

	TReal res2 = xUn * xUn + yUn * yUn + zUn * zUn - 1;

	OffsetSpatialLineConstraint ret = { res1, unit->getEstimatedValue() * -1, unitContrib1, res2, unitContrib2 };
	return ret;
}

pair<pair<TReal, TFreeVector>, pair<TReal, TFreeVector> >  TLGCObsLSContributionGenerator::getOffVerPlaneCnstrDsgnMxContributions(const LSOffsetToVerPlaneCstrConstIter obsIt)
{
	// practically there are two constraints - the first one being that the unit vector should STAY unit:
	// i.e. a ^ 2 + b ^ 2 + c ^ 2 == 1
	// the second constraint is that this unit vector should be horizontal in the local system
	// i.e. unit <dot_product> (0, 0, 1) == 0

	TFreeVector unitContrib(TCoordSysFactory::k3DCartesian);
	TFreeVector cContrib(TCoordSysFactory::k3DCartesian);
	LSFreeVecIter unit = obsIt->getUnitVector();
	LSPosVecIter origin = obsIt->getOrigin();
	TFreeVector fv(0, 0, 1, TCoordSysFactory::k3DCartesian);

	setToMLATransformation(origin->getEstimatedValue(), "temp");
	
	// we transform the local (0, 0, 1) vector to CCS, so we can work with it there
	vector<TARefFrameTransformation*>::iterator currentIter;
	currentIter = this->fMLAtoCCSBegin;
	while (currentIter != this->fMLAtoCCSEnd )
	{
		(*currentIter)->transform(fv);
		currentIter++;
	}

	TReal xUn = unit->getEstimatedValue().getX().getMetresValue();
	TReal yUn = unit->getEstimatedValue().getY().getMetresValue();
	TReal zUn = unit->getEstimatedValue().getZ().getMetresValue();

	TReal xFv = fv.getX().getMetresValue();
	TReal yFv = fv.getY().getMetresValue();
	TReal zFv = fv.getZ().getMetresValue();

	unitContrib = TFreeVector(2 * xUn, 2 * yUn,	2 * zUn, TCoordSysFactory::k3DCartesian);

	TReal miscVecEl1 = xUn * xUn + yUn * yUn	+ zUn * zUn - 1;
	TReal miscVecEl2 = xUn * xFv + yUn * yFv + zUn * zFv;
	return make_pair(make_pair(miscVecEl1, unitContrib), make_pair(miscVecEl2, fv));
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////////////////////////////////////
