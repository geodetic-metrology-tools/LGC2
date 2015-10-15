//  TVRefFrameTransformation.h
//
/** Classe de base purement abstraite pour une transformation d'objets d'un 
	referentiel dans un autre.

    The transformation maintains a pointer to each of the two reference frames
	that it links.
 
  
    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_V_REF_FRAME_TRANSFORMATION
#define SU_V_REF_FRAME_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <##classname##>
//using namespace std;
//
class  TVGraphRefFrame;
class  TCompositeAffTransform;
class  TSpatialPosition;
#include  "TVTransformation.h"
#include  "TRotationMatrix.h"
#include  "TRefSystemFactory.h"

// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup spatialobjects
	@{*/


//! Classe de base purement abstraite pour une transformation d'objets d'un referentiel dans un autre
class  TVRefFrameTransformation : public TVTransformation //: public TObject  
{
public:

	virtual ~TVRefFrameTransformation() { }
	/**@name Member Functions */
	//@{
		/// Return a pointer to a clone of this reference frame
	//	virtual  TVRefFrameTransformation*  clone() const = 0;

		/// Return a pointer to the inverse of this transformtion
	//	virtual  TVRefFrameTransformation*  inverse() const = 0;

		/// Return the transformation
	//	virtual  TVRefFrameTransformation*  getTransformation() = 0;

		/// Returns a pointer to the source reference frame
		virtual  TAReferenceFrame*  getSourceFrame()  const = 0;

		/// Returns a pointer to the destination reference frame
		virtual  TAReferenceFrame*  getDestinationFrame()  const = 0;

		//! return the transformation identifier
		virtual TRefSystemFactory::ERefFrameTransform getTransformId() const = 0;

		//! set the transformation identifier
		virtual void setTransformId( TRefSystemFactory::ERefFrameTransform id) = 0;
	//@}


private:

	//ClassDef(TVRefFrameTransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_V_REF_FRAME_TRANSFORMATION
