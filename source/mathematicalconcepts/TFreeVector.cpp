// TFreeVector.cpp
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
#include  "TFreeVector.h"
#include  "TVCoordinateSystem.h"
#include  "TDouble.h"
#include  "TScalar.h"
////////////////////////////////////////////////////////////////


//ClassImp(TFreeVector)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TFreeVector::TFreeVector(TCoordSysFactory::ECoordSys en)
{	// default constructor
	
	setCoordSys(en);
	setStatus(kNull);
}

TFreeVector::TFreeVector(const TReal& x, const TReal& y, const TReal&z, TCoordSysFactory::ECoordSys en)
{
	setX(0, x);
	setX(1, y);
	setX(2, z);
	setCoordSys(en);
	setStatus(kKnown);
}

/*TFreeVector::TFreeVector(const TReal& x, const TReal& y, const TReal&z)
{
	setX(0, x);
	setX(1, y);
	setX(2, z);
	setStatus(kKnown);
}
*/

TFreeVector::TFreeVector(  const TFreeVector& original )
{	// copy constructor
	setX(0, original.getX(0));
	setX(1, original.getX(1));
	setX(2, original.getX(2));
	setCoordSys(original.getCoordSys());
	setStatus(original.getStatus());
}


TFreeVector::~TFreeVector()
{
}


//////////////////////////////////////////////////////////////////////
// operator Functions 
//////////////////////////////////////////////////////////////////////

//Equivalence Operator 
bool TFreeVector::operator==( const TFreeVector& right) const
{
	return TACoordSysVector::operator ==(right);
}


TFreeVector TFreeVector::operator+( const TFreeVector& second)
{//!Add two FreeVector
	TFreeVector resultat (getCoordSys());
	TANumericValue::EStatus status = this->testStatus(second);
	if (status != kNull && testCoordSysCart(second.getCoordSys())==true)
	{	resultat.setX(0, getX(0) + second.getX(0));
		resultat.setX(1, getX(1) + second.getX(1));
		resultat.setX(2, getX(2) + second.getX(2));
	
	}
	else{status=TVNumericValue::kNull;}
	resultat.setStatus(status);
	return resultat;
}

	
TFreeVector& TFreeVector::operator+=( const TFreeVector& second)
{//!Add two FreeVector and replace *this
	*this=(*this)+second;
	return (*this);
}



TFreeVector TFreeVector::operator-( const TFreeVector& second)
{//!Substract two FreeVector
	TFreeVector resultat (getCoordSys());
	TANumericValue::EStatus status = this->testStatus(second);
	if (status != kNull && testCoordSysCart(second.getCoordSys())==true)
	{	resultat.setX(0, getX(0) - second.getX(0));
		resultat.setX(1, getX(1) - second.getX(1));
		resultat.setX(2, getX(2) - second.getX(2));
	}
	else{status=TVNumericValue::kNull;}
	resultat.setStatus(status);
	return resultat;
}


TFreeVector& TFreeVector::operator-=( const TFreeVector& second)
{//!Add two FreeVector and replace *this
	*this=(*this)-second;
	return (*this);
}


TFreeVector TFreeVector::operator*( const TDouble& factor)
{//!Multiplication by a TDouble object
	TFreeVector resultat (getCoordSys());
	TANumericValue::EStatus status = this->testStatus(factor);
	if (status != kNull)
    {	
        TReal scalar = factor.getValue();
		resultat.setX( 0, scalar * getX(0) ); 
		resultat.setX( 1, scalar * getX(1) );
		resultat.setX( 2, scalar * getX(2) );
    }
	resultat.setStatus(status);
	return resultat;
}

TFreeVector TFreeVector::operator*( const TScalar& factor)
{//!Multiplication by a TScalar object
	TFreeVector resultat (getCoordSys());
	TANumericValue::EStatus status = this->testStatus(factor);
	if (status != kNull)
	{	
		TReal scalar = factor.getValue();
		resultat.setX( 0, scalar * getX(0) ); 
		resultat.setX( 1, scalar * getX(1) );
		resultat.setX( 2, scalar * getX(2) );
	}
	resultat.setStatus(status);
	return resultat;
}

