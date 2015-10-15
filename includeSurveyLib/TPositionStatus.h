// TPositionStatus.h
//
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
/////////////////////////////////////////////////////////////////



#ifndef SU_TPositionStatus
#define SU_TPositionStatus


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <**classname**>
//
//
class  TVReferenceFrame;
#include  "TANumericValue.h"

// typedefs
//
//
/////////////////////////////////////////////////////////////////



//! Class storing the statuses of objects related to a 3D position
class  TPositionStatus //: public TObject  
{
public:
	friend class TAReferenceFrame;

	//!@name Constants
	//@{
	typedef TANumericValue::EStatus  ECoordStatus;
	enum  EPositionStatus {kPosNull, kUnknown,  kCala, kVx, kVy, kVz, kVxy, kVxz, kVyz, kVxyz};
	//@}


	/*!@name Constructors and Destructors */
	//@{
		//! Constructor 
		TPositionStatus(TVReferenceFrame* );

		//! Copy Constructor 
		TPositionStatus(const  TPositionStatus&);

		//! Destructor
		virtual  ~TPositionStatus();
	//@}


	//! Copy Assignment Operator 
	TPositionStatus& operator=( const TPositionStatus& );

	/*!@name Access methods*/
	//@{
	//! returns the 3D Cartesian coordinates status if defined for the position's reference frame
	virtual EPositionStatus		getPositionStatus() const;
	//! returns the X coordinate status if defined for the position's reference frame
	virtual ECoordStatus		getXCoordStatus() const;
	//! returns the Y coordinate status if defined for the position's reference frame
	virtual ECoordStatus		getYCoordStatus() const;
	//! returns the Z coordinate status if defined for the position's reference frame
	virtual ECoordStatus		getZCoordStatus() const;
	//@}

	/*!@name Set methods*/
	//@{
	//! sets the 3D Cartesian coordinates status if defined in the position's Reference Frame
	virtual bool			setPositionStatus(EPositionStatus);
	//! sets the X Cartesian coordinate status if defined in the position's Reference Frame
	virtual bool			setXCoordStatus(ECoordStatus);
	//! sets the Y Cartesian coordinate status if defined in the position's Reference Frame
	virtual bool			setYCoordStatus(ECoordStatus);
	//! sets the Z Cartesian coordinate status if defined in the position's Reference Frame
	virtual bool			setZCoordStatus(ECoordStatus);
	//@}

	/*!@return the position's number of variable coordinates*/
	virtual int				getVariableDimension() const;

private:

	//! sets the X1 coordinate status
	void  setX1Status( TANumericValue::EStatus s )  { fStatus[0] = s; return; }
	//! sets the X2 coordinate status
	void  setX2Status( TANumericValue::EStatus s )  { fStatus[1] = s; return; }
	//! sets the X3 coordinate status
	void  setX3Status( TANumericValue::EStatus s )  { fStatus[2] = s; return; }
	//! sets the position status
	void  setStatus( EPositionStatus s) {fPositionStatus = s; return;}


	//! gets the X1 coordinate status
	TANumericValue::EStatus  getX1Status() const  { return  fStatus[0]; }
	//! gets the X2 coordinate status
	TANumericValue::EStatus  getX2Status() const  { return  fStatus[1]; }
	//! gets the X3 coordinate status
	TANumericValue::EStatus  getX3Status() const  { return  fStatus[2]; }
	//! gets the position status
	EPositionStatus		getStatus() const { return fPositionStatus; }


private:

	EPositionStatus				fPositionStatus;
	TANumericValue::EStatus		fStatus[3];

	TVReferenceFrame*			fRefFrame;

	//ClassDef(TPositionStatus, 1)
};


/////////////////////
// Inline Definitions
/////////////////////


#endif // SU_TPositionStatus
