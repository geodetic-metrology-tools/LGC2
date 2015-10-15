//  TASpatialAttribute.h
//
/** An abstract class containing methods to access to spatial objects' attributes
  
       
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_SPATIAL_ATTRIBUTE
#define SU_SPATIAL_ATTRIBUTE


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <##classname##>
//
//class  ##classname##;
class		TAReferenceFrame;
#include	"TSpatialStatus.h"
#include	"counted_ptr.h"
#include	"TVNumericValue.h"

//using namespace std;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup SpatialObjectsAndSystems
	@{*/

//! An abstract class
class TASpatialAttribute : public TANumericValue
{
public:
	
	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TASpatialAttribute();

		/// Constructor taking a pointer to a reference frame
		TASpatialAttribute( TAReferenceFrame* );

		/// Constructor taking a pointer to a TVReferenceFrame and a pointer a TSpatialStatus
		TASpatialAttribute( TAReferenceFrame*, TSpatialStatus* );

		/// Constucteur taking a pointer to a TVReferenceFrame and a counted pointer to a Spatial Status
		TASpatialAttribute( TAReferenceFrame*, counted_ptr<TSpatialStatus> );

		/// Copy Constructor 
		TASpatialAttribute( const TASpatialAttribute& );

		/// Destructor
		virtual ~TASpatialAttribute();
	//@}
		
	TASpatialAttribute&  operator=(const TASpatialAttribute& right);
		
	/**@name Set Status Functions */
	//@{
		/// Set the global status of the spatial object
		virtual void setObjectStatus( TSpatialStatus::ESpatialStatus ps) {fSpatialStatus->setObjectStatus( ps ); return;}

		/// Set the global status of the spatial object (by copy)
		virtual void setObjectStatus( counted_ptr<TSpatialStatus> cpps) {fSpatialStatus = cpps;	return;}

	//@}


	/**@name Get Status Functions */
	//@{

		/// Get the global status of the spatial object
		virtual TSpatialStatus::ESpatialStatus getObjectStatus( ) const {return fSpatialStatus->getObjectStatus();}
	//@}

		//! return the number of Axis that have a variable status
		virtual int				getVariableDimension() const {return fSpatialStatus->getVariableDimension();}

		//! returns a boolean indicating the if X axis dimension is known
		virtual bool			knownXCoord() const;

		//! returns a boolean indicating the if Y axis dimension is known
		virtual bool			knownYCoord() const;

		//! returns a boolean indicating the if Z axis dimension is known
		virtual bool			knownZCoord() const;


		/// Returns the pointeur to the reference frame
		TAReferenceFrame*		getRefFrame() const { return fRefFrame; } 

		/// Returns a copy of this Spatial Attribute
		TSpatialStatus*			getAttribute() const { return fSpatialStatus.get(); }

protected:

		//! change the reference frame after a transformation to another reference frame
		void			changeRefFrameTo(TAReferenceFrame* rf) {fRefFrame = rf;}
		
		//! Set Reference Frame
		virtual void	setRefFrame( TAReferenceFrame* rf) { fRefFrame = rf; }

		//! Set Spatial Status
		//virtual void setStatus( TSpatialStatus* status) { fSpatialStatus = status; }


private:


	counted_ptr<TSpatialStatus>		fSpatialStatus;

	TAReferenceFrame*				fRefFrame;


	//ClassDef(##Template##, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_Spatial_Attribute
