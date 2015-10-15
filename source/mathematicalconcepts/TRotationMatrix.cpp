// TRotationMatrix.cpp
//
/** Class for the matrices of rotation used for points transformations */
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
#include  "TVCoordinateSystem.h"
#include  "TPositionVector.h"
#include  "TFreeVector.h"
#include  "TRotationMatrix.h"
////////////////////////////////////////////////////////////////


//ClassImp(TRotationMatrix)


//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

TRotationMatrix::TRotationMatrix()
{//default constructor
	setCoordSys(TCoordSysFactory::k3DCartesian);
	identity();
	setStatus(kNull);
}


TRotationMatrix::TRotationMatrix(ERotationType kR, TReal omega, TReal phi, TReal kappa)
{//Constructor taking the radians value of the angles in the order phi-omega-kappa, and the coordinate system
setCoordSys(TCoordSysFactory::k3DCartesian);
setAllRotations(kR, omega, phi, kappa);
setStatus(kKnown);
}


TRotationMatrix::TRotationMatrix( const  TRotationMatrix& original )
{	// copy constructor
	(*this) = original;
}


TRotationMatrix::~TRotationMatrix()
{
}


//////////////////////////////////////////////////////////////////////
// Operator Functions
//////////////////////////////////////////////////////////////////////

TRotationMatrix&  TRotationMatrix::operator=(const TRotationMatrix& right)
{//Copy Assignment operator
	if (this != &right)
	{
		for (int i=0; i<3; i++)
		{
			for (int j=0; j<3; j++)
			{
				setC(i,j,right.getC(i,j));
			}
		}
	setCoordSys(right.getCoordSys());
	setStatus(right.getStatus());
	}
return *this;
}


TRotationMatrix TRotationMatrix::operator*( const TRotationMatrix& right) const
{//R2=R*R1
	TRotationMatrix resultat;
	resultat.setStatus(TVNumericValue::kNull);
	TANumericValue::EStatus status;
	status=this->testStatus(right);
	if (status!= kNull && testCoordSysCart(right.getCoordSys())==true)	
	{for (int i=0; i<3; i++)
		{
		for (int k=0; k<3 ;k++)
			{
			TReal a=0;
			for (int j=0; j<3; j++)
				{
				a=a+getC(i,j)*right.getC(j,k);
				resultat.setC(i,k,a);
				}
			}
		}
	resultat.setStatus(status);
	}
	return resultat;
}


TFreeVector TRotationMatrix::operator*(const TFreeVector& fv) const
{// V2=R*V1
	TFreeVector resultat (fv.getCoordSys());
	resultat.setStatus(TVNumericValue::kNull);
	TANumericValue::EStatus status;
	status=this->testStatus(fv);
	if (status!= kNull && testCoordSysCart(fv.getCoordSys()))
	{
		resultat.setX(fv.getX()*getC(0,0)+fv.getY()*getC(0,1)+fv.getZ()*getC(0,2));
		resultat.setY(fv.getX()*getC(1,0)+fv.getY()*getC(1,1)+fv.getZ()*getC(1,2));
		resultat.setZ(fv.getX()*getC(2,0)+fv.getY()*getC(2,1)+fv.getZ()*getC(2,2));
		resultat.setStatus(status);
	}
	return resultat;
}


TPositionVector TRotationMatrix::operator*(const TPositionVector& pv) const
{// V2=R*V1
	TPositionVector resultat (pv.getCoordSys());
	resultat.setStatus(TVNumericValue::kNull);
	TANumericValue::EStatus status;
	status=this->testStatus(pv);
	if (status!= kNull && testCoordSysCart(pv.getCoordSys())==true)
	{
		resultat.setX(pv.getX()*getC(0,0)+pv.getY()*getC(0,1)+pv.getZ()*getC(0,2));
		resultat.setY(pv.getX()*getC(1,0)+pv.getY()*getC(1,1)+pv.getZ()*getC(1,2));
		resultat.setZ(pv.getX()*getC(2,0)+pv.getY()*getC(2,1)+pv.getZ()*getC(2,2));
		resultat.setStatus(status);
	}
	return resultat;
}



//////////////////////////////////////////////////////////////////////
//Member Functions
//////////////////////////////////////////////////////////////////////
void TRotationMatrix::setCoordSys(TCoordSysFactory::ECoordSys en)
{//sets the coordinate system
	this->setCoordSysTRotation(this, en);
return;
}


bool TRotationMatrix::identity()
{//change this into  the identity matrix
return getCoordSysPtr()->identity(this);
}

bool TRotationMatrix::setAllRotations(ERotationType kR, TReal &om, TReal &p, TReal &k)
{//set the 3 rotations taking angles in the specified order
return getCoordSysPtr()->setAllRotations(this, kR, om, p, k);
}

TRotationMatrix TRotationMatrix::inverse()
{/// Return the inverse rotation
	TRotationMatrix  copy(*this);
	copy.invert();
	return copy;
}

Angles TRotationMatrix::getAngles(const ERotationType kR) const
{//computation of the angles from the matrix with choosen order for the rotations
	return getCoordSysPtr()->getAngles(this, kR);
}


bool TRotationMatrix::invert()
{//Overwrites this Matrix by its inverse
*this=this->transposed();
return true;
}


TRotationMatrix TRotationMatrix::transposed() const
{//return the tranposed matrix of this one

	TRotationMatrix resultat (*this);
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			resultat.setC(i,j, getC(j,i));
		}
	}
	return resultat;
}


TReal TRotationMatrix::getElt(const int i, const int j) const
{//set a element into a 3D matrix
return this->getCoordSysPtr()->getElt(this, i, j);
}


		
