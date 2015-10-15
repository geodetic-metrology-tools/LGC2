//TLSFreeCnstrCG.cpp :Implementation file
//Base Class for a LS contrib generator processing the constraints in a free survey network
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include "TLSFreeCnstrCG.h"
#include "TLSInputMatrices.h"
#include "TDist.h"
#include "TLSCalcPosVectorParam.h"
#include "TLSConstraintIdentifier.h"

#include "TLSInputMatricesFiller.h"


//////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////
TLSFreeCnstrCG::TLSFreeCnstrCG()
{//default constructor
}


TLSFreeCnstrCG::~TLSFreeCnstrCG()
{// destructor
}






//////////////////////////
//MEMBER FUNCTION
//////////////////////////
bool TLSFreeCnstrCG::processFreeCnstr(const LSCalcDataSet& lsds,TLSInputMatrices& matrices, const TLSConstraintIdentifier& cnstr)
{//adds the offset's contributions to the input matrices
	bool successfullyProcessed = true;

	/////////////////////////////////////
	//init arguments
	TLength xCG = cnstr.getXGravityCenter();
	TLength yCG = cnstr.getYGravityCenter();
	TLength zCG = cnstr.getZGravityCenter();

	TLength xCGEst = cnstr.getXEstimatedGravityCenter();
	TLength yCGEst = cnstr.getYEstimatedGravityCenter();
	TLength zCGEst = cnstr.getZEstimatedGravityCenter();



	LSPosVecConstIter	ptIt = lsds.beginPV();
	LSPosVecConstIter	endIt = lsds.endPV();

	int numberOfPoints = lsds.numPosVectorParam();
	//n accepte pas la division par un entier???
	double nbrPts = numberOfPoints;

	double rotX, rotY, rotZ, ech;
	rotX=0;
	rotY=0;
	rotZ=0;
	ech =0;

	struct freeCnstrRank rank = cnstr.getIndiceOfConstraint();

	///////////////////////////////////////////////
	//build A2
	while ( ptIt != endIt )
	{
		// gets the point's (provisional) coordinates, status and indices
		TLength xPt = ptIt->getProvisionalValue().getX();
		TLength yPt = ptIt->getProvisionalValue().getY();
		TLength zPt = ptIt->getProvisionalValue().getZ();

		MatrixIndex xPtInd = ptIt->getXIndex();
		MatrixIndex yPtInd = ptIt->getYIndex();
		MatrixIndex zPtInd = ptIt->getZIndex();

		TALSCalcParameter::ELSStatus  stsxPt = ptIt->getXStatus();
		TALSCalcParameter::ELSStatus  stsyPt = ptIt->getYStatus();
		TALSCalcParameter::ELSStatus  stszPt = ptIt->getZStatus();


		//dx constraint coefficient
		if (cnstr.getFreeConstraints().dx)
		{
			if(stsxPt == TALSCalcParameter::kVariable)
			{
				double a = 1.0 ;
				successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(rank.dx, xPtInd, a);
			}
		}

		//dy constraint coefficient
		if (cnstr.getFreeConstraints().dy)
		{
			if(stsyPt == TALSCalcParameter::kVariable)
			{
				double a = 1.0;
				successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(rank.dy, yPtInd, a);
			}
		}

		//dz constraint coefficient
		if (cnstr.getFreeConstraints().dz)
		{
			if(stszPt == TALSCalcParameter::kVariable)
			{
				double a = 1.0;
				successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(rank.dz, zPtInd, a);
			}
		}

		//rx constraint coefficient
		if (cnstr.getFreeConstraints().rx)
		{
			if(stsyPt == TALSCalcParameter::kVariable)
			{
				double a = -1.0 * (zPt.getMetresValue() - zCGEst.getMetresValue());
				successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(rank.rx, yPtInd, a);
			}
			if(stszPt == TALSCalcParameter::kVariable)
			{
				double b = yPt.getMetresValue() - yCGEst.getMetresValue();
				successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(rank.rx, zPtInd, b);
			}

			rotX = rotX + getRxCalcValue(ptIt, cnstr);
		}

		//ry constraint coefficient
		if (cnstr.getFreeConstraints().ry)
		{
			if(stsxPt == TALSCalcParameter::kVariable)
			{
				double a = zPt.getMetresValue() - zCGEst.getMetresValue();
				successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(rank.ry, xPtInd, a);
			}
			if(stszPt == TALSCalcParameter::kVariable)
			{
				double b = -1.0 * (xPt.getMetresValue() - xCGEst.getMetresValue());
				successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(rank.ry, zPtInd, b);
			}

			rotY = rotY + getRyCalcValue(ptIt, cnstr);

		}

		//rz constraint coefficient
		if (cnstr.getFreeConstraints().rz)
		{
			if(stsxPt == TALSCalcParameter::kVariable)
			{
				double a = -1.0 * (yPt.getMetresValue() - yCGEst.getMetresValue());
				successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(rank.rz, xPtInd, a);
			}
			if(stsyPt == TALSCalcParameter::kVariable)
			{
				double b = xPt.getMetresValue() - xCGEst.getMetresValue();
				successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(rank.rz, yPtInd, b);
			}

			rotZ = rotZ + getRzCalcValue(ptIt, cnstr);

		}

		//scale constraint coefficient
		if (cnstr.getFreeConstraints().k)
		{
			if(stsxPt == TALSCalcParameter::kVariable)
			{
				double a = xPt.getMetresValue();
				successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(rank.k, xPtInd, a);
			}
			if(stsyPt == TALSCalcParameter::kVariable)
			{
				double b = yPt.getMetresValue();
				successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(rank.k, yPtInd, b);
			}
			if(stszPt == TALSCalcParameter::kVariable)
			{
				double c = zPt.getMetresValue();
				successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(rank.k, zPtInd, c);
			}

			ech = ech + getScaleCalcValue(ptIt, cnstr);
		}

		ptIt++;
	}


	//** setting the misclosure vector element
	//dx constraint coefficient
	if (cnstr.getFreeConstraints().dx)
	{
		double b = -1.0 * nbrPts * (cnstr.getXEstimatedGravityCenter().getMetresValue() - cnstr.getXGravityCenter().getMetresValue());
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.dx, b);
	}

	//dy constraint coefficient
	if (cnstr.getFreeConstraints().dy)
	{
		double b = -1.0 * nbrPts * (cnstr.getYEstimatedGravityCenter().getMetresValue() - cnstr.getYGravityCenter().getMetresValue());
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.dy, b);
	}

	//dz constraint coefficient
	if (cnstr.getFreeConstraints().dz)
	{
		double b = -1.0 * nbrPts * (cnstr.getZEstimatedGravityCenter().getMetresValue() - cnstr.getZGravityCenter().getMetresValue());
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.dz, b);
	}

	//rx constraint coefficient
	if (cnstr.getFreeConstraints().rx)
	{
		double rx = -1.0 * rotX;
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.rx, rx);
	}

	//ry constraint coefficient
	if (cnstr.getFreeConstraints().ry)
	{
		double ry = -1.0 * rotY;
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.ry, ry);
	}

	//rz constraint coefficient
	if (cnstr.getFreeConstraints().rz)
	{
		double rz = -1.0 * rotZ;
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.rz, rz);
	}

	//scale constraint coefficient
	if (cnstr.getFreeConstraints().k)
	{
		double k = -1.0 * ech;
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.k, k);
	}

	return successfullyProcessed;
}
//{//adds the offset's contributions to the input matrices
//	bool successfullyProcessed = true;
//
//	/////////////////////////////////////
//	//init arguments
//	TLength xCG = cnstr.getXGravityCenter();
//	TLength yCG = cnstr.getYGravityCenter();
//	TLength zCG = cnstr.getZGravityCenter();
//
//	TLength xCGEst = cnstr.getXEstimatedGravityCenter();
//	TLength yCGEst = cnstr.getYEstimatedGravityCenter();
//	TLength zCGEst = cnstr.getZEstimatedGravityCenter();
//
//
//
//	LSPosVecConstIter	ptIt = lsds.beginPV();
//	LSPosVecConstIter	endIt = lsds.endPV();
//
//	int numberOfPoints = lsds.numPosVectorParam();
//	//n accepte pas la division par un entier???
//	TReal nbrPts = numberOfPoints;
//	TReal factor = LITERAL(1.0) / nbrPts;
//
//	TReal rotX, rotY, rotZ, ech;
//	rotX=0;
//	rotY=0;
//	rotZ=0;
//	ech =0;
//
//	struct freeCnstrRank rank = cnstr.getIndiceOfConstraint();
//
//	///////////////////////////////////////////////
//	//build A2
//	pair<int, TReal>* cs = new pair<int, TReal>[4];
//	while ( ptIt != endIt )
//	{
//		// gets the point's (provisional) coordinates, status and indices
//		TLength xPt = ptIt->getProvisionalValue().getX();
//		TLength yPt = ptIt->getProvisionalValue().getY();
//		TLength zPt = ptIt->getProvisionalValue().getZ();
//
//		TALSCalcParameter::ELSStatus  stsxPt = ptIt->getXStatus();
//		TALSCalcParameter::ELSStatus  stsyPt = ptIt->getYStatus();
//		TALSCalcParameter::ELSStatus  stszPt = ptIt->getZStatus();
//
//		bool dontRx = false, dontRy = false, dontRz = false, dontEch = false;
//
//		int count = 0;
//
//		if (ptIt->getXStatus() == TALSCalcParameter::kVariable)
//		{
//			//dx constraint coefficient
//			if (cnstr.getFreeConstraints().dx)
//			{
//				if(stsxPt == TALSCalcParameter::kVariable)
//				{
//					TReal a = LITERAL(1.0) ;
//					cs[count].first = rank.dx;
//					cs[count++].second = a;
//				}
//			}
//
//			//ry constraint coefficient
//			if (cnstr.getFreeConstraints().ry)
//			{
//				if(stsxPt == TALSCalcParameter::kVariable)
//				{
//					TReal a = zPt.getMetresValue() - zCGEst.getMetresValue();
//					cs[count].first = rank.ry;
//					cs[count++].second = a;
//				}
//
//				rotY = rotY + getRyCalcValue(ptIt, cnstr);
//				dontRy = true;
//			}
//
//			//rz constraint coefficient
//			if (cnstr.getFreeConstraints().rz)
//			{
//				if(stsxPt == TALSCalcParameter::kVariable)
//				{
//					TReal a = -LITERAL(1.0) * (yPt.getMetresValue() - yCGEst.getMetresValue());
//					cs[count].first = rank.rz;
//					cs[count++].second = a;
//				}
//
//				rotZ = rotZ + getRzCalcValue(ptIt, cnstr);
//				dontRz = true;
//			}
//
//			//scale constraint coefficient
//			if (cnstr.getFreeConstraints().k)
//			{
//				if(stsxPt == TALSCalcParameter::kVariable)
//				{
//					TReal a = xPt.getMetresValue();
//					cs[count].first = rank.k;
//					cs[count++].second = a;
//				}
//
//				ech = ech + getScaleCalcValue(ptIt, cnstr);
//				dontEch = true;
//			}
//
//			qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//			for (int i = 0; i < count; i++)
//			{
//				matrices.setCnstrFirstDgnMtrxElement(cs[i].first, cs[i].second);
//			}
//
//			matrices.setConstraintNewColumn();
//
//			count = 0;
//		}
//
//		if (ptIt->getYStatus() == TALSCalcParameter::kVariable)
//		{
//			//dy constraint coefficient
//			if (cnstr.getFreeConstraints().dy)
//			{
//				if(stsyPt == TALSCalcParameter::kVariable)
//				{
//					TReal a = LITERAL(1.0);
//					cs[count].first = rank.dy;
//					cs[count++].second = a;
//				}
//			}
//
//			//rz constraint coefficient
//			if (cnstr.getFreeConstraints().rz)
//			{
//				if(stsyPt == TALSCalcParameter::kVariable)
//				{
//					TReal b = xPt.getMetresValue() - xCGEst.getMetresValue();
//					cs[count].first = rank.rz;
//					cs[count++].second = b;
//				}
//
//				if (!dontRz)
//				{
//					rotZ = rotZ + getRzCalcValue(ptIt, cnstr);
//				}
//			}
//
//			//rx constraint coefficient
//			if (cnstr.getFreeConstraints().rx)
//			{
//				if(stsyPt == TALSCalcParameter::kVariable)
//				{
//					TReal a = -LITERAL(1.0) * (zPt.getMetresValue() - zCGEst.getMetresValue());
//					cs[count].first = rank.rx;
//					cs[count++].second = a;
//				}
//
//				rotX = rotX + getRxCalcValue(ptIt, cnstr);
//				dontRx = true;
//			}
//
//			//scale constraint coefficient
//			if (cnstr.getFreeConstraints().k)
//			{
//				if(stsyPt == TALSCalcParameter::kVariable)
//				{
//					TReal b = yPt.getMetresValue();
//					cs[count].first = rank.k;
//					cs[count++].second = b;
//				}
//
//				if (!dontEch)
//				{
//					ech = ech + getScaleCalcValue(ptIt, cnstr);
//					dontEch = true;
//				}
//			}
//
//			qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//			for (int i = 0; i < count; i++)
//			{
//				matrices.setCnstrFirstDgnMtrxElement(cs[i].first, cs[i].second);
//			}
//
//			matrices.setConstraintNewColumn();
//
//			count = 0;
//		}
//
//		if (ptIt->getZStatus() == TALSCalcParameter::kVariable)
//		{
//			//dz constraint coefficient
//			if (cnstr.getFreeConstraints().dz)
//			{
//				if(stszPt == TALSCalcParameter::kVariable)
//				{
//					TReal a = LITERAL(1.0);
//					cs[count].first = rank.dz;
//					cs[count++].second = a;
//				}
//			}
//
//			//ry constraint coefficient
//			if (cnstr.getFreeConstraints().ry)
//			{
//				if(stszPt == TALSCalcParameter::kVariable)
//				{
//					TReal b = -LITERAL(1.0) * (xPt.getMetresValue() - xCGEst.getMetresValue());
//					cs[count].first = rank.ry;
//					cs[count++].second = b;
//				}			
//
//				if (!dontRy)
//				{
//					rotY = rotY + getRyCalcValue(ptIt, cnstr);
//				}
//			}
//
//			//rx constraint coefficient
//			if (cnstr.getFreeConstraints().rx)
//			{
//				if(stszPt == TALSCalcParameter::kVariable)
//				{
//					TReal b = yPt.getMetresValue() - yCGEst.getMetresValue();
//					cs[count].first = rank.rx;
//					cs[count++].second = b;
//				}
//
//				if (!dontRx)
//				{
//					rotX = rotX + getRxCalcValue(ptIt, cnstr);
//				}
//			}
//
//			//scale constraint coefficient
//			if (cnstr.getFreeConstraints().k)
//			{
//				if(stszPt == TALSCalcParameter::kVariable)
//				{
//					TReal c = zPt.getMetresValue();
//					cs[count].first = rank.k;
//					cs[count++].second = c;
//				}
//				if (!dontEch)
//				{
//					ech = ech + getScaleCalcValue(ptIt, cnstr);
//				}
//			}
//
//			qsort(cs, count, sizeof (pair<int, TReal>), pairCompare);
//
//			for (int i = 0; i < count; i++)
//			{
//				matrices.setCnstrFirstDgnMtrxElement(cs[i].first, cs[i].second);
//			}
//
//			matrices.setConstraintNewColumn();
//		}
//
//		ptIt++;
//	}
//
//	delete[] cs;
//
//
//	//** setting the misclosure vector element
//	//dx constraint coefficient
//	if (cnstr.getFreeConstraints().dx)
//	{
//		TReal b = -LITERAL(1.0) * nbrPts * (cnstr.getXEstimatedGravityCenter().getMetresValue() - cnstr.getXGravityCenter().getMetresValue());
//		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.dx, b);
//	}
//
//	//dy constraint coefficient
//	if (cnstr.getFreeConstraints().dy)
//	{
//		TReal b = -LITERAL(1.0) * nbrPts * (cnstr.getYEstimatedGravityCenter().getMetresValue() - cnstr.getYGravityCenter().getMetresValue());
//		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.dy, b);
//	}
//
//	//dz constraint coefficient
//	if (cnstr.getFreeConstraints().dz)
//	{
//		TReal b = -LITERAL(1.0) * nbrPts * (cnstr.getZEstimatedGravityCenter().getMetresValue() - cnstr.getZGravityCenter().getMetresValue());
//		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.dz, b);
//	}
//
//	//rx constraint coefficient
//	if (cnstr.getFreeConstraints().rx)
//	{
//		TReal rx = -LITERAL(1.0) * rotX;
//		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.rx, rx);
//	}
//
//	//ry constraint coefficient
//	if (cnstr.getFreeConstraints().ry)
//	{
//		TReal ry = -LITERAL(1.0) * rotY;
//		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.ry, ry);
//	}
//
//	//rz constraint coefficient
//	if (cnstr.getFreeConstraints().rz)
//	{
//		TReal rz = -LITERAL(1.0) * rotZ;
//		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.rz, rz);
//	}
//
//	//scale constraint coefficient
//	if (cnstr.getFreeConstraints().k)
//	{
//		TReal k = -LITERAL(1.0) * ech;
//		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(rank.k, k);
//	}
//
//	return successfullyProcessed;
//
//}

