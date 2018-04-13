#include "TLSResultsMatricesExtractor.h"
#include <TLGCData.h>
#include "TLSResultsMatrices.h"
#include "TLibrCnstrGenerator.h"
#include "TLSCalcRelativeError.h"



TLSResultsMatricesExtractor::TLSResultsMatricesExtractor(TLGCData* fData):
fDataSet(fData),
fLastIteration(false),
freeCnstr(nullptr)
{}

TLSResultsMatricesExtractor::~TLSResultsMatricesExtractor()
{//Destructor
}
/////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////
bool TLSResultsMatricesExtractor::extractResults(const TLSResultsMatrices& rm, TReal convCrit) {
	bool successfullExtraction = true;

	try{
		bool pt = extractPointParams(rm, convCrit);
		bool angl = extractAngleParams(rm, convCrit);
		bool pln = extractPlaneParams(rm, convCrit);
		bool len = extractLengthParams(rm, convCrit);
		bool trf = extractTransformationParams(rm, convCrit);
		bool ln = extractLineParams(rm, convCrit);

		if ( (pt && angl && pln && trf && len && ln) || fDataSet->getConfig().allfixed.isActive() )
			fLastIteration = true;

	}
	catch (std::exception const & excp) {
		fDataSet->getFileLogger() << TFileLogger::e_logType::LOG_ERROR <<  excp.what();
		successfullExtraction = false;
		return successfullExtraction;
	}
	return successfullExtraction;
}

bool TLSResultsMatricesExtractor::extractResults(const TLSResultsMatrices& rm, TReal convCrit, TLibrCnstrGenerator fCnstr) {
	bool successfullExtraction = true;
	freeCnstr = &fCnstr;

	try{
		bool pt = extractPointParams(rm, convCrit);
		bool angl = extractAngleParams(rm, convCrit);
		bool pln = extractPlaneParams(rm, convCrit);
		bool len = extractLengthParams(rm, convCrit);
		bool trf = extractTransformationParams(rm, convCrit);
		bool ln = extractLineParams(rm, convCrit);

		if ((pt && angl && pln && trf && len && ln) || fDataSet->getConfig().allfixed.isActive())
			fLastIteration = true;

	}
	catch (std::exception const & excp) {
		fDataSet->getFileLogger() << TFileLogger::e_logType::LOG_ERROR << excp.what();
		successfullExtraction = false;
		return successfullExtraction;
	}
	return successfullExtraction;
}

bool TLSResultsMatricesExtractor::extractResiduals(const TLSResultsMatrices& rm) {

	bool successfullExtraction = true;

	try{
		// Iteration through nodes of the LOR tree
		for (TDataTreeIterator itTree = fDataSet->getTree().begin(); itTree != fDataSet->getTree().end(); itTree++){		
			// Iterate through the Total station measurements (TSTN)
			for(auto itTSTN : itTree.node->data->measurements.fTSTN){
				// Iterate through every ROM of TSTN 
				for(auto itROM : itTSTN->roms){
					for(auto& itANGL:itROM->measANGL)
						extractAngleObs(rm, itANGL); //Extract residuals of ANGL

					for(auto& itZEND:itROM->measZEND)
						extractAngleObs(rm, itZEND); //Extract residuals of ZEND, same extractor used for both angle measurements

					for(auto& itDIST:itROM->measDIST)
						extractDistObs( rm, itDIST); //Extract residuals of DIST

					for(auto& itDHOR:itROM->measDHOR)
						extractDistObs(rm, itDHOR); //Extract residuals of DHOR
			
					for(auto& itPLR3D:itROM->measPLR3D)
						extractPLR3DObs(rm, itPLR3D);  //Extract residuals of PLR3D measurement
					
					for(auto& itECTH:itROM->measECTH)
						extractDistObs(rm, itECTH);

					for (auto& itECDIR : itROM->measECDIR)
						extractDistObs(rm, itECDIR);
					
				}
			}

			//Iterate through camera (CAM) measurements
			for(auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM){
				for(auto& itUVD:itCAM->measUVD)
					extractUVDObs(rm, itUVD);

				for(auto& itUVEC:itCAM->measUVEC)
					extractUVECObs(rm, itUVEC);
			}
			

			//In every node iterate through the EDM's measurements
			for(auto itEDM = itTree.node->data->measurements.fEDM.begin(); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM){
				//Iterate through DPST measurements
				for(auto& itDPST:itEDM->measDSPT){	
					extractDSPTObs(rm, itDPST);
				}
			}
			

			//In every node iterate through the LEVEL's measurements
			for(auto& itLEVEL:itTree.node->data->measurements.fLEVEL)
				extractLEVELObs(rm, itLEVEL);

			//In every node iterate through the ECHOROM's measurements
			for(auto& itECHO:itTree.node->data->measurements.fECHO)
				extractECHOROMObs(rm, itECHO);

			//In every node iterate through the ECVEROM's measurements
			for (auto& itECVE:itTree.node->data->measurements.fECVE)
				extractECVEROMObs(rm, itECVE);

			//In every node iterate through the ECSPROM's measurements
			for (auto& itECSP : itTree.node->data->measurements.fECSP)
				extractECSPROMObs(rm, itECSP);

			//In every node iterate through the ORIEROM's measurements
			for (auto& itORIE:itTree.node->data->measurements.fORIE)
				extractORIEROMObs(rm, itORIE);

			//Extract vertical distance DVER residuals
			extractDVERObs(rm, itTree.node->data->measurements.fDVER);

			//Extract pdor residuals
			extractPDORObs(rm, itTree.node->data->measurements.fPDOR);

			//Extract radi residuals
			extractRADIObs(rm, itTree.node->data->measurements.fRADI);

			//Extract OBSXYZ residuals
			extractOBSXYZObs(rm, itTree.node->data->measurements.fOBSXYZ);
		}
	}
	catch(std::exception const & excp) {
		fDataSet->getFileLogger() << TFileLogger::e_logType::LOG_ERROR <<  excp.what();
		successfullExtraction = false;
	}
	return successfullExtraction;
}

