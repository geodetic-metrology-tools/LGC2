// TLSLocalSysWTWMeasCG.cpp
//
/** Concrete Class for a LS contrib generator processing WTW distance measurements
 defined in a local system */
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include "TLSInputMatrices.h"

#include  "TLSLocalSysWTWMeasCG.h"
////////////////////////////////////////////////////////////////


//ClassImp(TLSLocalSysWTWMeasCG)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TLSLocalSysWTWMeasCG::TLSLocalSysWTWMeasCG()
{	// default constructor

}

/////////////////////////////////////////
// constructor setting the input matrices
/////////////////////////////////////////
TLSLocalSysWTWMeasCG::TLSLocalSysWTWMeasCG(TLSInputMatrices* lsim):
TALSWTWMeasContribGenerator(lsim){

}

// **This needs to be implemented**
TLSLocalSysWTWMeasCG::TLSLocalSysWTWMeasCG( const  TLSLocalSysWTWMeasCG& original ):
TALSWTWMeasContribGenerator(original)
{	// copy constructor

}


TLSLocalSysWTWMeasCG::~TLSLocalSysWTWMeasCG()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
// **This needs to be implemented**
TLSLocalSysWTWMeasCG&  TLSLocalSysWTWMeasCG::operator=(const TLSLocalSysWTWMeasCG& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		fInputMatrices = right.fInputMatrices;
		fS0APrioriScaleFactor = right.fS0APrioriScaleFactor;
		fCurrentStation = right.fCurrentStation;
	}
	return *this;
}


