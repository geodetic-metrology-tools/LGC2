// T3DMatrix.cpp
//
/** Class for a general 3x3 doubles matrix*/
//
// Patterns:
//
// 
// Copyright 2002 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "T3DMatrix.h"
#include "TVCoordinateSystem.h"
////////////////////////////////////////////////////////////////


//ClassImp(T3DMatrix)


//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////


T3DMatrix::T3DMatrix(TCoordSysFactory::ECoordSys en )
{//default constructor
	for (int i=0; i<3; i++)
		{	for (int j=0; j<3; j++)
			{setC(i,j,LITERAL(0.0));}
		}
	setStatus(kNull);
	setCoordSys(en);
}


T3DMatrix::T3DMatrix( const  T3DMatrix& original )
{//copy constructor	
	for (int i=0; i<3; i++)
		{	for (int j=0; j<3; j++)
			{setC(i,j, original.getC(i,j));}
		}
	setStatus(original.getStatus());
	setCoordSys(original.getCoordSys());
}


T3DMatrix::~T3DMatrix()
{//destructor
}


//////////////////////////////////////////////////////////////////////
///operator Functions
//////////////////////////////////////////////////////////////////////

T3DMatrix&  T3DMatrix::operator=(const T3DMatrix& right)
{//Copy assignement operator	
	if (this != &right)
	{
		for (int i=0; i<3; i++)
			{	for (int j=0; j<3; j++)
				{setC(i,j, right.getC(i,j));}
			}
	}
	setCoordSys(right.getCoordSys());
	setStatus(right.getStatus());
	return *this;
}


void T3DMatrix::operator=(const TReal& value)
{//inits all the matrix coefficients to a common value
	for (int i = 0; i<3; i++)
	{	for(int j=0; j<3; j++)
		{setElt(i,j, value);}
	}
}


T3DMatrix T3DMatrix::operator +(const T3DMatrix& right) const
{//returns the sum of this matrix and a second one
	T3DMatrix resultat (getCoordSys());
	resultat.setStatus(TVNumericValue::kNull);
	TANumericValue::EStatus status;
	status=this->testStatus(right);
	if (status!= kNull && testCoordSysGen(right.getCoordSys())==true)	
	{
		for (int i=0; i<3; i++)
		{	for (int j=0; j<3; j++)
			{resultat.setC(i,j, getC(i,j) + right.getC(i,j));}
		}
	resultat.setStatus(status);
	}
	return resultat;
}


T3DMatrix& T3DMatrix::operator +=(const T3DMatrix& right)
{//replaces this matrix by its sum with a second one
*this=(*this)+right;
return *this;
}


T3DMatrix T3DMatrix::operator -(const T3DMatrix& right) const
{//returns the difference of this matrix and a second one
	T3DMatrix resultat (getCoordSys());
	resultat.setStatus(TVNumericValue::kNull);
	TANumericValue::EStatus status;
	status=this->testStatus(right);
	if (status!= kNull && testCoordSysGen(right.getCoordSys())==true)	
	{
		for (int i=0; i<3; i++)
		{	for (int j=0; j<3; j++)
			{resultat.setC(i,j, getC(i,j) - right.getC(i,j));}
		}
	resultat.setStatus(status);
	}
	return resultat;
}


T3DMatrix& T3DMatrix::operator -=(const T3DMatrix& right)
{//replaces this matrix by its difference with a second one
*this=(*this)-right;
return *this;
}


T3DMatrix T3DMatrix::operator *(const T3DMatrix& right) const
{//returns the product of this matrix and a second one
	T3DMatrix resultat (getCoordSys());
	resultat.setStatus(TVNumericValue::kNull);
	TANumericValue::EStatus status;
	status=this->testStatus(right);
	if (status!= kNull && testCoordSysGen(right.getCoordSys())==true)	
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


T3DMatrix& T3DMatrix::operator *=(const T3DMatrix& right)
{//replaces this matrix by its product with a second one
*this=(*this)*right;
return *this;
}


TPositionVector T3DMatrix::operator*(const TPositionVector& pv) const
{//returns the product of this matrix by a position vector
	TPositionVector resultat (0,0,0, pv.getCoordSys());
	resultat.setStatus(TVNumericValue::kNull);
	TANumericValue::EStatus status;
	status=this->testStatus(pv);
	if (status!= kNull && testCoordSysGen(pv.getCoordSys())==true)
	{
		resultat.setX(pv.getX()*getC(0,0)+pv.getY()*getC(0,1)+pv.getZ()*getC(0,2));
		resultat.setY(pv.getX()*getC(1,0)+pv.getY()*getC(1,1)+pv.getZ()*getC(1,2));
		resultat.setZ(pv.getX()*getC(2,0)+pv.getY()*getC(2,1)+pv.getZ()*getC(2,2));
		resultat.setStatus(status);
	}
	return resultat;
}


TFreeVector T3DMatrix::operator*(const TFreeVector& fv) const
{//returns the product of this matrix by a position vector
	TFreeVector resultat (0,0,0, fv.getCoordSys());
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

T3DMatrix T3DMatrix::operator*( const TReal& k)
{//multiplies the Matrix by a scalar
	T3DMatrix resultat (*this);
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			resultat.setElt(i,j,getElt(i,j)*k);
		}
	}
	return resultat;
}