void TLSResultsMatricesExtractor::extractVarCovarParams(const TLSResultsMatrices& rm)
{
	extractPointVarCovar(rm);
	extractAngleVar(rm);
	extractLengthVar(rm);
	extractPlaneVarCovar(rm);
	extractTransformationVarCovar(rm);
}

//////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// Methods relative to the measurements (observations)
///////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractAngleObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>& anglMeas){
	MatrixIndex i = anglMeas.getFirstObservationIndex();
	if ( i < rm.getResidualsVctr()->size() ) {
		TAngle res(rm.getResidualsVctrElmt(i), TAngle::kRadians);
		anglMeas.setAngleResidual(res);
	}
	else
		throw std::runtime_error("Angle observation, problem during extraction residuals: observation index exceeds matrix dimensions");
}

void TLSResultsMatricesExtractor::extractDistObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget>& distanceMeas){
	MatrixIndex i = distanceMeas.getFirstObservationIndex();
	if ( i < rm.getResidualsVctr()->size() ) 
		distanceMeas.setDistanceResidual(TLength(rm.getResidualsVctrElmt(i)));
	else
		throw std::runtime_error("Distance observation made by a TSTN, problem during extraction residuals: observation index exceeds matrix dimensions");
}

void TLSResultsMatricesExtractor::extractDSPTObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TEDM::TTarget>& distanceMeas){
	MatrixIndex i = distanceMeas.getFirstObservationIndex();
	if ( i < rm.getResidualsVctr()->size() ) 
      distanceMeas.setDistanceResidual(TLength(rm.getResidualsVctrElmt(i)));
	else
		throw std::runtime_error("DSPT observation, problem during extraction residuals: observation index exceeds matrix dimensions");
}

void TLSResultsMatricesExtractor::extractDistObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TSCALE>& scaleMeas){
	MatrixIndex i = scaleMeas.getFirstObservationIndex();
	if (i < rm.getResidualsVctr()->size())
		scaleMeas.setDistanceResidual(TLength(rm.getResidualsVctrElmt(i)));
	else
		throw std::runtime_error("Distance observation made by a scale, problem during extraction residuals: observation index exceeds matrix dimensions");
}

void TLSResultsMatricesExtractor::extractPLR3DObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 1, EPLR3DAngles,  2>& plr3DMeas){
	MatrixIndex ANGLidx = plr3DMeas.getFirstObservationIndex();
	if ( ANGLidx < rm.getResidualsVctr()->size() ) {
		TAngle res(rm.getResidualsVctrElmt(ANGLidx), TAngle::kRadians);
		plr3DMeas.setAngleResidual(res, kANGL);
	}
	else
		throw std::runtime_error("PLR3D observation (angl), problem during extraction residuals: observation index exceeds matrix dimensions");

	MatrixIndex ZENDidx = plr3DMeas.getFirstObservationIndex() + 1;
	if ( ZENDidx < rm.getResidualsVctr()->size() ) {
		TAngle res(rm.getResidualsVctrElmt(ZENDidx), TAngle::kRadians);
		plr3DMeas.setAngleResidual(res, kZEND);
	}
	else
		throw std::runtime_error("PLR3D observation (zend), problem during extraction residuals: observation index exceeds matrix dimensions");

	MatrixIndex DISTidx = plr3DMeas.getFirstObservationIndex() + 2;
	if (DISTidx < rm.getResidualsVctr()->size())
	{
		plr3DMeas.setDistanceResidual(TLength(rm.getResidualsVctrElmt(DISTidx)));
		if (plr3DMeas.getDistance() + plr3DMeas.getDistanceResidual() < 0)
			throw std::runtime_error("PLR3D observation (dist), problem with the residuals: the distance is negative.\n Try to add ACST 200.0 after the V0 declaration and relaunch the calculation. \n *V0 ACST 200.0");
	}
	else
		throw std::runtime_error("PLR3D observation (d), problem during extraction residuals: observation index exceeds matrix dimensions");
}

void TLSResultsMatricesExtractor::extractUVDObs(const TLSResultsMatrices& rm, TUVD& uvdMeas){
	MatrixIndex XcompIDX = uvdMeas.getFirstObservationIndex();
	if ( XcompIDX < rm.getResidualsVctr()->size() ) 
		uvdMeas.setXVectorComponentResidual(rm.getResidualsVctrElmt(XcompIDX));
	else
		throw std::runtime_error("UVD observation (x), problem during extraction residuals: observation index exceeds matrix dimensions");

	MatrixIndex YcompIDX = uvdMeas.getFirstObservationIndex() + 1;
	if ( YcompIDX < rm.getResidualsVctr()->size() ) 
		uvdMeas.setYVectorComponentResidual(rm.getResidualsVctrElmt(YcompIDX));
	else
		throw std::runtime_error("UVD observation (y), problem during extraction residuals: observation index exceeds matrix dimensions");

	MatrixIndex sDistIDX = uvdMeas.getFirstObservationIndex() + 2;
	if ( sDistIDX < rm.getResidualsVctr()->size() ) 
		uvdMeas.setDistanceResidual(TLength(rm.getResidualsVctrElmt(sDistIDX)));
	else
		throw std::runtime_error("UVD observation (d), problem during extraction residuals: observation index exceeds matrix dimensions");
}


