// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <TPointTransformer.h>

#include "GeodeticConstants.h"
#include "TTreeEntry.h"
#include "TXYH2CCS.h"

TPointTransformer::TPointTransformer(const TDataTree *tree, const TRefSystemFactory::ERefFrame &refFrame) :
	fTree(tree), fRefFrame(refFrame), fMLAused(false), fCGRFused(false), fIsSphere(false), fccs2cgrf()
{
	fLastStationPtName = ""; // No point can have empty name

	if (refFrame == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
	{
		fIsSphere = true;
		fGeoidModel = TRefSystemFactory::EGeoid::kCGSphere;
		fccs2cgrf.reInitialize(fIsSphere); // Implicitly the ccs2cgrf transformation uses ellipsoidal CGRF, if SPHE is used, a SPHE CGRF (cgrfs) shoud be used
	}
	else if (refFrame == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		fGeoidModel = TRefSystemFactory::EGeoid::kCG2000Machine;
	else if (refFrame == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		fGeoidModel = TRefSystemFactory::EGeoid::kCG1985Machine;
	else
		fGeoidModel = TRefSystemFactory::EGeoid::kNoGeoid;
}

//////////////////////////////
// Tree update
//////////////////////////////
void TPointTransformer::updateTransformations()
{
	for (auto &lorPair : fLORTrafoMap)
	{
		lorPair.second.updateTree();
	}
	fLastStationPtName = "";
	fMLAused = false;
}

////////////////////////////////////////
// Transformations related functions
////////////////////////////////////////

void TPointTransformer::transformPointsToMLASystem(const std::string &originName, TPositionVector &originOfMLAPos, TPositionVector &additPointPos)
{
	if (!(fLastStationPtName == originName) || !fMLAused)
	{
		set2MLATransformation(originOfMLAPos);
		fLastStationPtName = originName;
	}
	transform2MLA(additPointPos);
	originOfMLAPos.setX(TLength(0.0));
	originOfMLAPos.setY(TLength(0.0));
	originOfMLAPos.setZ(TLength(0.0));
}

void TPointTransformer::transformVectorToMLASystem(const std::string &originName, const TPositionVector &originOfMLAPos, TFreeVector &freeVector)
{
	if (!(fLastStationPtName == originName) || !fMLAused)
	{
		set2MLATransformation(originOfMLAPos);
		fLastStationPtName = originName;
	}
	transform2MLA(freeVector);
}

void TPointTransformer::transformVectorFromMLAToCCS(const std::string &originName, const TPositionVector &originOfMLAPos, TFreeVector &freeVector)
{
	if (!(fLastStationPtName == originName) || !fMLAused)
	{
		set2MLATransformation(originOfMLAPos);
		fLastStationPtName = originName;
	}
	transformMLA2CGRF(freeVector);
	transformCGRF2CCS(freeVector);
}

void TPointTransformer::transform2MLA(TPositionVector &pv)
{
	fccs2cgrf.transform(pv);
	fcgrf2ilg.transform(pv);
	filg2ila.transform(pv);
	fla2mla.transform(pv);
}

void TPointTransformer::transform2MLA(TFreeVector &fv)
{
	fccs2cgrf.transform(fv);
	fcgrf2ilg.transform(fv);
	filg2ila.transform(fv);
	fla2mla.transform(fv);
}

void TPointTransformer::transform2MLA(TDenseMatrix &pmat, bool isFreeVector)
{
	// throw error if row dim is not 3
	if (pmat.rows() != 3)
	{
		throw std::runtime_error("TPointTransformer::transform2MLA attempting to transform columns of a matrix to MLA but row dimension not equal to 3.");
	}
	for (int colIdx = 0; colIdx < pmat.cols(); colIdx++)
	{
		TVector colVec = pmat.col(colIdx);
		if (isFreeVector)
		{
			// transform to FreeVector
			TFreeVector colVecFree(colVec);
			transform2MLA(colVecFree);
			pmat.col(colIdx) = colVecFree.toRealVector();
		}
		else
		{
			// transform to PositionVector
			TPositionVector colVecPos(colVec);
			transform2MLA(colVecPos);
			pmat.col(colIdx) = colVecPos.toRealVector();
		}
	}
}

// used only for the dver and incl measurements
void TPointTransformer::transformMLA2CGRF(TFreeVector &fv)
{
	fla2mla.transformInverse(fv);
	filg2ila.transformInverse(fv);
	fcgrf2ilg.transformInverse(fv);
}

// used only for the incl measurements
void TPointTransformer::transformCGRF2CCS(TFreeVector &fv)
{
	fccs2cgrf.transformInverse(fv);
}

// Set a new origin of LA system, initialize the transformation
void TPointTransformer::set2MLATransformation(TPositionVector originInCCS)
{
	// Use origin CGRF position as an origin of the Local Geodetic system
	fcgrf2ilg = TCGRF2LGTransformation(originInCCS, fIsSphere);
	// Use origin CCS position as an origin of the Local Astronomical system
	filg2ila = TILG2ILATransformation(originInCCS, fGeoidModel);
	// Use origin CCS position as an origin of the Local Astronomical system
	fla2mla = TLA2MLATransformation(originInCCS, fGeoidModel, TAngle(0.0), TAngle(0.0));
}

TLOR2LOR TPointTransformer::getIdentityTransformation()
{
	// ROOT2ROOT transformation index == an identity transformation
	auto rootFramePosition(fTree->begin());
	return TLOR2LOR(rootFramePosition, rootFramePosition, "IDENTITY");
}

const TLOR2LOR &TPointTransformer::getLORTransformation(TDataTreeIterator originalTreePos, TDataTreeIterator destinationTreePos)
{
	std::string originalFrameName = originalTreePos->get()->frame.getName();
	std::string destinationFrameName = destinationTreePos->get()->frame.getName();

	std::string trafoName = originalFrameName + destinationFrameName;

	auto it = fLORTrafoMap.find(trafoName);

	if (it != fLORTrafoMap.end())
	{
		return it->second;
	}
	else
	{
		it = fLORTrafoMap.emplace(trafoName, TLOR2LOR(originalTreePos, destinationTreePos, trafoName)).first;
		return it->second;
	}
}

void TPointTransformer::transformCCS22DH(TPositionVector &pv)
{
	// check if the position vector is defined in 3d cartesian system, the k2DCartesian is not used in LGC
	if (pv.getCoordSys() != TCoordSysFactory::ECoordSys::k3DCartesian)
		throw std::runtime_error("TPointTransformer::transformCCS22DH attempting to transform a 2D+H vector instead of a 3D vector");

	if (fGeoidModel != TRefSystemFactory::EGeoid::kNoGeoid)
	{
		if (fGeoidModel == TRefSystemFactory::EGeoid::kCGSphere)
			TXYH2CCS::CCS2XYHs(pv);
		else if (fGeoidModel == TRefSystemFactory::EGeoid::kCG2000Machine)
			TXYH2CCS::CCS2XYHg2000Machine(pv);
		else if (fGeoidModel == TRefSystemFactory::EGeoid::kCG1985Machine)
			TXYH2CCS::CCS2XYHg1985Machine(pv);
	}
}

void TPointTransformer::transform2DH2CCS(TPositionVector &pv)
{
	// check if the position vector is defined in 2D+H, the other TCoordSysFactory::ECoordSys are not used in LGC
	if (pv.getCoordSys() != TCoordSysFactory::ECoordSys::k2DPlusH)
		throw std::runtime_error("TPointTransformer::transform2DH2CCS attempting to transform a 3D vector instead of a 2D+H vector");

	if (fGeoidModel != TRefSystemFactory::EGeoid::kNoGeoid)
	{
		if (fGeoidModel == TRefSystemFactory::EGeoid::kCGSphere)
			TXYH2CCS::XYHs2CCS(pv);
		else if (fGeoidModel == TRefSystemFactory::EGeoid::kCG2000Machine)
			TXYH2CCS::XYHg2000Machine2CCS(pv);
		else if (fGeoidModel == TRefSystemFactory::EGeoid::kCG1985Machine)
			TXYH2CCS::XYHg1985Machine2CCS(pv);
	}
}
