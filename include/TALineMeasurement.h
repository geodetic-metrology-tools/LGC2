//TALineMeasurement.h
/*	Abstract Base Class. Defines the methods common to all
	survey measurements having a target Line	
	Patterns:
		
	Copyright 2003 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////


#ifndef SU_TALINEMEASUREMENT
#define SU_TALINEMEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//////////////////////////////////////////////////////////////////////////
//include files
#include <list>
using namespace std;

#include "TSpatialLineName.h"
#include "TLength.h"
#include "TDistConstants.h"
//////////////////////////////////////////////////////////////////////////

//typedefs
typedef string Series;

/*! \ingroup spatialmeasurements
	@{*/


class TALineMeasurement : public TAMeasurement
{
public:

	/*!@name operators*/
	//@{
		//! Equality operator
		virtual bool		operator==(const TALineMeasurement& right) const;
		//! copy assignement  operator
		virtual void		operator=(const TALineMeasurement& right);
	//@}



	/*!@name target's access and setting methods*/
	//@{
		/*!@return the name the target TSpatialPoint*/
		virtual TSpatialLineName	getTargetLineName() const;

	//@}



	/*!@name Measurement functions */
	//@{
		/*!\return the observed distance's value as a TLength */
		virtual TLength	getLengthValue() const;

		/*!\return the observed distance's precision as a TLength */
		virtual TLength	getSigma() const;

		/*!\return the observed angle's constante (if there is one) as a TAngle instance*/
		virtual TDistConstants		getMeasConst() const;

		/*! sets the sigma a priori if it isn't done yet */
		virtual void	ifNotDoneSetSigma(TLength sigma);

		/*!Sets the measurement constant */
		virtual void	ifNotDoneSetMeasConst(const TDistConstants& constant);

		virtual	void	setLineParameters(const TSpatialLineName targetName, const TLength obsDist, const TLength sigma);
//@}


protected:

	/*!@name Constructors / Destructor*/
	//@{
		//!Default constructor
		TALineMeasurement();

		/*! Constructor setting the target point's name
		\param tgName the target point's name
		\param obsDist the observed distance as a TLength
		\param sigma the observed distance's precision as a TLength */
		explicit TALineMeasurement(const TSpatialLineName tgName, const TLength obsDist, const TLength sigma);

		/*! Copy constructor */
		TALineMeasurement(const TALineMeasurement& source);

		/*! Destructor */
		~TALineMeasurement();
	//@}

	
	
	
	TSpatialLineName	fTargetLineName; /*!< name of the target line */

	TLength				fObservedDist; /*!< observed distance */
	TLength				fSigmaAPriori; /*!< error "a priori" on the observed distance*/

	TDistConstants		fMeasConstant; /*!< measurement constant if there is one */



};
#endif

/*@}*/

