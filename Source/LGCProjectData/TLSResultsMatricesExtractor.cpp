////////////////////////////////////////////////////////////////////
// TLGCCalcParams.cpp : implementation file
// Class responsible for extracting the results from the matrices 
// and updating the appropriate member data of the LS Calc objects
// Specific to a least squares calculation
//
// Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include "TLSResultsMatrices.h"
#include "LSCalcDataSet.h"
#include "TMatrix.h"
#include "TLSResultsMatricesExtractor.h"
#include "TLGCCalcParams.h"
#include "TNotInLepGridException.h"

//////////////////////////////////////////////////////////////////////////////
// constructor setting the links to the results matrices and the calc data set
//////////////////////////////////////////////////////////////////////////////
TLSResultsMatricesExtractor::TLSResultsMatricesExtractor(LSCalcDataSet* lscDS){

	fDataSet = lscDS;
	fLastIteration = false;
	fError = "";
	fS0 = LITERAL(1.0);
	fRefSurface = TRefSystemFactory::kNotInGraph;
	fIsSimulation = false;
}

/////////////
// destructor
/////////////
TLSResultsMatricesExtractor::~TLSResultsMatricesExtractor(){

}

//////////////////////////////////////////////////////////////////////////////
// Returns the boolean that indicates if a new iteration is necessary or not
//////////////////////////////////////////////////////////////////////////////
bool	TLSResultsMatricesExtractor::lastIteration() const {

	return fLastIteration;
}

//////////////////////////////////////////////////////////////
// initiates the simulation case
//////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::simulation() {

	fIsSimulation = true;
	return;
}

bool TLSResultsMatricesExtractor::extractAllPointsFixedUnknowns(const TLSResultsMatrices& , TLGCCalcParams& )
{
	LSHorAngIter horAnglIt = fDataSet->beginLSHorAng();
	LSHorAngIter endHorAnglIt = fDataSet->endLSHorAng();

	HorAngObsConstIter horAnglObs;

	while (horAnglIt != endHorAnglIt)
	{
		horAnglObs = horAnglIt->getHorAngleObs();
		TReal bearing = atan2q(horAnglIt->getTgEstCoordinate().getX().getMetresValue() - horAnglIt->getStEstCoordinate().getX().getMetresValue(),
							   horAnglIt->getTgEstCoordinate().getY().getMetresValue() - horAnglIt->getStEstCoordinate().getY().getMetresValue());
		
		TAngle v0(bearing - horAnglObs->getHorAngleValue().getRadiansValue());
		horAnglIt->setHasOwnV0();
		horAnglIt->setAllPointsFixedV0(v0);

		horAnglIt++;
	}

	LSZenDistIter zenDistIt = fDataSet->beginLSZenDist();
	LSZenDistIter endZenDistIt = fDataSet->endLSZenDist();

	ZenDistObsConstIter zenDistObs;

	while (zenDistIt != endZenDistIt)
	{
		zenDistObs = zenDistIt->getZenDistObs();
		if (zenDistIt->getHInstrStatus() == TALSCalcParameter::kVariable)
		{
			TReal distance = sqrtq((zenDistIt->getTgEstCoordinate().getX().getMetresValue() - zenDistIt->getStEstCoordinate().getX().getMetresValue()) *
				(zenDistIt->getTgEstCoordinate().getX().getMetresValue() - zenDistIt->getStEstCoordinate().getX().getMetresValue()) +
				(zenDistIt->getTgEstCoordinate().getY().getMetresValue() - zenDistIt->getStEstCoordinate().getY().getMetresValue()) *
				(zenDistIt->getTgEstCoordinate().getY().getMetresValue() - zenDistIt->getStEstCoordinate().getY().getMetresValue()));
			TLength ih(zenDistObs->getPrismHeight().getMetresValue()
				+ zenDistIt->getTgEstCoordinate().getZ().getMetresValue() - zenDistIt->getStEstCoordinate().getZ().getMetresValue()
				- distance / zenDistObs->getZenAngleValue().tangent());
			zenDistIt->setAllPointsFixedInstrumentHeight(ih);
			zenDistIt->setHasOwnInstrumentHeight();
		}

		zenDistIt++;
	}

	LSSpaDistIter spaDistIt = fDataSet->beginLSSpaDist();
	LSSpaDistIter endSpaDistIt = fDataSet->endLSSpaDist();

	while (spaDistIt != endSpaDistIt)
	{
		if (spaDistIt->getHInstStatus() == TALSCalcParameter::kVariable)
		{
			int sign = 0;
			if (spaDistIt->getStEstCoordinate().getZ().getMetresValue() + spaDistIt->getHInstrument()->getEstimatedValue().getMetresValue() >
				spaDistIt->getTgEstCoordinate().getZ().getMetresValue() + spaDistIt->getHPrism().getMetresValue())
			{
				sign = 1;
			}
			else
			{
				sign = -1;
			}

			LengthValue dx = spaDistIt->getTgEstCoordinate().getX().getMetresValue() - spaDistIt->getStEstCoordinate().getX().getMetresValue();
			LengthValue dy = spaDistIt->getTgEstCoordinate().getY().getMetresValue() - spaDistIt->getStEstCoordinate().getY().getMetresValue();
			TReal distance = dx * dx + dy * dy;
			TLength ih(spaDistIt->getHPrism().getMetresValue()
				+ spaDistIt->getTgEstCoordinate().getZ().getMetresValue() - spaDistIt->getStEstCoordinate().getZ().getMetresValue()
				+ sign * sqrtq(spaDistIt->getObsDist().getMetresValue() * spaDistIt->getObsDist().getMetresValue()
				- distance));
			spaDistIt->setAllPointsFixedInstrumentHeight(ih);
			spaDistIt->setHasOwnInstrumentHeight();
		}

		spaDistIt++;
	}

	LSOffsetToTheoPlaneIter offTheoPlaneIt = fDataSet->beginLSOffsetToTheoPlane();
	LSOffsetToTheoPlaneIter endOffTheoPlaneIt = fDataSet->endLSOffsetToTheoPlane();

	while (offTheoPlaneIt != endOffTheoPlaneIt)
	{
		LengthValue dx = offTheoPlaneIt->getFirstTgEstCoordinate().getX().getMetresValue() - offTheoPlaneIt->getStEstCoordinate().getX().getMetresValue();
		LengthValue dy = offTheoPlaneIt->getFirstTgEstCoordinate().getY().getMetresValue() - offTheoPlaneIt->getStEstCoordinate().getY().getMetresValue();
		TReal distance = sqrtq(dx * dx + dy * dy);
		TReal angle = asinq(offTheoPlaneIt->getObsDist().getMetresValue() / distance);
		
		TReal bearing = atan2q(offTheoPlaneIt->getFirstTgEstCoordinate().getX().getMetresValue() - offTheoPlaneIt->getStEstCoordinate().getX().getMetresValue(),
							   offTheoPlaneIt->getFirstTgEstCoordinate().getY().getMetresValue() - offTheoPlaneIt->getStEstCoordinate().getY().getMetresValue());
		
		TAngle v0(bearing + angle - offTheoPlaneIt->getTgAngle().getRadiansValue());
		offTheoPlaneIt->setHasOwnV0();
		offTheoPlaneIt->setAllPointsFixedV0(v0);

		offTheoPlaneIt++;
	}

	return true;
}

