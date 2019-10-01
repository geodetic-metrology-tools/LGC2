#include "TLibrCnstrGenerator.h"
#include <Logger.hpp>


TLibrCnstrGenerator::TLibrCnstrGenerator(TPointTransformer& fPointTransfoFunc, const TLGCData& fDataSet) :
	fPointTransfo(fPointTransfoFunc),
	data(fDataSet)
{
	fCnstrVector.dx = false;
	fCnstrVector.dy = false;
	fCnstrVector.dz = false;
	fCnstrVector.rx = false;
	fCnstrVector.ry = false;
	fCnstrVector.rz = false;
	fCnstrVector.k = false;

	TLength zero(0);
	fXcg = zero;
	fYcg = zero;
	fZcg = zero;
	fXcgEst = fXcg;
	fYcgEst = fYcg;
	fZcgEst = fZcg;

	fCnstrNumber.dx = (int)NO_VALf;
	fCnstrNumber.dy = (int)NO_VALf;
	fCnstrNumber.dz = (int)NO_VALf;
	fCnstrNumber.rx = (int)NO_VALf;
	fCnstrNumber.ry = (int)NO_VALf;
	fCnstrNumber.rz = (int)NO_VALf;
	fCnstrNumber.k = (int)NO_VALf;
}

//////////////////////////
//MEMBER FUNCTION
//////////////////////////
bool TLibrCnstrGenerator::processFreeCnstr(TLSInputMatrices& matrices)
{
	//adds the offset's contributions to the input matrices
	bool successfullyProcessed = true;


	///////////////////////////////////////////////
	// build A2
	//while (ptIt != endIt)

	// Subdivided into a first loop on points for the 3 Center of Gravity constraints
	// The center of gravity of the new estimated positions is recalculated in this first loop.
	// We need this center of gravity for the momentum calculations! (second loop)
	int nXpts = 0;
	int nYpts = 0;
	int nZpts = 0;
	double xGEstim = 0;
	double yGEstim = 0;
	double zGEstim = 0;
	double sumDX = 0;
	double sumDY = 0;
	double sumDZ = 0;
	Eigen::Vector3d vectSumMoments(0, 0, 0);
	double ech = 0.0;


	for (auto& ptIt : data.getPoints())
	{
		TSpatialStatus::ESpatialStatus  fPtStatus = ptIt.getSpatialStatus();
		bool xIsVariable = false;

		if (fPtStatus != TSpatialStatus::ESpatialStatus::kCala)
		{
			MatrixIndex fFisrtUnkIndex = ptIt.getFirstUidx();
			MatrixIndex fLastUnkIndex = ptIt.getLastUidx();

			//dx constraint coefficient
			if (fCnstrVector.dx)
			{
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVx ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					double a = 1.0;
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.dx, fFisrtUnkIndex, a);
					xIsVariable = true;

					nXpts++;
					xGEstim += ptIt.getEstimatedValue().getX().getMetresValue();
					sumDX += ptIt.getCorrection(0).getMetresValue();
				}
			}

			//dy constraint coefficient
			if (fCnstrVector.dy)
			{
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					double a = 1.0;
					if (xIsVariable)
						successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.dy, fFisrtUnkIndex + 1, a);
					else
						successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.dy, fFisrtUnkIndex, a);

					nYpts++;
					yGEstim += ptIt.getEstimatedValue().getY().getMetresValue();
					sumDY += ptIt.getCorrection(1).getMetresValue();
				}
			}

			//dz constraint coefficient
			if (fCnstrVector.dz)
			{
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					double a = 1.0;
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.dz, fLastUnkIndex, a);

					nZpts++;
					zGEstim += ptIt.getEstimatedValue().getZ().getMetresValue();
					sumDZ += ptIt.getCorrection(2).getMetresValue();
				}
			}
		}
	}

	// Estimating the center of gravity of the new estimated point positions
	xGEstim /= nXpts;
	yGEstim /= nYpts;
	zGEstim /= nZpts;

	// If we want to come back to previous method (use precalculted center of gravity), simply decomment the following lines!
