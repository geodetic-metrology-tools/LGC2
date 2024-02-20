#include "TLSResultsMatricesExtractor.h"

#include <Logger.hpp>
#include <TLGCData.h>

#include "TLSResultsMatrices.h"

TLSResultsMatricesExtractor::TLSResultsMatricesExtractor(TLGCData *fData) : fDataSet(fData), fLastIteration(false)
{
}

TLSResultsMatricesExtractor::~TLSResultsMatricesExtractor()
{ // Destructor
}
/////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////

/****************************************************************************************************************
	Public Method: extractResults
	Extracts resulting parameters of adjustable objects (points, planes, lengths,...) from calculated matrices
****************************************************************************************************************/
bool TLSResultsMatricesExtractor::extractResults(const TLSResultsMatrices &rm, TReal convCrit)
{
	bool successfullExtraction = true;
	fLastIteration = false;
	logDebug() << "Extract resulting parameters of adjustable objects (points, planes, lengths,...) from calculated matrices";
	try
	{
		bool pt = extractPointParams(rm, convCrit);
		bool angl = extractAngleParams(rm, convCrit);
		bool pln = extractPlaneParams(rm, convCrit);
		bool len = extractLengthParams(rm, convCrit);
		bool trf = extractTransformationParams(rm, convCrit);
		bool ln = extractLineParams(rm, convCrit);

		if ((pt && angl && pln && trf && len && ln) || fDataSet->getConfig().allfixed.isActive())
			fLastIteration = true;
	}
	catch (std::exception const &excp)
	{
		logWarning() << "Could not extract properly resulting parameters of adjustable objects (points, planes, lengths,...) from calculated matrices. Exception caught:"
					 << excp.what();
		successfullExtraction = false;
		return successfullExtraction;
	}
	return successfullExtraction;
}

/****************************************************************************************************************
	Public Method: extractResiduals
	Extracts residues from calculated matrices
****************************************************************************************************************/
bool TLSResultsMatricesExtractor::extractResiduals(const TLSResultsMatrices &rm)
{
	bool successfullExtraction = true;
	logDebug() << "Extract residuals from calculated matrices";

	try
	{
		// Iteration through nodes of the LOR tree
		for (TDataTreeIterator itTree = fDataSet->getTree().begin(); itTree != fDataSet->getTree().end(); itTree++)
		{
			// Iterate through the Total station measurements (TSTN)
			for (auto itTSTN : itTree.node->data->measurements.fTSTN)
			{
				// Iterate through every ROM of TSTN
				for (auto itROM : itTSTN->roms)
				{
					for (auto &itANGL : itROM->measANGL)
						extractAngleObs(rm, itANGL); // Extract residuals of ANGL

					for (auto &itZEND : itROM->measZEND)
						extractAngleObs(rm, itZEND); // Extract residuals of ZEND, same extractor used for both angle measurements

					for (auto &itDIST : itROM->measDIST)
						extractDistObs(rm, itDIST); // Extract residuals of DIST

					for (auto &itDHOR : itROM->measDHOR)
						extractDistObs(rm, itDHOR); // Extract residuals of DHOR

					for (auto &itPLR3D : itROM->measPLR3D)
						extractPLR3DObs(rm, itPLR3D); // Extract residuals of PLR3D measurement

					for (auto &itECTH : itROM->measECTH)
						extractDistObs(rm, itECTH);

					for (auto &itECDIR : itROM->measECDIR)
						extractDistObs(rm, itECDIR);
				}
			}

			// Iterate through camera (CAM) measurements
			for (auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM)
			{
				for (auto &itUVD : itCAM->measUVD)
					extractUVDObs(rm, itUVD);

				for (auto &itUVEC : itCAM->measUVEC)
					extractUVECObs(rm, itUVEC);
			}

			// In every node iterate through the EDM's measurements
			for (auto itEDM = itTree.node->data->measurements.fEDM.begin(); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM)
			{
				// Iterate through DPST measurements
				for (auto &itDPST : itEDM->measDSPT)
					extractDSPTObs(rm, itDPST);
			}

			// In every node iterate through the LEVEL's measurements
			for (auto &itLEVEL : itTree.node->data->measurements.fLEVEL)
				extractLEVELObs(rm, itLEVEL);

			// In every node iterate through the ECHOROM's measurements
			for (auto &itECHO : itTree.node->data->measurements.fECHO)
				extractECHOROMObs(rm, itECHO);

			// In every node iterate through the ECVEROM's measurements
			for (auto &itECVE : itTree.node->data->measurements.fECVE)
				extractECVEROMObs(rm, itECVE);

			// In every node iterate through the ECSPROM's measurements
			for (auto &itECSP : itTree.node->data->measurements.fECSP)
				extractECSPROMObs(rm, itECSP);

			// In every node iterate through the ORIEROM's measurements
			for (auto &itORIE : itTree.node->data->measurements.fORIE)
				extractORIEROMObs(rm, itORIE);

			// Extract vertical distance DVER residuals
			extractDVERObs(rm, itTree.node->data->measurements.fDVER);

			// Extract pdor residuals
			extractPDORObs(rm, itTree.node->data->measurements.fPDOR);

			// Extract radi residuals
			extractRADIObs(rm, itTree.node->data->measurements.fRADI);

			// Extract OBSXYZ residuals
			extractOBSXYZObs(rm, itTree.node->data->measurements.fOBSXYZ);

			// In every node iterate through the INCLYROM's measurements
			for (auto &itINCLY : itTree.node->data->measurements.fINCLY)
				extractINCLYROMObs(rm, itINCLY);

			// In every node iterate through the ECWSROM's measurements
			for (auto &itECWS : itTree.node->data->measurements.fECWS)
				extractECWSROMObs(rm, itECWS);

			// In every node iterate through the ECWIROM's measurements
			for (auto &itECWI : itTree.node->data->measurements.fECWI)
				extractECWIROMObs(rm, itECWI);
		}
	}
	catch (std::exception const &excp)
	{
		logCritical() << "Could not extract properly the residues from calculated matrices! Exception caught:" << excp.what();
		successfullExtraction = false;
	}
	return successfullExtraction;
}

