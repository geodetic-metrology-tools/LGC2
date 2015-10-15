// TSpatialVectorStatus.h
//
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
/////////////////////////////////////////////////////////////////



#ifndef SU_TSpatialVectorStatus
#define SU_TSpatialVectorStatus


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

using namespace std;
// typedefs
//
//
/////////////////////////////////////////////////////////////////



//! Class storing the statuses of objects related to a 3D position
class  TSpatialVectorStatus //: public TObject  
{
public:
	friend class TAReferenceFrame;

	//!@name Constants
	//@{
	typedef TANumericValue::EStatus  ECompStatus;
	enum  EVectorStatus {kPosNull, kUnknown,  kCala, kVx, kVy, kVz, kVxy, kVxz, kVyz, kVxyz};
	//@}


	/*!@name Constructors and Destructors */
	//@{
		//! Constructor 
		TSpatialVectorStatus(TVReferenceFrame* );

		//! Copy Constructor 
		TSpatialVectorStatus(const  TSpatialVectorStatus&);

		//! Destructor
		virtual  ~TSpatialVectorStatus();
	//@}


	//! Copy Assignment Operator 
	TSpatialVectorStatus& operator=( const TSpatialVectorStatus& );

	/*!@name Access methods*/
	//@{
	//! returns the spatial vector status if defined for the position's reference frame
	//virtual EVectorStatus		getSpatialVectorStatus() const;
	//! returns the X component status if defined for the position's reference frame
	virtual ECompStatus			getXCompStatus() const;
	//! returns the Y component status if defined for the position's reference frame
	virtual ECompStatus			getYCompStatus() const;
	//! returns the Z component status if defined for the position's reference frame
	virtual ECompStatus			getZCompStatus() const;
	//@}

	/*!@name Set methods*/
	//@{
	//! sets the spatial vector status if defined in the position's Reference Frame
	//virtual bool			setSpatialVectorStatus(EVectorStatus);
	//! sets the X component status if defined in the position's Reference Frame
	virtual bool			setXCompStatus(ECompStatus);
	//! sets the Y component status if defined in the position's Reference Frame
	virtual bool			setYCompStatus(ECompStatus);
	//! sets the Z component status if defined in the position's Reference Frame
	virtual bool			setZCompStatus(ECompStatus);
	//@}

	/*!@return the position's number of variable coordinates*/
	virtual int				getVariableDimension() const;

private:

	//! sets the X1 coordinate status
	void  setF1Status( ECompStatus s )  { fStatus[0] = s; return; }
	//! sets the X2 coordinate status
	void  setF2Status( ECompStatus s )  { fStatus[1] = s; return; }
	//! sets the X3 coordinate status
	void  setF3Status( ECompStatus s )  { fStatus[2] = s; return; }
	//! sets the vector status
	void  setStatus( EVectorStatus s) {fVectorStatus = s; return;}


	//! gets the X1 coordinate status
	ECompStatus  getF1Status() const  { return  fStatus[0]; }
	//! gets the X2 coordinate status
	ECompStatus  getF2Status() const  { return  fStatus[1]; }
	//! gets the X3 coordinate status
	ECompStatus  getF3Status() const  { return  fStatus[2]; }
	//! gets the vector status
	EVectorStatus			 getStatus() const { return fVectorStatus; }


private:

	EVectorStatus				fVectorStatus;
	ECompStatus					fStatus[3];
	
	TVReferenceFrame*			fRefFrame;

	//ClassDef(TPositionStatus, 1)
};


/////////////////////
// Inline Definitions
/////////////////////


#endif // SU_TSpatialVectorStatus
