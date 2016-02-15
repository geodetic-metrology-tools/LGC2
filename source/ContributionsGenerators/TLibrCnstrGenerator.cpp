#include "TLibrCnstrGenerator.h"


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

	fCnstrNumber.dx = NO_VALf;
	fCnstrNumber.dy = NO_VALf;
	fCnstrNumber.dz = NO_VALf;
	fCnstrNumber.rx = NO_VALf;
	fCnstrNumber.ry = NO_VALf;
	fCnstrNumber.rz = NO_VALf;
	fCnstrNumber.k = NO_VALf;
}

//////////////////////////
//MEMBER FUNCTION
//////////////////////////
bool TLibrCnstrGenerator::processFreeCnstr(TLSInputMatrices& matrices)
{//adds the offset's contributions to the input matrices
	bool successfullyProcessed = true;

	TReal nbrPts = data.getPoints().numObjects();

	double rotX, rotY, rotZ, ech;
	rotX = 0.0;
	rotY = 0.0;
	rotZ = 0.0;
	ech = 0.0;

	///////////////////////////////////////////////
	//build A2
	//while (ptIt != endIt)
	for (auto& ptIt : data.getPoints())
	{
		// gets the point's (provisional) coordinates, status and indices
		TLength xPt = ptIt.getEstimatedValue().getX();
		TLength yPt = ptIt.getEstimatedValue().getY();
		TLength zPt = ptIt.getEstimatedValue().getZ();

		TSpatialStatus::ESpatialStatus  fPtStatus = ptIt.getSpatialStatus();

		if (fPtStatus != TSpatialStatus::ESpatialStatus::kCala)
		{
			MatrixIndex fFisrtUnkIndex = ptIt.getFirstUidx();
			MatrixIndex fLastUnkIndex = ptIt.getLastUidx();




			bool xIsVariable = false;
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
				}
			}

			//rx constraint coefficient
			if (fCnstrVector.rx)
			{
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					double a = -1.0 * (zPt.getMetresValue() - fZcgEst.getMetresValue());
					if (xIsVariable)
						successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.rx, fFisrtUnkIndex + 1, a);
					else
						successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.rx, fFisrtUnkIndex + 1, a);
				}
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					double b = yPt.getMetresValue() - fYcgEst.getMetresValue();
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.rx, fLastUnkIndex, b);
				}

				rotX = rotX + getRxCalcValue(ptIt);
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
					double a = zPt.getMetresValue() - fZcgEst.getMetresValue();
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.ry, fFisrtUnkIndex, a);
				}
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					double b = -1.0 * (xPt.getMetresValue() - fXcgEst.getMetresValue());
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.ry, fLastUnkIndex, b);
				}

				rotY = rotY + getRyCalcValue(ptIt);

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
					double a = -1.0 * (yPt.getMetresValue() - fYcgEst.getMetresValue());
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.rz, fFisrtUnkIndex, a);
				}
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					double b = xPt.getMetresValue() - fXcgEst.getMetresValue();
					if (xIsVariable)
						successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.rz, fFisrtUnkIndex + 1, b);
					else
						successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.rz, fFisrtUnkIndex, b);
				}

				rotZ = rotZ + getRzCalcValue(ptIt);

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
					double a = xPt.getMetresValue();
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.k, fFisrtUnkIndex, a);
				}
				if (fPtStatus == TSpatialStatus::ESpatialStatus::kVy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxy ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kVxyz ||
					fPtStatus == TSpatialStatus::ESpatialStatus::kUnknown)
				{
					double b = yPt.getMetresValue();
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
					double c = zPt.getMetresValue();
					successfullyProcessed = matrices.setCnstrFirstDgnMtrxElement(fCnstrNumber.k, fLastUnkIndex, c);
				}

				ech = ech + getScaleCalcValue(ptIt);
			}

		}
	}


	//** setting the misclosure vector element
	//dx constraint coefficient
	if (fCnstrVector.dx)
	{
		double b = -1.0 * nbrPts * (fXcgEst.getMetresValue() - fXcg.getMetresValue());
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.dx, b);
	}

	//dy constraint coefficient
	if (fCnstrVector.dy)
	{
		double b = -1.0 * nbrPts * (fYcgEst.getMetresValue() - fYcg.getMetresValue());
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.dy, b);
	}

	//dz constraint coefficient
	if (fCnstrVector.dz)
	{
		double b = -1.0 * nbrPts * (fZcgEst.getMetresValue() - fZcg.getMetresValue());
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.dz, b);
	}

	//rx constraint coefficient
	if (fCnstrVector.rx)
	{
		double rx = -1.0 * rotX;
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.rx, rx);
	}

	//ry constraint coefficient
	if (fCnstrVector.ry)
	{
		double ry = -1.0 * rotY;
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.ry, ry);
	}

	//rz constraint coefficient
	if (fCnstrVector.rz)
	{
		double rz = -1.0 * rotZ;
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.rz, rz);
	}

	//scale constraint coefficient
	if (fCnstrVector.k)
	{
		double k = -1.0 * ech;
		successfullyProcessed = matrices.setCnstrMisclosureVectorElement(fCnstrNumber.k, k);

	}

	return successfullyProcessed;
}