//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// calculates and adds the WTW measurement's contributions to the matrices
//////////////////////////////////////////////////////////////////////////
void TLSLocalSysWTWMeasCG::processWTWDistMeas(TLSCalcWTWStation::CalcWTWDistMeasIterator iwm){


	//Meaning of local variables : 
	//s1 : position of the measurement along wire 1, measured from the wire's first point
	//s2 : position of the measurement along wire 2, measured from the wire's first point
	//xc : abscissa of wire 1's first point
	//yc : ordinate of wire 1's first point
	//xd : abscissa of wire 1's second point
	//yd : ordinate of wire 1's second point
	//xe : abscissa of wire 2's first point
	//ye : ordinate of wire 2's first point
	//xf : abscissa of wire 2's second point
	//yf : ordinate of wire 2's second point
	//D : measured inter-wire distance
	//Dcd : planimetric distance from C to D (horizontal projection of the first wire)
	//Def : planimetric distance from E to F (horizontal projection of the second wire)

	//s1 : position of the measurement along wire 1, measured from the wire's first point
	double s1;
	s1 = (iwm->getWire1Pos()).getMetresValue();

	//s2 : position of the measurement along wire 2, measured from the wire's first point
	double s2;
	s2 = (iwm->getWire2Pos()).getMetresValue();

	//**Get the first wire's first end's coordinates, status and indices
	double xc, yc, zc;
	MatrixIndex xcInd, ycInd, zcInd;
	TSpatialPosition::ECoordStatus stsxc, stsyc, stszc;
	bool xck, yck, zck;

	xck = iwm->getFirstWire()->getFirstEnd()->getProvPosition()->knownXCoord();
	yck = iwm->getFirstWire()->getFirstEnd()->getProvPosition()->knownYCoord();
	zck = iwm->getFirstWire()->getFirstEnd()->getProvPosition()->knownZCoord();

	if (xck){
		//xc : abscissa of wire 1's first point
		xc = (iwm->getFirstWire()->getFirstEnd()->getProvPosition()->getXCoord()).getMetresValue();
		xcInd = iwm->getFirstWire()->getFirstEnd()->getXIndex();
		stsxc = iwm->getFirstWire()->getFirstEnd()->getProvPosition()->getXCoordStatus();
	}

	if (yck){
		//yc : ordinate of wire 1's first point
		yc = (iwm->getFirstWire()->getFirstEnd()->getProvPosition()->getYCoord()).getMetresValue();
		ycInd = iwm->getFirstWire()->getFirstEnd()->getYIndex();
		stsyc = iwm->getFirstWire()->getFirstEnd()->getProvPosition()->getYCoordStatus();
	}

	if (zck){
		//zc : z of wire 1's first point
		zc = (iwm->getFirstWire()->getFirstEnd()->getProvPosition()->getZCoord()).getMetresValue();
		zcInd = iwm->getFirstWire()->getFirstEnd()->getZIndex();
		stszc = iwm->getFirstWire()->getFirstEnd()->getProvPosition()->getZCoordStatus();
	}

	
	//**Get the first wire's second end's coordinates, status and indices
	double xd, yd, zd;
	MatrixIndex xdInd, ydInd, zdInd;
	TSpatialPosition::ECoordStatus stsxd, stsyd, stszd;
	bool xdk, ydk, zdk;

	xdk = iwm->getFirstWire()->getSecondEnd()->getProvPosition()->knownXCoord();
	ydk = iwm->getFirstWire()->getSecondEnd()->getProvPosition()->knownYCoord();
	zdk = iwm->getFirstWire()->getSecondEnd()->getProvPosition()->knownZCoord();

	if (xdk){
		//xd : abscissa of wire 1's second point
		xd = (iwm->getFirstWire()->getSecondEnd()->getProvPosition()->getXCoord()).getMetresValue();
		xdInd = iwm->getFirstWire()->getSecondEnd()->getXIndex();
		stsxd = iwm->getFirstWire()->getSecondEnd()->getProvPosition()->getXCoordStatus();
	}

	if (ydk){
		//yd : ordinate of wire 1's second point
		yd = (iwm->getFirstWire()->getSecondEnd()->getProvPosition()->getYCoord()).getMetresValue();
		ydInd = iwm->getFirstWire()->getSecondEnd()->getYIndex();
		stsyd = iwm->getFirstWire()->getSecondEnd()->getProvPosition()->getYCoordStatus();
	}

	if (zdk){
		//zd : z of wire 1's second point
		zd = (iwm->getFirstWire()->getSecondEnd()->getProvPosition()->getZCoord()).getMetresValue();
		zdInd = iwm->getFirstWire()->getSecondEnd()->getZIndex();
		stszd = iwm->getFirstWire()->getSecondEnd()->getProvPosition()->getZCoordStatus();
	}



	//**Get the second wire's first end's coordinates, status and indices
	double xe, ye, ze;
	MatrixIndex xeInd, yeInd, zeInd;
	TSpatialPosition::ECoordStatus stsxe, stsye, stsze;
	bool xek, yek, zek;

	xek = iwm->getSecondWire()->getFirstEnd()->getProvPosition()->knownXCoord();
	yek = iwm->getSecondWire()->getFirstEnd()->getProvPosition()->knownYCoord();
	zek = iwm->getSecondWire()->getFirstEnd()->getProvPosition()->knownZCoord();

	if (xek){
		//xe : abscissa of wire 2's first point
		xe = (iwm->getSecondWire()->getFirstEnd()->getProvPosition()->getXCoord()).getMetresValue();
		xeInd = iwm->getSecondWire()->getFirstEnd()->getXIndex();
		stsxe = iwm->getSecondWire()->getFirstEnd()->getProvPosition()->getXCoordStatus();
	}

	if (yek){
		//ye : ordinate of wire 2's first point
		ye = (iwm->getSecondWire()->getFirstEnd()->getProvPosition()->getYCoord()).getMetresValue();
		yeInd = iwm->getSecondWire()->getFirstEnd()->getYIndex();
		stsye = iwm->getSecondWire()->getFirstEnd()->getProvPosition()->getYCoordStatus();
	}

	if (zek){
		//ze : z of wire 2's first point
		ze = (iwm->getSecondWire()->getFirstEnd()->getProvPosition()->getZCoord()).getMetresValue();
		zeInd = iwm->getSecondWire()->getFirstEnd()->getZIndex();
		stsze = iwm->getSecondWire()->getFirstEnd()->getProvPosition()->getZCoordStatus();
	}



	//**Get the second wire's second end's coordinates, status and indices
	double xf, yf, zf;
	MatrixIndex xfInd, yfInd, zfInd;
	TSpatialPosition::ECoordStatus stsxf, stsyf, stszf;
	bool xfk, yfk, zfk;

	xfk = iwm->getSecondWire()->getSecondEnd()->getProvPosition()->knownXCoord();
	yfk = iwm->getSecondWire()->getSecondEnd()->getProvPosition()->knownYCoord();
	zfk = iwm->getSecondWire()->getSecondEnd()->getProvPosition()->knownZCoord();

	if (xfk){
		//xf : abscissa of wire 1's second point
		xf = (iwm->getSecondWire()->getSecondEnd()->getProvPosition()->getXCoord()).getMetresValue();
		xfInd = iwm->getSecondWire()->getSecondEnd()->getXIndex();
		stsxf = iwm->getSecondWire()->getSecondEnd()->getProvPosition()->getXCoordStatus();
	}

	if (yfk){
		//yf : ordinate of wire 1's second point
		yf = (iwm->getSecondWire()->getSecondEnd()->getProvPosition()->getYCoord()).getMetresValue();
		yfInd = iwm->getSecondWire()->getSecondEnd()->getYIndex();
		stsyf = iwm->getSecondWire()->getSecondEnd()->getProvPosition()->getYCoordStatus();
	}

	if (zfk){
		//zf : z of wire 1's second point
		zf = (iwm->getSecondWire()->getSecondEnd()->getProvPosition()->getZCoord()).getMetresValue();
		zfInd = iwm->getSecondWire()->getSecondEnd()->getZIndex();
		stszf = iwm->getSecondWire()->getSecondEnd()->getProvPosition()->getZCoordStatus();
	}




	//Dcd : planimetric distance from C to D (horizontal projection of the first wire)
	double Dcd = dist(TLength(xc), TLength(yc), TLength(xd), TLength(yd));

	//Def : planimetric distance from E to F (horizontal projection of the second wire)
	double Def = dist(TLength(xe), TLength(ye), TLength(xf), TLength(yf));

	//DcdV : vertical plane distance from C to D (vertical projection of the first wire)
	double DcdV = dist(TLength(xc), TLength(zc), TLength(xd), TLength(zd));

	//DefV : vertical plane distance from E to F (vertical projection of the first wire)
	double DefV = dist(TLength(xe), TLength(ze), TLength(xf), TLength(zf));


	//A temporary variable used to make expressions more readable
	double A = (s1*(xd-xc)/Dcd + xc) - (s2*(xf-xe)/Def + xe);

	//B temporary variable used to make expressions more readable
	double B = (s1*(yd-yc)/Dcd + yc) - (s2*(yf-ye)/Def + ye);

	//Av temporary variable used to make expressions more readable
	double Av = (s1*(xd-xc)/DcdV + xc) - (s2*(xf-xe)/DefV + xe);

	//Bv temporary variable used to make expressions more readable
	double Bv = (s1*(zd-zc)/DcdV + zc) - (s2*(zf-ze)/DefV + ze);



	//get the horizontal observation's index
	MatrixIndex hDistOInd = iwm->getHDObsIndex();

	//get the vertical observation's index
	MatrixIndex vDistOInd = iwm->getVDObsIndex();

	//**Checks if the HDist's observation's contributions can be calculated
	// if yes, calculates them
	if ( xck && yck && xdk && ydk && xek && yek && xfk && yfk && (hDistOInd != 0)){

		//Calculated interwire horizontal distance
		double calcHDist = sqrt(pow(A,2) + pow(B,2));

		//horizontal distance's sigma a priori
		double sigmaH = (iwm->getHDistSigmaAPriori()).getMetresValue();

		//**gets the Hdist's provisinal value
		//checks if the observed value has to be simulated
		double obsHDist;
		if (fSimulate && (iwm->firstIteration())){
			obsHDist = getSimulatedValue(calcHDist, sigmaH);
			TLength temp(obsHDist);
			iwm->setSimulatedObsHDist(temp);
		}
		else
			obsHDist = (iwm->getObsHDist()).getMetresValue();

		//**Adding the HDist's contributions to the first design matrix
		//xc coefficient
		if (stsxc == TANumericValue::kVariable){
			double a = A*( -1 + pow((xd-xc),2)/pow(Dcd,2) + Dcd/s1);
			a += B*( (yd-yc)*(xd-xc) / pow(Dcd,2));
			a *= s1/(calcHDist * Dcd);
			fInputMatrices->setFirstDgnMtrxElement(hDistOInd, xcInd, a);
		}

		//yc coefficient
		if (stsyc == TANumericValue::kVariable){
			double b = A*( (xd-xc)*(yd-yc) / pow(Dcd,2));
			b += B*( -1 + pow((yd-yc),2)/pow(Dcd,2) + Dcd/s1);
			b *= s1/(calcHDist * Dcd);
			fInputMatrices->setFirstDgnMtrxElement(hDistOInd, ycInd, b);
		}

		//xd coefficient
		if (stsxd == TANumericValue::kVariable){
			double c = A*( 1 - pow((xd-xc),2)/pow(Dcd,2) );
			c += -B*( (yd-yc)*(xd-xc) / pow(Dcd,2));
			c *= s1/(calcHDist * Dcd);
			fInputMatrices->setFirstDgnMtrxElement(hDistOInd, xdInd, c);
		}

		//yd coefficient
		if (stsyd == TANumericValue::kVariable){
			double d = -A*( (xd-xc)*(yd-yc) / pow(Dcd,2));
			d += B*( 1 - pow((yd-yc),2)/pow(Dcd,2) );
			d *= s1/(calcHDist * Dcd);
			fInputMatrices->setFirstDgnMtrxElement(hDistOInd, ydInd, d);
		}

		//xe coefficient
		if (stsxe == TANumericValue::kVariable){
			double e = A*( 1 - pow((xf-xe),2)/pow(Def,2) - Def/s2 );
			e += -B*( (yf-ye)*(xf-xe) / pow(Def,2) );
			e *= s2/(calcHDist * Def);
			fInputMatrices->setFirstDgnMtrxElement(hDistOInd, xeInd, e);
		}

		//ye coefficient
		if (stsye == TANumericValue::kVariable){
			double f = -A*( (xf-xe)*(yf-ye)/pow(Def,2) );
			f += B*( 1 - pow((yf-ye),2)/pow(Def,2) - Def/s2 );
			f *= s2/(calcHDist * Def);
			fInputMatrices->setFirstDgnMtrxElement(hDistOInd, yeInd, f);
		}

		//xf coefficient
		if (stsxf == TANumericValue::kVariable){
			double g = A*( -1 + pow((xf-xe),2)/pow(Def,2) );
			g += B*( (yf-ye)*(xf-xe) / pow(Def,2) );
			g *= s2/(calcHDist * Def);
			fInputMatrices->setFirstDgnMtrxElement(hDistOInd, xfInd, g);
		}

		//yf coefficient
		if (stsyf == TANumericValue::kVariable){
			double h = A*( (xf-xe)*(yf-ye)/pow(Def,2) );
			h += B*( -1 + pow((yf-ye),2)/pow(Def,2) );
			h *= s2/(calcHDist * Def);
			fInputMatrices->setFirstDgnMtrxElement(hDistOInd, yfInd, h);
		}



		//** Adding the contribution to the second design matrix
		fInputMatrices->setSecondDgnMtrxElement(hDistOInd, hDistOInd, -1);


		//** setting the misclosure vector element
		double k = calcHDist - obsHDist;
		fInputMatrices->setMisclosureVectorElement(hDistOInd, k);

		
		//** adding the contributions to the weight matrix
		fInputMatrices->setWeightMtrxElement(hDistOInd, hDistOInd, pow(fS0APrioriScaleFactor,2)/pow(sigmaH,2));
	}


	//**Checks if the VDist's observation's contributions can be calculated
	// if yes, calculates them
	if ( xck && zck && xdk && zdk && xek && zek && xfk && zfk && (vDistOInd != 0)){

		//Calculated interwire vertical distance
		double calcVDist = sqrt(pow(Av,2) + pow(Bv,2));

		//vertical distance's sigma a priori
		double sigmaV = (iwm->getVDistSigmaAPriori()).getMetresValue();

		//**gets the Vdist's provisional value
		//checks if the observed value has to be simulated
		double obsVDist;
		if (fSimulate && (iwm->firstIteration())){
			obsVDist = getSimulatedValue(calcVDist, sigmaV);
			TLength temp(obsVDist);
			iwm->setSimulatedObsVDist(temp);
		}
		else
			obsVDist = (iwm->getObsVDist()).getMetresValue();

		//**Adding the VDist's contributions to the first design matrix
		//xc coefficient
		if (stsxc == TANumericValue::kVariable){
			double a = Av*( -1 + pow((xd-xc),2)/pow(DcdV,2) + DcdV/s1);
			a += Bv*( (zd-zc)*(xd-xc) / pow(DcdV,2));
			a *= s1/(calcVDist * DcdV);
			fInputMatrices->setFirstDgnMtrxElement(vDistOInd, xcInd, a);
		}

		//zc coefficient
		if (stszc == TANumericValue::kVariable){
			double b = Av*( (xd-xc)*(zd-zc) / pow(DcdV,2));
			b += Bv*( -1 + pow((zd-zc),2)/pow(DcdV,2) + DcdV/s1);
			b *= s1/(calcVDist * DcdV);
			fInputMatrices->setFirstDgnMtrxElement(vDistOInd, zcInd, b);
		}

		//xd coefficient
		if (stsxd == TANumericValue::kVariable){
			double c = Av*( 1 - pow((xd-xc),2)/pow(DcdV,2) );
			c += -Bv*( (zd-zc)*(xd-xc) / pow(DcdV,2));
			c *= s1/(calcVDist * DcdV);
			fInputMatrices->setFirstDgnMtrxElement(vDistOInd, xdInd, c);
		}

		//zd coefficient
		if (stszd == TANumericValue::kVariable){
			double d = -Av*( (xd-xc)*(zd-zc) / pow(DcdV,2));
			d += Bv*( 1 - pow((zd-zc),2)/pow(DcdV,2) );
			d *= s1/(calcVDist * DcdV);
			fInputMatrices->setFirstDgnMtrxElement(vDistOInd, zdInd, d);
		}

		//xe coefficient
		if (stsxe == TANumericValue::kVariable){
			double e = Av*( 1 - pow((xf-xe),2)/pow(DefV,2) - DefV/s2 );
			e += -Bv*( (zf-ze)*(xf-xe) / pow(DefV,2) );
			e *= s2/(calcVDist * DefV);
			fInputMatrices->setFirstDgnMtrxElement(vDistOInd, xeInd, e);
		}

		//ze coefficient
		if (stsze == TANumericValue::kVariable){
			double f = -Av*( (xf-xe)*(zf-ze)/pow(DefV,2) );
			f += Bv*( 1 - pow((zf-ze),2)/pow(DefV,2) - DefV/s2 );
			f *= s2/(calcVDist * DefV);
			fInputMatrices->setFirstDgnMtrxElement(vDistOInd, zeInd, f);
		}

		//xf coefficient
		if (stsxf == TANumericValue::kVariable){
			double g = Av*( -1 + pow((xf-xe),2)/pow(DefV,2) );
			g += Bv*( (zf-ze)*(xf-xe) / pow(DefV,2) );
			g *= s2/(calcVDist * DefV);
			fInputMatrices->setFirstDgnMtrxElement(vDistOInd, xfInd, g);
		}

		//zf coefficient
		if (stszf == TANumericValue::kVariable){
			double h = Av*( (xf-xe)*(zf-ze)/pow(DefV,2) );
			h += Bv*( -1 + pow((zf-ze),2)/pow(DefV,2) );
			h *= s2/(calcVDist * DefV);
			fInputMatrices->setFirstDgnMtrxElement(vDistOInd, zfInd, h);
		}



		//** Adding the contribution to the second design matrix
		fInputMatrices->setSecondDgnMtrxElement(vDistOInd, vDistOInd, -1);


		//** setting the misclosure vector element
		double k = calcVDist - obsVDist;
		fInputMatrices->setMisclosureVectorElement(vDistOInd, k);

		
		//** adding the contributions to the weight matrix
		fInputMatrices->setWeightMtrxElement(vDistOInd, vDistOInd, pow(fS0APrioriScaleFactor,2)/pow(sigmaV,2));
	}


}
