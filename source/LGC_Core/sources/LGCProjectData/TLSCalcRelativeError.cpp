#include "TLSCalcRelativeError.h"

#include <iostream>

#include <TPointTransformer.h>

#include "Logger.hpp"
#include "TLGCData.h"

// constructor taking 2 adjustable frames, as names
TLSCalcRelativeErrorFrame::TLSCalcRelativeErrorFrame(std::string fromFrame, std::string toFrame) : fFromFrame(fromFrame), fToFrame(toFrame)
{
}

// destructor
TLSCalcRelativeErrorFrame::~TLSCalcRelativeErrorFrame()
{
}

/////////////////////////
// SETTINGS
////////////////////////

void TLSCalcRelativeErrorFrame::computeErel(const TLGCData *projData)
{
	TDataTreeIterator fromIt = projData->locateNode(fFromFrame);
	TDataTreeIterator toIt = projData->locateNode(fToFrame);
	const TAdjustableHelmertTransformation &from = projData->locateNode(fFromFrame).node->data.get()->frame;
	const TAdjustableHelmertTransformation &to = projData->locateNode(fToFrame).node->data.get()->frame;
	if ((!from.isInitialized()) || (!to.isInitialized()))
	{
		throw std::logic_error("Both frames " + fFromFrame + " and " + fToFrame + " need to be initialized to compute their relative error.");
	}

	TLOR2LOR relativeChain(fromIt, toIt, "relativeTransformation");
	// store the result as adjustablke helmert transformation
	std::string name = "relTrafo";

	// improvable: this sets the provisional and estimated values at the same time, better: prov = 0 only set estimated vals
	TAdjustableHelmertTransformation relativeTrafo(relativeChain.computeComposedTransformationParameters(), std::bitset<3>(), std::bitset<3>(), std::bitset<1>(), name);
	// store the computed covariance in the adjustable object
	relativeTrafo.setCovar(relativeChain.getComposedParameterCovariance(projData));
	fRelativeTransformation = relativeTrafo;
	// indicate relative error is computed
	fIsComputed = true;
}

#if USE_SERIALIZER
void TLSCalcRelativeErrorFrame::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("fFromFrame", fFromFrame);
	obj.addProperty("fToFrame", fToFrame);

	obj.addProperty("fIsComputed", fIsComputed);
	// serialize results only if computation happened
	if (fIsComputed)
		obj.addProperty("fRelativeTransformation", fRelativeTransformation);
}
#endif // USE_SERIALIZER

// constructor taking 2 adjustable points and a destination frame, as names
TLSCalcRelativeErrorPoint::TLSCalcRelativeErrorPoint(std::string pt1, std::string pt2, std::string destFrame) : fPoint1(pt1), fPoint2(pt2), fDestinationFrame(destFrame)
{
}

// destructor
TLSCalcRelativeErrorPoint::~TLSCalcRelativeErrorPoint()
{
}

void TLSCalcRelativeErrorPoint::setSigma(TVector sigmas)
{
	if (sigmas.size() != 5)
	{
		throw std::logic_error("There need to be 5 relative error sigmas assigned.");
	}
	fSigmaL = TLength(sigmas(0));
	fSigmaG = TAngle(sigmas(1));
	fSigmaR = TLength(sigmas(2));
	fSigmaZ = TLength(sigmas(3));
	fSigmaV = TAngle(sigmas(4));
}
// sets the error in orientation
void TLSCalcRelativeErrorPoint::setSigmaG(TAngle sg)
{
	fSigmaG = sg;
	return;
}
// sets the longitudinal error
void TLSCalcRelativeErrorPoint::setSigmaL(TLength sl)
{
	fSigmaL = sl;
	return;
}
// sets the radial error
void TLSCalcRelativeErrorPoint::setSigmaR(TLength sr)
{
	fSigmaR = sr;
	return;
}

// sets the error in the vertical angle
void TLSCalcRelativeErrorPoint::setSigmaV(TAngle sv)
{
	fSigmaV = sv;
	return;
}

// sets the error in the height difference
void TLSCalcRelativeErrorPoint::setSigmaZ(TLength sz)
{
	fSigmaZ = sz;
	return;
}