/****************************************************************************************************************
	Public Method: extractVarCovarParams
	Extracts variances and covariances from calculated matrices
****************************************************************************************************************/
bool TLSResultsMatricesExtractor::extractVarCovarParams(const TLSResultsMatrices &rm)
{
	bool successfullExtraction = true;
	try
	{
		extractFullCovar(rm);
		extractPointVarCovar(rm);
		extractAngleVar(rm);
		extractLengthVar(rm);
		extractPlaneVarCovar(rm);
		extractTransformationVarCovar(rm);
	}
	catch (std::exception const &excp)
	{
		logCritical() << "Could not extract the covariances from calculated matrices! Exception caught:" << excp.what();
		successfullExtraction = false;
	}
	return successfullExtraction;
}

//////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// Methods relative to the measurements (observations)
///////////////////////////////////////////////////////////////

void TLSResultsMatricesExtractor::extractAngleObs(const TLSResultsMatrices &rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1> &anglMeas)
{
	MatrixIndex i = anglMeas.getFirstObservationIndex();
	if (i < rm.getResidualsVectByConst()->size())
	{
		TAngle res(rm.getResidualsVctrElmt(i), TAngle::kRadians);
		anglMeas.setAngleResidual(res);
	}
	else
	{
		logCritical() << "Angle observation, problem during extraction residuals: observation index exceeds matrix dimensions! (input line number:" << anglMeas.line << ")";
		throw std::runtime_error("Angle observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractDistObs(const TLSResultsMatrices &rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget> &distanceMeas)
{
	MatrixIndex i = distanceMeas.getFirstObservationIndex();
	if (i < rm.getResidualsVectByConst()->size())
		distanceMeas.setDistanceResidual(TLength(rm.getResidualsVctrElmt(i)));
	else
	{
		logCritical() << "Distance observation made by a TSTN, problem during extraction residuals: observation index exceeds matrix dimensions! (input line number:"
					  << distanceMeas.line << ")";
		throw std::runtime_error("Distance observation made by a TSTN, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractDSPTObs(const TLSResultsMatrices &rm, TAScalarMeas<TInstrumentData::TEDM::TTarget> &distanceMeas)
{
	MatrixIndex i = distanceMeas.getFirstObservationIndex();
	if (i < rm.getResidualsVectByConst()->size())
		distanceMeas.setDistanceResidual(TLength(rm.getResidualsVctrElmt(i)));
	else
	{
		logCritical() << "DSPT observation, problem during extraction residuals: observation index exceeds matrix dimensions! (input line number:" << distanceMeas.line << ")";
		throw std::runtime_error("DSPT observation, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractDistObs(const TLSResultsMatrices &rm, TAScalarMeas<TInstrumentData::TSCALE> &scaleMeas)
{
	MatrixIndex i = scaleMeas.getFirstObservationIndex();
	if (i < rm.getResidualsVectByConst()->size())
		scaleMeas.setDistanceResidual(TLength(rm.getResidualsVctrElmt(i)));
	else
	{
		logCritical() << "Distance observation made by a scale, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:"
					  << scaleMeas.line << ")";
		throw std::runtime_error("Distance observation made by a scale, problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractPLR3DObs(const TLSResultsMatrices &rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 1, EPLR3DAngles, 2> &plr3DMeas)
{
	MatrixIndex ANGLidx = plr3DMeas.getFirstObservationIndex();
	if (ANGLidx < rm.getResidualsVectByConst()->size())
	{
		TAngle res(rm.getResidualsVctrElmt(ANGLidx), TAngle::kRadians);
		plr3DMeas.setAngleResidual(res, kANGL);
	}
	else
	{
		logCritical() << "PLR3D observation (angl), problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << plr3DMeas.line << ")";
		throw std::runtime_error("PLR3D observation (angl), problem during extraction residuals: observation index exceeds matrix dimensions");
	}

	MatrixIndex ZENDidx = plr3DMeas.getFirstObservationIndex() + 1;
	if (ZENDidx < rm.getResidualsVectByConst()->size())
	{
		TAngle res(rm.getResidualsVctrElmt(ZENDidx), TAngle::kRadians);
		plr3DMeas.setAngleResidual(res, kZEND);
	}
	else
	{
		logCritical() << "PLR3D observation (zend), problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << plr3DMeas.line << ")";
		throw std::runtime_error("PLR3D observation (zend), problem during extraction residuals: observation index exceeds matrix dimensions");
	}

	MatrixIndex DISTidx = plr3DMeas.getFirstObservationIndex() + 2;
	if (DISTidx < rm.getResidualsVectByConst()->size())
	{
		plr3DMeas.setDistanceResidual(TLength(rm.getResidualsVctrElmt(DISTidx)));
		if (plr3DMeas.getDistance() + plr3DMeas.getDistanceResidual() < 0)
		{
			logCritical() << "PLR3D observation (dist), problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << plr3DMeas.line << ")";
			throw std::runtime_error("PLR3D observation (dist), problem with the residuals: the distance is negative.\n Try to add ACST 200.0 after the V0 declaration "
									 "and relaunch the calculation. \n *V0 ACST 200.0");
		}
	}
	else
	{
		logCritical() << "PLR3D observation (d), problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << plr3DMeas.line << ")";
		throw std::runtime_error("PLR3D observation (d), problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractUVDObs(const TLSResultsMatrices &rm, TUVD &uvdMeas)
{
	MatrixIndex XcompIDX = uvdMeas.getFirstObservationIndex();
	if (XcompIDX < rm.getResidualsVectByConst()->size())
		uvdMeas.setXVectorComponentResidual(rm.getResidualsVctrElmt(XcompIDX));
	else
	{
		logCritical() << "UVD observation (u), problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << uvdMeas.line << ")";
		throw std::runtime_error("UVD observation (u), problem during extraction residuals: observation index exceeds matrix dimensions");
	}

	MatrixIndex YcompIDX = uvdMeas.getFirstObservationIndex() + 1;
	if (YcompIDX < rm.getResidualsVectByConst()->size())
		uvdMeas.setYVectorComponentResidual(rm.getResidualsVctrElmt(YcompIDX));
	else
	{
		logCritical() << "UVD observation (v), problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << uvdMeas.line << ")";
		throw std::runtime_error("UVD observation (v), problem during extraction residuals: observation index exceeds matrix dimensions");
	}

	MatrixIndex sDistIDX = uvdMeas.getFirstObservationIndex() + 2;
	if (sDistIDX < rm.getResidualsVectByConst()->size())
		uvdMeas.setDistanceResidual(TLength(rm.getResidualsVctrElmt(sDistIDX)));
	else
	{
		logCritical() << "UVD observation (d), problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << uvdMeas.line << ")";
		throw std::runtime_error("UVD observation (d), problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractUVECObs(const TLSResultsMatrices &rm, TUVEC &uvecMeas)
{
	MatrixIndex XcompIDX = uvecMeas.getFirstObservationIndex();
	if (XcompIDX < rm.getResidualsVectByConst()->size())
		uvecMeas.setXVectorComponentResidual(rm.getResidualsVctrElmt(XcompIDX));
	else
	{
		logCritical() << "UVEC observation (u), problem during extraction residuals : observation index exceeds matrix dimensions (input line number:" << uvecMeas.line << ")";
		throw std::runtime_error("UVEC observation (u), problem during extraction residuals: observation index exceeds matrix dimensions");
	}

	MatrixIndex YcompIDX = uvecMeas.getFirstObservationIndex() + 1;
	if (YcompIDX < rm.getResidualsVectByConst()->size())
		uvecMeas.setYVectorComponentResidual(rm.getResidualsVctrElmt(YcompIDX));
	else
	{
		logCritical() << "UVEC observation (v), problem during extraction residuals : observation index exceeds matrix dimensions (input line number:" << uvecMeas.line << ")";
		throw std::runtime_error("UVEC observation (v), problem during extraction residuals: observation index exceeds matrix dimensions");
	}
}

void TLSResultsMatricesExtractor::extractLEVELObs(const TLSResultsMatrices &rm, TLEVEL &levelMeas)
{
	for (auto itDLEV(levelMeas.measDLEV.begin()); itDLEV != levelMeas.measDLEV.end(); ++itDLEV)
	{
		MatrixIndex obsUidx = itDLEV->getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVectByConst()->size())
			itDLEV->setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
		{
			logCritical()
				<< "DHOR observation under DLEV, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << itDLEV->line << ")";
			throw std::runtime_error("DHOR observation under DLEV, problem during extraction residuals: observation index exceeds matrix dimensions");
		}

		if (itDLEV->dhor)
		{ // If it is not nullptr
			obsUidx = itDLEV->dhor->getFirstObservationIndex();
			if (obsUidx < rm.getResidualsVectByConst()->size())
				itDLEV->dhor->setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
			else
			{
				logCritical() << "DLEV observation, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << itDLEV->line << ")";
				throw std::runtime_error("DLEV observation, problem during extraction residuals: observation index exceeds matrix dimensions");
			}
		}
	}
}

void TLSResultsMatricesExtractor::extractECHOROMObs(const TLSResultsMatrices &rm, TECHOROM &echoMeas)
{
	for (auto itECHO(echoMeas.measECHO.begin()); itECHO != echoMeas.measECHO.end(); ++itECHO)
	{
		MatrixIndex obsUidx = itECHO->getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVectByConst()->size())
			itECHO->setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
		{
			logCritical() << "ECHO observation, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << itECHO->line << ")";
			throw std::runtime_error("ECHO observation, problem during extraction residuals: observation index exceeds matrix dimensions");
		}
	}
}

void TLSResultsMatricesExtractor::extractECVEROMObs(const TLSResultsMatrices &rm, TECVEROM &ecveMeas)
{
	for (auto &itECVE : ecveMeas.measECVE)
	{
		MatrixIndex obsUidx = itECVE.getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVectByConst()->size())
			itECVE.setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
		{
			logCritical() << "ECVE observation, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << itECVE.line << ")";
			throw std::runtime_error("ECVE observation, problem during extraction residuals: observation index exceeds matrix dimensions");
		}
	}
}

void TLSResultsMatricesExtractor::extractECSPROMObs(const TLSResultsMatrices &rm, TECSPROM &ecspMeas)
{
	for (auto &itECSP : ecspMeas.measECSP)
	{
		MatrixIndex obsUidx = itECSP.getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVectByConst()->size())
			itECSP.setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
		{
			logCritical() << "ECSP observation, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << itECSP.line << ")";
			throw std::runtime_error("ECSP observation, problem during extraction residuals: observation index exceeds matrix dimensions");
		}
	}
}

void TLSResultsMatricesExtractor::extractORIEROMObs(const TLSResultsMatrices &rm, TORIEROM &orieMeas)
{
	for (auto &itORIE : orieMeas.measORIE)
	{
		MatrixIndex obsUidx = itORIE.getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVectByConst()->size())
			itORIE.setAngleResidual(TAngle(rm.getResidualsVctrElmt(obsUidx), TAngle::EUnits::kRadians));
		else
		{
			logCritical() << "ORIE observation, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << itORIE.line << ")";
			throw std::runtime_error("ORIE observation, problem during extraction residuals: observation index exceeds matrix dimensions");
		}
	}
}

