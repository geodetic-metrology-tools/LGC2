//  TReferenceEllipsoid.h
//
/** A Geodetic Reference ellipsoid 
  
    Patterns:
  
   
    Copyright 1999,2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_REFERENCE_ELLIPSOID
#define SU_REFERENCE_ELLIPSOID


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include	<float.h>
#include    <math.h>
//#include	<assert.h>
#include    <string>
//
class  TAngle;
#include "TRefSystemFactory.h"
//using namespace std;
//#include  "TVReferenceEllipsoid.h"
//
// typedefs
typedef TReal EllipseParameter;

////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! A Geodetic Reference ellipsoid
class	TReferenceEllipsoid //: public TVReferenceEllipsoid //: public TObject
{

public:


	
	
	/**@name Constructors and Destructors */
	//@{
		/// Constructor taking the name of the reference ellipsoid
		explicit  TReferenceEllipsoid( const  string& name );

		/// Destructor
		virtual	~TReferenceEllipsoid();
	//@}


	/**@name Member Functions */
	//@{
		/// Overloaded equivalence operator
		bool operator==(const TReferenceEllipsoid &) const;

		/** Set the parameters a and e^2 to define the geodetic ellipsoid
			
			@param a  the semi-major axis
			@param e^2  the eccentricity squared
		*/
		void  setAAndESquared(EllipseParameter a, EllipseParameter eSquared);

		/** Set the parameters a and 1/f to define the geodetic ellipsoid
			
			@param a  the semi-major axis
			@param f  the flattening
		*/
		void  setAAndReciprocalF(EllipseParameter a, EllipseParameter reciprocalF);
	

		/// return ellipsoid name
		virtual string  getName()  const;
	
		/// return major semi-axis value
		virtual EllipseParameter  getA()  const;

		/// return eccentricity value
		virtual EllipseParameter  getE()  const;

		/// return the square of eccentricity value
		virtual EllipseParameter  getESquared()  const;

		/// return the flattening value
		virtual EllipseParameter  getF()   const;

		/// return minor semi-axis value
		virtual EllipseParameter  getB()  const;

		/// return second eccentricity value
		virtual EllipseParameter  getEPrime()  const;

		/// return the square of second eccentricity value
		virtual EllipseParameter  getEPrimeSquared()  const;

		/** Get the Radius of Curvature of the Ellipsoid in the Prime Vertical
		    at a given latitude.

			@param  phi the latitude
		 */
		virtual EllipseParameter  getNu( const TAngle& phi )  const;
		
		/** Get the Radius of Curvature of the Ellipsoid in the Prime Vertical
		    at a given latitude.

			@param  phi the latitude in radians
		 */
		virtual EllipseParameter  getNu( const TReal phi )  const;

		/** Get the Radius of Curvature of the Ellipsoid in the Meridian
		    at a given latitude.

			@param  phi the latitude
		 */
		virtual EllipseParameter  getRho( const TAngle& phi )  const;

		/** Get the Radius of Curvature of the Ellipsoid in the Meridian
		    at a given latitude.

			@param  phi the latitude in radians
		 */
		virtual EllipseParameter  getRho( const TReal phi )  const;

		/** Get the Euler's Radius of Curvature of the Ellipsoid
		    at a given latitude, and azimuth.

			@param  phi the latitude
			@param  alpha the azimuth
		 */
		virtual EllipseParameter  getEuler( const TAngle& phi, const TAngle& alpha )  const;

		/** Get the Euler's Radius of Curvature of the Ellipsoid
		    at a given latitude, and azimuth.

			@param  phi the latitude in radians
			@param  alpha the azimuth in radians
		 */
		virtual EllipseParameter  getEuler( const TReal phi, const TReal alpha )  const;


		virtual void setEllId(const TRefSystemFactory::ERefEll ellId);


		virtual TRefSystemFactory::ERefEll getEllId() const;
	//@}


private:
	
	// **  We don't want these available **
	// Copy Constructor 
	TReferenceEllipsoid(const TReferenceEllipsoid&);
	// Copy Assignment Operator 
	TReferenceEllipsoid& operator=(const TReferenceEllipsoid&);

	// Set parameters b and (e')^2
	void setDerivedParameters();

	

private:
	EllipseParameter				fA;		          /*!< default = LITERAL(0.0) */
	EllipseParameter				fESquared;		  /*!< ellispoid parameter: e^2 */		
	EllipseParameter				fF;               /*!< ellispoid parameter: F */
	EllipseParameter				fB;               /*!< ellispoid parameter: B */
	EllipseParameter				fEPrimeSquared;   /*!< ellispoid parameter: e'^2 */
	string							fName;			/*!< name of the ellispoid */	

	TRefSystemFactory::ERefEll		fEllId;		/*!< ellispoid identifier */
	//ClassDef(TReferenceEllipsoid, 1)
};
/*@}*/

//////////////////////////////////////////////////////////////////////
//Inline Definitions
///////////////////////////////////////////////////////////////////////


	
inline EllipseParameter	TReferenceEllipsoid::getA() const {	return fA;}


inline EllipseParameter	TReferenceEllipsoid::getE() const {	return sqrtq( fESquared );}


inline EllipseParameter	TReferenceEllipsoid::getESquared() const {	return fESquared;}


inline EllipseParameter	TReferenceEllipsoid::getF() const {	return fF;}


inline EllipseParameter	TReferenceEllipsoid::getB() const {	return fB;}


inline EllipseParameter	TReferenceEllipsoid::getEPrime() const { return sqrtq( fEPrimeSquared );}


inline EllipseParameter	TReferenceEllipsoid::getEPrimeSquared() const {	return fEPrimeSquared;}


inline string TReferenceEllipsoid::getName() const { return fName;}



#endif