/////////////////////////////////////////////////////////////////
// extracts the results and tells if one more iteration is needed
/////////////////////////////////////////////////////////////////
bool TLSResultsMatricesExtractor::extractResults(const TLSResultsMatrices& rm, TLGCCalcParams& calcParams) {
	bool successfullExtraction = true;

	bool pv = extractPosVecPara(rm,calcParams.getConvCriteria());
	bool or = extractOrientPara(rm,calcParams.getConvCriteria());
	bool le = extractLengthPara(rm,calcParams.getConvCriteria());

	if ( (pv && or && le) || calcParams.areAllPointsFixed() )
		fLastIteration = true;
	
	if ( fError!="" )
		successfullExtraction = false;

	return successfullExtraction;
}


bool TLSResultsMatricesExtractor::extractResiduals(const TLSResultsMatrices& rm, TLGCCalcParams& calcParams) {

	bool successfullExtraction = true;

	if (rm.S0APosterioriVariances()) 
		fS0 = sqrtq(rm.getSigmaZero2());		

	extractHorAngleObs(rm,calcParams);
	extractZenDistObs(rm,calcParams);
	extractSpaDistObs(rm,calcParams);
	extractHorDistObs(rm,calcParams);
	// cover DVER and DLEV
	extractVertDistObs(rm,calcParams, 0);
	extractVertDistObs(rm,calcParams, 1);

	extractOffsetToVerLineObs(rm,calcParams);
	extractOffsetToSpaLineObs(rm,calcParams);
	extractOffsetToVerPlaneObs(rm,calcParams);
	extractOffsetToTheoPlaneObs(rm,calcParams);

	extractGyroOrieObs(rm,calcParams);

	extractRadOffCnstr(rm,calcParams);

	extractS0APosteriori(rm);

	if ( fError!="" )
		successfullExtraction = false;

	return successfullExtraction;
}


////////////////////////////////////////////////////////////////////////////////
// compute the error detection parameters and extract the corresponding results
////////////////////////////////////////////////////////////////////////////////
void	TLSResultsMatricesExtractor::computeAndExtractReliability(const TVector & obsVar, TLGCCalcParams& calcParams) 
{
	computeErrorHorAngle(obsVar,calcParams);
	computeErrorZenDist(obsVar,calcParams);
	computeErrorSpaDist(obsVar,calcParams);
	computeErrorHorDist(obsVar,calcParams);
	computeErrorVertDist(obsVar,calcParams, 0); // Cover DVER
	computeErrorVertDist(obsVar,calcParams, 1); // Cover DLEV
	computeErrorOffsetToVerLine(obsVar,calcParams);
	computeErrorOffsetToSpaLine(obsVar,calcParams);
	computeErrorOffsetToVerPlane(obsVar,calcParams);
	computeErrorOffsetToTheoPlane(obsVar,calcParams);
	computeErrorGyroOrie(obsVar, calcParams);
	computeErrorRadOff(obsVar,calcParams);	

	return;
}

//////////////////////////////////////////////////////////////////////////////////
// extract var. and covar. params
//////////////////////////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractVarCovarParams(const TLSResultsMatrices& rm)
{
	extractPosVecVar(rm);
	extractOrientVar(rm);
	extractLengthVar(rm);
	return;
}