TFreeVector TFreeVector::operator*( const TReal& factor)
{//!Multiplication by a TDouble object
	TFreeVector resultat (getCoordSys());
	if (this->isNull() == false)
	{	resultat.setX(0, factor*getX(0)); 
		resultat.setX(1, factor*getX(1));
		resultat.setX(2, factor*getX(2));
	}
	resultat.setStatus(getStatus());
	return resultat;
}


TFreeVector& TFreeVector::operator *=( const TScalar& right)
{//Multiply this vector by a TScalar
	*this = (*this) * right;
	return (*this);
}


TFreeVector& TFreeVector::operator *=( const TReal& right)
{//Multiply this vector by a TReal
	*this = (*this) * right;
	return (*this);
}


TFreeVector&  TFreeVector::operator=( const TFreeVector& right)
{//!Copy Assignment operator
	if (this != &right)
	{	setX(0, right.getX(0));
		setX(1, right.getX(1));
		setX(2, right.getX(2));
		if (right.isNull() == true)
			{setStatus(kNull);}
		else
			{setStatus(right.getStatus());}
	}
	setCoordSys(right.getCoordSys());
	return *this;
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


TLength TFreeVector::length() const
{///give the length of a TFreeVector (meters)
	TLength length;
	if(this->getStatus() != kNull)
	{
		length.setMetresValue(sqrtq(powq(getX().getValue(),2)+powq(getX().getValue(),2)+powq(getX().getValue(),2)));
		length.setStatus(this->getStatus());
	}
	else
	{
		length.setMetresValue(0);
		length.setStatus(kNull);
	}
	return length;
}


TFreeVector& TFreeVector::normalize() 
{
	TLength norml = length();
	if (norml.getStatus() != TANumericValue::kNull) {
		TReal norm = norml.getMetresValue();
		setX(0, getX(0)/norm);
		setX(1, getX(1)/norm);
		setX(2, getX(2)/norm);
	}
	return *this;
}


TFreeVector TFreeVector::cross(const TFreeVector& b) {
	TFreeVector resultat (getCoordSys());
	TANumericValue::EStatus status = this->testStatus(b);
	if (status != kNull && testCoordSysCart(b.getCoordSys())==true)
	{	
		resultat.setX(0, getX(1)*b.getX(2) - getX(2)*b.getX(1));
		resultat.setX(1, getX(2)*b.getX(0) - getX(0)*b.getX(2));
		resultat.setX(2, getX(0)*b.getX(1) - getX(1)*b.getX(0));
	
	}
	else{status=TVNumericValue::kNull;}
	resultat.setStatus(status);
	return resultat;
}
/*
TScalar TFreeVector::dot(const TFreeVector& b){
	TScalar result;
	TANumericValue::EStatus status = this->testStatus(b);
	if (status != kNull && testCoordSysCart(b.getCoordSys())==true)
		result.setValue(getX(0) * b.getX(0) + getX(1) * b.getX(1) + getX(2) * b.getX(2)); 	

	return result;
}
*/

TReal TFreeVector::dot(const TFreeVector& b){
/*
	return getX(0) * b.getX(0) + getX(1) * b.getX(1) + getX(2) * b.getX(2); 
	*/

	TReal  result;
	TANumericValue::EStatus status = this->testStatus(b);
	if (status != kNull && testCoordSysCart(b.getCoordSys())==true)
		result = getX(0) * b.getX(0) + getX(1) * b.getX(1) + getX(2) * b.getX(2); 	
	else
		throw std::runtime_error("One of the FreeVector elements is NULL or coordinate system does not match!");

	return result;
}


TLength TFreeVector::getHorDist() const
{///give the length of a TFreeVector (meters)
	TLength length;
	if(this->getStatus() != kNull)
	{
		length.setMetresValue( sqrtq( powq(getX().getValue(),2) + powq(getY().getValue(),2) ) );
		length.setStatus(this->getStatus());
	}
	else
	{
		length.setMetresValue(0);
		length.setStatus(kNull);
	}
	return length;
}
