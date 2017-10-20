/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_LS_EREL
#define SU_LS_EREL
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//LGC
#include "TAdjustablePoint.h"

/*!
\ingroup LGCProjectData

Class for writing the relative error between a list of points.
*/

class TLSCalcRelativeError {

public:

	/*!@name Constructor / destructor */
	//@{
		/*! Default constructor */
		TLSCalcRelativeError();

		/*! Constructor 
		\param pt1 position vector iterator for the 1st point 
		\param pt2 position vector iterator for the 2nd point */
		TLSCalcRelativeError(TAdjustablePoint& pt1, TAdjustablePoint& pt2);

		/*! Copy constructor */
		TLSCalcRelativeError(const TLSCalcRelativeError& source);

		/*! Destructor */
		virtual ~TLSCalcRelativeError();
	//@}

	/*! Overloaded equality operator */
	bool operator==(const TLSCalcRelativeError& right);

	/*!@name Acces methods : iterator */
	//@{
		/*! returns the iterator to point 1 */
	TAdjustablePoint*	getPoint1() const { return fPoint1; }

		/*! returns the iterator to point 2 */
	TAdjustablePoint*	getPoint2() const { return fPoint2; }
	//@}

	/*!@name Acces methods : facade */
	//@{
		/*! returns the point 1 name */
		string	getPoint1Name() const {return fPoint1->getName();}

		/*! returns the point 2 name*/
		string	getPoint2Name() const {return fPoint2->getName();}

		/*! returns the sigma longitudinal */
		TLength	getSigmaL() const {return fSigmaL;}

		/*! returns the sigma radial */
		TLength	getSigmaR() const {return fSigmaR;}

		/*! returns the sigma on height difference */
		TLength	getSigmaZ() const {return fSigmaZ;}

		/*! returns the sigma on orientation */
		TAngle  getSigmaG() const {return fSigmaG;}

		/*! returns the sigma on vertical angle */
		TAngle  getSigmaV() const {return fSigmaV;}
	//@}

	/*!@ Settings */
	//@{
		/*! sets the longitudinal error */
		void	setSigmaL(TLength);

		/*! sets the radial error */
		void	setSigmaR(TLength);

		/*! sets the height difference error */
		void	setSigmaZ(TLength);

		/*! sets the orientation error */
		void	setSigmaG(TAngle);

		/*! sets the vertical angle error */
		void	setSigmaV(TAngle);
	//@}

private:
	/*! Copy assignment operator: not implemented */
	TLSCalcRelativeError& operator=(const TLSCalcRelativeError& right);

	TAdjustablePoint*	fPoint1; /*!< position vector iterator for the 1st point */
	TAdjustablePoint*	fPoint2; /*!< position vector iterator for the 2nd point */

	/*!@name EREL related parameters */
	//@{
	TLength			fSigmaL; /*!< longitudinal error */
	TAngle			fSigmaG; /*!< error in orientation */
	TLength			fSigmaR; /*!< radial (transversal) error */
	TLength			fSigmaZ; /*!< error in the height difference*/
	TAngle			fSigmaV; /*!< error in the vertical angle */
	//@}
};

/*! List of relative errors */
typedef list<TLSCalcRelativeError>			LSRelErrorsContainer;
/*! Iterator of the list of relative errors */
typedef LSRelErrorsContainer::iterator		LSRelErrorIter;
/*! Const iterator of the list of relative errors */
typedef LSRelErrorsContainer::const_iterator	LSRelErrorConstIter;

#endif //SU_LS_EREL