/*	xGEstim = fXcgEst.getMetresValue();
	yGEstim = fYcgEst.getMetresValue();
	zGEstim = fZcgEst.getMetresValue(); */

	// Second loop on points for the 3 momentum-constraints

	for (auto& ptIt : data.getPoints())
	{
		TSpatialStatus::ESpatialStatus  fPtStatus = ptIt.getSpatialStatus();

		if (fPtStatus != TSpatialStatus::ESpatialStatus::kCala)
		{
			MatrixIndex fFisrtUnkIndex = ptIt.getFirstUidx();
			MatrixIndex fLastUnkIndex = ptIt.getLastUidx();
			bool xIsVariable = false;

			// Checks first if X is an unknown or not, in order to correctly handle unknown indices
			if (fCnstrVector.dx && (fPtStatus == TSpatialStatus::ESpatialStatus::kVx ||
				fPtStatus == TSpatialStatus::ESpatialStatus::kVxy ||
				fPtStatus == TSpatialStatus::ESpatialStatus::kVxz ||
				fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
				fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown))
				xIsVariable = true;

			// Computes the momentum of the current point with respect to the center of gravity and its spatial displacement
/*			Eigen::Vector3d vectGP(ptIt.getEstimatedValue().getX().getMetresValue() - fXcgEst.getMetresValue(),
								   ptIt.getEstimatedValue().getY().getMetresValue() - fYcgEst.getMetresValue(),
								   ptIt.getEstimatedValue().getZ().getMetresValue() - fZcgEst.getMetresValue()); */

			Eigen::Vector3d vectGP(ptIt.getEstimatedValue().getX().getMetresValue() - xGEstim,
				ptIt.getEstimatedValue().getY().getMetresValue() - yGEstim,
				ptIt.getEstimatedValue().getZ().getMetresValue() - zGEstim);

			Eigen::Vector3d vectDX(ptIt.getCorrection(0).getMetresValue(), ptIt.getCorrection(1).getMetresValue(), ptIt.getCorrection(2).getMetresValue());

			vectSumMoments += vectGP.cross(vectDX);

			//rx constraint coefficient
			if (fCnstrVector.rx)
			{
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					//	double a = -1.0 * (ptIt.getEstimatedValue().getZ().getMetresValue() - fZcgEst.getMetresValue());
					double a = -1.0 * (ptIt.getEstimatedValue().getZ().getMetresValue() - zGEstim);
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.rx, fFisrtUnkIndex + 1, a);
				}
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					//	double b = (ptIt.getEstimatedValue().getY().getMetresValue() - fYcgEst.getMetresValue());
					double b = ptIt.getEstimatedValue().getY().getMetresValue() - yGEstim;
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.rx, fLastUnkIndex, b);
				}
			}

			//ry constraint coefficient
			if (fCnstrVector.ry)
			{
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVx ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					//	double a = (ptIt.getEstimatedValue().getZ().getMetresValue() - fZcgEst.getMetresValue());
					double a = ptIt.getEstimatedValue().getZ().getMetresValue() - zGEstim;
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.ry, fFisrtUnkIndex, a);
				}
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					//	double b = -1.0 * (ptIt.getEstimatedValue().getX().getMetresValue() - fXcgEst.getMetresValue());
					double b = -1.0 * (ptIt.getEstimatedValue().getX().getMetresValue() - xGEstim);
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.ry, fLastUnkIndex, b);
				}
			}

			//rz constraint coefficient
			if (fCnstrVector.rz)
			{
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVx ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					//	double a = -1.0 * (ptIt.getEstimatedValue().getY().getMetresValue() - fYcgEst.getMetresValue());
					double a = -1.0 * (ptIt.getEstimatedValue().getY().getMetresValue() - yGEstim);
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.rz, fFisrtUnkIndex, a);
				}
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					//	double b = (ptIt.getEstimatedValue().getX().getMetresValue() - fXcgEst.getMetresValue());
					double b = ptIt.getEstimatedValue().getX().getMetresValue() - xGEstim;
					if (xIsVariable)
						successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.rz, fFisrtUnkIndex + 1, b);
					else
						successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.rz, fFisrtUnkIndex, b);
				}
			}

			//scale constraint coefficient
			if (fCnstrVector.k)
			{
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVx ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					double a = ptIt.getEstimatedValue().getX().getMetresValue();
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.k, fFisrtUnkIndex, a);
				}
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					double b = ptIt.getEstimatedValue().getY().getMetresValue();
					if (xIsVariable)
						successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.k, fFisrtUnkIndex + 1, b);
					else
						successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.k, fFisrtUnkIndex, b);
				}
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					double c = ptIt.getEstimatedValue().getZ().getMetresValue();
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.k, fLastUnkIndex, c);
				}

				ech = ech + getScaleCalcValue(ptIt);
			}

		}
	}

	//***********************************************
	//** setting the misclosure vector element     **
	//***********************************************
	logDebug() << "\nFree network adjustment: setting constraints misclosure vector W2";
	//dx constraint coefficient
	if (fCnstrVector.dx)
	{
		double a = -sumDX;
		//	double a = -1.0 * nbrPts * (fXcgEst.getMetresValue() - fXcg.getMetresValue());
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.dx, a);
		logDebug() << "W2[" << fCnstrNumber.dx << "] = " << a << "  (misclosure-constraint on center of gravity position Xg)";
	}

	//dy constraint coefficient
	if (fCnstrVector.dy)
	{
		double a = -sumDY;
		//	double a = -1.0 * nbrPts * (fYcgEst.getMetresValue() - fYcg.getMetresValue());
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.dy, a);
		logDebug() << "W2[" << fCnstrNumber.dy << "] = " << a << "  (misclosure-constraint on center of gravity position Yg)";
	}

	//dz constraint coefficient
	if (fCnstrVector.dz)
	{
		double a = -sumDZ;
		//	double a = -1.0 * nbrPts * (fZcgEst.getMetresValue() - fZcg.getMetresValue());
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.dz, a);
		logDebug() << "W2[" << fCnstrNumber.dz << "] = " << a << "  (misclosure-constraint on center of gravity position Zg)";
	}

	//rx constraint coefficient
	if (fCnstrVector.rx)
	{
		double rx = -1.0 * vectSumMoments[0];
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.rx, rx);
		logDebug() << "W2[" << fCnstrNumber.rx << "] = " << rx << "  (misclosure-constraint along X of momenta-sum)";
	}

	//ry constraint coefficient
	if (fCnstrVector.ry)
	{
		double ry = -1.0 * vectSumMoments[1];
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.ry, ry);
		logDebug() << "W2[" << fCnstrNumber.ry << "] = " << ry << "  (misclosure-constraint along Y of momenta-sum)";
	}

	//rz constraint coefficient
	if (fCnstrVector.rz)
	{
		double rz = -1.0 * vectSumMoments[2];
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.rz, rz);
		logDebug() << "W2[" << fCnstrNumber.rz << "] = " << rz << "  (misclosure-constraint along Z of momenta-sum)";
	}

	//scale constraint coefficient
	if (fCnstrVector.k)
	{
		double k = -1.0 * ech;
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.k, k);
		logDebug() << "W2[" << fCnstrNumber.k << "] = " << k << "  (misclosure-constraint on the scale-parameter)";

	}

	return successfullyProcessed;
}

