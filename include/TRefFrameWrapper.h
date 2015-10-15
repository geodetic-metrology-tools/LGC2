//  TRefFrameWrapper.h
//
/** A Wrapper around a reference frame.
    The wrapper enables a pointer to a reference frame to be stored
	in an STL container, and to add the transformations linking a
	given reference frame to others in a network.

  
    Patterns:
    Wrapper around TVReferenceFrame.
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_REF_FRAME_WRAPPER
#define SU_REF_FRAME_WRAPPER

#if _MSC_VER >= 1000
#pragma once
#pragma warning (disable:4786)
#endif // _MSC_VER >= 1000



////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
#include  <set>
//using namespace std;
//
//class  TVReferenceFrame;
class	TVTransformation;
class  TAReferenceFrame;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup spatialobjects
	@{*/

//! A Wrapper around a reference frame
class  TRefFrameWrapper //: public TObject  
{

public:
	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor
		TRefFrameWrapper();

		/// Constructor taking a pointer to a reference frame
		TRefFrameWrapper( TAReferenceFrame* );

		/// Copy Constructor 
		TRefFrameWrapper( const TRefFrameWrapper& );

		/// Destructor
		virtual  ~TRefFrameWrapper();
	//@}


	/**@name Member Functions */
	//@{
		/// Return the name of the reference frame
		std::string  getName() const;

		/// Copy Assignment Operator 
		TRefFrameWrapper& operator=( const TRefFrameWrapper& );

		/// Equivalence Operator
		bool operator==( const TRefFrameWrapper& right ) const;

		/// Less Than Comparison Operator
		bool operator<( const TRefFrameWrapper& right ) const;

		/// Return a pointer to the Reference Frame
		TAReferenceFrame*  getFrame() const;

		/// Set the pointer to a Reference Frame
		void setFrame( TAReferenceFrame* frame );

		/// Set the Ref Frame idetifier
		void setIdentifier( int i );

	//@}

private:
	TAReferenceFrame*  fFrame;

	//ClassDef(TRefFrameWrapper, 1)
};
/*@}*/

//typedef set< TRefFrameWrapper, less< TRefFrameWrapper > > RefFrameSet;
//typedef RefFrameSet::iterator refFrameIter;

#endif // SU_REF_FRAME_WRAPPER
