// TPositionVector.cpp
//
/** 3D Vector used as a Spatial Position for stations or as translation vector in Helmert transformation */
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
//#include  "TLength.h"
#include  "TPositionVector.h"
#include  "TVCoordinateSystem.h"
////////////////////////////////////////////////////////////////


//ClassImp(TPositionVector)


//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

TPositionVector::TPositionVector(TCoordSysFactory::ECoordSys en)
{	// default constructor
	setCoordSys(en);
	setStatus(kNull); 
}

TPositionVector::TPositionVector(const TReal& x, const TReal& y, const TReal& z,TCoordSysFactory::ECoordSys en)
{
	setX(0, x);
	setX(1, y);
	setX(2, z);
	setCoordSys(en);
	setStatus(kKnown); 
}

/*TPositionVector::TPositionVector(const TReal& x, const TReal& y, const TReal&z)
{
	TLength X(x), Y(y),Z(z);
	fPositionVector[0] = X;
	fPositionVector[1] = Y;
	fPositionVector[2] = Z;
	setStatus(kKnown); 
}
*/

TPositionVector::TPositionVector(  const TPositionVector& original )
{	// copy constructor

	for (int i=0; i<3; i++)
	{
		setX(i, original.getX(i));
	}
	setCoordSys(original.getCoordSys());
	setStatus(original.getStatus());
}


TPositionVector::~TPositionVector()
{
}



//////////////////////////////////////////////////////////////////////
// operator Functions 
//////////////////////////////////////////////////////////////////////

//Equivalence Operator 
bool TPositionVector::operator==( const TPositionVector& right) const
{
	return TACoordSysVector::operator ==(right);
}


TPositionVector TPositionVector::operator+(const TFreeVector& second)
{//!add a PositionVector  and FreeVector, return a PositionVector
	TPositionVector resultat (getCoordSys());
	TANumericValue::EStatus status = this->testStatus(second);
	if ( status!= kNull && testCoordSysCart(second.getCoordSys())==true)
	{
		resultat.setX(getX() + second.getX());
		resultat.setY(getY() + second.getY());
		resultat.setZ(getZ() + second.getZ());
	}
	else{status=TVNumericValue::kNull;}
	resultat.setStatus(status);
	return resultat;		
}

TPositionVector TPositionVector::operator-(const TFreeVector& second)
{//!add a PositionVector  and FreeVector, return a PositionVector
	TPositionVector resultat (getCoordSys());
	TANumericValue::EStatus status = this->testStatus(second);
	if ( status!= kNull && testCoordSysCart(second.getCoordSys())==true)
	{
		resultat.setX(getX() - second.getX());
		resultat.setY(getY() - second.getY());
		resultat.setZ(getZ() - second.getZ());
	}
	else{status=TVNumericValue::kNull;}
	resultat.setStatus(status);
	return resultat;		
}


TPositionVector& TPositionVector::operator+=(const TFreeVector& second)
{//!add a PositionVector  and FreeVector, replace this
	*this=(*this)+second;
	return *this;
}

TPositionVector& TPositionVector::operator-=(const TFreeVector& second) {
	*this=(*this)-second;
	return *this;
}


TFreeVector TPositionVector::operator-( const TPositionVector& second)
{//!substract two TPositionVector objects and return a FreeVector
	TFreeVector resultat (getCoordSys());
	TANumericValue::EStatus status = this->testStatus(second);
	if ( status!= kNull && testCoordSysCart(second.getCoordSys())==true)
		{
		resultat.setX(getX() - second.getX());
		resultat.setY(getY() - second.getY());
		resultat.setZ(getZ() - second.getZ());
		}
	else{status=TVNumericValue::kNull;}
	resultat.setStatus(status);
	return resultat;
}


TPositionVector TPositionVector::operator*( const TDouble& factor)
{//!Multiplication by a TDouble object
	TPositionVector resultat (getCoordSys());
	TANumericValue::EStatus status = this->testStatus(factor);
	if ( status!= kNull)
	{
		TReal scalar = factor.getValue();
		resultat.setX( 0, scalar * getX(0) ); 
		resultat.setX( 1, scalar * getX(1) );
		resultat.setX( 2, scalar * getX(2) );
	}
	resultat.setStatus(status);
	return resultat;
}


TPositionVector TPositionVector::operator *( const TScalar& factor)
{//!Multiplication by a TScalar
	TPositionVector resultat (getCoordSys());
	TANumericValue::EStatus status = this->testStatus(factor);
	if ( status!= kNull)
	{
		TReal scalar = factor.getValue();
		resultat.setX( 0, scalar * getX(0) ); 
		resultat.setX( 1, scalar * getX(1) );
		resultat.setX( 2, scalar * getX(2) );
	}
	resultat.setStatus(status);
	return resultat;
}


TPositionVector TPositionVector::operator*(const TReal& factor)
{//!Multiplication by a TReal object
	TPositionVector resultat (getCoordSys());
	if ( this->isNull()== false)
		{
		resultat.setX(0, getX(0)*factor); 
		resultat.setX(1, getX(1)*factor);
		resultat.setX(2, getX(2)*factor);
		}
	resultat.setStatus(getStatus());
	return resultat;
}


TPositionVector & TPositionVector::operator *=( const TScalar& right)
{//Multiply this vector by a TScalar
	*this = (*this) * right;
	return (*this);
}


TPositionVector & TPositionVector::operator *=( const TReal& right)
{//Multiply this vector by a TScalar
	*this = (*this) * right;
	return (*this);
}


TPositionVector&  TPositionVector::operator=( const TPositionVector& right)
{//! Copy Assignment operator
	if (this != &right)
	{
		setX(0, right.getX(0));
		setX(1, right.getX(1));
		setX(2, right.getX(2));
	}
	setStatus(right.getStatus());
	setCoordSys(right.getCoordSys());
	return *this;
}


/////////////////////////////////////////////////////////////////////////////////////
///Member Function
/////////////////////////////////////////////////////////////////////////////////////

TScalar TPositionVector::dist(const TPositionVector& pv) const
{//!Give the distance between this and a second point defined by theirs position vector
	TFreeVector fv (getCoordSys());
	TPositionVector vect(*this);
	fv = vect - pv;
	return fv.length();
}


TScalar TPositionVector::getH() const
{//!get the H value of a TPositionVector if is defined
	return getCoordSysPtr()->getH(this);
}


TAngle TPositionVector::getLambdaEllipsoid() const
{//!get the lambda value of a TPositionVector if is defined
	return getCoordSysPtr()->getLambdaEllipsoid(this);
}


TAngle TPositionVector::getPhiEllipsoid() const
{//!get the phi value of a TPositionVector if is defined
	return getCoordSysPtr()->getPhiEllipsoid(this);
}


bool TPositionVector::setH(const TScalar& h)
{//!set the H coordinate of a vector in a specific Coordinate System return true if X is defined
return getCoordSysPtr()->setH(this, h);
}


bool TPositionVector::setLambdaEllipsoid(const TAngle& lambda)
{//!set the Lambda coordinate of a vector in a geodetic Coordinate System return true if X is defined
return getCoordSysPtr()->setLambdaEllipsoid(this, lambda);
}


bool TPositionVector::setPhiEllipsoid(const TAngle& phi)
{//!set the Phi coordinate of a vector in a geodetic Coordinate System return true if X is defined
return getCoordSysPtr()->setPhiEllipsoid(this, phi);
}






