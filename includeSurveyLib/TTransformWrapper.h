//  TTransformWrapper.h
//
/** A Wrapper around a spatial transformation.
    The wrapper enables a pointer to a transformation to be stored
	in an STL container.

  
    Patterns:
    Wrapper around TVTransformation
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TRANSFORMATION_WRAPPER
#define SU_TRANSFORMATION_WRAPPER


#if _MSC_VER >= 1000
#pragma once
#pragma warning (disable:4786)
#endif // _MSC_VER >= 1000




////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <##classname##>
//using namespace std;
//
class  TVGraphRefFrame;
class  TARefFrameTransformation;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup ShortestPath
	@{*/

//! A Wrapper around a spatial transformation
class  TTransformWrapper //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{

		/// Default constructor
		TTransformWrapper();

		/// Constructor taking a pointer to a transformation
		TTransformWrapper( TARefFrameTransformation* transformer);

		/// Copy Constructor 
		TTransformWrapper( const TTransformWrapper& );

		/// Destructor
		virtual  ~TTransformWrapper();
	//@}


	/**@name Member Functions */
	//@{
		/// Copy Assignment Operator 
		TTransformWrapper& operator=( const TTransformWrapper& );

		/// Returns the transformation.
		TARefFrameTransformation*  getTransformation()  const;

		TARefFrameTransformation*  getCopyOfTransformation() const;

		void setTransformation (TARefFrameTransformation* transform);
	//@}

private:

		TARefFrameTransformation*  fTransformer;

	//ClassDef(TTransformWrapper, 1)
};

/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_TRANSFORMATION_WRAPPER