void TLibrCnstrGenerator::initCnstrIdentifier(const TLGCData& lsds)
{
	//Initialise elements
	bool T, F;
	T = true;
	F = false;

	bool bx = false;
	struct isFreeCnstr vx;
	vx.dx = T;
	vx.dy = F;
	vx.dz = F;
	vx.rx = F;
	vx.ry = F;
	vx.rz = F;
	vx.k = F;

	bool by = false;
	struct isFreeCnstr vy;
	vy.dx = F;
	vy.dy = T;
	vy.dz = F;
	vy.rx = F;
	vy.ry = F;
	vy.rz = F;
	vy.k = F;

	bool bz = false;
	struct isFreeCnstr vz;
	vz.dx = F;
	vz.dy = F;
	vz.dz = T;
	vz.rx = F;
	vz.ry = F;
	vz.rz = F;
	vz.k = F;

	bool bxy = false;
	struct isFreeCnstr vxy;
	vxy.dx = T;
	vxy.dy = T;
	vxy.dz = F;
	vxy.rx = F;
	vxy.ry = F;
	vxy.rz = T;
	vxy.k = F;

	bool bxz = false;
	struct isFreeCnstr vxz;
	vxz.dx = T;
	vxz.dy = F;
	vxz.dz = T;
	vxz.rx = F;
	vxz.ry = T;
	vxz.rz = F;
	vxz.k = F;

	bool byz = false;
	struct isFreeCnstr vyz;
	vyz.dx = F;
	vyz.dy = T;
	vyz.dz = T;
	vyz.rx = T;
	vyz.ry = F;
	vyz.rz = F;
	vyz.k = F;

	bool bxyz = false;
	struct isFreeCnstr vxyz;
	vxyz.dx = T;
	vxyz.dy = T;
	vxyz.dz = T;
	vxyz.rx = T;
	vxyz.ry = T;
	vxyz.rz = T;
	vxyz.k = T;

	bool bcala = false;
	struct isFreeCnstr vcala;
	vcala.dx = F;
	vcala.dy = F;
	vcala.dz = F;
	vcala.rx = F;
	vcala.ry = F;
	vcala.rz = F;
	vcala.k = F;

	bool bscale = false;
	struct isFreeCnstr vscale;
	vscale.dx = T;
	vscale.dy = T;
	vscale.dz = T;
	vscale.rx = T;
	vscale.ry = T;
	vscale.rz = T;
	vscale.k = F;

	bool brotation = false;
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
	int numberOfPoints = lsds.getPoints().numObjects();


	//n accepte pas la division par un entier???
	TReal div = numberOfPoints;
	TReal factor = 1 / div;


	//constraints on points
	while (iter != iterEnd)
	{
		TSpatialStatus::ESpatialStatus status = TSpatialStatus::kUnknown;

		status = iter->getSpatialStatus();
		switch (status)
		{
		case TSpatialStatus::kVx:
			if (bx == false)
				bx = true;
			break;
		case TSpatialStatus::kVy:
			if (by == false)
				by = true;
			break;

		case TSpatialStatus::kVz:
			if (bz == false)
				bz = true;
			break;

		case TSpatialStatus::kVxy:
			if (bxy == false)
				bxy = true;
			break;

		case TSpatialStatus::kVxz:
			if (bxz == false)
				bxz = true;
			break;

		case TSpatialStatus::kVyz:
			if (byz == false)
				byz = true;
			break;

		case TSpatialStatus::kVxyz:
			if (bxyz == false)
				bxyz = true;
			break;

		case TSpatialStatus::kCala:
			if (bcala == false)
				bcala = true;
			break;

		default:
			break;
		}

		//build gravity center
		fXcg = fXcg + (iter->getEstimatedValue().getX() * factor);
		fYcg = fYcg + (iter->getEstimatedValue().getY() * factor);
		fZcg = fZcg + (iter->getEstimatedValue().getZ() * factor);

		iter++;
	}

	setEstimatedGravityCenterCoord(fXcg, fYcg, fZcg);

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
		lsds.getMeasurementDimension(TMeasurementsGlobal::kECTH) > 0 )
		if (bscale == false)
			bscale = true;


	//constraints on orientation
	if (lsds.getMeasurementDimension(TMeasurementsGlobal::kORIE) > 0)
		if (brotation == false)
			brotation = true;


	//build constraint vector
	setCnstrIdentifier(T, T, T, T, T, T, T);

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

	if (bscale == true)
		addCnstr(vscale);

	if (brotation == true)
		addCnstr(vrotation);


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
TReal	TLibrCnstrGenerator::getRxCalcValue(const TAdjustablePoint& ptIt)
{
	TReal a = -LITERAL(1.0) *
		(
		ptIt.getEstimatedValue().getZ().getMetresValue()
		- fZcgEst.getMetresValue()
		)
		*
		(
		ptIt.getEstimatedValue().getY().getMetresValue()
		- fYcgEst.getMetresValue()
		);
	TReal b = (
		ptIt.getEstimatedValue().getY().getMetresValue()
		- fYcgEst.getMetresValue()
		)
		*
		(
		ptIt.getEstimatedValue().getZ().getMetresValue()
		- fZcgEst.getMetresValue()
		);

	TReal calcRx = a + b;
	return calcRx;
}