//////////////////////////////////////////////////////////////////////////////////
// extract var. and covar. position vector params
//////////////////////////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractPosVecVar(const TLSResultsMatrices& rm)
{
    LSPosVecIter	iterB = fDataSet->beginPV();
    LSPosVecIter	iterE = fDataSet->endPV();

    while (iterB!=iterE)
    {
        TFreeVector estSigma(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian);
        TFreeVector estCovariance(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian);
        MatrixIndex xi(0), yi(0), zi(0);

        if ( iterB->getXStatus() == TALSCalcParameter::kVariable )
        {
            xi = iterB->getXIndex();
            if ( xi <= rm.getUnkCovarMtrx()->rows() )
            {
                TLength xsigma(sqrtq(rm.getUnknownsCovarMtrxElmt(xi,xi)));
                estSigma.setX(xsigma);
            }
            else
            {
                fError += ("Position: " + iterB->getName());
                fError += " X unknown index exceeds matrix dimensions\n" ;
                return;
            }
        }
        if ( iterB->getYStatus() == TALSCalcParameter::kVariable )
        {
            yi = iterB->getYIndex();
            if (yi <= rm.getUnkCovarMtrx()->rows())
            {
                TLength ysigma(sqrtq(rm.getUnknownsCovarMtrxElmt(yi,yi)));
                estSigma.setY(ysigma);
            }
            else
            {
                fError += ("Position: " + iterB->getName());
                fError += " Y unknown index exceeds matrix dimensions\n";
                return;
            }
        }
        if (iterB->getZStatus() == TALSCalcParameter::kVariable ) 
        {
            zi = iterB->getZIndex();
            if (zi <= rm.getUnkCovarMtrx()->rows())
            {
                TLength zsigma(sqrtq(rm.getUnknownsCovarMtrxElmt(zi,zi)));
                estSigma.setZ(zsigma);
            }
            else
            {
                fError += ("Position: " + iterB->getName());
                fError += "Z unknown index exceeds matrix dimensions\n";
                return;
            }
        }

        if ( (iterB->getXStatus() == TALSCalcParameter::kVariable) && (iterB->getYStatus() == TALSCalcParameter::kVariable) )
        {
            if ( (xi <= rm.getUnkCovarMtrx()->rows()) && (yi <= rm.getUnkCovarMtrx()->rows()) )
            {
                TLength Cxy;
                Cxy.setMetresValue( rm.getUnknownsCovarMtrxElmt(xi,yi) * 1000 );
                estCovariance.setX(Cxy);
            }
            else {
                fError += ("Position: " + iterB->getName());
                fError += " X unknown index exceeds matrix dimensions\n" ;
                return;
            }
        }


        if ( (iterB->getYStatus() == TALSCalcParameter::kVariable) && (iterB->getZStatus() == TALSCalcParameter::kVariable) )
        {
            if ( (yi <= rm.getUnkCovarMtrx()->rows()) && (zi <= rm.getUnkCovarMtrx()->rows()) )
            {
                TLength Cyz;
                Cyz.setMetresValue( rm.getUnknownsCovarMtrxElmt(yi,zi) * 1000);
                estCovariance.setY(Cyz);
            }
            else {
                fError += ("Position: " + iterB->getName());
                fError += " Y unknown index exceeds matrix dimensions\n";
                return;
            }
        }

        if ( (iterB->getXStatus() == TALSCalcParameter::kVariable) && (iterB->getZStatus() == TALSCalcParameter::kVariable) ) 
        {
            if ( (xi <= rm.getUnkCovarMtrx()->rows()) && (zi <= rm.getUnkCovarMtrx()->rows()) )
            {
                TLength Cxz;
                Cxz.setMetresValue( rm.getUnknownsCovarMtrxElmt(xi,zi) * 1000 );
                estCovariance.setZ(Cxz);
            }
            else {
                fError += ("Position: " + iterB->getName());
                fError += "Z unknown index exceeds matrix dimensions\n";
                return;
            }
        }

        iterB->setEstimatedPrecision(estSigma);
        iterB->setEstimatedCovariance(estCovariance);

        iterB++;
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////
// extract var. and covar. length params
//////////////////////////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractLengthVar(const TLSResultsMatrices& rm)
{
	LSLengthIter  iterB = fDataSet->beginLength();
	LSLengthIter  iterE = fDataSet->endLength();

	while ( iterB!=iterE ) {

		MatrixIndex	li;
		TLength lsigma(LITERAL(0.0));

		if( iterB->getStatus() == TALSCalcParameter::kVariable ) {			
			li = iterB->getIndex();
			if ( li <= rm.getUnkCovarMtrx()->rows() ) 
			{
				lsigma.setMetresValue(sqrtq(rm.getUnknownsCovarMtrxElmt(li,li)));
			}
			else {
				fError += ("Length: " + iterB->getName());
				fError += " unknown index exceeds matrix dimensions\n";
				return;
			}
		}
		iterB->setEstimatedPrecision(lsigma);

		iterB++;
	}
	return;
}

//////////////////////////////////////////////////////////////////////////////////
// extract var. and covar. orientation params
//////////////////////////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractOrientVar(const TLSResultsMatrices& rm)
{
	LSOrientIter  iterB = fDataSet->beginOrient();
	LSOrientIter  iterE = fDataSet->endOrient();

	while ( iterB!=iterE ) {
		
		MatrixIndex oi, pi, ki;
		TAngle osigma(LITERAL(0.0)), psigma(LITERAL(0.0)), ksigma(LITERAL(0.0));

		if ( iterB->getOmegaStatus() == TALSCalcParameter::kVariable ) 
		{	
			oi = iterB->getOmegaIndex();
			if ( oi <= rm.getUnkCovarMtrx()->rows() ) 
			{
				osigma.setRadiansValue(sqrtq(rm.getUnknownsCovarMtrxElmt(oi,oi)));
			}
			else 
			{
				fError = ("Orientation: " + iterB->getName());
				fError += " Omega unknown index exceeds matrix dimensions\n";
				return;
			}
		}

		if ( iterB->getPhiStatus() == TALSCalcParameter::kVariable ) 
		{
			pi = iterB->getPhiIndex();
			if ( pi <= rm.getUnkCovarMtrx()->rows() ) {
				psigma.setRadiansValue(sqrtq(rm.getUnknownsCovarMtrxElmt(pi,pi)));
			}
			else 
			{
				fError += ("Orientation: " + iterB->getName());
				fError += " Phi unkown index exceeds matrix dimensions\n";
				return;
			}
		}

		if ( iterB->getKappaStatus() == TALSCalcParameter::kVariable ) 
		{	
			ki = iterB->getKappaIndex();
			if ( ki <= rm.getUnkCovarMtrx()->rows() ) 
			{
				ksigma.setRadiansValue(sqrtq(rm.getUnknownsCovarMtrxElmt(ki,ki)));
			}
			else 
			{
				fError += ("Orientation: " + iterB->getName());
				fError += " Kappa unknown index exceeds matrix dimensions\n";
				return;
			}
		}
		iterB->setEstimatedPrecision(osigma,psigma,ksigma);
		
		iterB++;
	}

	return;
}

//////////////////////////////////////////////////////////////////
// extracts the results related to the position vectors parameters
//////////////////////////////////////////////////////////////////
bool TLSResultsMatricesExtractor::extractPosVecPara(const TLSResultsMatrices& rm, TReal convCrit)  {

	bool critNotExceeded = true;
	
	LSPosVecIter	iterB = fDataSet->beginPV();
	LSPosVecIter	iterE = fDataSet->endPV();

	int numberOfPoints = fDataSet->numPosVectorParam();
	//n accepte pas la division par un entier???
	TReal div = numberOfPoints;
	TReal factor = LITERAL(1.0) / div;

	TLength zero (LITERAL(0.0));
	TLength xcg = zero;
	TLength ycg = zero;
	TLength zcg = zero;

	// add the iteration increment to each of the estimated point coordinates in turn
	while ( iterB!=iterE )
	{
		// etsablish a vector for the correction
		TFreeVector correction(LITERAL(0.0),LITERAL(0.0),LITERAL(0.0),TCoordSysFactory::k3DCartesian);
		MatrixIndex xi, yi, zi;

		// if the X-coordinate of the current position vector is variable
		// extract the estimated correction from the results matrices
		// and add it to the correction vector. Note if this parameter
		// has converged.
		if ( iterB->getXStatus() == TALSCalcParameter::kVariable ) {
			xi = iterB->getXIndex();
			if ( xi <= rm.getSolutionVctr()->size() )
			{
				TLength xcor(rm.getSolutionVctrElmt(xi));
				correction.setX(xcor);
				if ( fabsq(xcor.getMetresValue()) > convCrit )
				{
					critNotExceeded = false;
				}
			}
			else {
				fError += ("Position: " + iterB->getName());
				fError += " X unknown index exceeds matrix dimensions\n" ;
				return critNotExceeded;
			}
		}

		// if the Y-coordinate of the current position vector is variable
		// extract the estimated correction from the results matrices
		// and add it to the correction vector. Note if this parameter
		// has converged.
		if ( iterB->getYStatus() == TALSCalcParameter::kVariable ) {
			yi = iterB->getYIndex();
			if ( yi <= rm.getSolutionVctr()->size() )
			{
				TLength ycor(rm.getSolutionVctrElmt(yi));
				correction.setY(ycor);
				if ( fabsq(ycor.getMetresValue()) > convCrit )
				{
					critNotExceeded = false;
				}
			}
			else {
				fError += ("Position: " + iterB->getName());
				fError += " Y unknown index exceeds matrix dimensions\n";
				return critNotExceeded;
			}
		}

		// if the X-coordinate of the current position vector is variable
		// extract the estimated correction from the results matrices
		// and add it to the correction vector. Note if this parameter
		// has converged.
		if (iterB->getZStatus() == TALSCalcParameter::kVariable ) {
			zi = iterB->getZIndex();
			if ( zi <= rm.getSolutionVctr()->size() )
			{
				TLength zcor(rm.getSolutionVctrElmt(zi));
				correction.setZ(zcor);
				if ( fabsq(zcor.getMetresValue()) > convCrit )
				{
					critNotExceeded = false;
				}
			}
			else {
				fError += ("Position: " + iterB->getName());
				fError += "Z unknown index exceeds matrix dimensions\n";
				return critNotExceeded;
			}
		}

		// set the correction to the estimated coordinates
		iterB->setCorrection(correction);

		// update the estimated centre of Gravity for the data points
		xcg = xcg + (iterB->getEstimatedValue().getX() * factor);
		ycg = ycg + (iterB->getEstimatedValue().getY() * factor);
		zcg = zcg + (iterB->getEstimatedValue().getZ() * factor);
	
		// In case of 2D+H coordinates with the height value fixed
		// the calculation is still done with XYZ coordinates.
		// If the XY coordinates change and the Z coordinate is fixed
		// then the height will have changed. So take the new XY coordinates
		// with the original height, and recalculate the Z coordinate
		if (fRefSurface != TRefSystemFactory::kNotInGraph)
		{
			try {
				if ( iterB->getZStatus() == TALSCalcParameter::kFixed )
				{
					TPositionVector posVec = iterB->getProvisionalValue();
					TAReferenceFrame* rf(0); // TODO: What is a reasonable default system?
					if(fRefSurface == TRefSystemFactory::kCernXYHg85Machine)
						rf = TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHg85Machine);
					if(fRefSurface == TRefSystemFactory::kCernXYHg00Machine)
						rf = TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHg00Machine);
					if(fRefSurface == TRefSystemFactory::kCERNXYHsSphereSPS)
						rf = TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCERNXYHsSphereSPS);
			
					TSpatialPosition spaPos(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
					spaPos.setCoordinates(posVec);
					spaPos.transform(rf);
					posVec = spaPos.getCoordinates(TCoordSysFactory::k2DPlusH);
					posVec.setX(iterB->getEstimatedValue().getX());
					posVec.setY(iterB->getEstimatedValue().getY());
					spaPos.setCoordinates(posVec);
					spaPos.transform(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
					iterB->setEstimatedValueVector(spaPos.getCoordinates(TCoordSysFactory::k3DCartesian));
				}
			} catch (TNotInLepGridException &e) {
				char posstr[512];
				_snprintf(posstr, 512, " (%.3g,%.3g,%.3g) ", 
					iterB->getEstimatedValue().getX().getMetresValue(),
					iterB->getEstimatedValue().getY().getMetresValue(),
					iterB->getEstimatedValue().getZ().getMetresValue());

				this->fError += " Position " + iterB->getName() + posstr +  e.what();
				return false;
			}
		}
		

		iterB++;
	}

	if(fDataSet->isLibrOptionUsed())
	{
		TLSConstraintIdentifier cnstr = fDataSet->getFreeConstraints();
		cnstr.setEstimatedGravityCenterCoord(xcg, ycg, zcg);
		fDataSet->setFreeConstraints(cnstr);
	}

	return critNotExceeded;
}

//////////////////////////////////////////////////////////////////
// extracts the results related to the orientations parameters
//////////////////////////////////////////////////////////////////
bool TLSResultsMatricesExtractor::extractOrientPara(const TLSResultsMatrices& rm, TReal convCrit) {

	bool critNotExceeded = true;

	LSOrientIter  iterB = fDataSet->beginOrient();
	LSOrientIter  iterE = fDataSet->endOrient();

	while ( iterB!=iterE ) {
		
		MatrixIndex oi, pi, ki;
		TAngle ocor(LITERAL(0.0)), pcor(LITERAL(0.0)), kcor(LITERAL(0.0));//, osigma(LITERAL(0.0)), psigma(LITERAL(0.0)), ksigma(LITERAL(0.0));

		if ( iterB->getOmegaStatus() == TALSCalcParameter::kVariable ) {
			
			oi = iterB->getOmegaIndex();
			if ( oi <= rm.getSolutionVctr()->size() ) {
				ocor.setRadiansValue(rm.getSolutionVctrElmt(oi));
				if ( fabsq(ocor.getRadiansValue()) > convCrit )
					critNotExceeded = false;
			}
			else {
				fError = ("Orientation: " + iterB->getName());
				fError += " Omega unknown index exceeds matrix dimensions\n";
				return critNotExceeded;
			}
		}

		if ( iterB->getPhiStatus() == TALSCalcParameter::kVariable ) {

			pi = iterB->getPhiIndex();
			if ( pi <= rm.getSolutionVctr()->size() ) {
				pcor.setRadiansValue(rm.getSolutionVctrElmt(pi));
				if ( fabsq(pcor.getRadiansValue()) > convCrit )
					critNotExceeded = false;
			}
			else {
				fError += ("Orientation: " + iterB->getName());
				fError += " Phi unkown index exceeds matrix dimensions\n";
				return critNotExceeded;
			}
		}

		if ( iterB->getKappaStatus() == TALSCalcParameter::kVariable ) {
			
			ki = iterB->getKappaIndex();
			if ( ki <= rm.getSolutionVctr()->size() ) {
				kcor.setRadiansValue(rm.getSolutionVctrElmt(ki));
				if ( fabsq(kcor.getRadiansValue()) > convCrit )
					critNotExceeded = false;
			}
			else {
				fError += ("Orientation: " + iterB->getName());
				fError += " Kappa unknown index exceeds matrix dimensions\n";
				return critNotExceeded;
			}
		}

		iterB->setCorrection(ocor,pcor,kcor);
		
		iterB++;
	}

	return critNotExceeded;
}

//////////////////////////////////////////////////////////////////
// extracts the results related to the lengths parameters
//////////////////////////////////////////////////////////////////
bool TLSResultsMatricesExtractor::extractLengthPara(const TLSResultsMatrices& rm, TReal convCrit) {

	bool critNotExceeded = true;

	LSLengthIter  iterB = fDataSet->beginLength();
	LSLengthIter  iterE = fDataSet->endLength();

	while ( iterB!=iterE ) {

		MatrixIndex	li;
		TLength lcor(LITERAL(0.0));//, lsigma(LITERAL(0.0));

		if( iterB->getStatus() == TALSCalcParameter::kVariable ) {			
			li = iterB->getIndex();
			if ( li <= rm.getSolutionVctr()->size() ) {
				lcor.setMetresValue(rm.getSolutionVctrElmt(li));
				if ( lcor.getMetresValue() > convCrit )
					critNotExceeded = false;
			}
			else {
				fError += ("Length: " + iterB->getName());
				fError += " unknown index exceeds matrix dimensions\n";
				return critNotExceeded;
			}
		}

		iterB->setCorrection(lcor);

		iterB++;
	}
	return critNotExceeded;
}

///////////////////////////////////////////////////////////////////////
// extracts the results related to the horizontal angles observations
///////////////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractHorAngleObs(const TLSResultsMatrices& rm, TLGCCalcParams& ) {
	
	LSHorAngIter iterB = fDataSet->beginLSHorAng();
	LSHorAngIter iterE = fDataSet->endLSHorAng();

	while ( iterB!=iterE ) {
		
		MatrixIndex i = iterB->getObsIndex();

		if ( i <= rm.getResidualsVctr()->size() ) {
			TAngle res(rm.getResidualsVctrElmt(i));
			iterB->setResidual(res);
		}

		else {
			fError += "Horizontal Angle observation: observation index exceeds matrix dimensions\n";
			return;
		}

		iterB++;
	}
}