//////////////////////////
//MEMBER PRIVATE FUNCTION
//////////////////////////
TReal	TLSFreeCnstrCG::getRxCalcValue(LSPosVecConstIter ptIt, const TLSConstraintIdentifier& cnstr)
{
	TReal a = -LITERAL(1.0) *
		(
		ptIt->getProvisionalValue().getZ().getMetresValue()
		- cnstr.getZEstimatedGravityCenter().getMetresValue()
		) 
		* 
		(
		ptIt->getEstimatedValue().getY().getMetresValue()
		- cnstr.getYEstimatedGravityCenter().getMetresValue()
		);
	TReal b = (
		ptIt->getProvisionalValue().getY().getMetresValue()
		- cnstr.getYEstimatedGravityCenter().getMetresValue()
		)
		*
		(
		ptIt->getEstimatedValue().getZ().getMetresValue()
		- cnstr.getZEstimatedGravityCenter().getMetresValue()
		);

	TReal calcRx = a+b;
	return calcRx;
}


TReal	TLSFreeCnstrCG::getRyCalcValue(LSPosVecConstIter ptIt, const TLSConstraintIdentifier& cnstr)
{
	TReal a = -LITERAL(1.0) *
		(
		ptIt->getProvisionalValue().getX().getMetresValue()
		- cnstr.getXEstimatedGravityCenter().getMetresValue()
		) 
		* 
		(
		ptIt->getEstimatedValue().getZ().getMetresValue()
		- cnstr.getZEstimatedGravityCenter().getMetresValue()
		);
	TReal b = (
		ptIt->getProvisionalValue().getZ().getMetresValue()
		- cnstr.getZEstimatedGravityCenter().getMetresValue()
		)
		*
		(
		ptIt->getEstimatedValue().getX().getMetresValue()
		- cnstr.getXEstimatedGravityCenter().getMetresValue()
		);

	TReal calcRy = a+b;
	return calcRy;
}


