// TSpatialStatus.h
//
// General class for status of spatial objects. The three components are considered to be
// linked with axis.
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
/////////////////////////////////////////////////////////////////



#ifndef SU_SPATIAL_STATUS
#define SU_SPATIAL_STATUS


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////
// Forward declarations
//
//
//
class  TVReferenceFrame;
#include  "TANumericValue.h"

//using namespace std;

// typedefs
//
//
/////////////////////////////////////////////////////////////////

/*!\ingroup SpatialObjectsAndSystems
	@{*/

//! Class storing the statuses of objects related to a 3D position
class  TSpatialStatus //: public TObject  
{
public:
	//friend class TAReferenceFrame;

	//!@name Constants
	//@{
	typedef TANumericValue::EStatus  EAxisStatus;
	enum  ESpatialStatus {kPosNull, kUnknown,  kCala, kVx, kVy, kVz, kVxy, kVxz, kVyz, kVxyz};
	//@}


	/*!@name Constructors and Destructors */
	//@{
		//! Constructor 
		TSpatialStatus();

		//! Copy Constructor 
		TSpatialStatus(const  TSpatialStatus&);

		//! Destructor
		virtual  ~TSpatialStatus();
	//@}


		//! Copy Assignment Operator 
		TSpatialStatus& operator=( const TSpatialStatus& );


	/*!@name Access methods*/
	//@{
		//! returns the 3D status if defined for the position's reference frame
		virtual ESpatialStatus	getObjectStatus() const {return fSpatialStatus;}

		//! returns the X coordinate status if defined for the position's reference frame
		virtual EAxisStatus		getXAxisStatus() const {return fStatus[0];}

		//! returns the Y coordinate status if defined for the position's reference frame
		virtual EAxisStatus		getYAxisStatus() const {return fStatus[1];}

		//! returns the Z coordinate status if defined for the position's reference frame
		virtual EAxisStatus		getZAxisStatus() const {return fStatus[2];}
	//@}


	/*!@name Set methods*/
	//@{
		//! sets the 3D status if defined in the position's Reference Frame
		virtual void			setObjectStatus(ESpatialStatus s) {fSpatialStatus = s; return;}

		//! sets the X-Axis status if defined in the position's Reference Frame
		virtual void			setXAxisStatus(EAxisStatus s) {fStatus[0] = s; return;}

		//! sets the Y-Axis status if defined in the position's Reference Frame
		virtual void			setYAxisStatus(EAxisStatus s) {fStatus[1] = s; return;}

		//! sets the Z-Axis status if defined in the position's Reference Frame
		virtual void			setZAxisStatus(EAxisStatus s) {fStatus[2] = s; return;}
	//@}


		/*!@return the position's number of variable coordinates*/
		virtual int				getVariableDimension() const;



private:

	ESpatialStatus				fSpatialStatus;
	TANumericValue::EStatus		fStatus[3];

	//ClassDef(TPositionStatus, 1)
};
/*@}*/


#endif // SU_SPATIAL_STATUS