void TLSResultsMatricesExtractor::extractDVERObs(const TLSResultsMatrices &rm, std::list<TDVER> &dver)
{
	for (auto itDVER(dver.begin()); itDVER != dver.end(); ++itDVER)
	{
		MatrixIndex obsUidx = itDVER->getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVectByConst()->size())
			itDVER->setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
		{
			logCritical() << "DVER observation, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << itDVER->line << ")";
			throw std::runtime_error("DVER observation, problem during extraction residuals: observation index exceeds matrix dimensions");
		}
	}
}

void TLSResultsMatricesExtractor::extractPDORObs(const TLSResultsMatrices &rm, TPdorObs &pdorObs)
{
	if (pdorObs.isInitialised())
	{
		MatrixIndex obsUidx = pdorObs.getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVectByConst()->size())
			pdorObs.setAngleResidual(TAngle(rm.getResidualsVctrElmt(obsUidx), TAngle::EUnits::kRadians));
		else
		{
			logCritical() << "PDOR observation, problem during extraction residuals: observation index exceeds matrix dimensions";
			throw std::runtime_error("PDOR observation, problem during extraction residuals: observation index exceeds matrix dimensions");
		}
	}
}

void TLSResultsMatricesExtractor::extractRADIObs(const TLSResultsMatrices &rm, std::list<TRADI> &radi)
{
	for (auto &itRADI : radi)
	{
		MatrixIndex obsUidx = itRADI.getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVectByConst()->size())
			itRADI.setResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
		{
			logCritical() << "RADI observation, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << itRADI.line << ")";
			throw std::runtime_error("RADI observation, problem during extraction residuals: observation index exceeds matrix dimensions");
		}
	}
}