TReal	TLSFreeCnstrCG::getRzCalcValue(LSPosVecConstIter ptIt, const TLSConstraintIdentifier& cnstr)
{
	TReal a = -LITERAL(1.0) *
		(
		ptIt->getProvisionalValue().getY().getMetresValue()
		- cnstr.getYEstimatedGravityCenter().getMetresValue()
		) 
		* 
		(
		ptIt->getEstimatedValue().getX().getMetresValue()
		- cnstr.getXEstimatedGravityCenter().getMetresValue()
		);
	TReal b = (
		ptIt->getProvisionalValue().getX().getMetresValue()
		- cnstr.getXEstimatedGravityCenter().getMetresValue()
		)
		*
		(
		ptIt->getEstimatedValue().getY().getMetresValue()
		- cnstr.getYEstimatedGravityCenter().getMetresValue()
		);

	TReal calcRz = a+b;
	return calcRz;
}


TReal	TLSFreeCnstrCG::getScaleCalcValue(LSPosVecConstIter ptIt, const TLSConstraintIdentifier& cnstr)
{
	TReal xp = ptIt->getProvisionalValue().getX().getMetresValue();
	TReal xe = ptIt->getEstimatedValue().getX().getMetresValue();
	TReal xcg = cnstr.getXEstimatedGravityCenter().getMetresValue();

	TReal a = xp * xe + xcg*xcg;

	TReal yp = ptIt->getProvisionalValue().getY().getMetresValue();
	TReal ye = ptIt->getEstimatedValue().getY().getMetresValue();
	TReal ycg = cnstr.getYEstimatedGravityCenter().getMetresValue();

	TReal b = yp * ye + ycg*ycg;

	TReal zp = ptIt->getProvisionalValue().getZ().getMetresValue();
	TReal ze = ptIt->getEstimatedValue().getZ().getMetresValue();
	TReal zcg = cnstr.getZEstimatedGravityCenter().getMetresValue();

	TReal c = zp * ze + zcg*zcg;

	return (a+b+c);
}

////////////////////////////////////////////////
//END
////////////////////////////////////////////////
