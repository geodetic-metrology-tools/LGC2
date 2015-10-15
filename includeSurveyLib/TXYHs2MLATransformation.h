//  TXYHs2MLATransformation.h
//
/** The transformation from a XYHs projection to a modified local
    astronomical reference frame.
	
    
    Patterns:
  
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_XYHs_2_MLA_TRANSFORMATION
#define SU_XYHs_2_MLA_TRANSFORMATION


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <##classname##>
//
//using namespace system;
// #include  <cmath>
#include  "TARefFrameTransformation.h"
#include  "TAModifiedLocalAstronomicalRF.h"
#include  "TXYHeProjection.h"
#include  "TAReferenceFrame.h"


using namespace std;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup spatialobjects
	@{*/

//! The transformation from a XYHs projection to a modified local astronomical reference frame
class  TXYHs2MLATransformation : public TARefFrameTransformation //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TXYHs2MLATransformation();

		/// Constructor taking pointers to the source and destination reference frames
		TXYHs2MLATransformation( TXYHeProjection* from );

		/// Copy Constructor 
		TXYHs2MLATransformation(const  TXYHs2MLATransformation&);

		/// Destructor
		virtual  ~TXYHs2MLATransformation();
	//@}


	/**@name Member Functions */
	//@{
		using TARefFrameTransformation::transform;

		/// Copy Assignment Operator 
		TXYHs2MLATransformation&			operator=( const TXYHs2MLATransformation& );

		/// Return a pointer to a clone of this reference frame
		virtual TXYHs2MLATransformation*	clone() const;

		/// Return a pointer to the inverse of this transformation
		virtual TARefFrameTransformation*	inverse() const;

		/// Return the source frame
		virtual TAReferenceFrame*			getSourceFrame() const { return fFrom; }

		/// Return the destination frame
		virtual TAReferenceFrame*			getDestinationFrame() const { return fTo; }

		/// Return the ellipsoid of the transformation
		virtual TReferenceEllipsoid*		getEllipsoid() const { return fEllipsoid; }

		/// Set the XYHs source frame
		virtual void						setSourceFrame( TXYHeProjection* );

		//! tranformation of a position vector between reference frames
		virtual  bool						transform (TPositionVector& pv) const;
	//@}


protected:
	

private:

	/// Set the MLA destination frame
	virtual void	setDestinationFrame( TAModifiedLocalAstronomicalRF* LA) {fTo = LA; return;}

	/// Set the ellipsoid
	virtual void	setEllipsoid( TReferenceEllipsoid* ellipsoid) {fEllipsoid = ellipsoid; return;}

	virtual TAModifiedLocalAstronomicalRF*	getMLARF() const { return fTo; }
	virtual TXYHeProjection*				getXYHs() const { return fFrom; }
	
	

	
	TXYHeProjection*					fFrom;
	TAModifiedLocalAstronomicalRF*		fTo;
	TReferenceEllipsoid*				fEllipsoid;

	//ClassDef(TXYHs2MLATransformation, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_XYHs_2_MLA_TRANSFORMATION
