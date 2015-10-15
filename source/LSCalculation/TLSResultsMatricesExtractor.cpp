#include "TLSResultsMatricesExtractor.h"
#include "TLGCData.h"
#include "lsalgo\TLSResultsMatrices.h"

//////////////////////////////////////////////////////////////////////////////
// constructor setting, setting default values and storing pointer to project data
//////////////////////////////////////////////////////////////////////////////
TLSResultsMatricesExtractor::TLSResultsMatricesExtractor(TLGCData* fData):
fDataSet(fData),
fLastIteration(false),
fS0(1.0),
fIsSimulation(false)
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
		bool scl = extractScalarParams(rm, convCrit);
		bool trf = extractTransformationParams(rm, convCrit);
		//bool ln = extractLineParams(rm, convCrit);

		if ( (pt && angl && pln && scl && trf) || fDataSet->cfg.allfixed.isActive() )
			fLastIteration = true;

	}
	catch (std::exception const & excp) {
		fDataSet->fOutputFileWriter << TFileLogger::e_logType::LOG_ERROR <<  excp.what();
		successfullExtraction = false;
		return successfullExtraction;
	}
	return successfullExtraction;
}

bool TLSResultsMatricesExtractor::extractResiduals(const TLSResultsMatrices& rm) {

	bool successfullExtraction = true;

	try{
		if (rm.S0APosterioriVariances()) 
			fS0 = sqrtq(rm.getSigmaZero2());		

		// Iteration through nodes of the LOR tree
		for (TDataTreeIterator itTree = fDataSet->tree.begin(); itTree != fDataSet->tree.end(); itTree++){		
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


					for(auto itDIR3D(itROM->measDIR3D.begin()); itDIR3D != itROM->measDIR3D.end(); ++itDIR3D)
						extractDIR3DObs(rm, *itDIR3D); //Extract resiudals of DIR3D measurement

					#if 0
					for(auto itECTH(itROM->measECTH.begin()); itECTH != itROM->measECTH.end(); ++itECTH){

					}
					#endif
				}
			}

			//Iterate through camera (CAM) measurements
			for(auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM){
				for(auto itPLR3D(itCAM->measPLR3D.begin()); itPLR3D != itCAM->measPLR3D.end(); ++itPLR3D)
						extractPLR3DObs(rm, *itPLR3D);

				for(auto itDIR3D(itCAM->measDIR3D.begin()); itDIR3D != itCAM->measDIR3D.end(); ++itDIR3D)
						extractDIR3DObs(rm, *itDIR3D);
			}
			#if 0
			//In every node iterate through the EDM's measurements
			for(auto itEDM(itTree.node->data->measurements.fEDM.begin()); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM){
				//Iterate through DPST measurements
				for(auto itDPST(itEDM->measDSPT.begin()); itDPST != itEDM->measDSPT.end(); ++itDPST){	
					//To do
				}
			}
			#endif
			//In every node iterate through the LEVEL's measurements
			for(auto itLEVEL(itTree.node->data->measurements.fLEVEL.begin()); itLEVEL != itTree.node->data->measurements.fLEVEL.end(); ++itLEVEL)
				extractLEVELObs(rm, *itLEVEL);
		}
	}
	catch(std::exception const & excp) {
		fDataSet->fOutputFileWriter << TFileLogger::e_logType::LOG_ERROR <<  excp.what();
		successfullExtraction = false;
	}
	return successfullExtraction;
}

void TLSResultsMatricesExtractor::extractVarCovarParams(const TLSResultsMatrices& rm)
{
	extractPointVarCovar(rm);
	extractAngleVar(rm);
	extractScalarVar(rm);
	extractPlaneVarCovar(rm);
	extractTransformationVarCovar(rm);
}

//////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// Methods relative to the measurements (observations)
///////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractAngleObs(const TLSResultsMatrices& rm, TAMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>& anglMeas){
	MatrixIndex i = anglMeas.getFirstObservationIndex();
	if ( i < rm.getResidualsVctr()->size() ) {
		LGC::TAngle res(LGC::TAngle::kRadians, rm.getResidualsVctrElmt(i));
		anglMeas.setAngleResidual(res);
	}
	else
		throw std::runtime_error("Angle observation, problem during extraction residuals: observation index exceeds matrix dimensions");
}

void TLSResultsMatricesExtractor::extractDistObs(const TLSResultsMatrices& rm, TAMeas<TInstrumentData::TPOLAR::TTarget>& distanceMeas){
	MatrixIndex i = distanceMeas.getFirstObservationIndex();
	if ( i < rm.getResidualsVctr()->size() ) 
		distanceMeas.setScalarResidual(rm.getResidualsVctrElmt(i));
	else
		throw std::runtime_error("Distance observation, problem during extraction residuals: observation index exceeds matrix dimensions");
}

