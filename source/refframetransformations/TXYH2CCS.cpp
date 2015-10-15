#include "TXYH2CCS.h"
#include "GeodeticConstants.h"
#include "TSpatialPosition.h"
#include "TAGeoidModel.h"
#include "TARefFrameTransformation.h"
#include "TXYHe2MLATransformation.h"

bool  TXYH2CCS::XYHs2CCS(TPositionVector& pv)
{// Transformation of a position vector using the parameters of the two reference frames
	TReal dx, dy, hs, d;
	TReal d0, omega;
	// radius of the sphere
	TReal R = LITERAL(6371000.0);
	// transform TPositionVector
	TReal Z;

	// distance from P0 (CCS false origin) and XY-plane
	dx = pv.getX().getValue() - XP0;
	dy = pv.getY().getValue() - YP0;
	hs = pv.getH().getValue();
	d=sqrtq( (powq(dx,2)) + (powq(dy,2)) );

	//Z = sqrtq( (powq((R+hs),2) - (powq(d,2))) )-  R + LITERAL(2000.00079);
	d0 = d * R / (R + hs);
	omega = asinq(d0 / R);
	Z = LITERAL(2000.00079) + (hs * cosq(omega)) - (d0 * tanq(omega/LITERAL(2.0)));

	TScalar newZ (Z);

	// change the coordinate system of the position vector
	pv.setCoordSys(TCoordSysFactory::k3DCartesian);

	// change the H coordinate of the position vector into the third coordinate Z
	pv.setZ( newZ ); 

	return true;
}

bool  TXYH2CCS::CCS2XYHs(TPositionVector& pv)
{
	TReal dx, dy, dz, d;
	TReal d0, omega;
	// radius of the sphere
	TReal R = 6371000;	
	// transform TPositionVector
	TReal H;

	//distance from P0 in XY-plane
	dx = pv.getX().getValue() - XP0;
	dy = pv.getY().getValue() - YP0;
	dz = pv.getZ().getValue() - ZP0 + HP0;
	d=sqrtq( (powq(dx,2)) + (powq(dy,2)) );

	omega = atanq(d / (R+dz));
	d0 = d * R * cosq(omega) / (R + dz);
	H = ( dz + (d0 * tanq(omega/LITERAL(2.0))) ) / cosq(omega);

	TScalar newH (H);
	// Change the coordinate system of the position vector
	pv.setCoordSys(TCoordSysFactory::k2DPlusH);   
	// Change the third dimension Z of the position vector into H
	pv.setH(newH);

	return true;
}

bool	TXYH2CCS::XYHg2000Machine2CCS( TPositionVector& pv ){
	XYHg2XYHe(pv,TRefSystemFactory::EGeoid::kCG2000Machine);
	XYHe2CCS(pv);
	return true;
}

bool   TXYH2CCS::CCS2XYHg2000Machine( TPositionVector& pv ){
	CCS2XYHe(pv);
	XYHe2XYHg(pv,TRefSystemFactory::EGeoid::kCG2000Machine);
	return true;
}

bool	TXYH2CCS::XYHg1985Machine2CCS( TPositionVector& pv ){
	XYHg2XYHe(pv,TRefSystemFactory::EGeoid::kCG1985Machine);
	XYHe2CCS(pv);
	return true;
}

bool   TXYH2CCS::CCS2XYHg1985Machine( TPositionVector& pv ){
	CCS2XYHe(pv);
	XYHe2XYHg(pv,TRefSystemFactory::EGeoid::kCG1985Machine);
	return true;
}

