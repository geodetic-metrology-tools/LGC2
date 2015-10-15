// TVerticalDistMeasurement.h
//
/*! Class for a vertical distance measurement 

	Patterns:
 
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TVerticalDistMeasurement
#define SU_TVerticalDistMeasurement


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
using namespace std;
//
#include "TLength.h"
#include  "TAMeasurement.h"
#include  "TDistConstants.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a vertical distance measurement
class  TVerticalDistMeasurement : public TAPointMeasurement  
{
public:
	//constants

	/**@name Constructors and Destructors */
	//@{
	/*!Default constructor*/
	TVerticalDistMeasurement();
	/**Constructor
	@param refName the measurement's reference point's name
	@param tgfName the measurement's target point's name
	@param obsDist the observed vertical distance as a TLength
	@param sigma the observed dist's precision as a TLength*/
	TVerticalDistMeasurement(/*TSpatialPointName refName,*/ TSpatialPointName tgName, TLength obsDist, TLength sigma);
	/*! Destructor */
	virtual  ~TVerticalDistMeasurement();
	//@}

	/*! Copy Assignment Operator */
	TVerticalDistMeasurement& operator=( const TVerticalDistMeasurement& );

	/**@return a string indicating this measurement is a vertical distance measurement*/
	virtual string				getMeasKind() const;

	/**@name Access methods*/
	//@{
	/**@return the measured vertical distance as a TLength*/
	virtual TLength				getVDistValue() const;
	/**@return the measured distance's sigma a priori*/
	virtual TLength				getSigma() const;
	/*!@return the observed angle's constante (if there is one) as a TAngle instance*/
	virtual TDistConstants		getMeasConst() const {return fMeasConstant; }
	//@}

	///Sets the sigma a priori if it isn't done yet
	virtual void				ifNotDoneSetSigma(TLength sigma);
	/*!Sets the measurement constant */
	virtual void				ifNotDoneSetMeasConst(const TDistConstants& constant);


protected: 


private:

	TLength				fMeasuredDist; /*!< vertical measurement */
	TLength				fSigmaAPriori; /*!< error on the vertical measurement */

	TDistConstants		fMeasConstant; /*!< measurement constant if there is one */

	
};
/*@}*/


/**@name Typedefs*/
//@{
/// Type of the container used to store the vertical dist measurements
typedef list< TVerticalDistMeasurement > VertDistContainer;
/// Type of the iterator pointing to an element of the vertical dists container
typedef VertDistContainer::iterator VertDistMeasIterator;
/// Type of the const iterator pointing to an element of the vertical dists container
typedef VertDistContainer::const_iterator VertDistMeasConstIter;
//@}


#endif // SU_TVerticalDistMeasurement


