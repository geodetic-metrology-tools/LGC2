//TAPlaneMeasurement.h
/*	Abstract Base Class. Defines the methods common to all
	survey measurements having a target plane	
	Patterns:
		
	Copyright 2003 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////


#ifndef SU_TAPLANEMEASUREMENT
#define SU_TAPLANEMEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//////////////////////////////////////////////////////////////////////////
//include files
#include <list>
using namespace std;

#include "TSpatialPlaneName.h"
#include "TLength.h"
#include "TDistConstants.h"
//////////////////////////////////////////////////////////////////////////

//typedefs
typedef string Series;

/*! \ingroup spatialmeasurements
	@{*/


class TAPlaneMeasurement : public TAMeasurement//: public 
{
public:

	/*!@name operators*/
	//@{
		//! Equality operator
		virtual bool		operator==(const TAPlaneMeasurement& right) const;
		//! copy assignement  operator
		virtual void		operator=(const TAPlaneMeasurement& right);
	//@}



	/*!@name target's access and setting methods*/
	//@{
		/*!@return the name the target TSpatialPoint*/
		virtual TSpatialPlaneName	getTargetPlaneName() const;

		/*!\return the identifier of the measurement*/
//		virtual int		getId() const;

		/*!sets the identifier of the measurement
		\param id an int*/
//		virtual void	setId(int id);

		/*!\return the comment of the measurement*/
//		virtual string	getComment() const;
	
		/*!sets the comment of the measurement
		\param id a string*/
//		virtual void	setComment(string com);

		/*!@return the head comment of the measurement*/
//		virtual string				getHeaderComment() const { return fHeaderComment;}

		/*!sets the head comment of the measurement
		@param com the comment line as a string*/
//		virtual void				setHeaderComment(string com) { fHeaderComment = com; return;}

		virtual	void	setPlaneParameters( const TSpatialPlaneName tgName,
											const TLength obsDist,
											const TLength sigma);
		
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
	//@}


protected:

	/*!@name Constructors / Destructor*/
	//@{
		//!Default constructor
		TAPlaneMeasurement();

		/*! Constructor setting the target point's name
		\param tgName the target point's name
		\param obsDist the observed distance as a TLength
		\param sigma the observed distance's precision as a TLength */
		explicit TAPlaneMeasurement(const TSpatialPlaneName tgName, const TLength obsDist, const TLength sigma);

		/*! Copy constructor */
		TAPlaneMeasurement(const TAPlaneMeasurement& source);

		/*! Destructor */
		~TAPlaneMeasurement();
	//@}

	
	
	
	TSpatialPlaneName	fTargetPlaneName; /*!< name of the target point */

//	int					fIdentifier; /*< identifier from geode */
//	string				fComment; /*< comment from geode*/
//	string				fHeaderComment; /*< head comment line (starting with % in input file)*/
	
	TLength				fObservedDist; /*!< observed distance */
	TLength				fSigmaAPriori; /*!< error "a priori" on the observed distance*/

	TDistConstants		fMeasConstant; /*!< measurement constant if there is one */
};
#endif

/*@}*/
