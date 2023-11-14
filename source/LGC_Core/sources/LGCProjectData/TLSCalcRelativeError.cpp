#include "TLSCalcRelativeError.h"

#include <iostream>

#include <TPointTransformer.h>

#include "Logger.hpp"
#include "TLGCData.h"

// constructor taking 2 adjustable points and a destination frame
TLSCalcRelativeError::TLSCalcRelativeError(LGCAdjustablePoint &pt1, LGCAdjustablePoint &pt2, std::string destFrame) :
	fPoint1(&pt1), fPoint2(&pt2), fDestinationFrame(destFrame)
{
}

// copy constructor
TLSCalcRelativeError::TLSCalcRelativeError(const TLSCalcRelativeError &source)
{
	fPoint1 = source.fPoint1;
	fPoint2 = source.fPoint2;

	fSigmaL = source.fSigmaL;
	fSigmaR = source.fSigmaR;
	fSigmaZ = source.fSigmaZ;
	fSigmaG = source.fSigmaG;
	fSigmaV = source.fSigmaV;
	fDestinationFrame = source.fDestinationFrame;
}
// destructor
TLSCalcRelativeError::~TLSCalcRelativeError()
{
}

/* copy assignment operator: not implemented
TLSCalcRelativeError& TLSCalcRelativeError::operator=(const TLSCalcRelativeError& right) {
}*/

// overloaded equality operator
bool TLSCalcRelativeError::operator==(const TLSCalcRelativeError &right)
{
	if ((fPoint1 == right.fPoint1) && (fPoint2 == right.fPoint2) && (fDestinationFrame == right.fDestinationFrame))
		return true;
	else
		return false;
}
/////////////////////////
// SETTINGS
////////////////////////

void TLSCalcRelativeError::setSigma(TVector sigmas)
{
	if (sigmas.size() != 5)
	{
		std::logic_error("There need to be 5 relative error sigmas assigned.");
	}
	fSigmaL = TLength(sigmas(0));
	fSigmaG = TAngle(sigmas(1));
	fSigmaR = TLength(sigmas(2));
	fSigmaZ = TLength(sigmas(3));
	fSigmaV = TAngle(sigmas(4));
}
// sets the error in orientation
void TLSCalcRelativeError::setSigmaG(TAngle sg)
{
	fSigmaG = sg;
	return;
}
// sets the longitudinal error
void TLSCalcRelativeError::setSigmaL(TLength sl)
{
	fSigmaL = sl;
	return;
}
// sets the radial error
void TLSCalcRelativeError::setSigmaR(TLength sr)
{
	fSigmaR = sr;
	return;
}

// sets the error in the vertical angle
void TLSCalcRelativeError::setSigmaV(TAngle sv)
{
	fSigmaV = sv;
	return;
}

// sets the error in the height difference
void TLSCalcRelativeError::setSigmaZ(TLength sz)
{
	fSigmaZ = sz;
	return;
}

void TLSCalcRelativeError::computeErel(TLGCData *projData)
{
	TDataTreeIterator destFrameIt = projData->locateNode(fDestinationFrame);
	if ((!fPoint1->isInitialized()) || (!fPoint2->isInitialized()))
	{
		throw std::logic_error("Both points " + fPoint1->getName() + " and " + fPoint2->getName() + " need to be initialized to compute their relative error.");
	}

	// transform both points to destFrame coordinate system
	TLOR2LOR p1ToDestFrame(fPoint1->getFrameTreePosition(), destFrameIt, "subframe1ToRoot");
	TLOR2LOR p2ToDestFrame(fPoint2->getFrameTreePosition(), destFrameIt, "subframe2ToRoot");
	TPositionVector p1InDestFrame = fPoint1->getEstimatedValue();
	TPositionVector p2InDestFrame = fPoint2->getEstimatedValue();
	p1ToDestFrame.transform(p1InDestFrame);
	p2ToDestFrame.transform(p2InDestFrame);
	// fill the relative error function evaluations and jacobians
	TFreeVector dFree = p2InDestFrame - p1InDestFrame;
	TVector diff(3);
	diff << dFree.getX(), dFree.getY(), dFree.getZ();
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
	A = (ErrJac * (-p1ToDestFrame.getPointDerivative(projData, *fPoint1) + p2ToDestFrame.getPointDerivative(projData, *fPoint2))).sparseView();
	// in the previous version the covariance matrix was normalized with (1/pow2(projData->getS0APosteriori())) but this correction factor is already applied just before the covariance matrix is stored.
	TSparseMatrix covMat = *projData->getCovMatByConst();
	TDenseMatrix result = A * covMat * A.transpose();
	TVector sigmaSquares = result.diagonal();
	setSigma(sigmaSquares.cwiseSqrt());
}

std::vector<functionEval> TLSCalcRelativeError::evaluateRelErrorFunctions(const TVector &d)
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
		logWarning() << "Relative longitudinal error (L) Sigma between " + fPoint1->getName() + " and " + fPoint2->getName() + " is undefined because they have identical positions.";
	}
	LJac *= (1 / L);

	// Radial Error
	R = 0;
	double h = dxy.norm();
	RJac << -d(1), d(0), 0;
	if (h < nullLimit)
	{
		logWarning() << "Relative radial error (R) Sigma between " + fPoint1->getName() + " and " + fPoint2->getName() + " is undefined because they have identical x/y coordinates.";
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
		logWarning() << "Relative vertical angle error (V) Sigma between " + fPoint1->getName() + " and " + fPoint2->getName() + " is undefined because they have identical x/y coordinates.";
	}
	VJac *= -(1 / sqrt(1 - pow2q(d(2) / L))) * pow(L, -3);

	// height difference
	Z = d(2);
	ZJac << 0, 0, 1;

	std::vector<functionEval> result{functionEval{L, LJac}, functionEval{G, GJac}, functionEval{R, RJac}, functionEval{Z, ZJac}, functionEval{V, VJac}};
	return result;
}

#if USE_SERIALIZER
void TLSCalcRelativeError::serialize(ObjectSerializer &obj) const
{
	if (fPoint1)
		obj.addProperty("fPoint1", fPoint1->getName());
	if (fPoint2)
		obj.addProperty("fPoint2", fPoint2->getName());
	obj.addProperty("fSigmaL", fSigmaL);
	obj.addProperty("fSigmaG", fSigmaG);
	obj.addProperty("fSigmaR", fSigmaR);
	obj.addProperty("fSigmaZ", fSigmaZ);
	obj.addProperty("fSigmaV", fSigmaV);
	obj.addProperty("fDestinationFrame", fDestinationFrame);
}
#endif // USE_SERIALIZER