void TLSResultsMatricesExtractor::extractPLR3DObs(const TLSResultsMatrices& rm, TAMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 1, EPLR3DAngles,  2>& plr3DMeas){
	MatrixIndex ANGLidx = plr3DMeas.getFirstObservationIndex();
	if ( ANGLidx < rm.getResidualsVctr()->size() ) {
		LGC::TAngle res(LGC::TAngle::kRadians, rm.getResidualsVctrElmt(ANGLidx));
		plr3DMeas.setAngleResidual(res, kANGL);
	}
	else
		throw std::runtime_error("PLR3D observation, problem during extraction residuals: observation index exceeds matrix dimensions");

	MatrixIndex ZENDidx = plr3DMeas.getFirstObservationIndex() + 1;
	if ( ZENDidx < rm.getResidualsVctr()->size() ) {
		LGC::TAngle res(LGC::TAngle::kRadians, rm.getResidualsVctrElmt(ZENDidx));
		plr3DMeas.setAngleResidual(res, kZEND);
	}
	else
		throw std::runtime_error("PLR3D observation, problem during extraction residuals: observation index exceeds matrix dimensions");

	MatrixIndex DISTidx = plr3DMeas.getFirstObservationIndex() + 2;
	if ( DISTidx < rm.getResidualsVctr()->size() ) 
		plr3DMeas.setScalarResidual(rm.getResidualsVctrElmt(DISTidx));
	else
		throw std::runtime_error("PLR3D observation, problem during extraction residuals: observation index exceeds matrix dimensions");
}

void TLSResultsMatricesExtractor::extractDIR3DObs(const TLSResultsMatrices& rm, TAMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 0, EPLR3DAngles,  2>& dir3DMeas){
	MatrixIndex ANGLidx = dir3DMeas.getFirstObservationIndex();
	if ( ANGLidx < rm.getResidualsVctr()->size() ) {
		LGC::TAngle res(LGC::TAngle::kRadians, rm.getResidualsVctrElmt(ANGLidx));
		dir3DMeas.setAngleResidual(res, kANGL);
	}
	else
		throw std::runtime_error("DIR3D observation, problem during extraction residuals: observation index exceeds matrix dimensions");

	MatrixIndex ZENDidx = dir3DMeas.getFirstObservationIndex() + 1;
	if ( ZENDidx < rm.getResidualsVctr()->size() ) {
		LGC::TAngle res(LGC::TAngle::kRadians, rm.getResidualsVctrElmt(ZENDidx));
		dir3DMeas.setAngleResidual(res, kZEND);
	}
	else
		throw std::runtime_error("DIR3D observation, problem during extraction residuals: observation index exceeds matrix dimensions");
}


