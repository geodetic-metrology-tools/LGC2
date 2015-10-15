// TA2DPlus1MapProjection.h
/*!
	Abstract base class for a 2D+1 projection

	Pattern:

	Copyright Copyright 2002 CERN EST/SU. All rights reserved.
*/

#ifndef SU_2DPLUS1
#define SU_2DPLUS1

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TAReferenceFrame.h"

/*! \ingroup spatialobjects
	@{*/

//! Abstract base class for a 2D+1 projection
class TA2DPlus1MapProjection : public TAReferenceFrame
{
public:

	/*!@name Constructors and Destructors */
	//@{
	//! Constructor taking the name of the reference frame
	TA2DPlus1MapProjection( const string& name );

	//! Destructor
	virtual  ~TA2DPlus1MapProjection();
	//@}

	/*!@name default definition of TVReferenceFrame's Data extraction methods*/
	//@{
	//! return the position vector of a spatial position 
	virtual TPositionVector getCoordinates(const TSpatialPosition* sp, TCoordSysFactory::ECoordSys) const;
	//! return the free vector of a spatial vector
	virtual TFreeVector getElements(const TSpatialVector* sv, TCoordSysFactory::ECoordSys) const;	
	//! return the rotation matrix of a spatial orientation
	virtual TRotationMatrix getElements(const TSpatialOrientation* so, TCoordSysFactory::ECoordSys ) const;
	//@}

	/*!@name default definition of TVReferenceFrame's Settings methods*/
	//@{
	//! set the position vector of a spatial position 
	virtual bool setCoordinates(TSpatialPosition* sp, const TPositionVector& pv);
	//! set the free vector of a spatial vector
	virtual bool setElements(TSpatialVector* sv, const TFreeVector& fv);
	//! set the rotation matrix of a spatial orientation
	virtual bool setElements(TSpatialOrientation* so, const TRotationMatrix& rm);
	//@}
	
private:

	//! Copy constructor
	TA2DPlus1MapProjection(const TA2DPlus1MapProjection&);

	// Copy Assignment Operator
	TA2DPlus1MapProjection&  operator=( const TA2DPlus1MapProjection& );


};
/*@}*/

#endif //SU_2DPLUS1