///////////////////////////////////////////////////////////////////////
// extracts the results related to the zenithal angles observations
///////////////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractZenDistObs(const TLSResultsMatrices& rm,  TLGCCalcParams& ) {
	
	LSZenDistIter iterB = fDataSet->beginLSZenDist();
	LSZenDistIter iterE = fDataSet->endLSZenDist();

	while ( iterB!=iterE ) {
		
		MatrixIndex i = iterB->getObsIndex();

		if ( i <= rm.getResidualsVctr()->size() ) {
			TAngle res(rm.getResidualsVctrElmt(i));
			iterB->setResidual(res);
		}

		else {
			fError += "Zenithal Distance observation: observation index exceeds matrix dimensions\n";
			return;
		}

		iterB++;
	}
}

///////////////////////////////////////////////////////////////////////
// extracts the results related to the spatial distances observations
///////////////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractSpaDistObs(const TLSResultsMatrices& rm,  TLGCCalcParams& ) {
	
	LSSpaDistIter iterB = fDataSet->beginLSSpaDist();
	LSSpaDistIter iterE = fDataSet->endLSSpaDist();

	while ( iterB!=iterE ) {
		
		MatrixIndex i = iterB->getObsIndex();

		if ( i <= rm.getResidualsVctr()->size() ) {
			TLength res(rm.getResidualsVctrElmt(i));
			iterB->setResidual(res);
		}

		else {
			fError += "Spatial Distance observation: observation index exceeds matrix dimensions\n";
			return;
		}

		iterB++;
	}
}

