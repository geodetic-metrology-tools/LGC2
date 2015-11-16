#include "TLSResultsMatricesExtractor.h"
#include "TLGCData.h"
#include "TLSResultsMatrices.h"
#include "TUVEC.h"
#include "TUVD.h"


TLSResultsMatricesExtractor::TLSResultsMatricesExtractor(TLGCData* fData):
fDataSet(fData),
fLastIteration(false)
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
		//bool ln = extractLineParams(rm, convCrit);

		if ( (pt && angl && pln && trf && len) || fDataSet->getConfig().allfixed.isActive() )
			fLastIteration = true;

	}
	catch (std::exception const & excp) {
		fDataSet->getFileLogger() << TFileLogger::e_logType::LOG_ERROR <<  excp.what();
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
			for(auto itTSTN(itTree.node->data->measurements.fTSTN.begin()); itTSTN != itTree.node->data->measurements.fTSTN.end(); ++itTSTN){
				// Iterate through every ROM of TSTN 
				for(auto itROM(itTSTN->roms.begin()); itROM != itTSTN->roms.end(); ++itROM){
					for(auto itANGL(itROM->measANGL.begin()); itANGL != itROM->measANGL.end(); ++itANGL)
						extractAngleObs(rm, *itANGL); //Extract residuals of ANGL

					for(auto itZEND(itROM->measZEND.begin()); itZEND != itROM->measZEND.end(); ++itZEND)
						extractAngleObs(rm, *itZEND); //Extract residuals of ZEND, same extractor used for both angle measurements

					for(auto itDIST(itROM->measDIST.begin()); itDIST != itROM->measDIST.end(); ++itDIST)
						extractDistObs( rm, *itDIST); //Extract residuals of DIST

					for(auto itDHOR(itROM->measDHOR.begin()); itDHOR != itROM->measDHOR.end(); ++itDHOR)
						extractDistObs(rm, *itDHOR); //Extract residuals of DHOR

				
					for(auto itPLR3D(itROM->measPLR3D.begin()); itPLR3D != itROM->measPLR3D.end(); ++itPLR3D)
						extractPLR3DObs(rm, *itPLR3D);  //Extract residuals of PLR3D measurement
					
					for(auto itECTH(itROM->measECTH.begin()); itECTH != itROM->measECTH.end(); ++itECTH){
						extractDistObs(rm, *itECTH);
					}
					
				}
			}

			//Iterate through camera (CAM) measurements
			for(auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM){
				for(auto itUVD(itCAM->measUVD.begin()); itUVD != itCAM->measUVD.end(); ++itUVD)
					extractUVDObs(rm, *itUVD);

				for(auto itUVEC(itCAM->measUVEC.begin()); itUVEC != itCAM->measUVEC.end(); ++itUVEC)
					extractUVECObs(rm, *itUVEC);
			}
			

			//In every node iterate through the EDM's measurements
			for(auto itEDM(itTree.node->data->measurements.fEDM.begin()); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM){
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

			//In every node iterate through the ECSPROM's measurements
			for (auto& itECSP:itTree.node->data->measurements.fECSP)
				extractECSPROMObs(rm, itECSP);

			//In every node iterate through the ECVEROM's measurements
			for (auto& itECVE:itTree.node->data->measurements.fECVE)
				extractECVEROMObs(rm, itECVE);

			//In every node iterate through the ORIEROM's measurements
			for (auto& itORIE:itTree.node->data->measurements.fORIE)
				extractORIEROMObs(rm, itORIE);

			//Extract vertical distance DVER residuals
			extractDVERObs(rm, itTree.node->data->measurements.fDVER);
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
	if ( DISTidx < rm.getResidualsVctr()->size() ) 
      plr3DMeas.setDistanceResidual(TLength(rm.getResidualsVctrElmt(DISTidx)));
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

void TLSResultsMatricesExtractor::extractECSPROMObs(const TLSResultsMatrices& rm, TECSPROM& ecspMeas){
	for (auto& itECSP:ecspMeas.measECSP){
		MatrixIndex obsUidx = itECSP.getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVctr()->size())
			itECSP.setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
			throw std::runtime_error("ECSP observation, problem during extraction residuals: observation index exceeds matrix dimensions");
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

void TLSResultsMatricesExtractor::extractORIEROMObs(const TLSResultsMatrices& rm, TORIEROM& orieMeas){
	for (auto& itORIE:orieMeas.measORIE){
		MatrixIndex obsUidx = itORIE.getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVctr()->size())
			itORIE.setAngleResidual(TAngle(rm.getResidualsVctrElmt(obsUidx),TAngle::EUnits::kRadians));
		else
			throw std::runtime_error("ORIE observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractDVERObs(const TLSResultsMatrices& rm, std::vector<TDVER>& dver){
	for(auto itDVER(dver.begin()); itDVER != dver.end(); ++itDVER){
		MatrixIndex obsUidx = itDVER->getFirstObservationIndex();
		if ( obsUidx < rm.getResidualsVctr()->size() ) 
         itDVER->setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
			throw std::runtime_error("DVER observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

///////////////////////////////////////////////////////////////
// Methods relative to the adjustable objects
///////////////////////////////////////////////////////////////
bool TLSResultsMatricesExtractor::extractPointParams(const TLSResultsMatrices& rm, const TReal convCrit)  {
	bool critNotExceeded = true;

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
		}
	}
#if 0
	/*
		Update estimated center of gravity, to be added into TLGCData class???

		// update the estimated centre of Gravity for the data points
		xcg = xcg + (iterB->getEstimatedValue().getX() * factor);
		ycg = ycg + (iterB->getEstimatedValue().getY() * factor);
		zcg = zcg + (iterB->getEstimatedValue().getZ() * factor);
	*/
#endif
	if(fDataSet->getConfig().libre.isActive())
	{
		throw std::runtime_error("LIBR option is not implemented yet!");
		/*
			Do some staff :: To be implemented later.
		*/
		/*TLSConstraintIdentifier cnstr = fDataSet->getFreeConstraints();
		cnstr.setEstimatedGravityCenterCoord(xcg, ycg, zcg);
		fDataSet->setFreeConstraints(cnstr);*/
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
				if (unknIdx >= rm.getUnkCovarMtrx()->rows())
					throw std::runtime_error("Unknown index of a point: " + point.getName() + " exceeds matrix dimensions!");

				TReal sigma = sqrtq(rm.getUnkCovarMtrxElmt(unknIdx,unknIdx));	//Set standard deviation in metres [m], sqrt(variance)
				point.setEstimatedPrecision(unknIdx,sigma);
			}
			//Filling XY covariance
			if(!point.isCoordinateFixed(0) && !point.isCoordinateFixed(1)){
				int xi = point.getCoordinateUnknIndex(0);
				int yi = point.getCoordinateUnknIndex(1);
				if ( (xi >= rm.getUnkCovarMtrx()->rows()) || (yi >= rm.getUnkCovarMtrx()->rows()) )
					throw std::runtime_error("Unknown index of a point: " + point.getName() + " exceeds matrix dimensions!");
				point.setXYEstimatedCovariance(rm.getUnkCovarMtrxElmt(xi,yi)); //Here it is multiplied by a factor of 1000 in LGC1 , weird
			}
			//Filling YZ covariance
			if(!point.isCoordinateFixed(1) && !point.isCoordinateFixed(2)){
				int yi = point.getCoordinateUnknIndex(1);
				int zi = point.getCoordinateUnknIndex(2);
				if ( (yi >= rm.getUnkCovarMtrx()->rows()) || (zi >= rm.getUnkCovarMtrx()->rows()) )
					throw std::runtime_error("Unknown index of a point: " + point.getName() + " exceeds matrix dimensions!");
				point.setYZEstimatedCovariance(rm.getUnkCovarMtrxElmt(yi,zi) ); //Here it is multiplied by a factor of 1000 in LGC1 , weird
			}
			//Filling XZ covariance
			if(!point.isCoordinateFixed(0) && !point.isCoordinateFixed(2)){
				int xi = point.getCoordinateUnknIndex(0);
				int zi = point.getCoordinateUnknIndex(2);
				if ( (xi >= rm.getUnkCovarMtrx()->rows()) || (zi >= rm.getUnkCovarMtrx()->rows()) )
					throw std::runtime_error("Unknown index of a point: " + point.getName() + " exceeds matrix dimensions!");
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
			if (unknIdx >= rm.getUnkCovarMtrx()->rows())
				throw std::runtime_error("Unknown index of an angle: " + angle.getName() + " exceeds matrix dimensions!");
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
			if (unknIdx >= rm.getUnkCovarMtrx()->rows())
				throw std::runtime_error("Unknown index of a scalar: " + length.getName() + " exceeds matrix dimensions!");
			length.setEstimatedPrecision(unknIdx, sqrtq(rm.getUnkCovarMtrxElmt(unknIdx,unknIdx))); //Set estimated precision in METRES
		}
	}
}

void TLSResultsMatricesExtractor::extractPlaneVarCovar(const TLSResultsMatrices& rm){
	for (auto& plane : fDataSet->getPlanes()){
		if(plane.hasVariable()){
			//Filling standard deviations (estimated precision)
			for(int unknIdx = plane.getFirstUidx(); unknIdx <= plane.getLastUidx(); ++unknIdx){
				if (unknIdx >= rm.getUnkCovarMtrx()->rows())
					throw std::runtime_error("Unknown index of a plane: " + plane.getName() + " exceeds matrix dimensions!");

				TReal sigma = sqrtq(rm.getUnkCovarMtrxElmt(unknIdx,unknIdx));	
				plane.setEstimatedPrecision(unknIdx,sigma); //Store standard deviations in meters (reference point distance ) or radians (angles)
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
				if (unknIdx >= rm.getUnkCovarMtrx()->rows())
					throw std::runtime_error("Unknown index of a plane: " + line.getName() + " exceeds matrix dimensions!");

				TReal sigma = sqrtq(rm.getUnkCovarMtrxElmt(unknIdx, unknIdx));
				line.setLineVectorEstimatedPrecision(unknIdx, sigma); //Store standard deviations in meters (reference point distance ) or radians (angles)
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
				if (unknIdx >= rm.getSolutionVctr()->size())
					throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");

				TReal sigma = sqrtq(rm.getUnkCovarMtrxElmt(unknIdx,unknIdx));	
				trafo.setEstimatedPrecision(unknIdx, sigma); //Store standard deviations in METRES
			}
		}

    	//Filling XY covariance of a translation
		if(!trafo.isTranslationFixed(0) && !trafo.isTranslationFixed(1)){
			int xi = trafo.getTranslationUnknIndex(0);
			int yi = trafo.getTranslationUnknIndex(1);
			if ( (xi >= rm.getUnkCovarMtrx()->rows()) || (yi >= rm.getUnkCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of translation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			trafo.setXYTranslationCovariance(rm.getUnkCovarMtrxElmt(xi,yi)); //Store covariance in METRES
		}
		//Filling YZ covariance of a translation
		if(!trafo.isTranslationFixed(1) && !trafo.isTranslationFixed(2)){
			int yi = trafo.getTranslationUnknIndex(1);
			int zi = trafo.getTranslationUnknIndex(2);
			if ( (yi >= rm.getUnkCovarMtrx()->rows()) || (zi >= rm.getUnkCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of translation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			trafo.setYZTranslationCovariance(rm.getUnkCovarMtrxElmt(yi,zi)); //Store covariance in METRES
		}
		//Filling XZ covariance of a translation
		if(!trafo.isTranslationFixed(0) && !trafo.isTranslationFixed(2)){
			int xi = trafo.getTranslationUnknIndex(0);
			int zi = trafo.getTranslationUnknIndex(2);
			if ( (xi >= rm.getUnkCovarMtrx()->rows()) || (zi >= rm.getUnkCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of translation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			trafo.setXZTranslationCovariance(rm.getUnkCovarMtrxElmt(xi,zi)); //Store covariance in METRES
		}


    	//Filling XY covariance of a rotation
		if(!trafo.isRotationFixed(0) && !trafo.isRotationFixed(1)){
			int xi = trafo.getRotationUnknIndex(0);
			int yi = trafo.getRotationUnknIndex(1);
			if ( (xi >= rm.getUnkCovarMtrx()->rows()) || (yi >= rm.getUnkCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of rotation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			trafo.setXYRotationCovariance(rm.getUnkCovarMtrxElmt(xi,yi)); //Store covariance in METRES
		}
		//Filling YZ covariance of a rotation
		if(!trafo.isRotationFixed(1) && !trafo.isRotationFixed(2)){
			int yi = trafo.getRotationUnknIndex(1);
			int zi = trafo.getRotationUnknIndex(2);
			if ( (yi >= rm.getUnkCovarMtrx()->rows()) || (zi >= rm.getUnkCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of rotation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			trafo.setYZRotationCovariance(rm.getUnkCovarMtrxElmt(yi,zi)); //Store covariance in METRES
		}
		//Filling XZ covariance of a rotation
		if(!trafo.isRotationFixed(0) && !trafo.isRotationFixed(2)){
			int xi = trafo.getRotationUnknIndex(0);
			int zi = trafo.getRotationUnknIndex(2);
			if ( (xi >= rm.getUnkCovarMtrx()->rows()) || (zi >= rm.getUnkCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of rotation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			trafo.setXZRotationCovariance(rm.getUnkCovarMtrxElmt(xi,zi)); //Store covariance in METRES
		}
	}
}
