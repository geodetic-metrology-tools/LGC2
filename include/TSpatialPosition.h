// TSpatialPosition.h: 
//
  
/*   
  The Spatial Position of a point in a given reference frame

  Patterns:
  
   
  Copyright 1999,2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////




#ifndef SU_SPATIAL_POSITION
#define SU_SPATIAL_POSITION

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//////////////////////////////////////////////////////////
// Forward declarations
//////////////////////////////////////////////////////////
class  TLength;
class  TAngle;
class  TVReferenceFrame;
class  TModifiedLocalAstronomicalRF;
class  TReferenceEllipsoid;

class TVRefFrameTransformation;

#include  "TASpatialAttribute.h"
#include  "counted_ptr.h"
#include  "TANumericValue.h"
#include  "TPositionVector.h"
#include  "TGraph.h"
#include  "TVCoordinateSystem.h"


using namespace std;

// typedefs
typedef  TReal  Coordinate;
typedef  bool  CoordKnown;
//
//
//////////////////////////////

/*!\ingroup spatialobjects
	@{*/

//! The Spatial Position of a point in a given reference frame 
class TSpatialPosition : public TASpatialAttribute //: public TObject  
{

public:
	friend  class	TAReferenceFrame;
	friend  class	TA3DEuclideanRefFrame;
	friend  class	TModifiedLocalAstronomicalRF;

	/*!@name Constructors and Destructors */
	//@{
		explicit  TSpatialPosition( TAReferenceFrame* rf );

		TSpatialPosition(TAReferenceFrame* rf, const TPositionVector& pv);

		TSpatialPosition(TAReferenceFrame*, const TReal, const TReal, const TReal, TCoordSysFactory::ECoordSys );

		TSpatialPosition(TAReferenceFrame*, counted_ptr< TSpatialStatus >);

		TSpatialPosition( const TSpatialPosition& );

		virtual ~TSpatialPosition();
	//@}


	/*!@name Member Functions */
	//@{
		//! Copy Assignment Operator 
		TSpatialPosition& operator=(const TSpatialPosition&);
		
		//! Access to a copy of the position vector (managed by TVReferenceFrame)
		TPositionVector getCoordinates(TCoordSysFactory::ECoordSys) const;
		//! Setting of the position vector (managed by TVReferenceFrame)
		bool setCoordinates(const TPositionVector& pv);

		//! transformation between the definition reference frame of the spatial position and another reference frame
		bool transform(TAReferenceFrame* to);

	//@}

	


protected:

private:

	// private default constructor
	TSpatialPosition();


	/*!@name Access methods */
	//@{
		//! Access to the position vector
		TPositionVector getPositionVector() const;
	//@}
	
	/*!@name Settings methods */
	//@{
		//! Setting of the position vector
		bool setPositionVector(const TPositionVector& pv);
	//@}

	//! change the reference frame after a transformation to another reference frame
	void changeRefFrameTo(TAReferenceFrame*);

	//! get the coordinate system id of the position vector
	TCoordSysFactory::ECoordSys getCoordSys() const { return fX->getCoordSysPtr()->getCoordSysId(); }


	
private:
	// member attributes
	TPositionVector*  fX;

	
	//ClassDef(TSpatialPosition, 1)
};
/*@}*/
#endif // SU_SPATIAL_POSITION