///////////////////////////////////////////////////////////////////////
// extracts the results related to the horizontal distances observations
///////////////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractHorDistObs(const TLSResultsMatrices& rm,  TLGCCalcParams& ) {
	
	LSHorDistIter iterB = fDataSet->beginLSHorDist();
	LSHorDistIter iterE = fDataSet->endLSHorDist();

	while ( iterB!=iterE ) {
		
		MatrixIndex i = iterB->getObsIndex();

		if ( i <= rm.getResidualsVctr()->size() ) {
			TLength res(rm.getResidualsVctrElmt(i));
			iterB->setResidual(res);
		}

		else {
			fError += "Horizontal Distance observation: observation index exceeds matrix dimensions\n";
			return;
		}

		iterB++;
	}
}


///////////////////////////////////////////////////////////////////////
// extracts the results related to the vertical distances observations
///////////////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractVertDistObs(const TLSResultsMatrices& rm,  TLGCCalcParams& calcParams, bool isDLEV) {
	
	(void)calcParams;
	LSVertDistIter iterB = fDataSet->beginLSVertDist(isDLEV);
	LSVertDistIter iterE = fDataSet->endLSVertDist(isDLEV);

	while ( iterB!=iterE ) {
		
		MatrixIndex i = iterB->getObsIndex();

		if ( i <= rm.getResidualsVctr()->size() ) {
			TLength res(rm.getResidualsVctrElmt(i));
			iterB->setResidual(res);
		}
		else {
			fError +="Vertical Distance observation: observation index exceeds matrix dimensions\n";
			string refName = iterB->getRefPointName();
			string tgName = iterB->getTgPointName();
			fError +="problème sur la mesure entre " + refName + " et " + tgName + '\n';
			return;
		}

		iterB++;
	}
}


void TLSResultsMatricesExtractor::extractOffsetToVerLineObs(const TLSResultsMatrices& rm,  TLGCCalcParams& )
{//extracts the results related to the oggset ECVE observations
	
	LSOffsetToVerLineIter iterB = fDataSet->beginLSOffsetToVerLine();
	LSOffsetToVerLineIter iterE = fDataSet->endLSOffsetToVerLine();
	
	while ( iterB!=iterE )
	{
		MatrixIndex i = iterB->getObsIndex();

		if ( i <= rm.getResidualsVctr()->size() )
		{
			TLength res(rm.getResidualsVctrElmt(i));
			iterB->setResidual(res);
		}
		else
		{
			fError +="Offset to vertical line observation: observation index exceeds matrix dimensions\n";
			return;
		}
		iterB++;
	}
}