void TLibrCnstrGenerator::initCnstrIdentifier(const TLGCData& lsds)
{
	//Initialise elements
	bool T, F;
	T = true;
	F = false;

	struct isFreeCnstr fCnstr;

	bool bscale = false;
	struct isFreeCnstr vscale;
	vscale.dx = T;
	vscale.dy = T;
	vscale.dz = T;
	vscale.rx = T;
	vscale.ry = T;
	vscale.rz = T;
	vscale.k = F;

	// bool brotation = false;
	struct isFreeCnstr vrotation;
	vrotation.dx = T;
	vrotation.dy = T;
	vrotation.dz = T;
	vrotation.rx = T;
	vrotation.ry = T;
	vrotation.rz = F;
	vrotation.k = T;

	//get point's type used for calculation
	auto iter = lsds.getPoints().begin();
	auto iterEnd = lsds.getPoints().end();

	////constraints on points
	int nbCALA = lsds.getPointsDimension(TSpatialStatus::ESpatialStatus::kCala);
	int nbPOIN = lsds.getPointsDimension(TSpatialStatus::ESpatialStatus::kVxyz);
	int nbVXY = lsds.getPointsDimension(TSpatialStatus::ESpatialStatus::kVxy);
	int nbVXZ = lsds.getPointsDimension(TSpatialStatus::ESpatialStatus::kVxz);
	int nbVYZ = lsds.getPointsDimension(TSpatialStatus::ESpatialStatus::kVyz);
	int nbVZ = lsds.getPointsDimension(TSpatialStatus::ESpatialStatus::kVz);

	int nbORIE = lsds.getMeasurementDimension(TMeasurementsGlobal::kORIE);
	int nbPDOR = lsds.getMeasurementDimension(TMeasurementsGlobal::kPDOR);
	
	//GKA (30/09/2019): old method: int numberOfPoints = (int)lsds.getPoints().numObjects() does not work because includes the wire objects
	int numberOfPoints = nbCALA + nbPOIN + nbVXY + nbVXZ + nbVYZ + nbVYZ + nbVYZ + nbVZ;

	//n accepte pas la division par un entier???
	TReal div = numberOfPoints;
	TReal factor = 1 / div;

	//fill fCnstr
	switch (nbCALA)
	{

	case 0:  //0 CALA point

		// Only one point type
		if (nbPOIN == numberOfPoints)
		{
			if (nbORIE == 1)
			{
				fCnstr.dx = T;
				fCnstr.dy = T;
				fCnstr.dz = T;
				fCnstr.rx = T;
				fCnstr.ry = F;
				fCnstr.rz = T;
				fCnstr.k = T;
				break;
			}
			else if (nbORIE == 2)
			{
				fCnstr.dx = T;
				fCnstr.dy = T;
				fCnstr.dz = T;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			else if (nbORIE > 2)
			{
				fCnstr.dx = T;
				fCnstr.dy = T;
				fCnstr.dz = T;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = T;
				fCnstr.k = T;
				break;
			}
			else {
				fCnstr.dx = T;
				fCnstr.dy = T;
				fCnstr.dz = T;
				fCnstr.rx = T;
				fCnstr.ry = T;
				fCnstr.rz = T;
				fCnstr.k = T;
				break;
			}
		}
		else if (nbVXY == numberOfPoints)
		{
			fCnstr.dx = T;
			fCnstr.dy = T;
			fCnstr.dz = F;
			fCnstr.rx = F;
			fCnstr.ry = F;
			fCnstr.rz = T;
			fCnstr.k = T;
			break;
		}
		else if (nbVXZ == numberOfPoints)
		{
			fCnstr.dx = T;
			fCnstr.dy = F;
			fCnstr.dz = T;
			fCnstr.rx = F;
			fCnstr.ry = T;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}
		else if (nbVYZ == numberOfPoints)
		{
			fCnstr.dx = F;
			fCnstr.dy = T;
			fCnstr.dz = T;
			fCnstr.rx = T;
			fCnstr.ry = F;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}
		else if (nbVZ == numberOfPoints)
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = T;
			fCnstr.rx = F;
			fCnstr.ry = F;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}

		// Mixing point types
		if (nbVZ > 0)
		{
			if (nbVXY > 0) // VZ + VXY + ...
			{
				fCnstr.dx = F;
				fCnstr.dy = F;
				fCnstr.dz = F;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			else if ((nbVZ > 1 && (nbVXZ > 0 || nbVYZ > 0 || nbPOIN > 0)) || ((nbVXZ > 1 || (nbVXZ == 1 && nbVYZ > 0))) || ((nbORIE > 1 || (nbORIE == 1 && nbPOIN > 0)))) // VZ + VZ + (VXZ|VYZ|POIN)  || VZ+ VXZ + (VXZ | VYZ)  || VZ+ORIE+(POIN | ORIE)
			{
				fCnstr.dx = F;
				fCnstr.dy = F;
				fCnstr.dz = T;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			else if (nbVXZ == 1 && nbVXY == 0 && nbVYZ == 0 && nbPDOR == 0 && (nbPOIN > 0 || nbORIE > 0)) //VZ+ (POIN | ORIE) +VXZ
			{
				fCnstr.dx = F;
				fCnstr.dy = F;
				fCnstr.dz = T;
				fCnstr.rx = F;
				fCnstr.ry = T;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			else if (nbVYZ > 0 && nbVXY == 0 && nbVYZ > 0 && nbPDOR == 0) //VZ+ (POIN | ORIE | VYZ) +VYZ
			{
				fCnstr.dx = F;
				fCnstr.dy = F;
				fCnstr.dz = T;
				fCnstr.rx = T;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			else if (nbVZ == 1 && nbVXY == 0 && nbVYZ == 0 && nbPDOR == 0 && nbORIE == 0 && nbPOIN > 1) //VZ+ POIN + POIN
			{
				fCnstr.dx = F;
				fCnstr.dy = F;
				fCnstr.dz = T;
				fCnstr.rx = T;
				fCnstr.ry = T;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			else
			{
				fCnstr.dx = F;
				fCnstr.dy = F;
				fCnstr.dz = T;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
		}
		else if (nbVXY > 0)
		{
			if (nbVXZ > 0 && nbVYZ > 0)  // VXY + VXZ + VYZ
			{
				fCnstr.dx = F;
				fCnstr.dy = F;
				fCnstr.dz = F;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			else if (nbVXZ > 0) //VXY + VXZ + (VXZ , POIN, ORIE, VXY)
			{
				fCnstr.dx = T;
				fCnstr.dy = F;
				fCnstr.dz = F;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			else if (nbVYZ > 0) //VXY + VYZ + (VXY, VYZ, POIN, ORIE)
			{
				fCnstr.dx = F;
				fCnstr.dy = T;
				fCnstr.dz = F;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			else if ((nbVXY == 2 && nbORIE > 0) | (nbVXY == 1 && nbORIE > 0)) //VXY + VXY + ORIE | VXY +ORIE +(POIN|ORIE)
			{
				fCnstr.dx = T;
				fCnstr.dy = T;
				fCnstr.dz = F;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			else if ((nbVXY == 2 || nbVXY == 1) && nbPOIN > 0) //VXY + (POIN|VXY) +POIN
			{
				fCnstr.dx = T;
				fCnstr.dy = T;
				fCnstr.dz = F;
				fCnstr.rx = T;
				fCnstr.ry = T;
				fCnstr.rz = T;
				fCnstr.k = T;
				break;
			}
			else
			{
				fCnstr.dx = T;
				fCnstr.dy = T;
				fCnstr.dz = F;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = T;
				fCnstr.k = T;
				break;
			}
		}
		else if (nbVXZ > 0)
		{
			if (nbVYZ > 0) // VXZ + VYZ + (VXZ, VYZ, POIN, ORIE)
			{
				fCnstr.dx = F;
				fCnstr.dy = F;
				fCnstr.dz = T;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			else if (nbVXZ == 1 && nbPOIN == (numberOfPoints - 1) && nbORIE > 1)
			{
				fCnstr.dx = T;
				fCnstr.dy = F;
				fCnstr.dz = T;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			else // other case wich are not yet tested with VXZ : VXZ + VXZ + (POIN, ORIE) ou VXZ + POIN + (POIN, ORIE)
			{
				fCnstr.dx = T;
				fCnstr.dy = F;
				fCnstr.dz = T;
				fCnstr.rx = F;
				fCnstr.ry = T;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
		}
		else if (nbVYZ > 0) // last combinations with VYZ
		{
			if (nbVYZ == 1 && nbPOIN == (numberOfPoints - 1) && nbORIE > 1) // VYZ + ORIE +ORIE
			{
				fCnstr.dx = F;
				fCnstr.dy = T;
				fCnstr.dz = T;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
			if (nbVYZ == 1 && nbPOIN == (numberOfPoints - 1) && nbORIE == 0) // VYZ + POIN +POIN
			{
				fCnstr.dx = F;
				fCnstr.dy = T;
				fCnstr.dz = T;
				fCnstr.rx = T;
				fCnstr.ry = T;
				fCnstr.rz = T;
				fCnstr.k = T;
				break;
			}
			else // VYZ + VYZ + (POIN , ORIE) | VYZ +POIN + ORIE
			{
				fCnstr.dx = F;
				fCnstr.dy = T;
				fCnstr.dz = T;
				fCnstr.rx = T;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = T;
				break;
			}
		}




	case 1: // only 1 CALA is used
		if ((nbORIE > 0 || nbPDOR > 0) && (nbVXY > 0 || nbVXZ > 0 || nbVYZ > 0 || nbVZ > 0)) // CALA + (ORIE | PDOR) + (VXY | VXZ | VYZ | VZ)
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = F;
			fCnstr.ry = F;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}
		else if ((nbORIE == 1 || nbPDOR == 1) && nbVXY == 0 && nbVXZ == 0 && nbVYZ == 0 && nbVZ == 0) // CALA + (ORIE | PDOR) + POIN 
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = T;
			fCnstr.ry = T;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}
		else if ((nbORIE > 0 || nbPDOR > 0) && nbVXY == 0 && nbVXZ == 0 && nbVYZ == 0 && nbVZ == 0) // CALA + (ORIE | PDOR) + ORIE 
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = F;
			fCnstr.ry = F;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}
		else if (nbORIE == 0 && nbPDOR == 0 && nbVXY == 0 && nbVXZ == 0 && nbVYZ == 0 && (nbVZ == 0 || nbVZ == 1))  //CALA + POIN + POIN  ou CALA + POIN +VZ
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = T;
			fCnstr.ry = T;
			fCnstr.rz = T;
			fCnstr.k = T;
			break;
		}
		else if ((nbVXY > 0 || nbVXZ > 0 || nbVYZ > 0) && nbVZ > 0) // CALA + VZ + (VXY | VXZ | VYZ)
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = F;
			fCnstr.ry = F;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}
		else if ((nbVXY == 0 && nbVXZ == 0 && nbVYZ == 0 && nbPDOR == 0 && nbORIE == 0) && nbVZ > 0) // CALA + VZ + VZ
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = T;
			fCnstr.ry = T;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}
		else if ((nbVZ == 0 && nbVXZ == 0 && nbVYZ == 0) && nbVXY > 0) // CALA + VXY + (VXY |POIN)
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = F;
			fCnstr.ry = F;
			fCnstr.rz = T;
			fCnstr.k = T;
			break;
		}
		else if (nbVZ == 0 && (nbVXZ > 0 || nbVYZ > 0) && nbVXY > 0) // CALA + VXY + (VXZ | VYZ )
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = F;
			fCnstr.ry = F;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}
		else if (nbVZ == 0 && nbVXY == 0 && (nbVYZ > 0 || nbORIE > 0 || nbPDOR > 0) && nbVXZ > 0)// CALA + VXZ + (VYZ |PDOR|ORIE)
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = F;
			fCnstr.ry = F;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}
		else if (nbVZ == 0 && nbVXY == 0 && nbVYZ == 0 && nbVXZ > 1)// CALA + VXZ + VXZ 
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = F;
			fCnstr.ry = T;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}
		else if (nbVZ == 0 && nbVXY == 0 && nbVYZ == 0 && nbORIE == 0 && nbPDOR == 0 && nbVXZ == 1)// CALA + VXZ + POIN 
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = F;
			fCnstr.ry = T;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}
		else //CALA + VYZ +(VYZ | POIN)
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = T;
			fCnstr.ry = F;
			fCnstr.rz = F;
			fCnstr.k = T;
			break;
		}



	case 2: // only 2 CALA are used
	{
		if (nbVXY > 0 || nbVXZ > 0 || nbVYZ > 0 || nbVZ > 0 || nbPDOR > 0 || nbORIE > 0) // CALA + CALA + (VXY | VXZ | VYZ | VZ| PDOR| ORIE)
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = F;
			fCnstr.ry = F;
			fCnstr.rz = F;
			fCnstr.k = F;
			break;
		}
		else // CALA + CALA + POIN
		{
			fCnstr.dx = F;
			fCnstr.dy = F;
			fCnstr.dz = F;
			fCnstr.rx = T;
			fCnstr.ry = T;
			fCnstr.rz = T;
			fCnstr.k = F;
			break;
		}
	}

	default: // more than 3 CALA are used
	{
		fCnstr.dx = F;
		fCnstr.dy = F;
		fCnstr.dz = F;
		fCnstr.rx = F;
		fCnstr.ry = F;
		fCnstr.rz = F;
		fCnstr.k = F;
		break;
	}
	}


	//build gravity center
	while (iter != iterEnd)
	{
		//build gravity center
		fXcg = fXcg + (iter->getEstimatedValue().getX() * factor);
		fYcg = fYcg + (iter->getEstimatedValue().getY() * factor);
		fZcg = fZcg + (iter->getEstimatedValue().getZ() * factor);

		iter++;
	}

	//setEstimatedGravityCenterCoord(fXcg, fYcg, fZcg);
	fXcgEst = fXcg;
	fYcgEst = fYcg;
	fZcgEst = fZcg;

	logDebug() << "Init gravity Center: Xg=" << fXcg << " ; Yg =" << fYcg << " ; Zg=" << fZcg;

	//constraints on distances
	if (lsds.getMeasurementDimension(TMeasurementsGlobal::kDSPT) > 0 ||
		lsds.getMeasurementDimension(TMeasurementsGlobal::kPLR3D) > 0 ||
		lsds.getMeasurementDimension(TMeasurementsGlobal::kDIST) > 0 ||
		lsds.getMeasurementDimension(TMeasurementsGlobal::kDHOR) > 0 ||
		lsds.getMeasurementDimension(TMeasurementsGlobal::kDVER) > 0 ||
		lsds.getMeasurementDimension(TMeasurementsGlobal::kDLEV) > 0 ||
		lsds.getMeasurementDimension(TMeasurementsGlobal::kECHO) > 0 ||
		lsds.getMeasurementDimension(TMeasurementsGlobal::kECSP) > 0 ||
		lsds.getMeasurementDimension(TMeasurementsGlobal::kECVE) > 0 ||
		lsds.getMeasurementDimension(TMeasurementsGlobal::kECTH) > 0)
		if (bscale == false)
			bscale = true;


	//constraints on orientation
	//if (lsds.getMeasurementDimension(TMeasurementsGlobal::kORIE) > 0)
	//	if (brotation == false)
	//		brotation = true;


	//build constraint vector
	setCnstrIdentifier(T, T, T, T, T, T, T);

	addCnstr(fCnstr);

	/*
	if (bx == true)
		addCnstr(vx);

	if (by == true)
		addCnstr(vy);

	if (bz == true)
		addCnstr(vz);

	if (bxy == true)
		addCnstr(vxy);

	if (bxz == true)
		addCnstr(vxz);

	if (byz == true)
		addCnstr(vyz);

	if (bxyz == true)
		addCnstr(vxyz);

	if (bcala == true)
		addCnstr(vcala);
	*/

	if (bscale == true)
		addCnstr(vscale);

	//if (brotation == true)
	//	addCnstr(vrotation);


	//Calc the number of constraint
	int i = -1;
	if (fCnstrVector.dx)
	{
		i = i + 1;
		fCnstrNumber.dx = i;
	}
	if (fCnstrVector.dy)
	{
		i = i + 1;
		fCnstrNumber.dy = i;
	}
	if (fCnstrVector.dz)
	{
		i = i + 1;
		fCnstrNumber.dz = i;
	}
	if (fCnstrVector.rx)
	{
		i = i + 1;
		fCnstrNumber.rx = i;
	}
	if (fCnstrVector.ry)
	{
		i = i + 1;
		fCnstrNumber.ry = i;
	}
	if (fCnstrVector.rz)
	{
		i = i + 1;
		fCnstrNumber.rz = i;
	}
	if (fCnstrVector.k)
	{
		i = i + 1;
		fCnstrNumber.k = i;
	}

	return;
}


void TLibrCnstrGenerator::setEstimatedGravityCenterCoord(TLength x, TLength y, TLength z)
{
	// Keeps the former position of the center of gravity...
	fXcg = fXcgEst;
	fYcg = fYcgEst;
	fZcg = fZcgEst;

	//... and sets the new given coordinates.
	fXcgEst = x;
	fYcgEst = y;
	fZcgEst = z;

	return;
}


int TLibrCnstrGenerator::getNumberOfConstraint() const
{
	int i = 0;
	i = fCnstrVector.dx + fCnstrVector.dy + fCnstrVector.dz +
		fCnstrVector.rx + fCnstrVector.ry + fCnstrVector.rz +
		fCnstrVector.k;
	return i;
}


//////////////////////////
//MEMBER PRIVATE FUNCTION
//////////////////////////

TReal	TLibrCnstrGenerator::getScaleCalcValue(const TAdjustablePoint& ptIt)
{
	TReal xp = ptIt.getProvisionalValue().getX().getMetresValue();
	TReal xe = ptIt.getEstimatedValue().getX().getMetresValue();


	TReal a = xp * xe + fXcgEst.getMetresValue()*fXcgEst.getMetresValue();

	TReal yp = ptIt.getProvisionalValue().getY().getMetresValue();
	TReal ye = ptIt.getEstimatedValue().getY().getMetresValue();


	TReal b = yp * ye + fYcgEst.getMetresValue()*fYcgEst.getMetresValue();

	TReal zp = ptIt.getProvisionalValue().getZ().getMetresValue();
	TReal ze = ptIt.getEstimatedValue().getZ().getMetresValue();

	TReal c = zp * ze + fZcgEst.getMetresValue()*fZcgEst.getMetresValue();

	return (a + b + c);
}

void TLibrCnstrGenerator::setCnstrIdentifier(const bool dx, const bool dy, const bool dz,
	const bool rx, const bool ry, const bool rz,
	const bool k)
{
	fCnstrVector.dx = dx;
	fCnstrVector.dy = dy;
	fCnstrVector.dz = dz;
	fCnstrVector.rx = rx;
	fCnstrVector.ry = ry;
	fCnstrVector.rz = rz;
	fCnstrVector.k = k;

	return;
}

void	TLibrCnstrGenerator::addCnstr(const struct isFreeCnstr cnstr)
{
	fCnstrVector.dx = fCnstrVector.dx && cnstr.dx;
	fCnstrVector.dy = fCnstrVector.dy && cnstr.dy;
	fCnstrVector.dz = fCnstrVector.dz && cnstr.dz;
	fCnstrVector.rx = fCnstrVector.rx && cnstr.rx;
	fCnstrVector.ry = fCnstrVector.ry && cnstr.ry;
	fCnstrVector.rz = fCnstrVector.rz && cnstr.rz;
	fCnstrVector.k = fCnstrVector.k  && cnstr.k;

	return;
}