void TLSCalcRelativeErrorPoint::computeErel(TLGCData *projData)
{
	TDataTreeIterator destFrameIt = projData->locateNode(fDestinationFrame);
	const LGCAdjustablePoint &point1 = projData->getPoints().getObject(fPoint1);
	const LGCAdjustablePoint &point2 = projData->getPoints().getObject(fPoint2);

	if ((!point1.isInitialized()) || (!point2.isInitialized()))
	{
		throw std::logic_error("Both points " + fPoint1 + " and " + fPoint2 + " need to be initialized to compute their relative error.");
	}

	// transform both points to destFrame coordinate system
	TLOR2LOR p1ToDestFrame(point1.getFrameTreePosition(), destFrameIt, "subframe1ToDest");
	TLOR2LOR p2ToDestFrame(point2.getFrameTreePosition(), destFrameIt, "subframe2ToDest");
	TPositionVector p1InDestFrame = point1.getEstimatedValue();
	TPositionVector p2InDestFrame = point2.getEstimatedValue();
	p1ToDestFrame.transform(p1InDestFrame);
	p2ToDestFrame.transform(p2InDestFrame);
	// fill the relative error function evaluations and jacobians
	TFreeVector dFree = p2InDestFrame - p1InDestFrame;
	TVector diff = dFree.toRealVector();
	std::vector<functionEval> evaluations;
	try
	{
		evaluations = evaluateRelErrorFunctions(diff);
	}
	catch (std::exception const &excp)
	{
		logWarning() << excp.what();
	}
	TSparseMatrix A(5, projData->fUEOIndices.UIndex);
	TDenseMatrix ErrJac(5, 3);
	ErrJac.setZero();
	int idx = 0;
	for (auto eval : evaluations)
	{
		ErrJac.row(idx) = eval.jacobian;
		idx++;
	}
	A.setZero();
	// this equation considers that the Jacobian of d with respect to p1,p2 is -I,I where I is the 3x3 identity matrix
	A = (ErrJac * (-p1ToDestFrame.getPointDerivative(projData, point1) + p2ToDestFrame.getPointDerivative(projData, point2))).sparseView();
	// in the previous version the covariance matrix was normalized with (1/pow2(projData->getS0APosteriori())) but this correction factor is already applied just before the covariance matrix is stored.
	TSparseMatrix covMat = *projData->getCovMatByConst();
	TDenseMatrix result = A * covMat * A.transpose();
	TVector sigmaSquares = result.diagonal();
	setSigma(sigmaSquares.cwiseSqrt());

	// indicate relative error is computed
	fIsComputed = true;
}

std::vector<functionEval> TLSCalcRelativeErrorPoint::evaluateRelErrorFunctions(const TVector &d)
{
	if (d.size() != 3)
	{
		logFatal() << "Vector needs to be 3-dimensional.";
	}
	// evaluation of the relative error functions in the destination frame as a function of d = p2-p1
	TVector dxy = d({0, 1});
	double L, R, G, V, Z;
	TDenseMatrix LJac(1, 3), RJac(1, 3), GJac(1, 3), VJac(1, 3), ZJac(1, 3);

	// Longitudinal error L
	L = d.norm();
	LJac << d.transpose();
	if (L < nullLimit)
	{
		logWarning() << "Relative longitudinal error (L) Sigma between " + fPoint1 + " and " + fPoint2 + " is undefined because they have identical positions.";
	}
	LJac *= (1 / L);

	// Radial Error
	R = 0;
	double h = dxy.norm();
	RJac << -d(1), d(0), 0;
	if (h < nullLimit)
	{
		logWarning() << "Relative radial error (R) Sigma between " + fPoint1 + " and " + fPoint2 + " is undefined because they have identical x/y coordinates.";
	}
	RJac *= (1 / h);

	// Orientation Error G
	G = atan2(d(0), d(1));
	GJac << d(1), -d(0), 0;
	GJac *= (1 / pow2q(h));

	// vertical error
	V = acosq(d(2) / L);
	VJac << -d(0) * d(2), -d(1) * d(2), L * L - d(2) * d(2);
	if (h < nullLimit)
	{
		// square root argument can be zero when |dxy|=0
		logWarning() << "Relative vertical angle error (V) Sigma between " + fPoint1 + " and " + fPoint2 + " is undefined because they have identical x/y coordinates.";
	}
	VJac *= -(1 / sqrt(1 - pow2q(d(2) / L))) * pow(L, -3);

	// height difference
	Z = d(2);
	ZJac << 0, 0, 1;

	std::vector<functionEval> result{functionEval{L, LJac}, functionEval{G, GJac}, functionEval{R, RJac}, functionEval{Z, ZJac}, functionEval{V, VJac}};
	return result;
}

#if USE_SERIALIZER
void TLSCalcRelativeErrorPoint::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("fPoint1", fPoint1);
	obj.addProperty("fPoint2", fPoint2);
	obj.addProperty("fDestinationFrame", fDestinationFrame);
	obj.addProperty("fIsComputed", fIsComputed);
	// serialize results only if computation happened
	if (fIsComputed)
	{
		obj.addProperty("fSigmaL", fSigmaL);
		obj.addProperty("fSigmaG", fSigmaG);
		obj.addProperty("fSigmaR", fSigmaR);
		obj.addProperty("fSigmaZ", fSigmaZ);
		obj.addProperty("fSigmaV", fSigmaV);
	}
}
#endif // USE_SERIALIZER

#if USE_SERIALIZER
void TRelativeErrors::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("fRelErrorPoints", points);
	obj.addProperty("fRelErrorFrames", frames);
}
#endif // USE_SERIALIZER