void TLSResultsMatricesExtractor::extractOBSXYZObs(const TLSResultsMatrices &rm, std::list<TOBSXYZ> &obsxyz)
{
	for (auto &itOBSXYZ : obsxyz)
	{
		MatrixIndex obsUidx = itOBSXYZ.getFirstObservationIndex();
		if (obsUidx + 2 < rm.getResidualsVectByConst()->size())
		{
			itOBSXYZ.setXResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
			itOBSXYZ.setYResidual(TLength(rm.getResidualsVctrElmt(obsUidx + 1)));
			itOBSXYZ.setZResidual(TLength(rm.getResidualsVctrElmt(obsUidx + 2)));
		}
		else
		{
			logCritical() << "OBSXYZ observation, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << itOBSXYZ.line << ")";
			throw std::runtime_error("OBSXYZ observation, problem during extraction residuals: observation index exceeds matrix dimensions");
		}
	}
}

void TLSResultsMatricesExtractor::extractINCLYROMObs(const TLSResultsMatrices &rm, TINCLYROM &inclyMeas)
{
	for (auto &itINCLY : inclyMeas.measINCLY)
	{
		MatrixIndex obsUidx = itINCLY.getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVectByConst()->size())
			itINCLY.setAngleResidual(TAngle(rm.getResidualsVctrElmt(obsUidx)));
		else
		{
			logCritical() << "INCLY observation, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << itINCLY.line << ")";
			throw std::runtime_error("INCLY observation, problem during extraction residuals: observation index exceeds matrix dimensions");
		}
	}
}

