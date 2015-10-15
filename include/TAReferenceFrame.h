// TAReferenceFrame.h
//
/*!  
   Patterns:
   An Abstract STATE of a Spatial Position
   
   Copyright 2000 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////



#ifndef SU_A_REFERENCE_FRAME
#define SU_A_REFERENCE_FRAME


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
#include  <set>
#include  <vector>

#include  "TSpatialPosition.h"
#include  "TSpatialVector.h"
#include  "TSpatialOrientation.h"

#include  "TVReferenceFrame.h"


//using namespace std;
// typedefs
//
//
//////////////////////////////////////////////////////////////!/

/*!\ingroup spatialobjects
	@{*/


//! An abstract base class for a reference frame.
class  TAReferenceFrame : public  TVReferenceFrame //: public TObject  
{
public:

	/*!@name Constructors and Destructors */
	//@{
		//! Constructor taking the name of the reference frame
		explicit TAReferenceFrame( const string& name );

		//! Destructor
		virtual  ~TAReferenceFrame();
	//@}


	//!@name Member Function
	//@{
		//! Get the name of the reference frame
		virtual string  getName() const;

		/// return the reference frame id
		virtual TRefSystemFactory::ERefFrame  getRefFrameId() const;

		//! Set the identifier for the reference frame
		virtual void						setRefFrameId(TRefSystemFactory::ERefFrame id) { fRefFrameId = id; return; }

		//! test if the reference frame is in graph or not
		bool								isInRFFactory() const;

		//! transform a position from a reference frame to another
		virtual bool						transform(TSpatialPosition* sp, TAReferenceFrame* rf);

		//! transform a vector from a reference frame to another
		virtual bool						transform( TSpatialVector* sv, TAReferenceFrame* rf );

		//! return the geodetic reference frame if there is one
		virtual TGeodeticRefFrame*			getGeodeticRF() const;

		//! return the origin of the reference frame if there is one
		virtual TSpatialPosition			getOrigin() const { TSpatialPosition origin; return origin; }

		/*! get the gisement of the reference frame if there is one (MLA)*/
		virtual TAngle						getGisement() const { TAngle gis; return gis; }

		/*! get the slope of the reference frame if there is one (MLA)*/
		virtual TAngle						getSlope() const { TAngle slope; return slope; }
		
		//! transformation from this reference frame to CGRF (in case of a reference frame not included in TGraph)
		virtual TARefFrameTransformation*   getRFTransfo2CGRF();

		/*!return true if the refernec frame is in graph,
		so return false only for MLA*/
		virtual bool						isInGraph() const {return true; };
	//@}

protected:
	/*!@name Data extraction methods*/
	//@{
		//! return the position vector of a spatial position 
		virtual TPositionVector getPositionVector(const TSpatialPosition* sp) const;

		//! return the free vector of a spatial vector
		virtual TFreeVector		getVector(const TSpatialVector* sv) const;

		//! return the rotation matrix of a spatial orientation
		virtual TRotationMatrix getOrientation(const TSpatialOrientation* so) const;
	//@}

	/*!@name Settings methods*/
	//@{
		//! set the position vector of a spatial position 
		virtual bool setPositionVector(TSpatialPosition* sp, const TPositionVector& pv);

		//! set the free vector of a spatial vector
		virtual bool setVector(TSpatialVector* sv, const TFreeVector& fv);

		//! set the rotation matrix of a spatial orientation
		virtual bool setOrientation(TSpatialOrientation* so, const TRotationMatrix& rm);
	//@}



private:

	// Constructors and Destructors
	// Copy Constructor 
	TAReferenceFrame( const  TAReferenceFrame& );


	// Private Member Functions
	// Copy Assignment Operator
	TAReferenceFrame&  operator=( const TAReferenceFrame& );

	//void setIdentifier( TRefSurfServer::ERefFrame id ) { fRefFrameId = id; return; }


private:

	string  fName; //!< name of the reference frame

	TRefSystemFactory::ERefFrame fRefFrameId; //!< Identification of the reference frame in the Graph

//	TransformationSet  fTransformations;

	//ClassDef(TAReferenceFrame, 1)
};
/*@}*/

////////////////////////////////////////////////////////////////////
// Inline Definitions
////////////////////////////////////////////////////////////////////






#endif // SU_A_REFERENCE_FRAME
