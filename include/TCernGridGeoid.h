//  TCernGridGeoid.h
//
/** Geoid in CERN's area interpolated trought a grid of values

	Concrete Class derived from TVGeoid.


    Patterns:
  
   
    Copyright 1999,2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_CERN_GRID_GEOID
#define SU_CERN_GRID_GEOID


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include <string>

class	TRefSystemFactory;

#include	"TPositionVector.h"
#include	"TAGeoidModel.h"
#include	"TMatrix.h"

//using namespace std;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Concrete Class derived from TVGeoid
class  TCernGridGeoid : public TAGeoidModel //: public TObject  
{
public:



	/**@name Constructors and Destructors */
	//@{
		/// default constructor
		TCernGridGeoid();
		
		/// constructor taking the name, the definition reference frame and ellipsoid, and the calculation reference frame
		TCernGridGeoid( const string& name,
			const TMatrix* N, const TMatrix* Eta, const TMatrix* Xsi,
			const TPositionVector downLeft, const TPositionVector upRight,
			TAReferenceFrame* def, TReferenceEllipsoid* ell, TAReferenceFrame* calc);
		
		/// Destructor
		virtual  ~TCernGridGeoid();
	//@}


	/**@name Member Functions */
	//@{
		/// Returns the geoidal undulation (geoidal height) at the given Point's position	
		virtual  TLength  getN( const TSpatialPosition& ) const;
		
		/// Returns the deflection of the vertical in the prime vertical at the given Point's position	
		virtual  TAngle  getEta( const TSpatialPosition& ) const;
		
		/// Returns the deflection of the vertical in the meridian at the given Point's position	
		virtual  TAngle  getXi( const TSpatialPosition& ) const;
		
		/// Returns the Laplace correction at the given Point's position	
		virtual  TAngle  getDAlpha( const TSpatialPosition& ) const;

		/// Returns the Laplace correction at the given Point's position (phi is given)	
		virtual  TAngle  getDAlpha( const TSpatialPosition&, const TAngle& ) const;

		/// Returns the name of the geoid
		virtual  string	 getName()  const { return fName; }

		/// Set the definiton reference frame
		void setDefRefFrame(TAReferenceFrame* def) { fDefRFPtr = def; return; }

		/// Set the definition reference ellipsoid
		void setRefEll(TReferenceEllipsoid* ell) { fDefEllPtr = ell; return; }

		/// Set the Calculation Reference Frame
		void setCalcRefFrame(TAReferenceFrame* calc) { fCalcRFPtr = calc; return; }

		/// Get the definition reference frame
		virtual TAReferenceFrame* getDefRefFrame() const { return fDefRFPtr; }

		/// Get the definition reference ellipsoid
		virtual TReferenceEllipsoid* getDefRefEll() const { return fDefEllPtr; }

		/// Get the calculation reference frame
		virtual TAReferenceFrame* getCalcRefFrame() const { return fCalcRFPtr; }

		/// Set the geoid identifier as an enum type
		void setGeoidId(const TRefSystemFactory::EGeoid);

		/// Return the geoid identifier
		virtual TRefSystemFactory::EGeoid getGeoidId() const { return fGeoidId; }
	//@}


private:

	/**@name Private Functions */
	//@{
		/*!used to interpolate N*/
		TReal splineInterpolation(const TMatrix& mat, const TSpatialPosition& spos) const;
	//@}
	
	/**@name Member Attributes */
	//@{
		string						fName; /*!< name  */
			
		TMatrix						fNMatrix; /*!< N-matrix */
		TMatrix						fEtaMatrix; /*!< Eta-matrix */
		TMatrix						fXiMatrix; /*!< Xi-matrix */

		TPositionVector				fDownLeft; /*!< position vector  */
		TPositionVector				fUpRight; /*!< position vector  */
		
		TAReferenceFrame*			fDefRFPtr; /*!< pointer to the def. reference frame  */
		TReferenceEllipsoid*		fDefEllPtr; /*!< pointer to the reference ellispoid  */
		TAReferenceFrame*			fCalcRFPtr; /*!< pointer to the  calc. reference frame */


		TRefSystemFactory::EGeoid		fGeoidId; /*!< geoid indentifier */
	//@}


	//ClassDef(TCernGridGeoid, 1)
};
/*@}*/

#endif // SU_CERN_GRID_GEOID
