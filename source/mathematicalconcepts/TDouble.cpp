// TDouble.cpp
//
// Class for a Double
//
//
// Copyright 2002, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////


//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include	"T3DMatrix.h"
#include	"TFreeVector.h"
#include	"TDouble.h"
#include	"TANumericValue.h"
////////////////////////////////////////////////////////////////



//ClassImp(TDouble)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


TDouble::TDouble(): fValue(LITERAL(0.0))
{	// default constructor 
	setStatus( TANumericValue::kNull );
}



TDouble::TDouble(DoubleValue	value): fValue(value)
{	// constructor taking a given length value in metres
	setStatus( TANumericValue::kKnown );
}

TDouble::TDouble(const TDouble& tD)
{	// copy constructor
	fValue = tD.fValue;
	setStatus( tD.getStatus() );
}

TDouble::~TDouble()
{
}


//////////////////////////////////////////////////////////////////////
// operators definitions
//////////////////////////////////////////////////////////////////////

//////////////
// set and get
//////////////
void TDouble::setValue(const DoubleValue value)
{	// set a value to a TDouble Object
	fValue = value;
	valueSet();
	return;
}

DoubleValue TDouble::getValue() const
{	// give the value of a TDouble Object
	 return fValue;
}


////////////////////
// basic operator
////////////////////
bool TDouble::operator==(const TDouble& right) const
{
	if ( getValue() == right.getValue())
		return true;
	else
		return false;
}


bool TDouble::operator<(const TDouble& right) const
{//comparaison between two TDouble objects
	if ( getValue() < right.getValue())
		return true;
	else
		return false;
}


TDouble TDouble::operator+(const TDouble& double1)
{//add two Double objects
	TDouble resultat;
	TANumericValue::EStatus status;
	status=this->testStatus(double1);
	if (status!= kNull)
	{
		TDouble resul(double1.getValue() + this->getValue());
		resultat=resul;
	}
resultat.setStatus(status);
return resultat;
}


TDouble TDouble::operator-(const TDouble &double1)
{//substract two TDouble objects
	TDouble resultat;
	TANumericValue::EStatus status;
	status=this->testStatus(double1);
	if (status!= kNull)
	{
		TDouble resul(this->getValue() -double1.getValue());
		resultat=resul;
	}
resultat.setStatus(status);
return resultat;
}	

	
TDouble TDouble::operator*(const TDouble& factor)
{//multiply a TDouble object by an other
	TDouble resultat;
	TANumericValue::EStatus status;
	status=this->testStatus(factor);
	if (status!= kNull)
	{
		TDouble resul(this->getValue()*factor.getValue());
		resultat=resul;
	}
resultat.setStatus(status);
return resultat;
}


TDouble TDouble::operator/(const TDouble& div)
{//divide a TDouble object by an other
	TDouble resultat;
	TANumericValue::EStatus status;
	status=this->testStatus(div);
	if (div.getValue()!=0)
	{
		if (status!= kNull)
			{
			TDouble resul(this->getValue()/div.getValue());
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

TDouble&	TDouble::operator=(const TDouble &double1)
{//assign a TDouble object to an other
	if (this != &double1) 
	{
		fValue=(double1.getValue());
		setStatus( double1.getStatus() );
	}
	return *this;
}


TDouble& TDouble::operator+=(const TDouble &double1)
{//add two TDouble objects and rewrite this
	TDouble noconst (double1);
	*this=(*this)+noconst;
	return (*this);
}


TDouble& TDouble::operator-=(const TDouble &double1)
{//substract two TDouble objects and rewrite this
	TDouble noconst (double1);
	*this=(*this)-noconst;
	return *this;
}


TDouble& TDouble::operator*=(const TDouble& factor)
{//multiply a TDouble object by an other TDouble object and rewrite this
	TDouble noconst (factor);
	*this=(*this)*noconst;
	return *this;
}


TDouble& TDouble::operator/=(const TDouble& div)
{//divide a TDouble object by an other TDouble object and rewrite this
	TDouble noconst (div);
	*this=(*this)/noconst;
	return *this;
}

//////////////////////////
///Special Methods
//////////////////////////////
TAngle TDouble::operator *(const TAngle angle)
{//multiply a TDouble object by a TAngle object
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


TLength TDouble::operator *(const TLength length)
{//multiply a TDouble object by a TLength object
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


TMatrix TDouble::operator *(const TMatrix mx)
{//multiply a TDouble object by a TMatrix object
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

T3DMatrix TDouble::operator *(const T3DMatrix mx)
{//multiply a TDouble object by a T3DMatrix object
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

TColumnVector TDouble::operator *(const TColumnVector cv)
{//multiply a TDouble object by a TColumnVector object
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

TFreeVector TDouble::operator *(const TFreeVector cv)
{//multiply a TDouble object by a TColumnVector object
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

//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

	