bool  TXYH2CCS::XYHe2CCS(TPositionVector& pv) {

	TReal dx, dy, he, d, d0, Dzh;

	// distance from P0 in XY-plane
	dx = pv.getX().getValue() - XP0;
	dy = pv.getY().getValue() - YP0;
	he = pv.getH().getValue();
	d  = sqrtq( (powq(dx,2)) + (powq(dy,2)) );

	// bearing from P0 in the XY-plane
	TReal beta = atan2q(dx,dy);
	//Azimuth from P0
	TReal AzimuthRAD =  AzimuthCCSYaxis * M_PI/LITERAL(200);
	TReal azimuth =  beta + AzimuthRAD;

	// radius of the ellipsoid at azimuth and at phiP0
	TReal PhiP0RAD = PHIP0 * M_PI/LITERAL(200);
	TReal nuP0  = SemiMajorAxisGRS80 / (sqrtq(1.0 - eccentricity*powq(sinq(PhiP0RAD),2)));
	TReal secondEccentricity = eccentricity / (LITERAL(1.0) - eccentricity);

	TReal rhoalpha = nuP0 / (1 + secondEccentricity
		*powq( (cosq(PhiP0RAD)) * (cosq(azimuth)) ,2 ) );

	// angle between point and z-axis
	TReal omega = asinq( d / (rhoalpha + he) );
	// scale factor
	TReal k = rhoalpha  / (rhoalpha + he);
	d0 = k*d;

	// transform TPositionVector
	Dzh = ZP0 - HP0;
	TReal omegaBy2 = omega * LITERAL(0.5);
	TScalar newZ(Dzh + he * cosq(omega)
		- d0 * (tanq(omegaBy2)));

	// change the coordinate system of the position vector
	pv.setCoordSys(TCoordSysFactory::k3DCartesian);
	pv.setZ( newZ ); 
	return true;
}


bool TXYH2CCS::CCS2XYHe(TPositionVector& pv){
	TReal dx, dy, dz, d, d0;

	// distance from P0 in XY-plane
	dx = pv.getX().getValue() - XP0;
	dy = pv.getY().getValue() - YP0;
	dz = pv.getZ().getValue() - ZP0 + HP0;
	d = sqrtq( (powq(dx,2)) + (powq(dy,2)) );

	// bearing from P0 in the XY-plane
	TReal beta = atan2q(dx,dy);
	//Azimuth from P0
	TReal AzimuthRAD =  AzimuthCCSYaxis * M_PI/LITERAL(200);
	TReal azimuth =  beta + AzimuthRAD;
	
	// radius of the ellipsoid at azimuth and at phiP0
	TReal PhiP0RAD = PHIP0 * M_PI/LITERAL(200);
	TReal nuP0  = SemiMajorAxisGRS80 / (sqrtq(1.0 - eccentricity*powq(sinq(PhiP0RAD),2)));
	TReal secondEccentricity = eccentricity / (LITERAL(1.0) - eccentricity);

	TReal rhoalpha = nuP0 / (1 + secondEccentricity
		*powq( (cosq(PhiP0RAD)) * (cosq(azimuth)) ,2 ) );

	// angle between point and z-axis
	TReal omega = atanq( d / (rhoalpha + dz) );
	// scale factor
	TReal k = ( rhoalpha * cosq(omega) ) / (rhoalpha + dz);
	d0 = k*d;

	// transform TPositionVector
	TReal omegaBy2 = omega * LITERAL(0.5);
	TScalar newH( (dz + d0 * tanq(omegaBy2)) / cosq(omega) );
	
	// Change the coordinate system of the position vector
	pv.setCoordSys(TCoordSysFactory::k2DPlusH);             
	pv.setH(newH);
	return true;
}


bool  TXYH2CCS::XYHg2XYHe(TPositionVector& pv, TRefSystemFactory::EGeoid geoid)
{// Transformation of a position vector using N (height above geoid)
	TScalar hg(pv.getH());
	TReal x, y, h;
	x = pv.getX().getValue();
	y = pv.getY().getValue();
	h = hg.getValue();

	// extraction of N using a tspatialposition of coordinates = pv, and reference frame = CCS
	TSpatialPosition spaPos(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS), x, y, h, pv.getCoordSys()); 
	TAGeoidModel* geoidModel = TRefSystemFactory::getRefSystemFactory()->getGeoid(geoid);

	TLength N = geoidModel->getN(spaPos);
	pv.setH(hg + TScalar(N.getMetresValue()));
	return true;
}


bool TXYH2CCS::XYHe2XYHg(TPositionVector& pv, TRefSystemFactory::EGeoid geoid){
	TScalar he(pv.getH());
	TReal x, y, h;
	x = pv.getX().getValue();
	y = pv.getY().getValue();
	h = he.getValue();

	// extraction of N using a tspatialposition of coordinates = pv, and reference frame = CCS
	TSpatialPosition spaPos(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS), x, y, h, pv.getCoordSys()); 
	TAGeoidModel* geoidModel = TRefSystemFactory::getRefSystemFactory()->getGeoid(geoid);

	TLength N = geoidModel->getN(spaPos);
	pv.setH(he - TScalar(N.getMetresValue()));
	return true;
}