// TWorkingConstants.h : Header File.
//////////////////////////////////////////////////////////////////////

#ifndef SU_WORKING_CONSTANTS
#define SU_WORKING_CONSTANTS

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif 


#include <list>
#include <algorithm>
#include <sstream>
using namespace std;


#include "TLength.h"
#include "TAMeasurement.h"

#include "lsalgo/UEOIndices.h"
#include "TDistConstants.h"
#include "TAngleConstants.h"




//! Class used to store a set of TLength
class TWorkingConstants 
{
public:

	/*!@name Constructors/Destructor*/
	//@{
	//!Default constructor
	TWorkingConstants();

	//TWorkingConstants(InstNb in, PrismNb pn);

	//TWorkingConstants(InstNb in, PrismNb pn, TLength c, TANumericValue::EStatus s);

	//!Destructor
	virtual ~TWorkingConstants();
	//@}

	/*!@name Distance Constants methods*/
	//@{
	/*!@return the number of stored constants*/
	int							numberOfDistConstants() const;
	/*!@return an iterator pointing to the first constant of the container*/
	DistConstantIterator		getDistConstantsBeginIterator();
	/*!@return a const iterator pointing to the first constant of the container*/
	DistConstantConstIter		getDistConstantsBeginIterator() const;
	/*!@return an iterator pointing to one past the last constant of the container*/
	DistConstantIterator		getDistConstantsEndIterator();
	/*!@return a const iterator pointing to one past the last constant of the container*/
	DistConstantConstIter		getDistConstantsEndIterator() const;
	/*!@return an iterator for a dist constant*/
	//DistConstantIterator		getDistIterator(TDistConstants cte);
	/*!@return a const iterator for a dist constant*/
	//DistConstantConstIter		getDistIterator(TDistConstants cte) const;
	/*!@return an iterator pointing to a point specified through its name*/
	DistConstantIterator		getDistConstant(const string name);
	/*!@return a const iterator pointing to a point specified through its name*/
	DistConstantConstIter		getDistConstant(const string name) const;

	TDistConstants*				addDistConstant(TDistConstants *dc);
	//@}

	/*!@name Angle Constants methods*/
	//@{
	/*!@return the number of stored constants*/
	int							numberOfAngleConstants() const;
	/*!@return an iterator pointing to the first constant of the container*/
	AngleConstantIterator		getAngleConstantsBeginIterator();
	/*!@return a const iterator pointing to the first constant of the container*/
	AngleConstantConstIter		getAngleConstantsBeginIterator() const;
	/*!@return an iterator pointing to one past the last constant of the container*/
	AngleConstantIterator		getAngleConstantsEndIterator();
	/*!@return a const iterator pointing to one past the last constant of the container*/
	AngleConstantConstIter		getAngleConstantsEndIterator() const;
	/*!@return an iterator for an angle cosntant*/
	//AngleConstantIterator		getAngleIterator(TAngleConstants cte);
	/*!@return a const iterator for an angle cosntant*/
	//AngleConstantConstIter		getAngleIterator(TAngleConstants cte) const;
	/*!@return an iterator pointing to a point specified through its name*/
	AngleConstantIterator		getAngleConstant(const string name);
	/*!@return a const iterator pointing to a point specified through its name*/
	AngleConstantConstIter		getAngleConstant(const string name) const;

	TAngleConstants*			addAngleConstant(TAngleConstants *dc);
	//@}
	

		
private:
	
	//InstNb							fInstrumentNumber;
	//PrismNb							fPrismeNumber;
	//TLength							fValue;
	//TANumericValue::EStatus			fStatus;
	//MatrixIndex						fIndex;
	
	
	int								fLastDistNbr;
	DistConstContainer				fDistConstants;
	int								fLastAngleNbr;
	AngleConstContainer				fAngleConstants;
	
};
#endif 
