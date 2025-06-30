// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "LGCAdjustablePoint.h"

#include <Eigen/Core>
#include <Eigen/Eigenvalues>

#include <Logger.hpp>
#include <TLGCData.h>
#include <TPointTransformer.h>
#include <TTreeEntry.h>

#include "TXYH2CCS.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
bool LGCAdjustablePoint::allfixedParam = false;

LGCAdjustablePoint::LGCAdjustablePoint(const std::string &name) : TAdjustablePoint(name)
{
}

LGCAdjustablePoint::LGCAdjustablePoint(const TPositionVector &pos, bool isXfixed, bool isYfixed, bool isZHfixed, const std::string &name, TRefSystemFactory::ERefFrame referential, TDataTreeIterator positionInTree) :
	TAdjustablePoint(pos, isXfixed, isYfixed, isZHfixed, name, referential), fFramePosition(positionInTree)
{
}

LGCAdjustablePoint LGCAdjustablePoint::createUninitialized(const std::string &name)
{
	LGCAdjustablePoint ap(name);
	return ap;
}

///////////////////////////////////////////////////////////////////////////
// PUBLIC ACCESS METHODS
///////////////////////////////////////////////////////////////////////////

void LGCAdjustablePoint::setProvisionalValue(const TReal &x, const TReal &y, const TReal &z)
{
	fProvisionalValue = TPositionVector(x, y, z, fProvisionalValue.getCoordSys());
	fEstimatedValue = fProvisionalValue;

	// Use H instead of Z if necessary:
	if (fProvisionalValue.getCoordSys() == TCoordSysFactory::k2DPlusH)
	{
		fProvisionalValue.setH(TLength(z, TLength::kMetres));
		TAdjustablePoint::transformEstimatedValue();
	}
}

void LGCAdjustablePoint::setCovarianceMatrixInRoot(const TLGCData *fData)
{
	fCovarianceMatrixInRoot = transformCovar(*this, fData, fData->getTree().begin());
}