void TLSResultsMatricesExtractor::extractECWSROMObs(const TLSResultsMatrices &rm, TECWSROM &ecwsMeas)
{
	for (auto itECWS(ecwsMeas.measECWS.begin()); itECWS != ecwsMeas.measECWS.end(); ++itECWS)
	{
		MatrixIndex obsUidx = itECWS->getFirstObservationIndex();
		if (obsUidx < rm.getResidualsVectByConst()->size())
			itECWS->setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)));
		else
		{
			logCritical() << "ECWS observation, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << itECWS->line << ")";
			throw std::runtime_error("ECWS observation, problem during extraction residuals: observation index exceeds matrix dimensions");
		}
	}
}

void TLSResultsMatricesExtractor::extractECWIROMObs(const TLSResultsMatrices &rm, TECWIROM &ecwiMeas)
{
	for (auto itECWI(ecwiMeas.measECWI.begin()); itECWI != ecwiMeas.measECWI.end(); ++itECWI)
	{
		MatrixIndex obsUidx = itECWI->getFirstObservationIndex();
		if (obsUidx + 1 < rm.getResidualsVectByConst()->size())
		{
			itECWI->setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx)), EECWIDistances::kX);
			itECWI->setDistanceResidual(TLength(rm.getResidualsVctrElmt(obsUidx + 1)), EECWIDistances::kZ);
		}
		else
		{
			logCritical() << "ECWI observation, problem during extraction residuals: observation index exceeds matrix dimensions (input line number:" << itECWI->line << ")";
			throw std::runtime_error("ECWI observation, problem during extraction residuals: observation index exceeds matrix dimensions");
		}
	}
}