void TLSResultsMatricesExtractor::extractUVECObs(const TLSResultsMatrices& rm, TUVEC& uvecMeas){
	MatrixIndex XcompIDX = uvecMeas.getFirstObservationIndex();
	if ( XcompIDX < rm.getResidualsVctr()->size() ) 
		uvecMeas.setXVectorComponentResidual(rm.getResidualsVctrElmt(XcompIDX));
	else
		throw std::runtime_error("UVEC observation, problem during extraction residuals: observation index exceeds matrix dimensions");

	MatrixIndex YcompIDX = uvecMeas.getFirstObservationIndex() + 1;
	if ( YcompIDX < rm.getResidualsVctr()->size() ) 
		uvecMeas.setYVectorComponentResidual(rm.getResidualsVctrElmt(YcompIDX));
	else
		throw std::runtime_error("UVEC observation, problem during extraction residuals: observation index exceeds matrix dimensions");
}


void TLSResultsMatricesExtractor::extractLEVELObs(const TLSResultsMatrices& rm, TLEVEL& levelMeas){
	for(auto itDLEV(levelMeas.measDLEV.begin()); itDLEV != levelMeas.measDLEV.end(); ++itDLEV){
		MatrixIndex obsUidx = itDLEV->getFirstObservationIndex();
		if ( obsUidx < rm.getResidualsVctr()->size() ) 
         itDLEV->setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
			throw std::runtime_error("DHOR observation under DLEV, problem during extraction residuals: observation index exceeds matrix dimensions");

		if(itDLEV->dhor){ //If it is not nullptr
			obsUidx = itDLEV->dhor->getFirstObservationIndex();
			if ( obsUidx < rm.getResidualsVctr()->size() ) 
            itDLEV->dhor->setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
			else
				throw std::runtime_error("DLEV observation, problem during extraction residuals: observation index exceeds matrix dimensions");
		}
	}

}