void LGCAdjustablePoint::transformProvisionalCoordinates(const TLGCData *fData)
{
	TDataTreeIterator root = fData->getTree().begin();
	TRefSystemFactory::ERefFrame globalRef = fData->getConfig().referential;

	if (root == getFrameTreePosition())
	{
		// the point is defined in the ROOT frame, therefore assign the provisional values in the ROOT frame.
		fProvisionalValueInRoot = fProvisionalValue;
		if (globalRef != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		{
			fProvisionalHeightInRoot = fProvisionalValueInRoot.getH();
			if (globalRef == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
				TXYH2CCS::XYHs2CCS(fProvisionalValueInRoot);
			else if (globalRef == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
				TXYH2CCS::XYHg2000Machine2CCS(fProvisionalValueInRoot);
			else if (globalRef == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
				TXYH2CCS::XYHg1985Machine2CCS(fProvisionalValueInRoot);
		}
	}
	else
	{
		// the point is not defined in the ROOT frame, therefore assign the provisional values in the subframe.
		fProvisionalValueInRoot = fProvisionalValue;

		TLOR2LOR transfo = TLOR2LOR(getFrameTreePosition(), root, "transfo");
		// transform coordinates in the ROOT frame
		transfo.transform(fProvisionalValueInRoot);
		try
		{
			if (globalRef != TRefSystemFactory::ERefFrame::kLocalRefFrame)
			{
				TPositionVector fProvisionalInRootForHCalc(fProvisionalValueInRoot);

				if (globalRef == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
					TXYH2CCS::CCS2XYHs(fProvisionalInRootForHCalc);
				else if (globalRef == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
					TXYH2CCS::CCS2XYHg2000Machine(fProvisionalInRootForHCalc);
				else if (globalRef == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
					TXYH2CCS::CCS2XYHg1985Machine(fProvisionalInRootForHCalc);

				fProvisionalHeightInRoot = fProvisionalInRootForHCalc.getH();
			}
		}
		catch (...)
		{
		}
	}
}

void LGCAdjustablePoint::changeProvValueToCCS(const TLGCData *fData)
{
	TDataTreeIterator root = fData->getTree().begin();
	TRefSystemFactory::ERefFrame globalRef = fData->getConfig().referential;
	if (root == getFrameTreePosition())
	{
		// the point is defined in the ROOT frame, therefore assign the provisional values in the ROOT frame.
		if (globalRef != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		{
			if (globalRef == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
				TXYH2CCS::XYHs2CCS(fProvisionalValue);
			else if (globalRef == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
				TXYH2CCS::XYHg2000Machine2CCS(fProvisionalValue);
			else if (globalRef == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
				TXYH2CCS::XYHg1985Machine2CCS(fProvisionalValue);
		}
	}
}

void LGCAdjustablePoint::transformEstimatedCoordinates(const TLGCData *fData)
{
	TDataTreeIterator root = fData->getTree().begin();
	TRefSystemFactory::ERefFrame globalRef = fData->getConfig().referential;

	if (root == getFrameTreePosition())
	{
		// the point is defined in the ROOT frame, therefore assign the estimated values in the ROOT frame.
		fEstimatedValueInRoot = fEstimatedValue;
	}
	else
	{
		// the point is not defined in the ROOT frame, therefore assign the estimated values in the subframe.
		fEstimatedValueInRoot = fEstimatedValue;

		TLOR2LOR transfo = TLOR2LOR(getFrameTreePosition(), root, "transfo");
		// transform coordinates in the ROOT frame
		transfo.transform(fEstimatedValueInRoot);
	}

	if (globalRef != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		TPositionVector fEstimatedInRootForHCalc(fEstimatedValueInRoot);

		if (globalRef == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
			TXYH2CCS::CCS2XYHs(fEstimatedInRootForHCalc);
		else if (globalRef == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
			TXYH2CCS::CCS2XYHg2000Machine(fEstimatedInRootForHCalc);
		else if (globalRef == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
			TXYH2CCS::CCS2XYHg1985Machine(fEstimatedInRootForHCalc);

		fEstimatedHeightInRoot = fEstimatedInRootForHCalc.getH();
	}
}

void LGCAdjustablePoint::setCorrection(int idx, TReal value)
{
	for (int i = 0; i < 3; i++)
	{
		if (uidx[i] == idx)
		{
			if (i == 0)
			{
				fCorrection[i] = (TLength(value));
				fEstimatedValue.setX(fEstimatedValue.getX() + TLength(value));
				fXValueSet = true;
			}
			else if (i == 1)
			{
				fCorrection[i] = (TLength(value));
				fEstimatedValue.setY(fEstimatedValue.getY() + TLength(value));
				fYValueSet = true;
			}
			else
			{
				fCorrection[2] = (TLength(value));
				fEstimatedValue.setZ(fEstimatedValue.getZ() + TLength(value));
			}

			// If H value is fixed and all variables were set in this step, we need to make transformation: X1Y1Z1 -> X1Y1H0 --> X1Y1Z0new
			if (fHfixed && fXValueSet && fYValueSet)
			{
				transformEstimatedValue();
				if (!(fixedState[0] | allfixedParam))
					fXValueSet = false;
				if (!(fixedState[1] | allfixedParam))
					fYValueSet = false;
			}
			return;
		}
	}

	throw std::logic_error("Invalid unknown index in parameter access.");
}

/*!
	See \ref TVAdjustableObject::setFirstUidx

	\throws Throws a logic_error if no component of the point is variable, i.e. a fixed point.
*/
void LGCAdjustablePoint::setFirstUidx(int idx)
{
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to a fixed point.");
	else if (allfixedParam)
		throw std::logic_error("Trying to assign unknown index to a fixed point.(ALLFIXED is used)");
	for (int i = 0; i < 3; i++)
		if (!(fixedState[i] | allfixedParam))
			uidx[i] = idx++;
}

/// Update the adjustment information of an uninitialized point
void LGCAdjustablePoint::updateFixedState(bool lx, bool ly, bool lz)
{
	fixedState[0] = (lx | allfixedParam);
	fixedState[1] = (ly | allfixedParam);
	fixedState[2] = (lz | allfixedParam);
	// make sure also the h is updated
	if (fProvisionalValue.getCoordSys() == TCoordSysFactory::k2DPlusH)
		fHfixed = fixedState[2];

	fXValueSet = (lx | allfixedParam);
	fYValueSet = (ly | allfixedParam);
}

int LGCAdjustablePoint::getNumUnkn() const
{
	if (!allfixedParam)
		return !(int)fixedState[0] + !(int)fixedState[1] + !(int)fixedState[2];
	else
		return 0;
}

bool LGCAdjustablePoint::hasVariable() const
{
	if (!allfixedParam)
		return !fixedState[0] || !fixedState[1] || !fixedState[2];
	else
		return false;
}

int LGCAdjustablePoint::getFirstUidx() const
{
	if (allfixedParam)
		throw std::logic_error("Trying to get unknown index from fixed coordinate. (ALLFIXED is used)");
	else
	{
		for (int i = 0; i < 3; i++)
			if (!fixedState[i])
				return uidx[i];
		throw std::logic_error("Trying to get unknown index from fixed coordinate.");
	}
}

int LGCAdjustablePoint::getLastUidx() const
{
	if (allfixedParam)
		throw std::logic_error("Trying to get unknown index from fixed coordinate. (ALLFIXED is used)");
	else
	{
		for (int i = 2; i >= 0; i--)
			if (!fixedState[i])
				return uidx[i];
		throw std::logic_error("Trying to get unknown index from fixed coordinate.");
	}
}

int LGCAdjustablePoint::getCoordinateUnknIndex(int d) const
{
	assert3D(d);
	if (allfixedParam)
		throw std::logic_error("Trying to get unknown index from fixed coordinate. (ALLFIXED is used)");
	else if (!fixedState[d])
		return uidx[d];
	else
		throw std::logic_error("Trying to get unknown index from fixed coordinate.");
}

#if USE_SERIALIZER
// Inherited via Serializable
void LGCAdjustablePoint::serialize(ObjectSerializer &obj) const
{
	TAdjustablePoint::serialize(obj);
	obj.addProperty("allfixedParam", allfixedParam);
	obj.addProperty("fFramePosition_ID", fFramePosition.node->data.get()->ID);
	obj.addProperty("fFramePosition_Name", fFramePosition.node->data.get()->frame.getName());

	obj.addProperty("fProvisionalValueInRoot", fProvisionalValueInRoot);
	obj.addProperty("fProvisionalHeightInRoot", fProvisionalHeightInRoot);

	obj.addProperty("fEstimatedValueInRoot", fEstimatedValueInRoot);
	obj.addProperty("fEstimatedHeightInRoot", fEstimatedHeightInRoot);

	obj.addProperty("fCovarianceMatrixInRoot", fCovarianceMatrixInRoot);
	obj.addProperty("fHasPointSigma", fHasPointSigma);
	obj.addProperty("fPointSigma", fPointSigma);
}
#endif

TFreeVector LGCAdjustablePoint::transformSigmaInRoot(const LGCAdjustablePoint &pv, const TLGCData *fData)
{
	return transformSigma(pv, fData, fData->getTree().begin());
}

TFreeVector LGCAdjustablePoint::transformSigma(const LGCAdjustablePoint &pv, const TLGCData *fData, const std::string toFrame)
{
	return transformSigma(pv, fData, fData->locateNode(toFrame));
}

bool LGCAdjustablePoint::isInRootFrame()
{
	return fFramePosition->get()->isROOTNode();
}

TFreeVector LGCAdjustablePoint::transformSigma(const LGCAdjustablePoint &pv, const TLGCData *fData, const TDataTreeIterator toFrame)
{
	// transfor the covariance matrix of a point to the given frame
	Eigen::Matrix3d ptCovar = transformCovar(pv, fData, toFrame);

	// extract sigmas
	// return the modified sigma
	TFreeVector sigmaDestFrame(pv.getEstimatedValue().getCoordSys());
	sigmaDestFrame.setX(TLength(sqrt(ptCovar.coeff(0, 0))));
	sigmaDestFrame.setY(TLength(sqrt(ptCovar.coeff(1, 1))));
	sigmaDestFrame.setZ(TLength(sqrt(ptCovar.coeff(2, 2))));

	return sigmaDestFrame;
}

Eigen::Matrix3d LGCAdjustablePoint::transformCovar(const LGCAdjustablePoint &pv, const TLGCData *fData, const TDataTreeIterator toFrame)
{
	if (pv.getFrameTreePosition() == toFrame)
		return pv.getCovarianceMatrix();
	else
	{
		// get the global covariance matrix
		const TSparseMatrix& covar = *fData->getCovMatByConst();

		int nPar = fData->fUEOIndices.UIndex;
		if ((covar.cols() != nPar) || (covar.rows() != nPar))
		{
			throw std::logic_error("Unknown Covariance matrix is not initialized.");
		}

		// get the global derivative of the transformed point
		TPointTransformer fPointTransfo(&fData->getTree(), fData->getConfig().referential);
		TLOR2LOR completeTrafo = fPointTransfo.getLORTransformation(pv.getFrameTreePosition(), toFrame);
		const TSparseMatrix& jac = completeTrafo.getPointDerivative(fData, pv);

		// compute the covariance matrix in the destination Frame
		return (jac * covar) * jac.transpose();
	}
}

Eigen::Vector3d pointSigmaData::calcRotOffset(const LGCAdjustablePoint &pt, const TLGCData *fData) const
{
	TPositionVector provVal = pt.getProvisionalValue();

	if (provVal.getCoordSys() == TCoordSysFactory::k2DPlusH)
	{ // If position is given in 2D + H system transform it to CCS
		TPointTransformer fPointTransfo(&fData->getTree(), fData->getConfig().referential);
		fPointTransfo.transform2DH2CCS(provVal);
	}

	Eigen::Vector3d provValVect = provVal.toRealVector();
	Eigen::Vector3d estValVect = pt.getEstimatedValue().toRealVector();
	return fRotMat * (estValVect - provValVect);
}