///////////////////////////////////////////////////////////////
// Methods relative to the adjustable objects
///////////////////////////////////////////////////////////////

bool TLSResultsMatricesExtractor::extractPointParams(const TLSResultsMatrices &rm, const TReal convCrit)
{
	logDebug() << "Extract parameters of the adjustable points from the calculated matrices";
	bool critNotExceeded = true;
	int nParamsOutsideCriteria = 0;
	int nParamsTotal = 0;

	for (auto &point : fDataSet->getPoints())
	{
		if (point.hasVariable())
		{
			for (int unknIdx = point.getFirstUidx(); unknIdx <= point.getLastUidx(); ++unknIdx)
			{
				if (unknIdx >= rm.getSolutionVectByConst()->size())
				{
					logCritical() << "Extract parameters of the adjustable points from the calculated matrices: Unknown index of point" << point.getName() << " exceeds matrix dimensions!";
					throw std::runtime_error("Unknown index of an point: " + point.getName() + " exceeds matrix dimensions!");
				}

				TReal correction = rm.getSolutionVctrElmt(unknIdx);
				point.setCorrection(unknIdx, correction);

				nParamsTotal++;
				if (fabsq(correction) > convCrit)
				{
					nParamsOutsideCriteria++;
					critNotExceeded = false;
				}
			}
		}
	}
	logDebug() << "Checking converging criteria on point parameters: " << nParamsOutsideCriteria << "of" << nParamsTotal << "coordinates outside criteria (" << convCrit << ")";

	return critNotExceeded;
}

bool TLSResultsMatricesExtractor::extractAngleParams(const TLSResultsMatrices &rm, const TReal convCrit)
{
	logDebug() << "Extract parameters of the adjustable angles from the calculated matrices";

	bool critNotExceeded = true;

	for (auto &angle : fDataSet->getAngles())
	{
		if (!angle.isFixed())
		{
			MatrixIndex unknIdx = angle.getFirstUidx(); // first=last only one unknown fo angle class

			if (unknIdx >= rm.getSolutionVectByConst()->size())
				throw std::runtime_error("Unknown index of an angle: " + angle.getName() + " exceeds matrix dimensions!");

			TReal correctionVal = rm.getSolutionVctrElmt(unknIdx);
			angle.setCorrection(unknIdx, correctionVal);
			if (fabsq(correctionVal) > convCrit)
				critNotExceeded = false;
		}
	}
	return critNotExceeded;
}

bool TLSResultsMatricesExtractor::extractPlaneParams(const TLSResultsMatrices &rm, const TReal convCrit)
{
	logDebug() << "Extract parameters of the adjustable planes from the calculated matrices";

	bool critNotExceeded = true;

	for (auto &plane : fDataSet->getPlanes())
	{
		if (plane.hasVariable())
		{
			for (int unknIdx = plane.getFirstUidx(); unknIdx <= plane.getLastUidx(); unknIdx++)
			{
				if (unknIdx >= rm.getSolutionVectByConst()->size())
					throw std::runtime_error("Unknown index of a plane: " + plane.getName() + " exceeds matrix dimensions!");

				TReal correction = rm.getSolutionVctrElmt(unknIdx);
				plane.setCorrection(unknIdx, correction);
				if (fabsq(correction) > convCrit)
					critNotExceeded = false;
			}
		}
	}
	return critNotExceeded;
}

bool TLSResultsMatricesExtractor::extractLineParams(const TLSResultsMatrices &rm, const TReal convCrit)
{
	logDebug() << "Extract parameters of the adjustable lines from the calculated matrices";

	bool critNotExceeded = true;

	for (auto &line : fDataSet->getLines())
	{
		if (!line.isFixed())
		{
			for (int unknIdx = line.getFirstUidx(); unknIdx <= line.getLastUidx(); unknIdx++)
			{
				if (unknIdx >= rm.getSolutionVectByConst()->size())
					throw std::runtime_error("Unknown index of a plane: " + line.getName() + " exceeds matrix dimensions!");

				TReal correction = rm.getSolutionVctrElmt(unknIdx);
				line.setCorrection(unknIdx, correction);
				if (fabsq(correction) > convCrit)
					critNotExceeded = false;
			}
		}
	}
	return critNotExceeded;
}