void TLSResultsMatricesExtractor::extractOffsetToSpaLineObs(const TLSResultsMatrices& rm,  TLGCCalcParams& )
{//extracts the results related to the oggset ECVE observations
	
	LSOffsetToSpaLineIter iterB = fDataSet->beginLSOffsetToSpaLine();
	LSOffsetToSpaLineIter iterE = fDataSet->endLSOffsetToSpaLine();
	
	while ( iterB!=iterE )
	{
		MatrixIndex i = iterB->getObsIndex();

		if ( i <= rm.getResidualsVctr()->size() )
		{
			TLength res(rm.getResidualsVctrElmt(i));
			iterB->setResidual(res);
		}
		else
		{
			fError +="Offset to spatial line observation: observation index exceeds matrix dimensions\n";
			return;
		}
		iterB++;
	}
}



void TLSResultsMatricesExtractor::extractOffsetToVerPlaneObs(const TLSResultsMatrices& rm,  TLGCCalcParams& )
{//extracts the results related to the oggset ECVE observations
	
	LSOffsetToVerPlaneIter iterB = fDataSet->beginLSOffsetToVerPlane();
	LSOffsetToVerPlaneIter iterE = fDataSet->endLSOffsetToVerPlane();
	
	while ( iterB!=iterE )
	{
		MatrixIndex i = iterB->getObsIndex();

		if ( i <= rm.getResidualsVctr()->size() )
		{
			TLength res(rm.getResidualsVctrElmt(i));
			iterB->setResidual(res);
		}
		else
		{
			fError +="Offset to vertical plane observation: observation index exceeds matrix dimensions\n";
			return;
		}
		iterB++;
	}
}



void TLSResultsMatricesExtractor::extractOffsetToTheoPlaneObs(const TLSResultsMatrices& rm,  TLGCCalcParams& )
{//extracts the results related to the oggset ECVE observations
	
	LSOffsetToTheoPlaneIter iterB = fDataSet->beginLSOffsetToTheoPlane();
	LSOffsetToTheoPlaneIter iterE = fDataSet->endLSOffsetToTheoPlane();
	
	while ( iterB!=iterE )
	{
		MatrixIndex i = iterB->getObsIndex();

		if ( i <= rm.getResidualsVctr()->size() )
		{
			TLength res(rm.getResidualsVctrElmt(i));
			iterB->setResidual(res);
		}
		else
		{
			fError +="Offset to theodolite plane observation: observation index exceeds matrix dimensions\n";
			return;
		}
		iterB++;
	}
}


void TLSResultsMatricesExtractor::extractGyroOrieObs(const TLSResultsMatrices& rm,  TLGCCalcParams& )
{//extracts the results related to the oggset ECVE observations
	
	LSGyroOrieIter iterB = fDataSet->beginLSGyroOrie();
	LSGyroOrieIter iterE = fDataSet->endLSGyroOrie();
	
	while ( iterB!=iterE )
	{
		MatrixIndex i = iterB->getObsIndex();


		if ( i <= rm.getResidualsVctr()->size() )
		{
			TAngle res(rm.getResidualsVctrElmt(i));
			iterB->setResidual(res);
		}
		else
		{
			fError +="Offset to gyro. orientation observation: observation index exceeds matrix dimensions\n";
			return;
		}
		iterB++;
	}
}


void TLSResultsMatricesExtractor::extractRadOffCnstr(const TLSResultsMatrices& rm,  TLGCCalcParams& )
{//extracts the results related to the oggset ECVE observations
	
	LSRadOffCnstrIter iterB = fDataSet->beginLSRadOffCnstr();
	LSRadOffCnstrIter iterE = fDataSet->endLSRadOffCnstr();
	
	while ( iterB!=iterE )
	{
		MatrixIndex i = iterB->getObsIndex();

		if ( i <= rm.getResidualsVctr()->size() )
		{
			TLength res(rm.getResidualsVctrElmt(i));
			iterB->setResidual(res);
		}
		else
		{
			fError +="Radial offset constraint: observation index exceeds matrix dimensions\n";
			return;
		}
		iterB++;
	}
}

//Compute the fault for horizontal angle observations and updates the latter 
void TLSResultsMatricesExtractor::computeErrorHorAngle(const TVector & obsVar, TLGCCalcParams& calcParams) 
{

	LSHorAngIter iterB = fDataSet->beginLSHorAng();
	LSHorAngIter iterE = fDataSet->endLSHorAng();

	while (iterB!=iterE)
	{
		MatrixIndex i = iterB->getObsIndex();
		if (i <= obsVar.size())
		{
			if (isSimulation())
				iterB->wIsNotToCompute();

			iterB->setSigmaAPosteriori(sqrtq(obsVar(i)),calcParams.getAlpha(),calcParams.getBeta(),fS0);
		}
		else
			fError +="Horizontal Angles: observation index exceeds matrix dimensions for fault computation\n";

		iterB++;
	}

	return;
}

// Compute the fault for the zenith. distance observations and updates the latter 
void TLSResultsMatricesExtractor::computeErrorZenDist(const TVector & obsVar, TLGCCalcParams& calcParams)
{
	LSZenDistIter iterB = fDataSet->beginLSZenDist();
	LSZenDistIter iterE = fDataSet->endLSZenDist();

	while ( iterB!=iterE ) 
	{		
		MatrixIndex i = iterB->getObsIndex();
		if (i <= obsVar.size())
		{
			if (isSimulation())
				iterB->wIsNotToCompute();

			iterB->setSigmaAPosteriori(sqrtq(obsVar(i)),calcParams.getAlpha(),calcParams.getBeta(),fS0);
		}
		else
			fError +="Zenithal distances: observation index exceeds matrix dimensions for fault computation\n";

		iterB++;
	}
	
	return;
}

// Compute the fault for spatial distance observations and updates the latter 
void TLSResultsMatricesExtractor::computeErrorSpaDist(const TVector & obsVar, TLGCCalcParams& calcParams) 
{
	LSSpaDistIter iterB = fDataSet->beginLSSpaDist();
	LSSpaDistIter iterE = fDataSet->endLSSpaDist();

	while (iterB!=iterE)
	{
		MatrixIndex i = iterB->getObsIndex();
		if (i <= obsVar.size())
		{
			if (isSimulation())
				iterB->wIsNotToCompute();

			iterB->setSigmaAPosteriori(sqrtq(obsVar(i)),calcParams.getAlpha(),calcParams.getBeta(),fS0);
		}
		else
			fError +="Spatial distances: observation index exceeds matrix dimensions for fault computation\n";

		iterB++;
	}
	return;
}

