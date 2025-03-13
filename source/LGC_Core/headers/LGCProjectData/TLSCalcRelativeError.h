/*
© Copyright CERN 2000-2024. All rigths reserved. This software is released under a CERN proprietary software licence.
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
#include "TAdjustableHelmertTransformation.h"
#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER

/*!
\ingroup LGCProjectData

Class for writing the relative error between two frames
*/

#if USE_SERIALIZER
struct TLSCalcRelativeErrorFrame : public Serializable
#else
struct TLSCalcRelativeErrorFrame
#endif // USE_SERIALIZER
{
public:
	/*!@name Constructor / destructor */
	//@{
	/*! Constructor
	\param source and destination frame names */
	TLSCalcRelativeErrorFrame(std::string fromFrame, std::string toFrame);

	/*! Destructor */
	virtual ~TLSCalcRelativeErrorFrame();
	//@}
	void computeErel(const TLGCData *projData);
	const TAdjustableHelmertTransformation &getResult() const { return fRelativeTransformation; };

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif // USE_SERIALIZER

	/*!@name Acces methods : */
	//@{
	/*! returns the name of source frame */
	std::string getFromFrame() const { return fFromFrame; }

	/*! returns the name of destination frame */
	std::string getToFrame() const { return fToFrame; }
	//@}

private:
	std::string fFromFrame; /*!< frame name for source frame */
	std::string fToFrame; /*!< frame name for destination frame*/

	TAdjustableHelmertTransformation fRelativeTransformation; // the transformation going from fromFrame to toFrame
	bool fIsComputed{false};
};

/*!
\ingroup LGCProjectData

Class for writing the relative error between two points
*/
struct functionEval
{
	// storing a relative error value and Jacobian
	double value;
	TDenseMatrix jacobian;
};

#if USE_SERIALIZER
struct TLSCalcRelativeErrorPoint : public Serializable
#else
struct TLSCalcRelativeErrorPoint
#endif // USE_SERIALIZER
{
public:
	/*!@name Constructor / destructor */
	//@{
	/*! Constructor
	\param pt1 name for the 1st point
	\param pt2 name for the 2nd point */
	TLSCalcRelativeErrorPoint(std::string pt1, std::string pt2, std::string destFrame);

	/*! Destructor */
	virtual ~TLSCalcRelativeErrorPoint();
	//@}

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif // USE_SERIALIZER

	/*!@name Acces methods : iterator */
	//@{
	/*! returns the iterator to point 1 */
	std::string getPoint1() const { return fPoint1; }

	/*! returns the iterator to point 2 */
	std::string getPoint2() const { return fPoint2; }
	//@}

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
	std::string fPoint1; /*!< name of the 1st point */
	std::string fPoint2; /*!< name of the 2nd point */
	std::string fDestinationFrame; /*!< frame name in defining coordinate system in which error is calculated */

	bool fIsComputed{false};

	/*!@name EREL related parameters */
	//@{
	TLength fSigmaL = TLength(LITERAL(0.0)); /*!< longitudinal error */
	TAngle fSigmaG = TAngle(LITERAL(0.0)); /*!< error in orientation */
	TLength fSigmaR = TLength(LITERAL(0.0)); /*!< radial (transversal) error */
	TLength fSigmaZ = TLength(LITERAL(0.0)); /*!< error in the height difference*/
	TAngle fSigmaV = TAngle(LITERAL(0.0)); /*!< error in the vertical angle */
	//@}
	// evaluate all the relative errors and their jacobians in the destination frame
	std::vector<functionEval> evaluateRelErrorFunctions(const TVector &d);
};

// struct for  collecting both points and frame errors
#if USE_SERIALIZER
struct TRelativeErrors : public Serializable
#else
struct TRelativeErrors
#endif
{
	std::vector<TLSCalcRelativeErrorPoint> points;
	std::vector<TLSCalcRelativeErrorFrame> frames;
#if USE_SERIALIZER
	virtual void serialize(ObjectSerializer &obj) const override;
#endif
};

#endif // SU_LS_EREL