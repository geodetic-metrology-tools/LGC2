//T3DCartesianCoordSys.h 
//
//Class for explain matrix and vector in a 3D cartesian
//Coordinate system 
//
//Copyright 2002, CERN, EST/SU. All rights reserved.
/////////////////////////////////////////////////////////

//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
#include "T3DCartesianCoordSys.h"
//
// other forward declarations

////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
///Public Functions for Vector
//////////////////////////////////////////////////////////////////


TScalar T3DCartesianCoordSys::getX(const TACoordSysVector* vec) const
{//get the X coordinate of a position vector or a free vector
	TScalar retVal;
	retVal.setValue(this->TACoordinateSystem::getX(vec, 0));
	return retVal;
}

TScalar T3DCartesianCoordSys::getY(const TACoordSysVector* vec) const
{//get the Y coordinate of a position vector or a free vector
	TScalar retVal;
	retVal.setValue(this->TACoordinateSystem::getX(vec, 1));
	return retVal;
}

TScalar T3DCartesianCoordSys::getZ(const TACoordSysVector* vec) const
{//get the Z coordinate of a position vector or a free vector
	TScalar retVal;
	retVal.setValue(this->TACoordinateSystem::getX(vec, 2));
	return retVal;
}

bool T3DCartesianCoordSys::setX(TACoordSysVector* vec, const TScalar& len)
{//set the X in meters Coordinate of a position vector or a free vector 
this->TACoordinateSystem::setX(vec,0,len.getValue());
return true; 
}

bool  T3DCartesianCoordSys::setY(TACoordSysVector* vec, const TScalar& len)
{//set the Y Coordinate of a position vector or a free vector, default def: return false
this->TACoordinateSystem::setX(vec,1,len.getValue());
return true;
}

bool  T3DCartesianCoordSys::setZ(TACoordSysVector* vec, const TScalar& len)
{//set the Z Coordinate of a position vector or a free vector, default def: return false
this->TACoordinateSystem::setX(vec,2,len.getValue());
return true;
}


//////////////////////////////////////////////////////////////////////////////////
///Public Functions for Matrix
//////////////////////////////////////////////////////////////////////////////////


bool T3DCartesianCoordSys::setAllRotations(TRotationMatrix* mx, TRotationMatrix::ERotationType kR, TReal &omega, TReal &phi, TReal &kappa)
{//create a TRotationMatrix default def: return false

	//computation of sine and cosine from the angle and setting the value to 0 when less than 2 epsilon
	TAngle om(omega), p(phi), k(kappa);
	TReal xcos, xsin, ycos, ysin, zcos, zsin;

	xcos = om.cosine();
	if (fabsq(xcos) <= 2*DBL_EPSILON)
	{xcos = 0;}

	xsin = om.sine();
	if (fabsq(xsin) <= 2*DBL_EPSILON)
	{xsin = 0;}

	ycos = p.cosine();
	if (fabsq(ycos) <= 2*DBL_EPSILON)
	{ycos = 0;}

	ysin = p.sine();
	if (fabsq(ysin) <= 2*DBL_EPSILON)
	{ysin = 0;}

	zcos = k.cosine();
	if (fabsq(zcos) <= 2*DBL_EPSILON)
	{zcos = 0;}

	zsin = k.sine();
	if (fabsq(zsin) <= 2*DBL_EPSILON)
	{zsin = 0;}

	
	// fill matrix in function of the rotation type
	switch (kR){
	case (TRotationMatrix::kRzyx):
	setC(mx, 0,0, zcos*ycos);
	setC(mx, 0,1, zsin*xcos+zcos*ysin*xsin);
	setC(mx, 0,2, zsin*xsin-zcos*ysin*xcos);
	setC(mx, 1,0, -zsin*ycos);
	setC(mx, 1,1, zcos*xcos-zsin*ysin*xsin);
	setC(mx, 1,2, zcos*xsin+zsin*ysin*xcos);
	setC(mx, 2,0, ysin);
	setC(mx, 2,1, -ycos*xsin);
	setC(mx, 2,2, ycos*xcos);
	//mx->setStatus(TVNumericValue::kFixed);
	break;

	case (TRotationMatrix::kRzxy):
	setC(mx, 0,0, ycos*zcos+zsin*xsin*ysin);
	setC(mx, 1,0, -zsin*ycos+zcos*xsin*ysin);
	setC(mx, 2,0, xcos*ysin);
	setC(mx, 0,1, zsin*xcos);
	setC(mx, 1,1, xcos*zcos);
	setC(mx, 2,1, -xsin);
	setC(mx, 0,2, -zcos*ysin+zsin*xsin*ycos);
	setC(mx, 1,2, zsin*ysin+zcos*xsin*ycos);
	setC(mx, 2,2, ycos*xcos);
	//mx->setStatus(TVNumericValue::kFixed);
	break;
	}
	return true;
}