// Compute the fault for horizontal distance observations and updates the latter 
void TLSResultsMatricesExtractor::computeErrorHorDist(const TVector & obsVar, TLGCCalcParams& calcParams)
{
	LSHorDistIter iterB = fDataSet->beginLSHorDist();
	LSHorDistIter iterE = fDataSet->endLSHorDist();

	while (iterB!=iterE)
	{
		MatrixIndex i = iterB->getObsIndex();
		if (i <= obsVar.size())
		{
			if (isSimulation())
				iterB->wIsNotToCompute();

			iterB->setSigmaAPosteriori(sqrtq(obsVar(i)),calcParams.getAlpha(),calcParams.getBeta(),fS0);
		}
		else
			fError +="Horizontal distances: observation index exceeds matrix dimensions for fault computation\n";

		iterB++;
	}
	return;

}

// Compute the fault for vertical distance observations and updates the latter 
void TLSResultsMatricesExtractor::computeErrorVertDist(const TVector & obsVar, TLGCCalcParams& calcParams, bool isDLEV)
{
	LSVertDistIter iterB = fDataSet->beginLSVertDist(isDLEV);
	LSVertDistIter iterE = fDataSet->endLSVertDist(isDLEV);

	while (iterB!=iterE)
	{
		MatrixIndex i = iterB->getObsIndex();
		if (i <= obsVar.size())
		{
			if (isSimulation())
				iterB->wIsNotToCompute();

			iterB->setSigmaAPosteriori(sqrtq(obsVar(i)),calcParams.getAlpha(),calcParams.getBeta(),fS0);
		}
		else
			fError +="Vertical distances: observation index exceeds matrix dimensions for fault computation\n";

		iterB++;
	}
	return;
}
	
// Compute the fault for offset to vertical line observations and updates the latter 
void TLSResultsMatricesExtractor::computeErrorOffsetToVerLine(const TVector & obsVar, TLGCCalcParams& calcParams)
{
	LSOffsetToVerLineIter iterB = fDataSet->beginLSOffsetToVerLine();
	LSOffsetToVerLineIter iterE = fDataSet->endLSOffsetToVerLine();
	
	while (iterB!=iterE)
	{
		MatrixIndex i = iterB->getObsIndex();
		if (i <= obsVar.size())
		{
			if (isSimulation())
				iterB->wIsNotToCompute();

			iterB->setSigmaAPosteriori(sqrtq(obsVar(i)),calcParams.getAlpha(),calcParams.getBeta(),fS0);
		}
		else
			fError +="Offsets to vertical line: observation index exceeds matrix dimensions for fault computation\n";

		iterB++;
	}
	return;
}

// Compute the fault for offset to spatial line observations and updates the latter 
void TLSResultsMatricesExtractor::computeErrorOffsetToSpaLine(const TVector & obsVar, TLGCCalcParams& calcParams)
{
	LSOffsetToSpaLineIter iterB = fDataSet->beginLSOffsetToSpaLine();
	LSOffsetToSpaLineIter iterE = fDataSet->endLSOffsetToSpaLine();

	while (iterB!=iterE)
	{
		MatrixIndex i = iterB->getObsIndex();
		if (i <= obsVar.size())
		{
			if (isSimulation())
				iterB->wIsNotToCompute();

			iterB->setSigmaAPosteriori(sqrtq(obsVar(i)),calcParams.getAlpha(),calcParams.getBeta(),fS0);
		}
		else
			fError +="Offsets to spatial line: observation index exceeds matrix dimensions for fault computation\n";

		iterB++;
	}
	return;
}

// Compute the fault for offset to vertical plane observations and updates the latter  
void TLSResultsMatricesExtractor::computeErrorOffsetToVerPlane(const TVector & obsVar, TLGCCalcParams& calcParams)
{
	LSOffsetToVerPlaneIter iterB = fDataSet->beginLSOffsetToVerPlane();
	LSOffsetToVerPlaneIter iterE = fDataSet->endLSOffsetToVerPlane();

	while (iterB!=iterE)
	{
		MatrixIndex i = iterB->getObsIndex();
		if (i <= obsVar.size())
		{
			if (isSimulation())
				iterB->wIsNotToCompute();

			iterB->setSigmaAPosteriori(sqrtq(obsVar(i)),calcParams.getAlpha(),calcParams.getBeta(),fS0);
		}
		else
			fError +="Offsets to vertical plane: observation index exceeds matrix dimensions for fault computation\n";

		iterB++;
	}
	return;
}

// Compute the fault for offset to theodolite plane observations and updates the latter 
void TLSResultsMatricesExtractor::computeErrorOffsetToTheoPlane(const TVector & obsVar, TLGCCalcParams& calcParams)
{
	LSOffsetToTheoPlaneIter iterB = fDataSet->beginLSOffsetToTheoPlane();
	LSOffsetToTheoPlaneIter iterE = fDataSet->endLSOffsetToTheoPlane();

	while (iterB!=iterE)
	{
		MatrixIndex i = iterB->getObsIndex();
		if (i <= obsVar.size())
		{
			if (isSimulation())
				iterB->wIsNotToCompute();

			iterB->setSigmaAPosteriori(sqrtq(obsVar(i)),calcParams.getAlpha(),calcParams.getBeta(),fS0);
		}
		else
			fError +="Offsets to theo plane: observation index exceeds matrix dimensions for fault computation\n";

		iterB++;
	}
	return;
}
	
// Compute the fault for gyro. orientation observations and updates the latter 
void TLSResultsMatricesExtractor::computeErrorGyroOrie(const TVector & obsVar, TLGCCalcParams& calcParams)
{
	LSGyroOrieIter iterB = fDataSet->beginLSGyroOrie();
	LSGyroOrieIter iterE = fDataSet->endLSGyroOrie();

	while (iterB!=iterE)
	{
		MatrixIndex i = iterB->getObsIndex();
		if (i <= obsVar.size())
		{
			if (isSimulation())
				iterB->wIsNotToCompute();

			iterB->setSigmaAPosteriori(sqrtq(obsVar(i)),calcParams.getAlpha(),calcParams.getBeta(),fS0);
		}
		else
			fError +="Orientations: observation index exceeds matrix dimensions for fault computation\n";

		iterB++;
	}
	return;
}
	
