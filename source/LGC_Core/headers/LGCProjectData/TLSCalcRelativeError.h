/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_LS_EREL
#define SU_LS_EREL
#if _MSC_VER >= 1000
#	pragma once
#endif // _MSC_VER >= 1000

#include <vector>

#include "TSparseMatrix.h"
// LGC
#include <TTreeEntry.h>

#include "LGCAdjustablePoint.h"
#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER

struct functionEval
{
	// storing a relative error value and Jacobian
	double value;
	TDenseMatrix jacobian;
};

/*!
\ingroup LGCProjectData

Class for writing the relative error between a list of points.
*/
#if USE_SERIALIZER
struct TLSCalcRelativeError : public Serializable
#else
struct TLSCalcRelativeError
#endif // USE_SERIALIZER
{
public:
	/*!@name Constructor / destructor */
	//@{
	/*! Constructor
	\param pt1 position vector iterator for the 1st point
	\param pt2 position vector iterator for the 2nd point */
	TLSCalcRelativeError(LGCAdjustablePoint &pt1, LGCAdjustablePoint &pt2, std::string destFrame);

	/*! Copy constructor */
	TLSCalcRelativeError(const TLSCalcRelativeError &source);

	/*! Destructor */
	virtual ~TLSCalcRelativeError();
	//@}

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif // USE_SERIALIZER

	/*! Overloaded equality operator */
	bool operator==(const TLSCalcRelativeError &right);

	/*!@name Acces methods : iterator */
	//@{
	/*! returns the iterator to point 1 */
	LGCAdjustablePoint *getPoint1() const { return fPoint1; }

	/*! returns the iterator to point 2 */
	LGCAdjustablePoint *getPoint2() const { return fPoint2; }
	//@}

	/*!@name Acces methods : facade */
	//@{
	/*! returns the point 1 name */
	std::string getPoint1Name() const { return fPoint1->getName(); }

	/*! returns the point 2 name*/
	std::string getPoint2Name() const { return fPoint2->getName(); }
	/*! returns the destination frame name*/
	std::string getDestinationFrame() const { return fDestinationFrame; }

	/*! returns the sigma longitudinal */
	TLength getSigmaL() const { return fSigmaL; }

	/*! returns the sigma radial */
	TLength getSigmaR() const { return fSigmaR; }

	/*! returns the sigma on height difference */
	TLength getSigmaZ() const { return fSigmaZ; }

	/*! returns the sigma on orientation */
	TAngle getSigmaG() const { return fSigmaG; }

	/*! returns the sigma on vertical angle */
	TAngle getSigmaV() const { return fSigmaV; }
	//@}

	/*!@ Settings */
	//@{
	// sets the sigmas for all relative errors
	void setSigma(TVector sigmas);
	/*! sets the longitudinal error */
	void setSigmaL(TLength);

	/*! sets the radial error */
	void setSigmaR(TLength);

	/*! sets the height difference error */
	void setSigmaZ(TLength);

	/*! sets the orientation error */
	void setSigmaG(TAngle);

	/*! sets the vertical angle error */
	void setSigmaV(TAngle);
	void computeErel(TLGCData *projData);
	//@}

private:
	/*! Copy assignment operator: not implemented */
	TLSCalcRelativeError &operator=(const TLSCalcRelativeError &right);

	LGCAdjustablePoint *fPoint1; /*!< position vector iterator for the 1st point */
	LGCAdjustablePoint *fPoint2; /*!< position vector iterator for the 2nd point */

	/*!@name EREL related parameters */
	//@{
	TLength fSigmaL = TLength(LITERAL(0.0)); /*!< longitudinal error */
	TAngle fSigmaG = TAngle(LITERAL(0.0)); /*!< error in orientation */
	TLength fSigmaR = TLength(LITERAL(0.0)); /*!< radial (transversal) error */
	TLength fSigmaZ = TLength(LITERAL(0.0)); /*!< error in the height difference*/
	TAngle fSigmaV = TAngle(LITERAL(0.0)); /*!< error in the vertical angle */
	std::string fDestinationFrame; /*!< frame name in defining coordinate system in which error is calculated */
	//@}
	// evaluate all the relative errors and their jacobians in the destination frame
	std::vector<functionEval> evaluateRelErrorFunctions(const TVector &d);
};

/*! List of relative errors */
typedef std::list<TLSCalcRelativeError> LSRelErrorsContainer;
/*! Iterator of the list of relative errors */
typedef LSRelErrorsContainer::iterator LSRelErrorIter;
/*! Const iterator of the list of relative errors */
typedef LSRelErrorsContainer::const_iterator LSRelErrorConstIter;

#endif // SU_LS_EREL