T3DMatrix T3DMatrix::operator*( const TDouble& k)
{//multiplies the Matrix by a TDouble
	T3DMatrix resultat (this->getCoordSys());
	TANumericValue::EStatus status;
	status=this->testStatus(k);
	if (status!= kNull)	
	{
		resultat = (*this) * k.getValue();
	}
	resultat.setStatus(status);
	return resultat;
}



////////////////////////////////////
///Member Functions
////////////////////////////////////

T3DMatrix T3DMatrix::transposed() const
{//return the tranposed matrix of this one

	T3DMatrix resultat (*this);
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			resultat.setC(i,j, getC(j,i));
		}
	}
	return resultat;
}





T3DMatrix T3DMatrix::inverse()
{//Returns the inverse of this matrix (the latter isn't affected)
T3DMatrix copy (*this);
copy.invert();
//copy=*this;
return copy;
}


void T3DMatrix::clear()
{//return a matrice (i,j)=0, status kNull
	for (int i=0; i< 3; i++)
	{
		for (int j=0; j< 3; j++)
		{setElt(i,j,0);}
	}
	if (isNull()==true)
	{setStatus(TVNumericValue::kNull);}
	else
	{setStatus(TVNumericValue::kKnown);}
}


bool T3DMatrix::initDiag( const TReal& comVal)
{//inits all the matrix diagonal coefficients to a common value
	for (int i = 0; i<3 ; i++)
		{
			for (int j=0; j<3; j++)
			{
				if (i == j)
				{setElt(i,j,comVal);}
				else
				{setElt(i,j,LITERAL(0.0));}
			}
		}
	setStatus(kKnown);
	return true;
}


bool T3DMatrix::invert()
{//Overwrites this Matrix by its inverse
	T3DMatrix resultat (this->getCoordSys());
	resultat.setStatus(TVNumericValue::kNull);
	if (this->getStatus()!= kNull)	
	{
		//calcul du determinant
		TReal det=0;
		det=getC(0,0)*getC(1,1)*getC(2,2)-getC(0,0)*getC(1,2)*getC(2,1)
			-getC(0,1)*getC(1,0)*getC(2,2)+getC(0,1)*getC(1,2)*getC(2,0)
			+getC(0,2)*getC(1,0)*getC(2,1)-getC(0,2)*getC(1,1)*getC(2,0);

		if(det!=0)
		{TReal d=1/det;
			resultat.setC(0,0,d*(getC(1,1)*getC(2,2)-getC(2,1)*getC(1,2)));
			resultat.setC(1,0,-d*(getC(1,0)*getC(2,2)-getC(2,0)*getC(1,2)));
			resultat.setC(2,0,d*(getC(1,0)*getC(2,1)-getC(2,0)*getC(1,1)));
			resultat.setC(0,1,-d*(getC(0,1)*getC(2,2)-getC(2,1)*getC(0,2)));
			resultat.setC(1,1,d*(getC(0,0)*getC(2,2)-getC(2,0)*getC(0,2)));
			resultat.setC(2,1,-d*(getC(0,0)*getC(2,1)-getC(2,0)*getC(0,1)));
			resultat.setC(0,2,d*(getC(0,1)*getC(1,2)-getC(1,1)*getC(0,2)));
			resultat.setC(1,2,-d*(getC(0,0)*getC(1,2)-getC(1,0)*getC(0,2)));
			resultat.setC(2,2,d*(getC(0,0)*getC(1,1)-getC(1,0)*getC(0,1)));
			resultat.setStatus(getStatus());
		
		*this=resultat;
		return true;
		}
		else
		{
			*this=resultat;
			return false;
		}
	}
	else
	{
	*this=resultat;
	return false;
	}

}

	
bool T3DMatrix::setElt(const int i, const int j, const TReal value)
{//set a element into a 3D matrix
return this->getCoordSysPtr()->setElt(this, i, j, value);
}	

TReal T3DMatrix::getElt(const int i, const int j) const
{//set a element into a 3D matrix
return this->getCoordSysPtr()->getElt(this, i, j);
}




