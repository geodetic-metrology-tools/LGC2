//  TARefFrameTransformation.h
//
/** Classe de base abstraite pour une transformation d'objets d'un 
	referentiel dans un autre.

    The transformation maintains a pointer to each of the two reference frames
	that it links.
 
  
    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_A_REF_FRAME_TRANSFORMATION
#define SU_A_REF_FRAME_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <##classname##>
//using namespace std;
//
class		TAReferenceFrame;
#include	"TVRefFrameTransformation.h"
#include	"TAGeoidModel.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Classe de base abstraite pour une transformation d'objets d'un referentiel dans un autre
class  TARefFrameTransformation : public TVRefFrameTransformation //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{
		/*! Default Constructor */
		TARefFrameTransformation();

		/*! Destructor */
		virtual  ~TARefFrameTransformation();
	//@}


	/**@name Member Functions */
	//@{
		using TVRefFrameTransformation::transform;

		/** Indicates if the transformation is initialised or not.
		 *   
		 *	The transformation is initialised as soon as both the source
		 *	and destination reference frames have been set.
		 */
		//bool  isInitialised()  const {return ( 0 != fFrom  &&  0 != fTo ); } 
	
		/// Return a pointer to a clone of this reference frame
		virtual TARefFrameTransformation*				clone() const {TARefFrameTransformation* res = 0; return res;}

		/// Return a pointer to the inverse of this transformtion
		virtual TARefFrameTransformation*				inverse() const {TARefFrameTransformation* res = 0; return res;}

		/*! Returns a pointer to the transformation to be used */
		virtual TARefFrameTransformation*				getTransformation() { return this; } 

		/*! return the transformation identifier */
		virtual TRefSystemFactory::ERefFrameTransform	getTransformId() const { return fTransformationId; }

		/*! set the transformation identifier */
		virtual void		setTransformId( TRefSystemFactory::ERefFrameTransform id) { fTransformationId = id; return; }


		/*! set default transformation of a position vector to false */
		virtual  bool		transform( TPositionVector& ) const { return false; }

		/*! set default transformation of a free vector to false */
		virtual bool		transform(TFreeVector&) const { return false; }
		
		/*! set default transformation of a rotation matrix to false */
		virtual bool		transform(TRotationMatrix&) const { return false; }
	//@}

private:

	TRefSystemFactory::ERefFrameTransform	fTransformationId; /*!< transformation's identifier */

	//ClassDef(TARefFrameTransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_A_REF_FRAME_TRANSFORMATION
