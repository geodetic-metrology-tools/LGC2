///////////////////////
// TVReferenceFrame.h
///////////////////////
/*!	 
	definition of the global members for concrete reference frames 
	
	Patterns:
	An Abstract STATE of a Spatial Position
   
	Copyright 2000 CERN EST/SU. All rights reserved.
*/

  
////////////////////////////////////////////////////////////////////



#ifndef SU_V_REFERENCE_FRAME
#define SU_V_REFERENCE_FRAME


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
#include  <set>
//

//class  TSpatialOrientation;

class TSpatialPosition;
class TSpatialVector;
class TSpatialOrientation;
class TSpatialStatus;
class TGeodeticRefFrame;
class TModifiedLocalGeodeticRF;
class TModifiedLocalAstronomicalRF;

#include  "TAngle.h"
#include  "TLength.h"

#include  "TCoordSysFactory.h"
#include  "TRefSystemFactory.h"


//#include  "TSpatialOrientation.h"


#include  "TVAffineTransformation.h"
//#include  "TAAffineTransformation.h"
using namespace std;
//#include  "TTransformWrapper.h"
// typedefs
//
//
//////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects 
	@{*/

//!	A Purely abstract base class for a reference frame
class  TVReferenceFrame //: public TObject  
{
public:
	friend class TA3DEuclideanRefFrame;
	//typedef set< TTransformWrapper, less< TTransformWrapper > > TransformationSet;
	//typedef TransformationSet::iterator iterator;

	virtual ~TVReferenceFrame() { }

		//! Get the name of the reference frame
		virtual string  getName() const = 0;

		//! returns identifier of the reference frame in the graph
	    virtual TRefSystemFactory::ERefFrame  getRefFrameId() const = 0;

		//! test if the reference frame is in graph or not
		virtual bool isInRFFactory() const = 0;

		/*!@name Access methods */
		//@{
			//! Default method for accessing a position vector in a spatial position
			virtual TPositionVector getCoordinates(const TSpatialPosition* sp, TCoordSysFactory::ECoordSys) const = 0;
			
			//! Default method for accessing a free vector in a spatial vector
			virtual TFreeVector getElements(const TSpatialVector* sv, TCoordSysFactory::ECoordSys) const = 0;
			
			//! Default method for accessing a rotation matrix in a spatial orientation
			virtual TRotationMatrix getElements(const TSpatialOrientation* so, TCoordSysFactory::ECoordSys) const = 0;
		//@}
		
		/*!@name Settings methods */
		//@{
			//! Default method for setting a position vector in a spatial position
			virtual bool setCoordinates(TSpatialPosition* sp, const TPositionVector& pv) = 0;

			//! Default method for setting a free vector in a spatial vector
			virtual bool setElements(TSpatialVector* sv, const TFreeVector& fv) =0;

			//! Default method for setting a rotation matrix in a spatial orientation
			virtual bool setElements(TSpatialOrientation* so, const TRotationMatrix& rm) = 0;
		//@}

		/*@name Transformation of reference frames methods*/
		//@{
			//! transform a position from a reference frame to another
//			virtual bool transform( TSpatialPosition* sp, TVReferenceFrame* rf ) = 0;

			//! transform a vector from a reference frame to another
//			virtual bool transform( TSpatialVector* sv, TVReferenceFrame* rf ) = 0;
		//@}
		

		//! transformation from this reference frame to CGRF (in case of a reference frame not included in TGraph)
		virtual TARefFrameTransformation*   getRFTransfo2CGRF() = 0;


		/// return the geodetic reference frame if there is one
		virtual TGeodeticRefFrame* getGeodeticRF() const  = 0;

		/// return the local geodetic reference frame if there is one
		//virtual TModifiedLocalGeodeticRF* getLocalGeodeticRF() const  = 0;

		/// return the origin of the reference frame if there is one
		virtual TSpatialPosition getOrigin() const = 0;
		
		/*! get the gisement of the reference frame if there is one (MLA)*/
		virtual TAngle getGisement() const = 0;

		/*! get the slope of the reference frame if there is one (MLA)*/
		virtual TAngle getSlope() const = 0;


		/*!return true if the refernec frame is in graph,
		so return false only for MLA*/
		virtual bool isInGraph() const = 0;
		
protected:


private:

	//ClassDef(TVReferenceFrame, 1)
};
/*@}*/

/////////////////////////////////////////////////////////////////////
// Inline Definitions
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// Global Function Definitions
/////////////////////////////////////////////////////////////////////

// Equivalence Operator
bool operator==( const TVReferenceFrame& left, const TVReferenceFrame& right );

// Less Than Comparison Operator
bool operator<( const TVReferenceFrame& left, const TVReferenceFrame& right );







#endif // SU_V_REFERENCE_FRAME


// deleted methods
		//! transform a position from a reference frame to CGRF
/*		virtual bool transformToCGRF( TSpatialPosition* sp ) = 0;

		//! transform a position from CGRF to another reference frame
		virtual bool transformFromCGRF( TSpatialPosition* sp ) = 0;

		//! transform a vector from a reference frame to another
		virtual bool transform( TSpatialVector* sp, TVReferenceFrame* rf ) = 0;

*/

		// Return a pointer to a clone of this reference frame
		//virtual TVReferenceFrame*  clone() const = 0;

		
		/// set the reference frame identifier
//		virtual void setRefFrameId(const TGraph::ERefFrame refFrameId) = 0;
