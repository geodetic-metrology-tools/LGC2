//  THelmertRefFrameTransform.h
//
/** A Helmert transformation between two reference frames.
    
	The two reference frames should be Euclidean reference frames,
	and the parameters of the transformation have to be defined 
	explicitly.

  
    Patterns:
    A WRAPPER around a THelmertTransformation
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_HELMERT_REF_FRAME_TRANSFORMATION
#define SU_HELMERT_REF_FRAME_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <##classname##>
//using namespace std;
//
class  THelmertTransformation;
#include  "TARefFrameTransformation.h"
#include  "TSpatialPosition.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup spatialobjects
	@{*/

//! A Helmert transformation between two reference frames
class  THelmertRefFrameTransform : public TARefFrameTransformation //: public TObject  
{
public:
	/**@name Constants */
	//@{
		/// To be removed if there are none!!
	//@}


	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		THelmertRefFrameTransform();

		/// Constructor taking pointers to the source and destination reference frames
		THelmertRefFrameTransform( TAReferenceFrame* from, TAReferenceFrame* to, THelmertTransformation* );

		//!!!DECIDE!!! eventually constructor taking concrete values
		/// Constructor taking parameters of a THelmertTransformation
	//	THelmertRefFrameTransform( TAReferenceFrame* from, TAReferenceFrame* to, const TScaleFactor&, const TRotation&, const TTranslation&);

		/// Copy Constructor 
		THelmertRefFrameTransform(const  THelmertRefFrameTransform&);

		/// Destructor
		virtual  ~THelmertRefFrameTransform();
	//@}


	/**@name Member Functions */
	//@{
		/// Copy Assignment Operator 
		THelmertRefFrameTransform&			operator=( const THelmertRefFrameTransform& );

		/// Return a pointer to a clone of this reference frame
		virtual THelmertRefFrameTransform*	clone() const;

		/// Return a pointer to the inverse of this transformtion
		virtual THelmertRefFrameTransform*	inverse() const;

		//! Invert the transformation, replaces the current transformation parameters
		virtual void  invert();

		/// Return the LG source frame
		virtual TAReferenceFrame*			getSourceFrame() const { return fFrom; }

		/// Return the LA destination frame
		virtual TAReferenceFrame*			getDestinationFrame() const { return fTo; }

		/// Return the helmert transformation
		virtual THelmertTransformation*		getTransform() const { return fTransform; }

		/// Set the LG source frame
		virtual void						setSourceFrame( TAReferenceFrame* from) { fFrom = from; return; }

		/// Set the LA destination frame
		virtual void						setDestinationFrame( TAReferenceFrame* to) { fTo = to; return; }

		/// Set the Helmert transformation
		virtual void						setTransform( THelmertTransformation* helmert);

		/// Set the helmert transformation using the parameters of the transformation
		//virtual void						setTransform( const TScaleFactor&, const TRotation&, const TTranslation& );

		/// transform a position vector
		virtual  bool						transform( TPositionVector& pv ) const;

		/// transform a free vector
		virtual  bool						transform( TFreeVector& fv ) const;

		/// transform a Rotation Matrix
		virtual  bool						transform( TRotationMatrix& rmx ) const;

		bool								isInitialised() const { return (fFrom != 0 && fTo!=0); }
	//@}


private:

	THelmertTransformation*		fTransform;
	TAReferenceFrame*			fFrom;
	TAReferenceFrame*			fTo;


	//ClassDef(THelmertRefFrameTransform, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_HELMERT_REF_FRAME_TRANSFORMATION