TReal	TLibrCnstrGenerator::getRyCalcValue(const TAdjustablePoint& ptIt)
{
	TReal a = -LITERAL(1.0) *
		(
		ptIt.getEstimatedValue().getX().getMetresValue()
		- fXcgEst.getMetresValue()
		)
		*
		(
		ptIt.getEstimatedValue().getZ().getMetresValue()
		- fZcgEst.getMetresValue()
		);
	TReal b = (
		ptIt.getEstimatedValue().getZ().getMetresValue()
		- fZcgEst.getMetresValue()
		)
		*
		(
		ptIt.getEstimatedValue().getX().getMetresValue()
		- fXcgEst.getMetresValue()
		);

	TReal calcRy = a + b;
	return calcRy;
}


TReal	TLibrCnstrGenerator::getRzCalcValue(const TAdjustablePoint& ptIt)
{
	TReal a = -LITERAL(1.0) *
		(
		ptIt.getEstimatedValue().getY().getMetresValue()
		- fYcgEst.getMetresValue()
		)
		*
		(
		ptIt.getEstimatedValue().getX().getMetresValue()
		- fXcgEst.getMetresValue()
		);
	TReal b = (
		ptIt.getEstimatedValue().getX().getMetresValue()
		- fXcgEst.getMetresValue()
		)
		*
		(
		ptIt.getEstimatedValue().getY().getMetresValue()
		- fYcgEst.getMetresValue()
		);

	TReal calcRz = a + b;
	return calcRz;
}


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