struct Angles T3DCartesianCoordSys::getAngles(const TRotationMatrix* mx, const TRotationMatrix::ERotationType kR) const
{//get angles of a TRotationMatrix return a structure (omega, phi, kappa), default def return false
	TReal omega, phi, kappa;
	TAngle ang, om, p, k;
	Angles xyz;

	switch (kR){
	case (TRotationMatrix::kRzyx):
		{	
			omega = ang.aTan2(-getC(mx,2,1),getC(mx,2,2)).getRadiansValue();
			kappa = ang.aTan2(-getC(mx,1,0),getC(mx,0,0)).getRadiansValue();
			phi = ang.aTan2((getC(mx,2,0)),sqrtq(powq(getC(mx,0,0),2)+powq(getC(mx,1,0),2))).getRadiansValue();

			om.setRadiansValue(omega);
			p.setRadiansValue(phi);
			k.setRadiansValue(kappa);

			if ( getC(mx,2,2) == 0 || getC(mx,2,1) == 0) 
			{
				om.setStatus(TANumericValue::kNull);
			}
			else
			{
				om.setStatus(TANumericValue::kKnown);
			}

			if ( (getC(mx,0,0) == 0)  || ((getC(mx,2,0) == 0) & (getC(mx,1,0) == 0))) 
			{
				p.setStatus(TANumericValue::kNull);
			}
			else
			{
				p.setStatus(TANumericValue::kKnown);
			}

			if ( getC(mx,0,0) == 0  || getC(mx,1,0) == 0) 
			{
				k.setStatus(TANumericValue::kNull);
			}
			else
			{
				k.setStatus(TANumericValue::kKnown);
			}
		

			xyz.omega = om;
			xyz.phi = p;
			xyz.kappa = k;

		}
		break;



	case (TRotationMatrix::kRzxy):
		{
			phi = ang.aTan2(getC(mx,2,0),getC(mx,2,2)).getRadiansValue();
			omega = ang.aTan2((-getC(mx,2,1)),sqrtq(powq(getC(mx,1,1),2)+powq(getC(mx,0,1),2))).getRadiansValue();
			kappa = ang.aTan2(getC(mx,0,1),getC(mx,1,1)).getRadiansValue();

			om.setRadiansValue(omega);
			p.setRadiansValue(phi);
			k.setRadiansValue(kappa);
			

			if ( getC(mx,2,2) == 0  || getC(mx,2,0) == 0) 
			{
				om.setStatus(TANumericValue::kNull);
			}
			else
			{
				om.setStatus(TANumericValue::kKnown);
			}

			if ( (getC(mx,1,1) == 0)  || ((getC(mx,2,1) == 0) & (getC(mx,0,1) == 0))) 
			{
				p.setStatus(TANumericValue::kNull);
			}
			else
			{
				p.setStatus(TANumericValue::kKnown);
			}

			if ( getC(mx,1,1) == 0  || getC(mx,0,1) == 0) 
			{
				k.setStatus(TANumericValue::kNull);
			}
			else
			{
				k.setStatus(TANumericValue::kKnown);
			}
			
			
			xyz.omega=om;
			xyz.phi=p;
			xyz.kappa=k;
		}
		break;
	}
	return xyz;
}

	
bool T3DCartesianCoordSys::identity(TRotationMatrix* mx)
{//change a rotation matrix into the identity matrix, default def: return false
	for (int i = 0; i<3 ; i++)
		{
			for (int j=0; j<3; j++)
			{
				if (i == j)
				{setC(mx,i,j,LITERAL(1.0));}
				else
				{setC(mx,i,j,LITERAL(0.0));}
			}
		}
	mx->setStatus(TANumericValue::kKnown);
return true;
}


bool T3DCartesianCoordSys::setElt(T3DMatrix* mx, const int i, const int j, const TReal value)
{//set a element into a 3D matrix
setC(mx, i, j, value);
return true;
}


TReal T3DCartesianCoordSys::getElt(const T3DMatrix* mx, const int i, const int j) const
{//get a element from a 3D matrix
return getC(mx, i, j);
}


TReal T3DCartesianCoordSys::getElt(const TRotationMatrix* mx, const int i, const int j) const
{//get a element from a 3D matrix
return getC(mx, i, j);
}

//////////

//ClassImp(T3DCartesianCoordSys)



	