// Extract the results relative to the radial constraint and updates the latter 
void TLSResultsMatricesExtractor::computeErrorRadOff(const TVector & obsVar, TLGCCalcParams& calcParams)
{
	LSRadOffCnstrIter iterB = fDataSet->beginLSRadOffCnstr();
	LSRadOffCnstrIter iterE = fDataSet->endLSRadOffCnstr();

	while (iterB!=iterE)
	{
		MatrixIndex i = iterB->getObsIndex();
		if (i <= obsVar.size())
		{
		if (isSimulation())
			iterB->wIsNotToCompute();

		iterB->setSigmaAPosteriori(sqrtq(obsVar(i)),calcParams.getAlpha(),calcParams.getBeta(),fS0);
		}
		else
			fError +="Radial offsets: observation index exceeds matrix dimensions for fault computation\n";

		iterB++;
	}
	return;
}


void TLSResultsMatricesExtractor::extractS0APosteriori(const TLSResultsMatrices& rm)
{// Extract the square sigma zero from the results matrices in order to
	//update the sigma zero in LSCalcDataSet 

	if(rm.getSigmaZero2() >= LITERAL(0.0))
	{
		TReal	s0(sqrtq(rm.getSigmaZero2()));
		fDataSet->setS0APosteriori(s0);
	}
	else
	{
		fError = fError + "Erreur : (Sigma Zéro)^2 négatif.";
		fError = fError + '\n';
	}
	return;
}

// Extract results for relative error computation 
void	TLSResultsMatricesExtractor::extractRelError(const TLSResultsMatrices& rm, TLGCCalcParams& calcParams) {

	if (calcParams.getWarning() != "") {
		// if a problem occured before reading, it is passed to the ls calc working point pairs
		fDataSet->setRelErrorWarning(calcParams.getWarning());
	}
	else {

		if (rm.S0APosterioriVariances()) 
			fS0 = sqrtq(rm.getSigmaZero2());		

		PtNamePairIter iterb = calcParams.beginPtNamePairs();

		while (iterb != calcParams.endPtNamePairs()) {
			
			LSPosVecIter pt1 = fDataSet->getPV(iterb->getPt1Name().getName());
			LSPosVecIter pt2 = fDataSet->getPV(iterb->getPt2Name().getName());
			
			// if points are not defined -> warning...
			if (((pt1 != fDataSet->endPV()) && (pt2 != fDataSet->endPV()))){
				
				TLSCalcRelativeError	re(pt1,pt2);

				// matrix indices
				MatrixIndex	ind[6];
				for (int k=0;k<6;k++)
					ind[k] = -1;

				if (pt1->getXStatus() == TALSCalcParameter::kVariable)
					ind[0] = pt1->getXIndex(); 
				if (pt1->getYStatus() == TALSCalcParameter::kVariable)
					ind[1] = pt1->getYIndex(); 			
				if (pt1->getZStatus() == TALSCalcParameter::kVariable)
					ind[2] = pt1->getZIndex(); 			
				if (pt2->getXStatus() == TALSCalcParameter::kVariable)
					ind[3] = pt2->getXIndex(); 
				if (pt2->getYStatus() == TALSCalcParameter::kVariable)
					ind[4] = pt2->getYIndex(); 
				if (pt2->getZStatus() == TALSCalcParameter::kVariable)
					ind[5] = pt2->getZIndex(); 
				
				// sous-matrice cofacteur
				TMatrix Q12(6,6);
				int i,j;

				for (i=0;i<6;i++) {
					for (j=0;j<6;j++) {
						if ((ind[i] > -1) && (ind[j] > -1))
							Q12(i,j) = (*rm.getUnkCovarMtrx()).coeff(ind[i],ind[j]) / powq(fS0,2);
						else
							Q12(i,j) = LITERAL(0.0);
					}
				}

				// computation of horizontal & spatial distances, and vertical angle & orientation
				TReal ds, dh, G, V, x1, y1, z1, x2, y2, z2;
				x1 = pt1->getEstimatedValue().getX().getMetresValue();
				y1 = pt1->getEstimatedValue().getY().getMetresValue();
				z1 = pt1->getEstimatedValue().getZ().getMetresValue();
				x2 = pt2->getEstimatedValue().getX().getMetresValue();
				y2 = pt2->getEstimatedValue().getY().getMetresValue();
				z2 = pt2->getEstimatedValue().getZ().getMetresValue();
				// length unit: meter
				ds = sqrtq(powq(x2-x1,2)+powq(y2-y1,2)+powq(z2-z1,2));
				dh = sqrtq(powq(x2-x1,2)+powq(y2-y1,2));
				// angle unit: radian
				G = atan2q(x2-x1,y2-y1);
				V = acosq((z2-z1)/ds);

				// computation of sigmaL
				TReal sl(LITERAL(0.0));
				TReal co[6]; // error propagation
				co[0] = -(x2-x1)/ds;
				co[1] = -(y2-y1)/ds;
				co[2] = -(z2-z1)/ds;
				co[3] = -co[0];
				co[4] = -co[1];
				co[5] = -co[2];

				for (i=0;i<6;i++) {
					for (j=0;j<6;j++)
						sl += Q12(i,j) * co[i] * co[j];
				}

				TLength sigmaL(sqrtq(sl));
				re.setSigmaL(sigmaL);

				// computation of sigmaG 
				TReal sg(LITERAL(0.0)) ,sr(LITERAL(0.0));
				TAngle Gis(G); // normalisation of G
				co[0] = -(Gis.cosine())/dh;
				co[1] = (Gis.sine())/dh;
				co[2] = LITERAL(0.0);
				co[3] = -co[0];
				co[4] = -co[1];
				co[5] = -co[2];

				for (i=0;i<6;i++) {
					for (j=0;j<6;j++) 
						sg += Q12(i,j) * co[i] * co[j];
				}

				TAngle sigmaG(sqrtq(sg));
				re.setSigmaG(sigmaG);

				// computation of sigmaR
				sr = dh * sqrtq(sg);
				TLength sigmaR(sr);
				re.setSigmaR(sigmaR);

				// Computation of sigmaZ
				TReal sz(LITERAL(0.0));
				sz = Q12(2,2) + Q12(5,5) - Q12(2,5)*2;
				TLength sigmaZ(sqrtq(sz));
				re.setSigmaZ(sigmaZ);

				// Computation of sigmaV
				TReal sv(LITERAL(0.0));
				sv = (sqrtq(sz)*sinq(V))/ds;			
				TAngle sigmaV(sv);
				re.setSigmaV(sigmaV);

				fDataSet->push_backRelError(re);
			}
			else 
			{
				string warning(iterb->getPt1Name().getName() + " ou " + iterb->getPt2Name().getName() + " n'est pas dans la liste des points du calcul");
				fDataSet->setRelErrorWarning(calcParams.getWarning() + warning);
			}

			iterb++;
		}
	}

	return;
}





