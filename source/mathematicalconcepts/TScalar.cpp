// TScalar.cpp
//
// Class for a Scalar
//
//
// Copyright 2002, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////


//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include	"TFreeVector.h"
#include	"T3DMatrix.h"
#include	"TScalar.h"
#include	"TANumericValue.h"
#include	"TDouble.h"

////////////////////////////////////////////////////////////////



//ClassImp(TScalar)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


TScalar::TScalar(): fValue(LITERAL(0.0))
{	// default constructor 
	setStatus( TANumericValue::kNull );
}



TScalar::TScalar(ScalarValue	value): fValue(value)
{	// constructor taking a given scalar value
	setStatus( TANumericValue::kKnown );
}

TScalar::TScalar(TDouble&	value)
{	// constructor taking a given scalar value as a TDouble
	fValue = value.getValue();
	setStatus( TANumericValue::kKnown );
}

TScalar::TScalar(const TScalar& tD)
{	// copy constructor
	fValue = tD.fValue;
	setStatus( tD.getStatus() );
}

TScalar::~TScalar()
{
}


//////////////////////////////////////////////////////////////////////
// operators definitions
//////////////////////////////////////////////////////////////////////

//////////////
// set and get
//////////////
void TScalar::setValue(const ScalarValue value)
{	// set a value to a TScalar Object
	fValue = value;
	valueSet();
	return;
}

ScalarValue TScalar::getValue() const
{	// give the value of a TScalar Object
	 return fValue;
}


////////////////////
// basic operator
////////////////////
bool TScalar::operator==(const TScalar& right) const
{
	if ( getValue() == right.getValue())
		return true;
	else
		return false;
}


bool TScalar::operator<(const TScalar& right) const
{//comparaison between two TScalar objects
	if ( getValue() < right.getValue())
		return true;
	else
		return false;
}


bool TScalar::operator>(const TScalar& right) const
{//comparaison between two TScalar objects
	if ( getValue() > right.getValue())
		return true;
	else
		return false;
}


TScalar TScalar::operator+(const TScalar& Scalar1)
{//add two Scalar objects
	TScalar resultat;
	TANumericValue::EStatus status;
	status=this->testStatus(Scalar1);
	if (status!= kNull)
	{
		TScalar resul(Scalar1.getValue() + this->getValue());
		resultat=resul;
	}
resultat.setStatus(status);
return resultat;
}


TScalar TScalar::operator-(const TScalar &Scalar1)
{//substract two TScalar objects
	TScalar resultat;
	TANumericValue::EStatus status;
	status=this->testStatus(Scalar1);
	if (status!= kNull)
	{
		TScalar resul(this->getValue() -Scalar1.getValue());
		resultat=resul;
	}
resultat.setStatus(status);
return resultat;
}	

	
TScalar TScalar::operator*(const TScalar& factor)
{//multiply a TScalar object by an other
	TScalar resultat;
	TANumericValue::EStatus status;
	status=this->testStatus(factor);
	if (status!= kNull)
	{
		TScalar resul(this->getValue()*factor.getValue());
		resultat=resul;
	}
resultat.setStatus(status);
return resultat;
}


TScalar TScalar::operator/(const TScalar& div)
{//divide a TScalar object by an other
	TScalar resultat;
	TANumericValue::EStatus status;
	status=this->testStatus(div);
	if (div.getValue()!=0)
	{
		if (status!= kNull)
			{
			TScalar resul(this->getValue()/div.getValue());
			resultat=resul;
			}
	}
	else
	{
	status=kNull;
	}
	
	
resultat.setStatus(status);
return resultat;
}

TScalar&	TScalar::operator=(const TScalar &Scalar1)
{//assign a TScalar object to an other
	if (this != &Scalar1) 
	{
		fValue=(Scalar1.getValue());
		setStatus( Scalar1.getStatus() );
	}
	return *this;
}


TScalar& TScalar::operator+=(const TScalar &Scalar1)
{//add two TScalar objects and rewrite this
	TScalar noconst (Scalar1);
	*this=(*this)+noconst;
	return (*this);
}


TScalar& TScalar::operator-=(const TScalar &Scalar1)
{//substract two TScalar objects and rewrite this
	TScalar noconst (Scalar1);
	*this=(*this)-noconst;
	return *this;
}


TScalar& TScalar::operator*=(const TScalar& factor)
{//multiply a TScalar object by an other TScalar object and rewrite this
	TScalar noconst (factor);
	*this=(*this)*noconst;
	return *this;
}


TScalar& TScalar::operator/=(const TScalar& div)
{//divide a TScalar object by an other TScalar object and rewrite this
	TScalar noconst (div);
	*this=(*this)/noconst;
	return *this;
}

//////////////////////////
///Special Methods
//////////////////////////////
TAngle TScalar::operator *(const TAngle angle)
{//multiply a TScalar object by a TAngle object
	TAngle resultat;
	TANumericValue::EStatus status;
	status=this->testStatus(angle);
	if (status!= kNull)
	{	
		TAngle noconst(angle);
		TAngle	resul=noconst*(this->getValue());
		resultat=resul;
	}
resultat.setStatus(status);
return resultat;
}


TLength TScalar::operator *(const TLength length)
{//multiply a TScalar object by a TLength object
	TLength resultat;
	TANumericValue::EStatus status;
	status=this->testStatus(length);
	if (status!= kNull)
	{	
		TLength noconst (length);
		TLength resul=noconst*(this->getValue());
		resultat=resul;
	}
resultat.setStatus(status);
return resultat;
}


TMatrix TScalar::operator *(const TMatrix mx)
{//multiply a TScalar object by a TMatrix object
	TMatrix resultat(mx.numRows(),mx.numCols());
	TANumericValue::EStatus status;
	status=this->testStatus(mx);
	if (status!= kNull)
	{	
		TMatrix noconst(mx);
		TMatrix resul= noconst * (this->getValue());
		resultat=resul;
	}
resultat.setStatus(status);
return resultat;
}

T3DMatrix TScalar::operator *(const T3DMatrix mx)
{//multiply a TScalar object by a T3DMatrix object
	T3DMatrix resultat(mx.getCoordSys());
	TANumericValue::EStatus status;
	status=this->testStatus(mx);
	if (status!= kNull)
	{	
		T3DMatrix noconst(mx);
		T3DMatrix resul= noconst * (this->getValue());
		resultat=resul;
	}
resultat.setStatus(status);
return resultat;
}

TColumnVector TScalar::operator *(const TColumnVector cv)
{//multiply a TScalar object by a TColumnVector object
	TColumnVector resultat (cv.dimension());
	TANumericValue::EStatus status;
	status=this->testStatus(cv);
	if (status!= kNull)
	{	
		TColumnVector noconst (cv);
		TColumnVector resul=noconst*(this->getValue());
		resultat=resul;
	}
resultat.setStatus(status);
return resultat;
}

TFreeVector TScalar::operator *(const TFreeVector cv)
{//multiply a TScalar object by a TColumnVector object
	TFreeVector resultat (cv.getCoordSys());
	TANumericValue::EStatus status;
	status=this->testStatus(cv);
	if (status!= kNull)
	{	
		TFreeVector noconst (cv);
		TFreeVector resul=noconst*(this->getValue());
		resultat=resul;
	}
resultat.setStatus(status);
return resultat;
}

TScalar TScalar::operator*(const TReal factor)
{//multiply a TLength object by a factor
	TScalar resultat;
	if(isNull()!=true)
	{resultat.setValue(factor*(this->getValue()));}
	resultat.setStatus(getStatus());
	return resultat;
}

//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

	


