//  TVTransformation.h
//
/** Classe de base purement abstraite pour une transformation spatiale
 
  
    Patterns:
  
    Copyright 2000-10 CERN SU, M.Jones. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_V_TRANSFORMATION
#define SU_V_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <##classname##>
//using namespace std;
//
class  TPositionVector;
class  TFreeVector;
class  TRotationMatrix;

//#include  "TGlobalDefs.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Classe de base purement abstraite pour une transformation spatiale
class /*SU_DECLSPEC*/ TVTransformation //: public TObject  
{
public:

	virtual ~TVTransformation() { }

	/**@name Member Functions */
	//@{
		//! transform a position vector
		virtual  bool  transform( TPositionVector& ) const = 0;

		//! transform a free vector
		virtual  bool  transform( TFreeVector& ) const = 0;

		//! transform a Rotation Matrix
		virtual  bool  transform( TRotationMatrix& ) const = 0;
	//@}


private:

	//ClassDef(TVTransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_V_TRANSFORMATION