bool TLSResultsMatricesExtractor::extractLengthParams(const TLSResultsMatrices &rm, const TReal convCrit)
{
	logDebug() << "Extract parameters of the adjustable lengths from the calculated matrices";

	bool critNotExceeded = true;
	for (auto &length : fDataSet->getLength())
	{
		if (!length.isFixed())
		{
			MatrixIndex unknIdx = length.getFirstUidx(); // first=last only one unknown fo angle class

			if (unknIdx >= rm.getSolutionVectByConst()->size())
				throw std::runtime_error("Unknown index of a scalar: " + length.getName() + " exceeds matrix dimensions!");

			TReal correction = rm.getSolutionVctrElmt(unknIdx);
			length.setCorrection(unknIdx, correction);
			if (fabsq(correction) > convCrit)
				critNotExceeded = false;
		}
	}
	return critNotExceeded;
}

bool TLSResultsMatricesExtractor::extractTransformationParams(const TLSResultsMatrices &rm, const TReal convCrit)
{
	logDebug() << "Extract parameters of the adjustable transformations from the calculated matrices";

	bool critNotExceeded = true;

	for (auto it(fDataSet->getTree().begin()); it != fDataSet->getTree().end(); ++it)
	{
		auto &trafo(it.node->data.get()->frame);

		if (trafo.hasVariable())
		{
			for (int unknIdx = trafo.getFirstUidx(); unknIdx <= trafo.getLastUidx(); unknIdx++)
			{
				if (unknIdx >= rm.getSolutionVectByConst()->size())
					throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");

				TReal correction = rm.getSolutionVctrElmt(unknIdx);
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
bool TLSResultsMatricesExtractor::lastIteration() const
{
	return fLastIteration;
}

//////////////////////////////////////////////////////////////////////////////////
// extract var. and covar. params
//////////////////////////////////////////////////////////////////////////////////
void TLSResultsMatricesExtractor::extractFullCovar(const TLSResultsMatrices &rm)
{
	fDataSet->setCovMat(*rm.getUnkCovarMtrx());
}

void TLSResultsMatricesExtractor::extractPointVarCovar(const TLSResultsMatrices &rm)
{
	for (auto &point : fDataSet->getPoints())
	{
		Eigen::Matrix3d fullCovar = Eigen::Matrix3d::Zero();
		if (point.hasVariable())
		{
			// Filling the full covariance matrix
			std::vector<int> relUnkIdx = point.getRelativeUnknIndices();
			const TSparseMatrix *covMat = rm.getUnkCovarMtrxByConst();
			int dimPoint = point.getNumUnkn();
			if (dimPoint > 0)
			{
				int firstIdx = point.getFirstUidx();
				fullCovar(relUnkIdx, relUnkIdx) = (covMat->block(firstIdx, firstIdx, dimPoint, dimPoint)).toDense();
			}
		}
		// Calas will have 0 as covariance matrix
		point.setCovarianceMatrix(fullCovar);
	}
}

void TLSResultsMatricesExtractor::extractAngleVar(const TLSResultsMatrices &rm)
{
	for (auto &angle : fDataSet->getAngles())
	{
		if (!angle.isFixed())
		{
			// Filling standard deviations (estimated precision)
			int unknIdx = angle.getFirstUidx();

			if (rm.getUnkCovarMtrxByConst()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				angle.setEstimatedPrecision(unknIdx, -1);
			else if (unknIdx >= rm.getUnkCovarMtrxByConst()->rows())
				throw std::runtime_error("Unknown index of an angle: " + angle.getName() + " exceeds matrix dimensions!");
			else
				angle.setEstimatedPrecision(unknIdx, sqrtq(rm.getUnkCovarMtrxElmt(unknIdx, unknIdx))); // Set estimated precision in RADs
		}
	}
}

void TLSResultsMatricesExtractor::extractLengthVar(const TLSResultsMatrices &rm)
{
	for (auto &length : fDataSet->getLength())
	{
		if (!length.isFixed())
		{
			// Filling standard deviations (estimated precision)
			int unknIdx = length.getFirstUidx();

			if (rm.getUnkCovarMtrxByConst()->rows() == 0)
				// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
				// -> Sets the values arbitrary to -1
				length.setEstimatedPrecision(unknIdx, -1);
			else if (unknIdx >= rm.getUnkCovarMtrxByConst()->rows())
				throw std::runtime_error("Unknown index of a scalar: " + length.getName() + " exceeds matrix dimensions!");
			else
				length.setEstimatedPrecision(unknIdx, sqrtq(rm.getUnkCovarMtrxElmt(unknIdx, unknIdx))); // Set estimated precision in METRES
		}
	}
}

void TLSResultsMatricesExtractor::extractPlaneVarCovar(const TLSResultsMatrices &rm)
{
	for (auto &plane : fDataSet->getPlanes())
	{
		if (plane.hasVariable())
		{
			// Filling standard deviations (estimated precision)
			for (int unknIdx = plane.getFirstUidx(); unknIdx <= plane.getLastUidx(); ++unknIdx)
			{
				if (rm.getUnkCovarMtrxByConst()->rows() == 0)
					// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
					// -> Sets the values arbitrary to -1
					plane.setEstimatedPrecision(unknIdx, -1);
				else if (unknIdx >= rm.getUnkCovarMtrxByConst()->rows())
					throw std::runtime_error("Unknown index of a plane: " + plane.getName() + " exceeds matrix dimensions!");
				else
				{
					TReal sigma = sqrtq(rm.getUnkCovarMtrxElmt(unknIdx, unknIdx));
					plane.setEstimatedPrecision(unknIdx, sigma); // Store standard deviations in meters (reference point distance ) or radians (angles)
				}
			}

			/* Eventually store covarinace between angles if needed in the future.*/
		}
	}
}

void TLSResultsMatricesExtractor::extractLineVarCovar(const TLSResultsMatrices &rm)
{
	for (auto &line : fDataSet->getLines())
	{
		if (!line.isFixed())
		{
			// Filling standard deviations (estimated precision)
			for (int unknIdx = line.getFirstUidx(); unknIdx <= line.getLastUidx(); ++unknIdx)
			{
				if (rm.getUnkCovarMtrxByConst()->rows() == 0)
					// The unknown variance-covariance matrix has not been well calculated (not inverted for instance)
					// -> Sets the values arbitrary to -1
					line.setLineVectorEstimatedPrecision(unknIdx, -1);
				else if (unknIdx >= rm.getUnkCovarMtrxByConst()->rows())
					throw std::runtime_error("Unknown index of a plane: " + line.getName() + " exceeds matrix dimensions!");
				else
				{
					TReal sigma = sqrtq(rm.getUnkCovarMtrxElmt(unknIdx, unknIdx));
					line.setLineVectorEstimatedPrecision(unknIdx, sigma); // Store standard deviations in meters (reference point distance ) or radians (angles)
				}
			}

			/* Eventually store covariance between angles if needed in the future.*/
		}
	}
}

void TLSResultsMatricesExtractor::extractTransformationVarCovar(const TLSResultsMatrices &rm)
{
	for (auto it(fDataSet->getTree().begin()); it != fDataSet->getTree().end(); ++it)
	{
		auto &trafo(it.node->data.get()->frame);
		Eigen::Matrix<double, 7, 7> fullCovar = Eigen::Matrix<double, 7, 7>::Zero();
		std::vector<int> relUnkIdx = trafo.getRelativeUnknIndices();
		const TSparseMatrix *covMat = rm.getUnkCovarMtrxByConst();
		int dimTrafo = trafo.getNumUnkn();
		if (dimTrafo > 0)
		{
			int firstIdx = trafo.getFirstUidx();
			fullCovar(relUnkIdx, relUnkIdx) = (covMat->block(firstIdx, firstIdx, dimTrafo, dimTrafo)).toDense();
		}
		trafo.setCovar(fullCovar);
	}
}

bool TLSResultsMatricesExtractor::extractRelError(const TLSResultsMatrices &rm)
{
	try
	{
		// point tuples
		// copy rel error object, compute, store in TLGCData
		// no reference here because the erels in the config are only initialized with names and do not contain the computed error data
		// the finished erel objects are only stored in the TLGCData object, they are not part of the configuration
		for (auto erelPointTuple : fDataSet->getConfig().fRelErrors.points)
		{
			erelPointTuple.computeErel(fDataSet);
			fDataSet->getRelError().points.push_back(erelPointTuple);
		}
		// frame tuples
		for (auto erelFrameTuple : fDataSet->getConfig().fRelErrors.frames)
		{
			erelFrameTuple.computeErel(fDataSet);
			fDataSet->getRelError().frames.push_back(erelFrameTuple);
		}
	}
	catch (std::exception const &excp)
	{
		fDataSet->getFileLogger() << TFileLogger::e_logType::LOG_ERROR << excp.what();
		return false;
	}

	return true;
}