void TLSResultsMatricesExtractor::extractLEVELObs(const TLSResultsMatrices& rm, TLEVEL& levelMeas){
	for(auto itDLEV(levelMeas.measDLEV.begin()); itDLEV != levelMeas.measDLEV.end(); ++itDLEV){
		MatrixIndex obsUidx = itDLEV->getObservationIndex();
		if ( obsUidx < rm.getResidualsVctr()->size() ) 
			itDLEV->setVertDistResidual(rm.getResidualsVctrElmt(obsUidx));
		else
			throw std::runtime_error("DLEV observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}

	for(auto itDHOR(levelMeas.measDHOR.begin()); itDHOR != levelMeas.measDHOR.end(); ++itDHOR){
		MatrixIndex obsUidx = itDHOR->getFirstObservationIndex();
		if ( obsUidx < rm.getResidualsVctr()->size() ) 
			itDHOR->setScalarResidual(rm.getResidualsVctrElmt(obsUidx));
		else
			throw std::runtime_error("DLEV observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

///////////////////////////////////////////////////////////////
// Methods relative to the adjustable objects
///////////////////////////////////////////////////////////////
bool TLSResultsMatricesExtractor::extractPointParams(const TLSResultsMatrices& rm, const TReal convCrit)  {
	bool critNotExceeded = true;

	for (auto it = fDataSet->points.objects.begin(); it != fDataSet->points.objects.end(); ++it){
		if(it->hasVariable()){
			for(int unknIdx = it->getFirstUidx(); unknIdx <= it->getLastUidx(); ++unknIdx){
				if (unknIdx >= rm.getSolutionVctr()->size())
					throw std::runtime_error("Unknown index of an angle: " + it->getName() + " exceeds matrix dimensions!");

				TReal correction = rm.getSolutionVctrElmt(unknIdx);
				it->setCorrection(unknIdx, correction);

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
/*
	The part below is treated in Adjustable Point itself and therefore here should be omitted.
*/	
#if 0
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
#endif
	if(fDataSet->cfg.libre.isActive())
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

	for (auto it = fDataSet->angles.objects.begin(); it != fDataSet->angles.objects.end(); ++it){
		if(!it->isFixed()){
				MatrixIndex	unknIdx = it->getFirstUidx();	//first=last only one unknown fo angle class

				if (unknIdx >= rm.getSolutionVctr()->size())
					throw std::runtime_error("Unknown index of an angle: " + it->getName() + " exceeds matrix dimensions!");

				TReal	correctionVal = rm.getSolutionVctrElmt(unknIdx);
				it->setCorrection(unknIdx, correctionVal);
				if ( fabsq(correctionVal) > convCrit )
					critNotExceeded = false;			
		}
	}
	return critNotExceeded;
}


bool TLSResultsMatricesExtractor::extractPlaneParams(const TLSResultsMatrices& rm, const TReal convCrit){
	bool critNotExceeded = true;

	for (auto it = fDataSet->planes.objects.begin(); it != fDataSet->planes.objects.end(); ++it){
		if(it->hasVariable()){
			for(int unknIdx = it->getFirstUidx(); unknIdx <= it->getLastUidx(); unknIdx++){
					if (unknIdx >= rm.getSolutionVctr()->size())
						throw std::runtime_error("Unknown index of a plane: " + it->getName() + " exceeds matrix dimensions!");

					TReal	correction = rm.getSolutionVctrElmt(unknIdx);
					it->setCorrection(unknIdx, correction);
					if (fabsq(correction) > convCrit)
						critNotExceeded = false;		
			}
		}
	}
	return critNotExceeded;
}
	

bool TLSResultsMatricesExtractor::extractScalarParams(const TLSResultsMatrices& rm, const TReal convCrit){
	bool critNotExceeded = true;
	for (auto it = fDataSet->scalars.objects.begin(); it != fDataSet->scalars.objects.end(); ++it){
		if(!it->isFixed()){
				MatrixIndex unknIdx = it->getFirstUidx();	//first=last only one unknown fo angle class

				if (unknIdx >= rm.getSolutionVctr()->size())
					throw std::runtime_error("Unknown index of a scalar: " + it->getName() + " exceeds matrix dimensions!");

				TReal	correction = rm.getSolutionVctrElmt(unknIdx);
				it->setCorrection(unknIdx, correction);
				if ( fabsq(correction) > convCrit )
					critNotExceeded = false;			
		}
	}
	return critNotExceeded;
}


bool TLSResultsMatricesExtractor::extractTransformationParams(const TLSResultsMatrices& rm, const TReal convCrit){
	bool critNotExceeded = true;

	for (auto it(fDataSet->tree.begin()); it != fDataSet->tree.end(); ++it){	
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
	for (auto it = fDataSet->points.objects.begin(); it != fDataSet->points.objects.end(); ++it){
		if(it->hasVariable()){
			//Filling standard deviations (estimated precision)
			for(int unknIdx = it->getFirstUidx(); unknIdx <= it->getLastUidx(); ++unknIdx){
				if (unknIdx >= rm.getResCovarMtrx()->rows())
					throw std::runtime_error("Unknown index of a point: " + it->getName() + " exceeds matrix dimensions!");

				TReal sigma = sqrtq(rm.getResCovarMtrxElmt(unknIdx,unknIdx));	//Set standard deviation in metres [m]
				it->setEstimatedPrecision(unknIdx,sigma);
			}
			//Filling XY covariance
			if(!it->isCoordinateFixed(0) && !it->isCoordinateFixed(1)){
				int xi = it->getCoordinateUnknIndex(0);
				int yi = it->getCoordinateUnknIndex(1);
				if ( (xi >= rm.getResCovarMtrx()->rows()) || (yi >= rm.getResCovarMtrx()->rows()) )
					throw std::runtime_error("Unknown index of a point: " + it->getName() + " exceeds matrix dimensions!");
				it->setXYEstimatedCovariance(rm.getResCovarMtrxElmt(xi,yi)); //Here it is multiplied by a factor of 1000 in LGC1 , weird
			}
			//Filling YZ covariance
			if(!it->isCoordinateFixed(1) && !it->isCoordinateFixed(2)){
				int yi = it->getCoordinateUnknIndex(1);
				int zi = it->getCoordinateUnknIndex(2);
				if ( (yi >= rm.getResCovarMtrx()->rows()) || (zi >= rm.getResCovarMtrx()->rows()) )
					throw std::runtime_error("Unknown index of a point: " + it->getName() + " exceeds matrix dimensions!");
				it->setYZEstimatedCovariance(rm.getResCovarMtrxElmt(yi,zi) ); //Here it is multiplied by a factor of 1000 in LGC1 , weird
			}
			//Filling XZ covariance
			if(!it->isCoordinateFixed(0) && !it->isCoordinateFixed(2)){
				int xi = it->getCoordinateUnknIndex(0);
				int zi = it->getCoordinateUnknIndex(2);
				if ( (xi >= rm.getResCovarMtrx()->rows()) || (zi >= rm.getResCovarMtrx()->rows()) )
					throw std::runtime_error("Unknown index of a point: " + it->getName() + " exceeds matrix dimensions!");
				it->setXZEstimatedCovariance(rm.getResCovarMtrxElmt(xi,zi) );//Here it is multiplied by a factor of 1000 in LGC1 , weird 
			}
		}
	}
}

void TLSResultsMatricesExtractor::extractAngleVar(const TLSResultsMatrices& rm)
{
	for (auto it = fDataSet->angles.objects.begin(); it != fDataSet->angles.objects.end(); ++it){
		if(!it->isFixed()){
			//Filling standard deviations (estimated precision)
			int unknIdx = it->getFirstUidx(); 
			if (unknIdx >= rm.getResCovarMtrx()->rows())
				throw std::runtime_error("Unknown index of an angle: " + it->getName() + " exceeds matrix dimensions!");
			it->setEstimatedPrecision(unknIdx, sqrtq(rm.getResCovarMtrxElmt(unknIdx,unknIdx))); //Set estimated precision in RADs
		}
	}
}

void TLSResultsMatricesExtractor::extractScalarVar(const TLSResultsMatrices& rm)
{
	for (auto it = fDataSet->scalars.objects.begin(); it != fDataSet->scalars.objects.end(); ++it){
		if(!it->isFixed()){
			//Filling standard deviations (estimated precision)
			int unknIdx = it->getFirstUidx(); 
			if (unknIdx >= rm.getResCovarMtrx()->rows())
				throw std::runtime_error("Unknown index of a scalar: " + it->getName() + " exceeds matrix dimensions!");
			it->setEstimatedPrecision(unknIdx, sqrtq(rm.getResCovarMtrxElmt(unknIdx,unknIdx))); //Set estimated precision in METRES
		}
	}
}

void TLSResultsMatricesExtractor::extractPlaneVarCovar(const TLSResultsMatrices& rm){
	for (auto it = fDataSet->planes.objects.begin(); it != fDataSet->planes.objects.end(); ++it){
		if(it->hasVariable()){
			//Filling standard deviations (estimated precision)
			for(int unknIdx = it->getFirstUidx(); unknIdx <= it->getLastUidx(); ++unknIdx){
				if (unknIdx >= rm.getResCovarMtrx()->rows())
					throw std::runtime_error("Unknown index of a plane: " + it->getName() + " exceeds matrix dimensions!");

				TReal sigma = sqrtq(rm.getResCovarMtrxElmt(unknIdx,unknIdx));	
				it->setEstimatedPrecision(unknIdx,sigma); //Store standard deviations in METRES
			}
			//Filling XY covariance for vector of the plane
			if(!it->isPlaneVectorFixed(0) && !it->isPlaneVectorFixed(1)){
				int xi = it->getPlaneVectorUnknIndex(0);
				int yi = it->getPlaneVectorUnknIndex(1);
				if ( (xi >= rm.getResCovarMtrx()->rows()) || (yi >= rm.getResCovarMtrx()->rows()) )
					throw std::runtime_error("Unknown index of a plane: " + it->getName() + " exceeds matrix dimensions!");
				it->setXYNormalVectorCovariance(rm.getResCovarMtrxElmt(xi,yi)); //Store covariance in METRES
			}
			//Filling YZ covariance for vector of the plane
			if(!it->isPlaneVectorFixed(1) && !it->isPlaneVectorFixed(2)){
				int yi = it->getPlaneVectorUnknIndex(1);
				int zi = it->getPlaneVectorUnknIndex(2);
				if ( (yi >= rm.getResCovarMtrx()->rows()) || (zi >= rm.getResCovarMtrx()->rows()) )
					throw std::runtime_error("Unknown index of a point: " + it->getName() + " exceeds matrix dimensions!");
				it->setYZNormalVectorCovariance(rm.getResCovarMtrxElmt(yi,zi)); //Store covariance in METRES
			}
			//Filling XZ covariance for vector of the plane
			if(!it->isPlaneVectorFixed(0) && !it->isPlaneVectorFixed(2)){
				int xi = it->getPlaneVectorUnknIndex(0);
				int zi = it->getPlaneVectorUnknIndex(2);
				if ( (xi >= rm.getResCovarMtrx()->rows()) || (zi >= rm.getResCovarMtrx()->rows()) )
					throw std::runtime_error("Unknown index of a plane: " + it->getName() + " exceeds matrix dimensions!");
				it->setXZNormalVectorCovariance(rm.getResCovarMtrxElmt(xi,zi)); //Store covariance in METRES
			}
		}
	}
}


void TLSResultsMatricesExtractor::extractTransformationVarCovar(const TLSResultsMatrices& rm){
	for (auto it(fDataSet->tree.begin()); it != fDataSet->tree.end(); ++it){	
		auto& trafo(it.node->data.get()->frame);

		if(trafo.hasVariable()){
			//Filling standard deviations (estimated precision)
			for(int unknIdx = trafo.getFirstUidx(); unknIdx <= trafo.getLastUidx(); unknIdx++){
				if (unknIdx >= rm.getSolutionVctr()->size())
					throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");

				TReal sigma = sqrtq(rm.getResCovarMtrxElmt(unknIdx,unknIdx));	
				trafo.setEstimatedPrecision(unknIdx, sigma); //Store standard deviations in METRES
			}
		}

    	//Filling XY covariance of a translation
		if(!trafo.isTranslationFixed(0) && !trafo.isTranslationFixed(1)){
			int xi = trafo.getTranslationUnknIndex(0);
			int yi = trafo.getTranslationUnknIndex(1);
			if ( (xi >= rm.getResCovarMtrx()->rows()) || (yi >= rm.getResCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of translation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			trafo.setXYTranslationCovariance(rm.getResCovarMtrxElmt(xi,yi)); //Store covariance in METRES
		}
		//Filling YZ covariance of a translation
		if(!trafo.isTranslationFixed(1) && !trafo.isTranslationFixed(2)){
			int yi = trafo.getTranslationUnknIndex(1);
			int zi = trafo.getTranslationUnknIndex(2);
			if ( (yi >= rm.getResCovarMtrx()->rows()) || (zi >= rm.getResCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of translation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			trafo.setYZTranslationCovariance(rm.getResCovarMtrxElmt(yi,zi)); //Store covariance in METRES
		}
		//Filling XZ covariance of a translation
		if(!trafo.isTranslationFixed(0) && !trafo.isTranslationFixed(2)){
			int xi = trafo.getTranslationUnknIndex(0);
			int zi = trafo.getTranslationUnknIndex(2);
			if ( (xi >= rm.getResCovarMtrx()->rows()) || (zi >= rm.getResCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of translation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			trafo.setXZTranslationCovariance(rm.getResCovarMtrxElmt(xi,zi)); //Store covariance in METRES
		}


    	//Filling XY covariance of a rotation
		if(!trafo.isRotationFixed(0) && !trafo.isRotationFixed(1)){
			int xi = trafo.getRotationUnknIndex(0);
			int yi = trafo.getRotationUnknIndex(1);
			if ( (xi >= rm.getResCovarMtrx()->rows()) || (yi >= rm.getResCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of rotation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			trafo.setXYRotationCovariance(rm.getResCovarMtrxElmt(xi,yi)); //Store covariance in METRES
		}
		//Filling YZ covariance of a rotation
		if(!trafo.isRotationFixed(1) && !trafo.isRotationFixed(2)){
			int yi = trafo.getRotationUnknIndex(1);
			int zi = trafo.getRotationUnknIndex(2);
			if ( (yi >= rm.getResCovarMtrx()->rows()) || (zi >= rm.getResCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of rotation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			trafo.setYZRotationCovariance(rm.getResCovarMtrxElmt(yi,zi)); //Store covariance in METRES
		}
		//Filling XZ covariance of a rotation
		if(!trafo.isRotationFixed(0) && !trafo.isRotationFixed(2)){
			int xi = trafo.getRotationUnknIndex(0);
			int zi = trafo.getRotationUnknIndex(2);
			if ( (xi >= rm.getResCovarMtrx()->rows()) || (zi >= rm.getResCovarMtrx()->rows()) )
				throw std::runtime_error("Unknown index of rotation of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
			trafo.setXZRotationCovariance(rm.getResCovarMtrxElmt(xi,zi)); //Store covariance in METRES
		}
	}
}