void TLSResultsMatricesExtractor::extractECHOROMObs(const TLSResultsMatrices& rm, TECHOROM& echoMeas){
	for (auto itECHO(echoMeas.measECHO.begin()); itECHO != echoMeas.measECHO.end(); ++itECHO){
		MatrixIndex obsUidx = itECHO->getFirstObservationIndex();
		if ( obsUidx < rm.getResidualsVctr()->size() ) 
         itECHO->setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
			throw std::runtime_error("ECHO observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractECVEROMObs(const TLSResultsMatrices& rm, TECVEROM& ecveMeas){
	for (auto& itECVE:ecveMeas.measECVE){
		MatrixIndex obsUidx = itECVE.getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVctr()->size())
			itECVE.setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
			throw std::runtime_error("ECVE observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractECSPROMObs(const TLSResultsMatrices& rm, TECSPROM& ecspMeas){
	for (auto& itECSP : ecspMeas.measECSP){
		MatrixIndex obsUidx = itECSP.getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVctr()->size())
			itECSP.setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
			throw std::runtime_error("ECSP observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractORIEROMObs(const TLSResultsMatrices& rm, TORIEROM& orieMeas){
	for (auto& itORIE:orieMeas.measORIE){
		MatrixIndex obsUidx = itORIE.getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVctr()->size())
			itORIE.setAngleResidual(TAngle(rm.getResidualsVctrElmt(obsUidx),TAngle::EUnits::kRadians));
		else
			throw std::runtime_error("ORIE observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractDVERObs(const TLSResultsMatrices& rm, std::list<TDVER>& dver){
	for(auto itDVER(dver.begin()); itDVER != dver.end(); ++itDVER){
		MatrixIndex obsUidx = itDVER->getFirstObservationIndex();
		if ( obsUidx < rm.getResidualsVctr()->size() ) 
         itDVER->setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
			throw std::runtime_error("DVER observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractPDORObs(const TLSResultsMatrices& rm, TPdorObs& pdorObs){
	MatrixIndex obsUidx = pdorObs.getFirstObservationIndex();
	if (obsUidx < rm.getResidualsVctr()->size())
		pdorObs.setAngleResidual(TAngle(rm.getResidualsVctrElmt(obsUidx), TAngle::EUnits::kRadians));
	else
		throw std::runtime_error("PDOR observation, problem during extraction residuals: observation index exceeds matrix dimensions");
}

void TLSResultsMatricesExtractor::extractRADIObs(const TLSResultsMatrices& rm, std::list<TRADI>& radi){
	for (auto& itRADI:radi){
		MatrixIndex obsUidx = itRADI.getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVctr()->size())
			itRADI.setResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
			throw std::runtime_error("RADI observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractOBSXYZObs(const TLSResultsMatrices& rm, std::list<TOBSXYZ>& obsxyz){
	for (auto& itOBSXYZ : obsxyz){
		MatrixIndex obsUidx = itOBSXYZ.getFirstObservationIndex();
		if (obsUidx+2 < rm.getResidualsVctr()->size())
		{
			itOBSXYZ.setXResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
			itOBSXYZ.setYResidual(TLength(rm.getResidualsVctrElmt(obsUidx+1)));
			itOBSXYZ.setZResidual(TLength(rm.getResidualsVctrElmt(obsUidx+2)));
		}
		else
			throw std::runtime_error("OBSXYZ observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

///////////////////////////////////////////////////////////////
// Methods relative to the adjustable objects
///////////////////////////////////////////////////////////////
bool TLSResultsMatricesExtractor::extractPointParams(const TLSResultsMatrices& rm, const TReal convCrit)  {
	bool critNotExceeded = true;

	TReal numberOfPoints = (TReal)fDataSet->getPoints().numObjects();
	TReal factor = LITERAL(1.0) / numberOfPoints;

	TLength zero(LITERAL(0.0));
	TLength xcg = zero;
	TLength ycg = zero;
	TLength zcg = zero;

	for (auto& point : fDataSet->getPoints()){
		if(point.hasVariable()){
			for(int unknIdx = point.getFirstUidx(); unknIdx <= point.getLastUidx(); ++unknIdx){
				if (unknIdx >= rm.getSolutionVctr()->size())
					throw std::runtime_error("Unknown index of an angle: " + point.getName() + " exceeds matrix dimensions!");

				TReal correction = rm.getSolutionVctrElmt(unknIdx);
				point.setCorrection(unknIdx, correction);

				if ( fabsq(correction) > convCrit )
					critNotExceeded = false;
			}

			if (fDataSet->getConfig().libre.isActive())
			{
				// update the estimated centre of Gravity for the data points
				xcg = xcg + (point.getEstimatedValue().getX() * factor);
				ycg = ycg + (point.getEstimatedValue().getY() * factor);
				zcg = zcg + (point.getEstimatedValue().getZ() * factor);

			}
		}
	}

	if (fDataSet->getConfig().libre.isActive())
	{
		freeCnstr->setEstimatedGravityCenterCoord(xcg, ycg, zcg);
		//fDataSet->setFreeConstraints(cnstr);
	}
	return critNotExceeded;
}

bool TLSResultsMatricesExtractor::extractAngleParams(const TLSResultsMatrices& rm, const TReal convCrit) {
	bool critNotExceeded = true;

	for (auto& angle : fDataSet->getAngles()){
		if(!angle.isFixed()){
				MatrixIndex	unknIdx = angle.getFirstUidx();	//first=last only one unknown fo angle class

				if (unknIdx >= rm.getSolutionVctr()->size())
					throw std::runtime_error("Unknown index of an angle: " + angle.getName() + " exceeds matrix dimensions!");

				TReal	correctionVal = rm.getSolutionVctrElmt(unknIdx);
				angle.setCorrection(unknIdx, correctionVal);
				if ( fabsq(correctionVal) > convCrit )
					critNotExceeded = false;			
		}
	}
	return critNotExceeded;
}


bool TLSResultsMatricesExtractor::extractPlaneParams(const TLSResultsMatrices& rm, const TReal convCrit){
	bool critNotExceeded = true;

	for (auto& plane : fDataSet->getPlanes()){
		if(plane.hasVariable()){
			for(int unknIdx = plane.getFirstUidx(); unknIdx <= plane.getLastUidx(); unknIdx++){
					if (unknIdx >= rm.getSolutionVctr()->size())
						throw std::runtime_error("Unknown index of a plane: " + plane.getName() + " exceeds matrix dimensions!");

					TReal	correction = rm.getSolutionVctrElmt(unknIdx);
					plane.setCorrection(unknIdx, correction);
					if (fabsq(correction) > convCrit)
						critNotExceeded = false;		
			}
		}
	}
	return critNotExceeded;
}
	
bool TLSResultsMatricesExtractor::extractLineParams(const TLSResultsMatrices& rm, const TReal convCrit){
	bool critNotExceeded = true;

	for (auto& line : fDataSet->getLines()){
		if (!line.isFixed()){
			for (int unknIdx = line.getFirstUidx(); unknIdx <= line.getLastUidx(); unknIdx++){
				if (unknIdx >= rm.getSolutionVctr()->size())
					throw std::runtime_error("Unknown index of a plane: " + line.getName() + " exceeds matrix dimensions!");

				TReal	correction = rm.getSolutionVctrElmt(unknIdx);
				line.setCorrection(unknIdx, correction);
				if (fabsq(correction) > convCrit)
					critNotExceeded = false;
			}
		}
	}
	return critNotExceeded;
}

bool TLSResultsMatricesExtractor::extractLengthParams(const TLSResultsMatrices& rm, const TReal convCrit){
	bool critNotExceeded = true;
	for (auto& length : fDataSet->getLength()){
		if(!length.isFixed()){
				MatrixIndex unknIdx = length.getFirstUidx();	//first=last only one unknown fo angle class

				if (unknIdx >= rm.getSolutionVctr()->size())
					throw std::runtime_error("Unknown index of a scalar: " + length.getName() + " exceeds matrix dimensions!");

				TReal	correction = rm.getSolutionVctrElmt(unknIdx);
				length.setCorrection(unknIdx, correction);
				if ( fabsq(correction) > convCrit )
					critNotExceeded = false;			
		}
	}
	return critNotExceeded;
}

bool TLSResultsMatricesExtractor::extractTransformationParams(const TLSResultsMatrices& rm, const TReal convCrit){
	bool critNotExceeded = true;

	for (auto it(fDataSet->getTree().begin()); it != fDataSet->getTree().end(); ++it){	
		auto& trafo(it.node->data.get()->frame);

		if(trafo.hasVariable()){
			for(int unknIdx = trafo.getFirstUidx(); unknIdx <= trafo.getLastUidx(); unknIdx++){
				if (unknIdx >= rm.getSolutionVctr()->size())
					throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");

				TReal	correction = rm.getSolutionVctrElmt(unknIdx);
				trafo.setCorrection(unknIdx, correction);
				if (fabsq(correction) > convCrit)
					critNotExceeded = false;	
			}
		}
	}
	return critNotExceeded;
}


//////////////////////////////////////////////////////////////////////////////
// Returns the boolean that indicates if a new iteration is necessary or not
//////////////////////////////////////////////////////////////////////////////
bool	TLSResultsMatricesExtractor::lastIteration() const {

	return fLastIteration;
}

//////////////////////////////////////////////////////////////////////////////////
// extract var. and covar. params
//////////////////////////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractPointVarCovar(const TLSResultsMatrices& rm)
{
	for (auto& point : fDataSet->getPoints()){
		if(point.hasVariable()){
			//Filling standard deviations (estimated precision)
			for(int unknIdx = point.getFirstUidx(); unknIdx <= point.getLastUidx(); ++unknIdx){

				if (rm.getUnkCovarMtrx()->rows() == 0)
					// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
					// -> Sets the values arbitrary to -1
					point.setEstimatedPrecision(unknIdx, -1);
				else if (unknIdx >= rm.getUnkCovarMtrx()->rows())
					throw std::runtime_error("Unknown index of a point: " + point.getName() + " exceeds matrix dimensions!");
				else {
					TReal sigma = sqrtq(rm.getUnkCovarMtrxElmt(unknIdx, unknIdx));	//Set standard deviation in metres [m], sqrt(variance)
					point.setEstimatedPrecision(unknIdx, sigma);
				}

			}
			//Filling XY covariance
			if(!point.isCoordinateFixed(0) && !point.isCoordinateFixed(1)){
				int xi = point.getCoordinateUnknIndex(0);
				int yi = point.getCoordinateUnknIndex(1);
				if (rm.getUnkCovarMtrx()->rows() == 0)
					// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
					// -> Sets the values arbitrary to -1
					point.setXYEstimatedCovariance(-1);
				else if ( (xi >= rm.getUnkCovarMtrx()->rows()) || (yi >= rm.getUnkCovarMtrx()->rows()) )
					throw std::runtime_error("Unknown index of a point: " + point.getName() + " exceeds matrix dimensions!");
				else
					point.setXYEstimatedCovariance(rm.getUnkCovarMtrxElmt(xi,yi)); //Here it is multiplied by a factor of 1000 in LGC1 , weird
			}
			//Filling YZ covariance
			if(!point.isCoordinateFixed(1) && !point.isCoordinateFixed(2)){
				int yi = point.getCoordinateUnknIndex(1);
				int zi = point.getCoordinateUnknIndex(2);
				if (rm.getUnkCovarMtrx()->rows() == 0)
					// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
					// -> Sets the values arbitrary to -1
					point.setYZEstimatedCovariance(-1);
				else if ( (yi >= rm.getUnkCovarMtrx()->rows()) || (zi >= rm.getUnkCovarMtrx()->rows()) )
					throw std::runtime_error("Unknown index of a point: " + point.getName() + " exceeds matrix dimensions!");
				else
					point.setYZEstimatedCovariance(rm.getUnkCovarMtrxElmt(yi,zi) ); //Here it is multiplied by a factor of 1000 in LGC1 , weird
			}
			//Filling XZ covariance
			if(!point.isCoordinateFixed(0) && !point.isCoordinateFixed(2)){
				int xi = point.getCoordinateUnknIndex(0);
				int zi = point.getCoordinateUnknIndex(2);
				if (rm.getUnkCovarMtrx()->rows() == 0)
					// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
					// -> Sets the values arbitrary to -1
					point.setXZEstimatedCovariance(-1);
				else if ( (xi >= rm.getUnkCovarMtrx()->rows()) || (zi >= rm.getUnkCovarMtrx()->rows()) )
					throw std::runtime_error("Unknown index of a point: " + point.getName() + " exceeds matrix dimensions!");
				else
					point.setXZEstimatedCovariance(rm.getUnkCovarMtrxElmt(xi,zi) );//Here it is multiplied by a factor of 1000 in LGC1 , weird 
			}
		}
	}
}

void TLSResultsMatricesExtractor::extractAngleVar(const TLSResultsMatrices& rm)
{
	for (auto& angle : fDataSet->getAngles()){
		if(!angle.isFixed()){
			//Filling standard deviations (estimated precision)
			int unknIdx = angle.getFirstUidx(); 

			if (rm.getUnkCovarMtrx()->rows() == 0) 
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				angle.setEstimatedPrecision(unknIdx, -1);
			else if (unknIdx >= rm.getUnkCovarMtrx()->rows())
				throw std::runtime_error("Unknown index of an angle: " + angle.getName() + " exceeds matrix dimensions!");
			else
				angle.setEstimatedPrecision(unknIdx, sqrtq(rm.getUnkCovarMtrxElmt(unknIdx,unknIdx))); //Set estimated precision in RADs
		}
	}
}

void TLSResultsMatricesExtractor::extractLengthVar(const TLSResultsMatrices& rm)
{
	for (auto& length : fDataSet->getLength()){
		if(!length.isFixed()){
			//Filling standard deviations (estimated precision)
			int unknIdx = length.getFirstUidx(); 
			
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				length.setEstimatedPrecision(unknIdx, -1);
			else if (unknIdx >= rm.getUnkCovarMtrx()->rows())
				throw std::runtime_error("Unknown index of a scalar: " + length.getName() + " exceeds matrix dimensions!");
			else
				length.setEstimatedPrecision(unknIdx, sqrtq(rm.getUnkCovarMtrxElmt(unknIdx,unknIdx))); //Set estimated precision in METRES
		}
	}
}

void TLSResultsMatricesExtractor::extractPlaneVarCovar(const TLSResultsMatrices& rm){
	for (auto& plane : fDataSet->getPlanes()){
		if(plane.hasVariable()){
			//Filling standard deviations (estimated precision)
			for(int unknIdx = plane.getFirstUidx(); unknIdx <= plane.getLastUidx(); ++unknIdx){
				
				if (rm.getUnkCovarMtrx()->rows() == 0)
					// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
					// -> Sets the values arbitrary to -1
					plane.setEstimatedPrecision(unknIdx, -1); 
				else if (unknIdx >= rm.getUnkCovarMtrx()->rows())
					throw std::runtime_error("Unknown index of a plane: " + plane.getName() + " exceeds matrix dimensions!");
				else {
					TReal sigma = sqrtq(rm.getUnkCovarMtrxElmt(unknIdx, unknIdx));
					plane.setEstimatedPrecision(unknIdx, sigma); //Store standard deviations in meters (reference point distance ) or radians (angles)
				}
			}

			/* Eventually store covarinace between angles if needed in the future.*/
		}
	}
}

void TLSResultsMatricesExtractor::extractLineVarCovar(const TLSResultsMatrices& rm){
	for (auto& line : fDataSet->getLines()){
		if (!line.isFixed()){
			//Filling standard deviations (estimated precision)
			for (int unknIdx = line.getFirstUidx(); unknIdx <= line.getLastUidx(); ++unknIdx){

				if (rm.getUnkCovarMtrx()->rows() == 0)
					// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
					// -> Sets the values arbitrary to -1
					line.setLineVectorEstimatedPrecision(unknIdx, -1);
				else if (unknIdx >= rm.getUnkCovarMtrx()->rows())
					throw std::runtime_error("Unknown index of a plane: " + line.getName() + " exceeds matrix dimensions!");
				else {
					TReal sigma = sqrtq(rm.getUnkCovarMtrxElmt(unknIdx, unknIdx));
					line.setLineVectorEstimatedPrecision(unknIdx, sigma); //Store standard deviations in meters (reference point distance ) or radians (angles)
				}
			}

			/* Eventually store covarinace between angles if needed in the future.*/
		}
	}
}

void TLSResultsMatricesExtractor::extractTransformationVarCovar(const TLSResultsMatrices& rm){
	for (auto it(fDataSet->getTree().begin()); it != fDataSet->getTree().end(); ++it){	
		auto& trafo(it.node->data.get()->frame);

		if(trafo.hasVariable()){
			//Filling standard deviations (estimated precision)
			for(int unknIdx = trafo.getFirstUidx(); unknIdx <= trafo.getLastUidx(); unknIdx++){

				if (rm.getUnkCovarMtrx()->rows() == 0)
					// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
					// -> Sets the values arbitrary to -1
					trafo.setEstimatedPrecision(unknIdx, -1);
				else if (unknIdx >= rm.getUnkCovarMtrx()->rows()) //rm.getSolutionVctr()->size())
					throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
				else {
					TReal sigma = sqrtq(rm.getUnkCovarMtrxElmt(unknIdx, unknIdx));
					trafo.setEstimatedPrecision(unknIdx, sigma); //Store standard deviations in METRES
				}

			}
		}

    	//Filling XY covariance of a translation
		if(!trafo.isTranslationFixed(0) && !trafo.isTranslationFixed(1)){
			int xi = trafo.getTranslationUnknIndex(0);
			int yi = trafo.getTranslationUnknIndex(1);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setXYTranslationCovariance(-1);
			else if ( (xi >= rm.getUnkCovarMtrx()->rows()) || (yi >= rm.getUnkCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of translation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setXYTranslationCovariance(rm.getUnkCovarMtrxElmt(xi,yi)); //Store covariance in METRES
		}
		//Filling YZ covariance of a translation
		if(!trafo.isTranslationFixed(1) && !trafo.isTranslationFixed(2)){
			int yi = trafo.getTranslationUnknIndex(1);
			int zi = trafo.getTranslationUnknIndex(2);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setYZTranslationCovariance(-1);
			else if ( (yi >= rm.getUnkCovarMtrx()->rows()) || (zi >= rm.getUnkCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of translation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setYZTranslationCovariance(rm.getUnkCovarMtrxElmt(yi,zi)); //Store covariance in METRES
		}
		//Filling XZ covariance of a translation
		if(!trafo.isTranslationFixed(0) && !trafo.isTranslationFixed(2)){
			int xi = trafo.getTranslationUnknIndex(0);
			int zi = trafo.getTranslationUnknIndex(2);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setXZTranslationCovariance(-1);
			else if ( (xi >= rm.getUnkCovarMtrx()->rows()) || (zi >= rm.getUnkCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of translation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setXZTranslationCovariance(rm.getUnkCovarMtrxElmt(xi,zi)); //Store covariance in METRES
		}


    	//Filling XY covariance of a rotation
		if(!trafo.isRotationFixed(0) && !trafo.isRotationFixed(1)){
			int xi = trafo.getRotationUnknIndex(0);
			int yi = trafo.getRotationUnknIndex(1);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setXYRotationCovariance(-1);
			else if ( (xi >= rm.getUnkCovarMtrx()->rows()) || (yi >= rm.getUnkCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of rotation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setXYRotationCovariance(rm.getUnkCovarMtrxElmt(xi,yi)); //Store covariance in METRES
		}
		//Filling YZ covariance of a rotation
		if(!trafo.isRotationFixed(1) && !trafo.isRotationFixed(2)){
			int yi = trafo.getRotationUnknIndex(1);
			int zi = trafo.getRotationUnknIndex(2);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setYZRotationCovariance(-1);
			else if ( (yi >= rm.getUnkCovarMtrx()->rows()) || (zi >= rm.getUnkCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of rotation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setYZRotationCovariance(rm.getUnkCovarMtrxElmt(yi,zi)); //Store covariance in METRES
		}
		//Filling XZ covariance of a rotation
		if(!trafo.isRotationFixed(0) && !trafo.isRotationFixed(2)){
			int xi = trafo.getRotationUnknIndex(0);
			int zi = trafo.getRotationUnknIndex(2);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setXZRotationCovariance(-1);
			else if ( (xi >= rm.getUnkCovarMtrx()->rows()) || (zi >= rm.getUnkCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of rotation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setXZRotationCovariance(rm.getUnkCovarMtrxElmt(xi,zi)); //Store covariance in METRES
		}

		
		//Filling scale covariance
		if (!trafo.isTranslationFixed(0) && !trafo.isScaleFixed()){
			int xi = trafo.getTranslationUnknIndex(0);
			int li = trafo.getScaleUnknIndex();
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setScaleCovariance(0,-1);
			else if ((xi >= rm.getUnkCovarMtrx()->rows()) || (li >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setScaleCovariance(0, rm.getUnkCovarMtrxElmt(xi, li));
		}
		if (!trafo.isTranslationFixed(1) && !trafo.isScaleFixed()){
			int yi = trafo.getTranslationUnknIndex(1);
			int li = trafo.getScaleUnknIndex();
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setScaleCovariance(1, -1);
			else if ((yi >= rm.getUnkCovarMtrx()->rows()) || (li >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setScaleCovariance(1, rm.getUnkCovarMtrxElmt(yi, li));
		}
		if (!trafo.isTranslationFixed(2) && !trafo.isScaleFixed()){
			int zi = trafo.getTranslationUnknIndex(2);
			int li = trafo.getScaleUnknIndex();
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setScaleCovariance(2, -1);
			else if ((zi >= rm.getUnkCovarMtrx()->rows()) || (li >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setScaleCovariance(2, rm.getUnkCovarMtrxElmt(zi, li));
		}
		if (!trafo.isRotationFixed(0) && !trafo.isScaleFixed()){
			int xi = trafo.getRotationUnknIndex(0);
			int li = trafo.getScaleUnknIndex();
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setScaleCovariance(3, -1);
			else if ((xi >= rm.getUnkCovarMtrx()->rows()) || (li >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setScaleCovariance(3,rm.getUnkCovarMtrxElmt(xi, li)); 
		}
		if (!trafo.isRotationFixed(1) && !trafo.isScaleFixed()){
			int yi = trafo.getRotationUnknIndex(1);
			int li = trafo.getScaleUnknIndex();
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setScaleCovariance(4, -1);
			else if ((yi >= rm.getUnkCovarMtrx()->rows()) || (li >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setScaleCovariance(4, rm.getUnkCovarMtrxElmt(yi, li)); 
		}
		if (!trafo.isRotationFixed(2) && !trafo.isScaleFixed()){
			int zi = trafo.getRotationUnknIndex(2);
			int li = trafo.getScaleUnknIndex();
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setScaleCovariance(5, -1);
			else if ((zi >= rm.getUnkCovarMtrx()->rows()) || (li >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setScaleCovariance(5, rm.getUnkCovarMtrxElmt(zi, li)); 
		}
		//Filling translation-rotation covariance
		//tx-rot
		if (!trafo.isRotationFixed(0) && !trafo.isTranslationFixed(0)){
			int xi = trafo.getRotationUnknIndex(0);
			int ti = trafo.getTranslationUnknIndex(0);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setTrRotCovariance(0, -1);
			else if ((xi >= rm.getUnkCovarMtrx()->rows()) || (ti >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setTrRotCovariance(0, rm.getUnkCovarMtrxElmt(xi, ti));
		}
		if (!trafo.isRotationFixed(1) && !trafo.isTranslationFixed(0)){
			int yi = trafo.getRotationUnknIndex(1);
			int ti = trafo.getTranslationUnknIndex(0);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setTrRotCovariance(1, -1);
			else if ((yi >= rm.getUnkCovarMtrx()->rows()) || (ti >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setTrRotCovariance(1, rm.getUnkCovarMtrxElmt(yi, ti));
		}
		if (!trafo.isRotationFixed(2) && !trafo.isTranslationFixed(0)){
			int zi = trafo.getRotationUnknIndex(2);
			int ti = trafo.getTranslationUnknIndex(0);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setTrRotCovariance(2, -1);
			else if ((zi >= rm.getUnkCovarMtrx()->rows()) || (ti >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setTrRotCovariance(2, rm.getUnkCovarMtrxElmt(zi, ti));
		}
		//ty-rot
		if (!trafo.isRotationFixed(0) && !trafo.isTranslationFixed(1)){
			int xi = trafo.getRotationUnknIndex(0);
			int ti = trafo.getTranslationUnknIndex(1);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setTrRotCovariance(3, -1);
			else if ((xi >= rm.getUnkCovarMtrx()->rows()) || (ti >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setTrRotCovariance(3, rm.getUnkCovarMtrxElmt(xi, ti));
		}
		if (!trafo.isRotationFixed(1) && !trafo.isTranslationFixed(1)){
			int yi = trafo.getRotationUnknIndex(1);
			int ti = trafo.getTranslationUnknIndex(1);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setTrRotCovariance(4, -1);
			else if ((yi >= rm.getUnkCovarMtrx()->rows()) || (ti >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setTrRotCovariance(4, rm.getUnkCovarMtrxElmt(yi, ti));
		}
		if (!trafo.isRotationFixed(2) && !trafo.isTranslationFixed(1)){
			int zi = trafo.getRotationUnknIndex(2);
			int ti = trafo.getTranslationUnknIndex(1);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setTrRotCovariance(5, -1);
			else if ((zi >= rm.getUnkCovarMtrx()->rows()) || (ti >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setTrRotCovariance(5, rm.getUnkCovarMtrxElmt(zi, ti));
		}
		//tz-rot
		if (!trafo.isRotationFixed(0) && !trafo.isTranslationFixed(2)){
			int xi = trafo.getRotationUnknIndex(0);
			int ti = trafo.getTranslationUnknIndex(2);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setTrRotCovariance(6, -1);
			else if ((xi >= rm.getUnkCovarMtrx()->rows()) || (ti >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setTrRotCovariance(6, rm.getUnkCovarMtrxElmt(xi, ti));
		}
		if (!trafo.isRotationFixed(1) && !trafo.isTranslationFixed(2)){
			int yi = trafo.getRotationUnknIndex(1);
			int ti = trafo.getTranslationUnknIndex(2);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setTrRotCovariance(7, -1);
			else if ((yi >= rm.getUnkCovarMtrx()->rows()) || (ti >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setTrRotCovariance(7, rm.getUnkCovarMtrxElmt(yi, ti));
		}
		if (!trafo.isRotationFixed(2) && !trafo.isTranslationFixed(2)){
			int zi = trafo.getRotationUnknIndex(2);
			int ti = trafo.getTranslationUnknIndex(2);
			if (rm.getUnkCovarMtrx()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				trafo.setTrRotCovariance(8, -1);
			else if ((zi >= rm.getUnkCovarMtrx()->rows()) || (ti >= rm.getUnkCovarMtrx()->rows()))
				throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			else
				trafo.setTrRotCovariance(8, rm.getUnkCovarMtrxElmt(zi, ti));
		}
		
	}
}

void TLSResultsMatricesExtractor::extractRelError(const TLSResultsMatrices& rm)
{
	try{
		TReal fS0 = 0.0;
		if (rm.getSigmaZero2() != NO_VALf)
			fS0 = sqrtq(rm.getSigmaZero2());

		auto iterb = fDataSet->getConfig().erelPairs.begin();

		while (iterb != fDataSet->getConfig().erelPairs.end()) {

			auto& pt1 = fDataSet->getPoints().getObject(iterb->first);
			auto& pt2 = fDataSet->getPoints().getObject(iterb->second);

			// if points are not defined -> warning...
			if (pt1.getFrameTreePosition() == pt2.getFrameTreePosition()){

				TLSCalcRelativeError	re(pt1, pt2);

				// matrix indices
				MatrixIndex	ind[6];
				for (int k = 0; k < 6; k++)
					ind[k] = -1;

				if (!pt1.isCoordinateFixed(0))
					ind[0] = pt1.getCoordinateUnknIndex(0);
				if (!pt1.isCoordinateFixed(1))
					ind[1] = pt1.getCoordinateUnknIndex(1);
				if (!pt1.isCoordinateFixed(2))
					ind[2] = pt1.getCoordinateUnknIndex(2);
				if (!pt2.isCoordinateFixed(0))
					ind[3] = pt2.getCoordinateUnknIndex(0);
				if (!pt2.isCoordinateFixed(1))
					ind[4] = pt2.getCoordinateUnknIndex(1);
				if (!pt2.isCoordinateFixed(2))
					ind[5] = pt2.getCoordinateUnknIndex(2);

				// sous-matrice cofacteur
				TMatrix Q12(6, 6);
				int i, j;

				for (i = 0; i<6; i++) {
					for (j = 0; j<6; j++) {
						if ((ind[i] > -1) && (ind[j] > -1))
							Q12(i, j) = (*rm.getUnkCovarMtrx()).coeff(ind[i], ind[j]) / powq(fS0, 2);
						else
							Q12(i, j) = LITERAL(0.0);
					}
				}

				// computation of horizontal & spatial distances, and vertical angle & orientation
				TReal ds, dh, G, V, x1, y1, z1, x2, y2, z2;
				x1 = pt1.getEstimatedValue().getX().getMetresValue();
				y1 = pt1.getEstimatedValue().getY().getMetresValue();
				z1 = pt1.getEstimatedValue().getZ().getMetresValue();
				x2 = pt2.getEstimatedValue().getX().getMetresValue();
				y2 = pt2.getEstimatedValue().getY().getMetresValue();
				z2 = pt2.getEstimatedValue().getZ().getMetresValue();
				// length unit: meter
				ds = sqrtq(powq(x2 - x1, 2) + powq(y2 - y1, 2) + powq(z2 - z1, 2));
				dh = sqrtq(powq(x2 - x1, 2) + powq(y2 - y1, 2));
				// angle unit: radian
				G = atan2q(x2 - x1, y2 - y1);
				V = acosq((z2 - z1) / ds);

				// computation of sigmaL
				TReal sl(LITERAL(0.0));
				TReal co[6]; // error propagation
				co[0] = -(x2 - x1) / ds;
				co[1] = -(y2 - y1) / ds;
				co[2] = -(z2 - z1) / ds;
				co[3] = -co[0];
				co[4] = -co[1];
				co[5] = -co[2];

				for (i = 0; i < 6; i++) {
					for (j = 0; j < 6; j++)
						sl += Q12(i, j) * co[i] * co[j];
				}

				TLength sigmaL(sqrtq(sl));
				re.setSigmaL(sigmaL);

				// computation of sigmaG 
				TReal sg(LITERAL(0.0)), sr(LITERAL(0.0));
				TAngle Gis(G); // normalisation of G
				co[0] = -(Gis.cosine()) / dh;
				co[1] = (Gis.sine()) / dh;
				co[2] = LITERAL(0.0);
				co[3] = -co[0];
				co[4] = -co[1];
				co[5] = -co[2];

				for (i = 0; i < 6; i++) {
					for (j = 0; j < 6; j++)
						sg += Q12(i, j) * co[i] * co[j];
				}

				TAngle sigmaG(sqrtq(sg));
				re.setSigmaG(sigmaG);

				// computation of sigmaR
				sr = dh * sqrtq(sg);
				TLength sigmaR(sr);
				re.setSigmaR(sigmaR);

				// Computation of sigmaZ
				TReal sz(LITERAL(0.0));
				sz = Q12(2, 2) + Q12(5, 5) - Q12(2, 5) * 2;
				TLength sigmaZ(sqrtq(sz));
				re.setSigmaZ(sigmaZ);

				// Computation of sigmaV
				TReal sv(LITERAL(0.0));
				sv = (sqrtq(sz)*sinq(V)) / ds;
				TAngle sigmaV(sv);
				re.setSigmaV(sigmaV);

				fDataSet->getRelError().push_back(re);

			}
			else
			{
				string warning(iterb->first + " et " + iterb->second + " ne sont pas definit dans le meme frame.");
				throw std::logic_error(warning);
			}

			iterb++;
		}
	}
	catch (std::exception const & excp) {
		fDataSet->getFileLogger() << TFileLogger::e_logType::LOG_ERROR << excp.what();
	}

	return;
}
