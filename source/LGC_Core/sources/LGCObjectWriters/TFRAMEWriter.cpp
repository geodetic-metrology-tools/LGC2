#include "TFRAMEWriter.h"

#include <TPointTransformer.h>

#include "TAStreamFormatter.h"
#include "TCAMWriter.h"
#include "TDirectTransformation.h"
#include "TEDMWriter.h"
#include "THLSRWriter.h"
#include "TINCLWriter.h"
#include "TInverseTransformation.h"
#include "TLEVELWriter.h"
#include "TLOR2LOR.h"
#include "TMeasurements.h"
#include "TOtherMeasurementsWriter.h"
#include "TPointConverter.h"
#include "TSCALEWriter.h"
#include "TSpatialStatus.h"
#include "TTSTNWriter.h"
#include "TWPSRWriter.h"
#include "TXYH2CCS.h"

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
TFRAMEWriter::TFRAMEWriter(TAStreamFormatter &stream, const TLGCData *data) : TALGCObjectWriter(stream), fProjectData(data)
{
	// Separate
	for (AdjPointIter it(fProjectData->getPoints().cbegin()); it != fProjectData->getPoints().cend(); ++it)
	{
		if (it->getSpatialStatus() == TSpatialStatus::kCala)
			pointCALA.emplace_back(it);
		if (it->getSpatialStatus() == TSpatialStatus::kVxyz)
			pointVXYZ.emplace_back(it);
		if (it->getSpatialStatus() == TSpatialStatus::kVxy)
			pointVXY.emplace_back(it);
		if (it->getSpatialStatus() == TSpatialStatus::kVxz)
			pointVXZ.emplace_back(it);
		if (it->getSpatialStatus() == TSpatialStatus::kVyz)
			pointVYZ.emplace_back(it);
		if (it->getSpatialStatus() == TSpatialStatus::kVz)
			pointVZ.emplace_back(it);
	}
}

TFRAMEWriter::~TFRAMEWriter()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void TFRAMEWriter::writeFRAMEAll(TDataTreeIterator frameIt)
{
	TAStreamFormatter *stream = getStream();
	std::string TABs = stream->getCurrSpace();
	stream->setTreeDepth((int)frameIt->get()->ID.size()
		- 1); // Size of the ID is equal to the depth in the tree, which corresponds to the number o TABs to be used in formatting. Zero TABs for ROOT (depth 1).

	writeFRAMEHeader(frameIt->get()->frame.getName(), frameIt->get()->ID);

	if (frameIt->get()->frame.getName() != "ROOT")
		writeFRAMEDefinition(frameIt->get()->frame);

	writePoints(frameIt);

	// legend of tables for the points in ROOT
	if (frameIt->get()->isROOTNode())
		(*stream) << TABs + "SFP = Sub-Frame Point; * = TRUE" << endl;
	(*stream) << endl << endl;

	// Start to write the measurements
	TTSTNWriter tstnWriter(*stream, fProjectData->getConfig().histo.isActive());
	tstnWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TCAMWriter camWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TEDMWriter edmWriter(*stream, fProjectData->getConfig().histo.isActive());
	edmWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TSCALEWriter scaleWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TLEVELWriter levelWriter(*stream, fProjectData->getConfig().histo.isActive());
	levelWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TOtherMeasurementWriter otherMeasWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TINCLWriter inclWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	THLSRWriter hlsrWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TWPSRWriter wpsrWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter

	auto &tmeas = (*frameIt)->measurements;

	// If PDOR
	if (tmeas.fPDOR.isInitialised())
		otherMeasWriter.writePDORResults(tmeas.fPDOR);

	// Summary
	if (frameIt->get()->isROOTNode())
	{
		initialiseAllObsSummaries();
		(*stream) << TABs << "*** RESUME DES MESURES ***" << endl << endl;
		writeMeasurementsSummaryRootOnly();
		if (fProjectData->getConfig().histo.isActive())
			writeHistogrammeRootOnly();
	}

	// Measures
	(*stream) << endl << endl << TABs << "*** MESURES ***" << endl << endl;
	for (auto &itTSTN : tmeas.fTSTN)
		tstnWriter.writeTSTNResults(itTSTN);

	for (auto &itCAM : tmeas.fCAM)
		camWriter.writeCAMResults(itCAM);

	for (auto &itLEVEL : tmeas.fLEVEL)
		levelWriter.writeLEVELResults(itLEVEL);

	// No instrument for DVER, so no loop to have each instrument.
	if (!tmeas.fDVER.empty())
	{
		(*stream) << endl;
		otherMeasWriter.writeDVERResults(tmeas.fDVER);
	}

	for (auto &itECHO : tmeas.fECHO)
		scaleWriter.writeECHOResults(itECHO);

	for (auto &itECVE : tmeas.fECVE)
		scaleWriter.writeECVEResults(itECVE);

	for (auto &itECSP : tmeas.fECSP)
		scaleWriter.writeECSPResults(itECSP);

	for (auto &itORIE : tmeas.fORIE)
	{
		(*stream) << endl;
		otherMeasWriter.writeORIEResults(itORIE.measORIE, *itORIE.instrumentPos);
	}

	if (!tmeas.fRADI.empty())
	{
		(*stream) << endl;
		otherMeasWriter.writeRADIResults(tmeas.fRADI);
	}

	if (!tmeas.fOBSXYZ.empty())
	{
		(*stream) << endl;
		otherMeasWriter.writeOBSXYZResults(tmeas.fOBSXYZ);
	}

	otherMeasWriter.writePointSigmaResults(fProjectData->getPoints(), frameIt);

	for (auto &itEDM : tmeas.fEDM)
		edmWriter.writeEDMResults(itEDM);

	for (auto &itINCLY : tmeas.fINCLY)
		inclWriter.writeINCLYResults(itINCLY);

	for (auto &itROLLY : tmeas.fROLLY)
		inclWriter.writeROLLYResults(itROLLY);

	for (auto &itECWS : tmeas.fECWS)
		hlsrWriter.writeECWSResults(itECWS);

	for (auto &itECWI : tmeas.fECWI)
		wpsrWriter.writeECWIResults(itECWI);
}

/// write measurements summary in the root only
void TFRAMEWriter::writeHistogrammeRootOnly()
{
	TAStreamFormatter *stream = getStream();
	std::string TABs = stream->getCurrSpace();
	// int				nameWidth = getNameWidth();

	// Start to write the measurements
	TTSTNWriter tstnWriter(*stream, fProjectData->getConfig().histo.isActive());
	tstnWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TCAMWriter camWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TSCALEWriter scaleWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TLEVELWriter levelWriter(*stream, fProjectData->getConfig().histo.isActive());
	levelWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TOtherMeasurementWriter otherMeasWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TINCLWriter inclWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	THLSRWriter hlsrWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TWPSRWriter wpsrWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter

	// TSTN
	// ANGL
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kANGL) >= 5)
	{
		(*stream) << endl;
		tstnWriter.writeHisto(TLGCObsSummary::merge(allANGLSummaries_), "ANGL");
	}

	// ZEND
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kZEND) >= 5)
	{
		(*stream) << endl;
		tstnWriter.writeHisto(TLGCObsSummary::merge(allZENDSummaries_), "ZEND");
	}

	// DIST
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDIST) >= 5)
	{
		(*stream) << endl;
		tstnWriter.writeHisto(TLGCObsSummary::merge(allDISTSummaries_), "DIST");
	}

	// DHOR
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDHOR) >= 5)
	{
		(*stream) << endl;
		tstnWriter.writeHisto(TLGCObsSummary::merge(allDHORSummaries_), "DHOR");
	}

	// PLR3D
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kPLR3D) >= 5)
	{
		(*stream) << endl;
		tstnWriter.writeHisto(TLGCObsSummary::merge(allPlrANGLSummaries_), "PLR3D: ANGL");
		(*stream) << endl;
		tstnWriter.writeHisto(TLGCObsSummary::merge(allPlrZENDSummaries_), "PLR3D: ZEND");
		(*stream) << endl;
		tstnWriter.writeHisto(TLGCObsSummary::merge(allPlrDISTSummaries_), "PLR3D: DIST");
	}

	// ECTH
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECTH) >= 5)
	{
		(*stream) << endl;
		tstnWriter.writeHisto(TLGCObsSummary::merge(allECTHSummaries_), "ECTH");
	}

	// ECDIR
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECDIR) >= 5)
	{
		(*stream) << endl;
		tstnWriter.writeHisto(TLGCObsSummary::merge(allECDIRSummaries_), "ECDIR");
	}

	// BCAM
	// UVD
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kUVD) >= 5)
	{
		(*stream) << endl;
		scaleWriter.writeHisto(TLGCObsSummary::merge(allUvdXSummaries_), "UVD: XVEC");
		(*stream) << endl;
		scaleWriter.writeHisto(TLGCObsSummary::merge(allUvdYSummaries_), "UVD: YVEC");
		(*stream) << endl;
		scaleWriter.writeHisto(TLGCObsSummary::merge(allUvdDSummaries_), "UVD: DIST");
	}

	// UVEC
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kUVEC) >= 5)
	{
		(*stream) << endl;
		scaleWriter.writeHisto(TLGCObsSummary::merge(allUvecXSummaries_), "UVEC: XVEC");
		(*stream) << endl;
		scaleWriter.writeHisto(TLGCObsSummary::merge(allUvecYSummaries_), "UVEC: YVEC");
	}

	// DLEV
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDLEV) >= 5)
	{
		(*stream) << endl;
		levelWriter.writeHisto(TLGCObsSummary::merge(allDLEVSummaries_), "DLEV");
		if (!allDlevDHORSummaries_.empty())
		{
			(*stream) << "\n";
			levelWriter.writeHisto(TLGCObsSummary::merge(allDlevDHORSummaries_), "DLEVDHOR");
		}
	}

	// DVER
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDVER) >= 5)
	{
		(*stream) << endl;
		otherMeasWriter.writeHisto(TLGCObsSummary::merge(allDVERSummaries_), "DVER");
	}

	// ECHO
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECHO) >= 5)
	{
		(*stream) << endl;
		scaleWriter.writeHisto(TLGCObsSummary::merge(allECHOSummaries_), "ECHO");
	}

	// DSPT
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDSPT) >= 5)
	{
		(*stream) << endl;
		tstnWriter.writeHisto(TLGCObsSummary::merge(allDSPTSummaries_), "DSPT");
	}

	// ECVE
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECVE) >= 5)
	{
		(*stream) << endl;
		scaleWriter.writeHisto(TLGCObsSummary::merge(allECVESummaries_), "ECVE");
	}

	// ECSP
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECSP) >= 5)
	{
		(*stream) << endl;
		scaleWriter.writeHisto(TLGCObsSummary::merge(allECSPSummaries_), "ECSP");
	}

	// ORIE
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kORIE) >= 5)
	{
		(*stream) << endl;
		otherMeasWriter.writeHisto(TLGCObsSummary::merge(allORIESummaries_), "ORIE");
	}

	// RADI
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kRADI) >= 5)
	{
		(*stream) << endl;
		otherMeasWriter.writeHisto(TLGCObsSummary::merge(allRADISummaries_), "RADI");
	}

	// OBSXYZ
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kOBSXYZ) >= 5)
	{
		(*stream) << endl;
		otherMeasWriter.writeHisto(TLGCObsSummary::merge(allObsxyzXSummaries_), "OBSXYZ: X");
		(*stream) << endl;
		otherMeasWriter.writeHisto(TLGCObsSummary::merge(allObsxyzYSummaries_), "OBSXYZ: Y");
		(*stream) << endl;
		otherMeasWriter.writeHisto(TLGCObsSummary::merge(allObsxyzZSummaries_), "OBSXYZ: Z");
	}
	// INCLY - Write histogram for inclinometer Y-axis measurements if enough data points exist
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kINCLY) >= 5)
	{
		(*stream) << endl;
		inclWriter.writeHisto(TLGCObsSummary::merge(allINCLYSummaries_), "INCLY");
	}

	// ROLLY - Write histogram for roll Y-axis measurements using same inclinometer writer
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kROLLY) >= 5)
	{
		(*stream) << endl;
		inclWriter.writeHisto(TLGCObsSummary::merge(allROLLYSummaries_), "ROLLY");
	}

	////ECWS
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECWS) >= 5)
	{
		(*stream) << endl;
		hlsrWriter.writeHisto(TLGCObsSummary::merge(allECWSSummaries_), "ECWS");
	}

	// ECWI
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECWI) >= 5)
	{
		(*stream) << endl;
		wpsrWriter.writeHisto(TLGCObsSummary::merge(allEcwiXSummaries_), "ECWI: X");
		(*stream) << endl;
		wpsrWriter.writeHisto(TLGCObsSummary::merge(allEcwiZSummaries_), "ECWI: Z");
	}
}

void TFRAMEWriter::initialiseAllObsSummaries()
{
	allRADISummaries_.clear();
	allPlrANGLSummaries_.clear();
	allPlrZENDSummaries_.clear();
	allPlrDISTSummaries_.clear();
	allANGLSummaries_.clear();
	allZENDSummaries_.clear();
	allDISTSummaries_.clear();
	allDHORSummaries_.clear();
	allECTHSummaries_.clear();
	allECDIRSummaries_.clear();
	allDVERSummaries_.clear();
	allUvdXSummaries_.clear();
	allUvdYSummaries_.clear();
	allUvdDSummaries_.clear();
	allUvecXSummaries_.clear();
	allUvecYSummaries_.clear();
	allDSPTSummaries_.clear();
	allDLEVSummaries_.clear();
	allDlevDHORSummaries_.clear();
	allORIESummaries_.clear();
	allECHOSummaries_.clear();
	allECVESummaries_.clear();
	allECSPSummaries_.clear();
	allINCLYSummaries_.clear();
	allROLLYSummaries_.clear();
	allECWSSummaries_.clear();
	allEcwiXSummaries_.clear();
	allEcwiZSummaries_.clear();
	allObsxyzXSummaries_.clear();
	allObsxyzYSummaries_.clear();
	allObsxyzZSummaries_.clear();

	// Tteration through the tree nodes
	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++)
	{
		auto &tmeas = (*itTree)->measurements;
		if (tmeas.fINCLY.size() > 0)
			for (auto &itINCLY : tmeas.fINCLY)
			{
				allINCLYSummaries_.push_back(&itINCLY.getINCLYObsSummary(itINCLY.positionInTree.node->data->frame.getName()));
			}

		if (tmeas.fROLLY.size() > 0)
			for (auto &itROLLY : tmeas.fROLLY)
			{
				allROLLYSummaries_.push_back(&itROLLY.getROLLYObsSummary(itROLLY.positionInTree.node->data->frame.getName()));
			}

		// TOCHECK
		if (tmeas.fOBSXYZ.size() > 0)
		{
			allObsxyzXSummaries_.push_back(&tmeas.getOBSXYZObsSummary(itTree.node->data->frame.getName()).obsXObsSum);
			allObsxyzYSummaries_.push_back(&tmeas.getOBSXYZObsSummary(itTree.node->data->frame.getName()).obsYObsSum);
			allObsxyzZSummaries_.push_back(&tmeas.getOBSXYZObsSummary(itTree.node->data->frame.getName()).obsZObsSum);
		}

		// TOCHECK
		if (tmeas.fDVER.size() > 0)
			allDVERSummaries_.push_back(&tmeas.getDVERObsSummary(itTree.node->data->frame.getName()));

		// TOCHECK
		if (tmeas.fRADI.size() > 0)
			allRADISummaries_.push_back(&tmeas.getRADIObsSummary(itTree.node->data->frame.getName()));

		// TOCHECK
		if (tmeas.fORIE.size() > 0)
			for (auto &itORIE : tmeas.fORIE)
				allORIESummaries_.push_back(&itORIE.getORIEObsSummary(itORIE.instrumentPos->getName()));

		if (tmeas.fECSP.size() > 0)
			for (auto &itECSP : tmeas.fECSP)
				allECSPSummaries_.push_back(&itECSP.getECSPObsSummary(itECSP.romName));

		if (tmeas.fECVE.size() > 0)
			for (auto &itECVE : tmeas.fECVE)
				allECVESummaries_.push_back(&itECVE.getECVEObsSummary(itECVE.fMeasuredLine->getName()));

		if (tmeas.fEDM.size() > 0)
			for (auto &itEDM : tmeas.fEDM)
				allDSPTSummaries_.push_back(&itEDM.getDSPTObsSummary(itEDM.instrumentPos->getName()));

		if (tmeas.fECHO.size() > 0)
			for (auto &itECHO : tmeas.fECHO)
				allECHOSummaries_.push_back(&itECHO.getECHOObsSummary(itECHO.fMeasuredPlane->getReferencePoint()->getName()));

		if (tmeas.fLEVEL.size() > 0)
			for (auto &itLEVEL : tmeas.fLEVEL)
			{
				allDLEVSummaries_.push_back(&itLEVEL.getDLEVObsSummary(itLEVEL.fMeasuredPlane->getReferencePoint()->getName()));
				if (itLEVEL.hasDHOR)
					allDlevDHORSummaries_.push_back(&itLEVEL.getDHORObsSummary(itLEVEL.fMeasuredPlane->getReferencePoint()->getName()));
			}

		if (tmeas.fECWS.size() > 0)
			for (auto &itECWS : tmeas.fECWS)
				allECWSSummaries_.push_back(&itECWS.getECWSObsSummary(itECWS.fMeasuredWSHeight->getName()));

		if (tmeas.fECWI.size() > 0)
			for (auto &itECWI : tmeas.fECWI)
			{
				allEcwiXSummaries_.push_back(&itECWI.getECWIObsSummary(itECWI.romName).xObsSum);
				allEcwiZSummaries_.push_back(&itECWI.getECWIObsSummary(itECWI.romName).zObsSum);
			}

		if (tmeas.fCAM.size() > 0)
		{
			for (auto &itCAM : tmeas.fCAM)
			{
				std::string stationName = itCAM.instrumentPos->getName();
				if (itCAM.measUVD.size() > 0)
				{
					allUvdXSummaries_.push_back(&itCAM.getUVDObsSummary(stationName).xVectorCompObsSum);
					allUvdYSummaries_.push_back(&itCAM.getUVDObsSummary(stationName).yVectorCompObsSum);
					allUvdDSummaries_.push_back(&itCAM.getUVDObsSummary(stationName).distObsSum);
				}
				if (itCAM.measUVEC.size() > 0)
				{
					allUvecXSummaries_.push_back(&itCAM.getUVECObsSummary(stationName).xVectorCompObsSum);
					allUvecYSummaries_.push_back(&itCAM.getUVECObsSummary(stationName).yVectorCompObsSum);
				}
			}
		}

		if (tmeas.fTSTN.size() > 0)
		{
			for (auto &itTSTN : tmeas.fTSTN)
			{
				std::string stationName = itTSTN->instrumentPos->getName();
				for (auto &itrom : itTSTN->roms)
				{
					if (itrom->measANGL.size() > 0)
						allANGLSummaries_.push_back(&itrom->getANGLObsSummary(stationName));
					if (itrom->measZEND.size() > 0)
						allZENDSummaries_.push_back(&itrom->getZENDObsSummary(stationName));

					if (itrom->measDIST.size() > 0)
						allDISTSummaries_.push_back(&itrom->getDISTObsSummary(stationName));

					if (itrom->measDHOR.size() > 0)
						allDHORSummaries_.push_back(&itrom->getDHORObsSummary(stationName));

					if (itrom->measPLR3D.size() > 0)
					{
						allPlrANGLSummaries_.push_back(&itrom->getPLR3DObsSummary(stationName).anglObsSum);
						allPlrZENDSummaries_.push_back(&itrom->getPLR3DObsSummary(stationName).zendObsSum);
						allPlrDISTSummaries_.push_back(&itrom->getPLR3DObsSummary(stationName).distObsSum);
					}
					if (itrom->measECTH.size() > 0)
						allECTHSummaries_.push_back(&itrom->getECTHObsSummary(stationName));

					if (itrom->measECDIR.size() > 0)
						allECDIRSummaries_.push_back(&itrom->getECDIRObsSummary(stationName));
				}
			}
		}
	}
}

/// write measurements Histogram in the root only
void TFRAMEWriter::writeMeasurementsSummaryRootOnly()
{
	TAStreamFormatter *stream = getStream();
	std::string TABs = stream->getCurrSpace();
	int nameWidth = getNameWidth();

	// Start to write the measurements
	TTSTNWriter tstnWriter(*stream, fProjectData->getConfig().histo.isActive());
	tstnWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TCAMWriter camWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TSCALEWriter scaleWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TLEVELWriter levelWriter(*stream, fProjectData->getConfig().histo.isActive());
	levelWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TOtherMeasurementWriter otherMeasWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TEDMWriter edmWriter(*stream, fProjectData->getConfig().histo.isActive());
	edmWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TINCLWriter inclWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	THLSRWriter hlsrWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TWPSRWriter wpsrWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter

	// ANGL
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kANGL) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ANGL");
		(*stream) << endl;
		tstnWriter.writeANGLHeaderSynthesis();
		tstnWriter.writeDefResultsSynthesis(allANGLSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		tstnWriter.writeAngleResultsSummary(TLGCObsSummary::merge(allANGLSummaries_), TABs);
	}

	// ZEND
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kZEND) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ZEND");
		(*stream) << endl;
		tstnWriter.writeANGLHeaderSynthesis();
		tstnWriter.writeDefResultsSynthesis(allZENDSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		tstnWriter.writeAngleResultsSummary(TLGCObsSummary::merge(allZENDSummaries_), TABs);
	}

	// DIST
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDIST) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "DIST");
		(*stream) << endl;
		tstnWriter.writeDISTHeaderSynthesis();
		tstnWriter.writeDefResultsSynthesis(allDISTSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		tstnWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allDISTSummaries_), TABs);
	}

	// DHOR
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDHOR) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "DHOR");
		(*stream) << endl;
		tstnWriter.writeDISTHeaderSynthesis();
		tstnWriter.writeDefResultsSynthesis(allDHORSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		tstnWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allDHORSummaries_), TABs);
	}

	// PLR3D : change of format w.r.t. previous version to keep some consistency
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kPLR3D) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "PLR3D: ANGL");
		(*stream) << endl;
		tstnWriter.writeANGLHeaderSynthesis();
		tstnWriter.writeDefResultsSynthesis(allPlrANGLSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		tstnWriter.writeAngleResultsSummary(TLGCObsSummary::merge(allPlrANGLSummaries_), TABs);

		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "PLR3D: ZEND");
		(*stream) << endl;
		tstnWriter.writeANGLHeaderSynthesis();
		tstnWriter.writeDefResultsSynthesis(allPlrZENDSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		tstnWriter.writeAngleResultsSummary(TLGCObsSummary::merge(allPlrZENDSummaries_), TABs);

		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "PLR3D: DIST");
		(*stream) << endl;
		tstnWriter.writeDISTHeaderSynthesis();
		tstnWriter.writeDefResultsSynthesis(allPlrDISTSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		tstnWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allPlrDISTSummaries_), TABs);
	}

	// ECTH
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECTH) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ECTH");
		(*stream) << endl;
		tstnWriter.writeDISTHeaderSynthesis();
		tstnWriter.writeDefResultsSynthesis(allECTHSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		tstnWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allECTHSummaries_), TABs);
	}

	// ECDIR
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECDIR) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ECDIR");
		(*stream) << endl;
		tstnWriter.writeDISTHeaderSynthesis();
		tstnWriter.writeDefResultsSynthesis(allECDIRSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		tstnWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allECDIRSummaries_), TABs);
	}

	// UVD : change of format w.r.t. previous version to keep some consistency
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kUVD) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "UVD: XVEC");
		(*stream) << endl;
		camWriter.writeResultsSynthesisHeaderUnitless();
		camWriter.writeDefResultsSynthesis(allUvdXSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		camWriter.writeUnitlessResultsSummary(TLGCObsSummary::merge(allUvdXSummaries_), TABs);

		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "UVD: YVEC");
		(*stream) << endl;
		camWriter.writeResultsSynthesisHeaderUnitless();
		camWriter.writeDefResultsSynthesis(allUvdYSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		camWriter.writeUnitlessResultsSummary(TLGCObsSummary::merge(allUvdYSummaries_), TABs);

		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "UVD: DIST");
		(*stream) << endl;
		camWriter.writeResultsSynthesisHeaderDistance();
		camWriter.writeDefResultsSynthesis(allUvdDSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		camWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allUvdDSummaries_), TABs);
	}

	// UVEC : change of format w.r.t. previous version to keep some consistency
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kUVEC) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "UVEC: XVEC");
		(*stream) << endl;
		camWriter.writeResultsSynthesisHeaderUnitless();
		camWriter.writeDefResultsSynthesis(allUvecXSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		camWriter.writeUnitlessResultsSummary(TLGCObsSummary::merge(allUvecXSummaries_), TABs);

		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "UVEC: YVEC");
		(*stream) << endl;
		camWriter.writeResultsSynthesisHeaderUnitless();
		camWriter.writeDefResultsSynthesis(allUvecYSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		camWriter.writeUnitlessResultsSummary(TLGCObsSummary::merge(allUvecYSummaries_), TABs);
	}

	// DLEV
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDLEV) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "DLEV");
		(*stream) << "\n";
		levelWriter.writeLEVELSynthesisHeader();
		levelWriter.writeLEVELResultsSynthesis(allDLEVSummaries_);
		levelWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allDLEVSummaries_), TABs);
		// DLEV:DHOR
		if (!allDlevDHORSummaries_.empty())
		{
			(*stream) << TABs;
			(*stream).writeStringLeft(nameWidth, "DLEVDHOR");
			(*stream) << "\n";
			levelWriter.writeLEVELSynthesisHeader();
			levelWriter.writeLEVELResultsSynthesis(allDlevDHORSummaries_);
			levelWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allDlevDHORSummaries_), TABs);
		}
	}

	// DVER
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDVER) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "DVER");
		(*stream) << endl;
		otherMeasWriter.writeResultsSynthesisHeader();
		otherMeasWriter.writeDefResultsSynthesis(allDVERSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		otherMeasWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allDVERSummaries_), TABs);
	}

	// ECHO
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECHO) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ECHO");
		(*stream) << endl;
		scaleWriter.writeSCALESynthesisHeader();
		scaleWriter.writeDefResultsSynthesis(allECHOSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		scaleWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allECHOSummaries_), TABs);
	}

	// DSPT
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDSPT) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "DSPT");
		(*stream) << endl;
		edmWriter.writeEDMSynthesisHeader();
		edmWriter.writeDefResultsSynthesis(allDSPTSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		edmWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allDSPTSummaries_), TABs);
	}

	// ECVE
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECVE) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ECVE");
		(*stream) << endl;
		scaleWriter.writeSCALESynthesisHeader();
		scaleWriter.writeDefResultsSynthesis(allECVESummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		scaleWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allECVESummaries_), TABs);
	}

	// ECSP
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECSP) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ECSP");
		(*stream) << endl;
		scaleWriter.writeSCALESynthesisHeader();
		scaleWriter.writeDefResultsSynthesis(allECSPSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		scaleWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allECSPSummaries_), TABs);
	}

	// ORIE
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kORIE) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ORIE");
		(*stream) << endl;
		otherMeasWriter.writeResultsSynthesisHeaderAngles();
		otherMeasWriter.writeDefResultsSynthesis(allORIESummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		otherMeasWriter.writeAngleResultsSummary(TLGCObsSummary::merge(allORIESummaries_), TABs);
	}

	// RADI
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kRADI) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "RADI");
		(*stream) << endl;
		otherMeasWriter.writeResultsSynthesisHeader();
		otherMeasWriter.writeDefResultsSynthesis(allRADISummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		otherMeasWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allRADISummaries_), TABs);
	}

	// OBSXYZ : change of format w.r.t. previous version to keep some consistency
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kOBSXYZ) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "OBSXYZ: X");
		(*stream) << endl;
		tstnWriter.writeDISTHeaderSynthesis();
		otherMeasWriter.writeDefResultsSynthesis(allObsxyzXSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		otherMeasWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allObsxyzXSummaries_), TABs);

		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "OBSXYZ: Y");
		(*stream) << endl;
		tstnWriter.writeDISTHeaderSynthesis();
		tstnWriter.writeDefResultsSynthesis(allObsxyzYSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		tstnWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allObsxyzYSummaries_), TABs);

		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "OBSXYZ: Z");
		(*stream) << endl;
		tstnWriter.writeDISTHeaderSynthesis();
		tstnWriter.writeDefResultsSynthesis(allObsxyzZSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		tstnWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allObsxyzZSummaries_), TABs);
	}

	// INCLY - Write results for inclinometer Y-axis measurements if data exists
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kINCLY) > 0)
	{
		// Add newlines and tabs for formatting
		(*stream) << endl;
		(*stream) << TABs;
		// Write INCLY header with instrument name
		(*stream).writeStringLeft(nameWidth, "INCLY"); // instrument name
		(*stream) << endl;
		
		// Write synthesis header and results for INCLY measurements
		inclWriter.writeINCLSynthesisHeader();
		inclWriter.writeINCLYResultsSynthesis(allINCLYSummaries_);
		// Write summary of angle results with proper formatting
		inclWriter.writeAngleResultsSummary(TLGCObsSummary::merge(allINCLYSummaries_), TABs);
	}

	// ROLLY - Write results for roll Y-axis measurements if data exists
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kROLLY) > 0)
	{
		// Add newlines and tabs for formatting  
		(*stream) << endl;
		(*stream) << TABs;
		// Write ROLLY header with instrument name
		(*stream).writeStringLeft(nameWidth, "ROLLY"); // instrument name
		(*stream) << endl;
		
		// Write synthesis header and results for ROLLY measurements
		inclWriter.writeINCLSynthesisHeader();
		inclWriter.writeROLLYResultsSynthesis(allROLLYSummaries_);
		// Write summary of angle results with proper formatting
		inclWriter.writeAngleResultsSummary(TLGCObsSummary::merge(allROLLYSummaries_), TABs);
	}

	////ECWS
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECWS) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ECWS");
		(*stream) << endl;
		hlsrWriter.writeHLSRSynthesisHeader();
		hlsrWriter.writeDefResultsSynthesis(allECWSSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		hlsrWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allECWSSummaries_), TABs);
	}

	////ECWI
	if (fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECWI) > 0)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ECWI: X");
		(*stream) << endl;
		wpsrWriter.writeWPSRSynthesisHeader();
		wpsrWriter.writeDefResultsSynthesis(allEcwiXSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		wpsrWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allEcwiXSummaries_), TABs);

		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ECWI: Z");
		(*stream) << endl;
		wpsrWriter.writeWPSRSynthesisHeader();
		wpsrWriter.writeDefResultsSynthesis(allEcwiZSummaries_, getObsResWidth(), std::max(getLengthResidualPrecision() - 3, 0));
		wpsrWriter.writeDistanceResultsSummary(TLGCObsSummary::merge(allEcwiZSummaries_), TABs);
	}
}

void TFRAMEWriter::writeFRAMESimu(TDataTreeIterator frameIt)
{
	TAStreamFormatter *stream = getStream();
	std::string TABs = stream->getCurrSpace();
	stream->setTreeDepth((int)frameIt->get()->ID.size()
		- 1); // Size of the ID is equal to the depth in the tree, which corresponds to the number o TABs to be used in formatting. Zero TABs for ROOT (depth 1).

	writeFRAMEHeader(frameIt->get()->frame.getName(), frameIt->get()->ID);

	if (frameIt->get()->frame.getName() != "ROOT")
		writeFRAMEDefinition(frameIt->get()->frame);
	else
	{
		writePoints(frameIt); // Write points for SIMU only in ROOT, not in local nodes
		(*stream) << "SFP = Sub-Frame Point; * = TRUE" << "\n";
	}
	(*stream) << "\n" << "\n";

	TTSTNWriter tstnWriter(*stream, fProjectData->getConfig().histo.isActive());
	TCAMWriter camWriter(*stream, fProjectData->getConfig().histo.isActive());
	TEDMWriter edmWriter(*stream, fProjectData->getConfig().histo.isActive());
	TSCALEWriter scaleWriter(*stream, fProjectData->getConfig().histo.isActive());
	TLEVELWriter levelWriter(*stream, fProjectData->getConfig().histo.isActive());
	TOtherMeasurementWriter otherMeasWriter(*stream, fProjectData->getConfig().histo.isActive());
	TINCLWriter inclWriter(*stream, fProjectData->getConfig().histo.isActive());
	THLSRWriter hlsrWriter(*stream, fProjectData->getConfig().histo.isActive());
	TWPSRWriter wpsrWriter(*stream, fProjectData->getConfig().histo.isActive());

	auto &tmeas = (*frameIt)->measurements;

	// If PDOR
	if (tmeas.fPDOR.isInitialised())
		otherMeasWriter.writePDORResults(tmeas.fPDOR);

	// Measures
	(*stream) << endl << endl << TABs << "*** MESURES ***" << endl << endl;
	for (auto &itTSTN : tmeas.fTSTN)
		tstnWriter.writeTSTNResultsSIMU(itTSTN);

	for (auto &itCAM : tmeas.fCAM)
		camWriter.writeCAMResultsSIMU(itCAM);

	for (auto &itLEVEL : tmeas.fLEVEL)
		levelWriter.writeLEVELSIMUResults(itLEVEL);

	// No instrument for DVER, so no loop to have each instrument.
	if (!tmeas.fDVER.empty())
	{
		(*stream) << TABs << "DVER" << endl << endl;
		otherMeasWriter.writeDVERSIMUResults(tmeas);
	}

	for (auto &itECHO : tmeas.fECHO)
		scaleWriter.writeECHOSIMUResults(itECHO);

	for (auto &itECVE : tmeas.fECVE)
		scaleWriter.writeECVESIMUResults(itECVE);

	for (auto &itECSP : tmeas.fECSP)
		scaleWriter.writeECSPSIMUResults(itECSP);

	for (auto &itORIE : tmeas.fORIE)
		otherMeasWriter.writeORIESIMUResults(itORIE);

	for (auto &itEDM : tmeas.fEDM)
		edmWriter.writeEDMSIMUResults(itEDM);

	for (auto &itINCLY : tmeas.fINCLY)
		inclWriter.writeINCLYSIMUResults(itINCLY);

	for (auto &itROLLY : tmeas.fROLLY)
		inclWriter.writeROLLYSIMUResults(itROLLY);

	for (auto &itECWS : tmeas.fECWS)
		hlsrWriter.writeECWSSIMUResults(itECWS);

	for (auto &itECWI : tmeas.fECWI)
		wpsrWriter.writeECWISIMUResults(itECWI);
}

void TFRAMEWriter::writeFRAMEAllReliability(TDataTreeIterator frameIt)
{
	TAStreamFormatter *stream = getStream();
	TEDMWriter edmWriter(*stream, fProjectData->getConfig().histo.isActive());
	TOtherMeasurementWriter otherMeasWriter(*stream, fProjectData->getConfig().histo.isActive());

	auto &tmeas = (*frameIt)->measurements;

	stream->setTreeDepth((int)frameIt->get()->ID.size()
		- 1); // Size of the ID is equal to the depth in the tree, which corresponds to the number o TABs to be used in formatting. Zero TABs for ROOT (depth 1).

	writeFRAMEHeader(frameIt->get()->frame.getName(), frameIt->get()->ID);

	if (frameIt->get()->frame.getName() != "ROOT")
		writeFRAMEDefinition(frameIt->get()->frame);

	writeTSTNReliability(frameIt);
	writeCAMReliability(frameIt);
	writeLEVELReliability(frameIt);

	if (!tmeas.fDVER.empty())
	{
		(*stream) << endl << "DVER observations" << endl;
		otherMeasWriter.writeDVERReliabilityHeader();
		otherMeasWriter.writeDVERReliabilityData(tmeas.fDVER, fProjectData->getStatistics());
	}

	writeSCALEReliability(frameIt);
	writeINCLReliability(frameIt);
	writeROLLYReliability(frameIt);
	writeHLSRReliability(frameIt);
	writeWPSRReliability(frameIt);

	bool ORIEheaderWritten = false;
	for (auto &itORIE : tmeas.fORIE)
	{
		if (!ORIEheaderWritten)
		{
			(*stream) << endl << "ORIE observations" << endl;
			otherMeasWriter.writeORIEReliabilityHeader();
			ORIEheaderWritten = true;
		}
		otherMeasWriter.writeORIEReliabilityData(itORIE.measORIE, fProjectData->getStatistics(), *itORIE.instrumentPos);
	}

	if (!tmeas.fRADI.empty())
	{
		(*stream) << endl << "RADI observations" << endl;
		otherMeasWriter.writeRADIReliabilityHeader();
		otherMeasWriter.writeRADIReliabilityData(tmeas.fRADI, fProjectData->getStatistics());
	}

	if (!frameIt->get()->measurements.fOBSXYZ.empty())
	{
		(*stream) << endl << "OBSXYZ observations" << endl;
		otherMeasWriter.writeOBSXYZReliabilityHeader();
		otherMeasWriter.writeOBSXYZReliabilityData(frameIt->get()->measurements.fOBSXYZ, fProjectData->getStatistics());
	}

	bool EDMheaderWritten = false;
	for (auto &itEDM : tmeas.fEDM)
	{
		if (!EDMheaderWritten)
		{
			(*stream) << endl << "DSPT observations" << endl;
			edmWriter.writeReliabilityHeader();
			EDMheaderWritten = true;
		}
		edmWriter.writeReliabilityData(itEDM, fProjectData->getStatistics());
	}
}

void TFRAMEWriter::writeFRAMEHeader(const std::string &name, const std::vector<int> &ID)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	std::string TABs = stream->getCurrSpace();
	int obsResWidth = getObsResWidth();
	////////////////////////////////////////////////////////////
	// first line

	std::string nameID;
	for (std::vector<int>::const_iterator it = ID.begin(); it != ID.end(); ++it)
	{
		if (it == ID.begin())
			nameID += std::to_string(*it);
		else
			nameID += "_" + std::to_string(*it);
	}
	std::string slaveNote{""};
	for (TLGCFrameConstraintGroup group : fProjectData->getSlaveGroups())
	{
		if (group.isPartOfGroup(name))
		{
			slaveNote = " NOTE: This Frame belongs to Slave Group \"" + group.getGroupName() + "\"";
			break;
		}
	}

	(*stream) << endl << endl;
	(*stream) << TABs;
	std::string namestring = "FRAME\t" + name;
	if (ID.size() > 0)
		namestring += "  ID(" + nameID + ")";
	namestring += slaveNote;
	(*stream).writeStringLeft(nameWidth, namestring);
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	if (ID.size() != 1) // Only for ROOT it is equal to 1 and in this case we do not want to write out the header
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "PARAMETER");
		(*stream).writeString(obsWidth, "UNIT");
		(*stream).writeString(obsWidth, "INITIAL");
		(*stream).writeString(obsWidth, "CALCULE");
		(*stream).writeString(obsWidth, "SUNIT");
		(*stream).writeString(obsWidth, "SIGMA");
		(*stream).writeString(obsWidth, "FIXED");
		(*stream) << endl;
	}
}

void TFRAMEWriter::writeFRAMEDefinition(const TAdjustableHelmertTransformation &frame)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	std::string separator = getSeparator();
	int lengthPrecision = getLengthPrecision();
	int anglePrecision = getAnglePrecision();
	std::string TABs = stream->getCurrSpace();
	int obsResWidth = getObsResWidth();
	int lengthResidualPrecision = getLengthResidualPrecision();
	// set length parameters
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);

	// Writing translations
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "TX");
	writeTranslationParameter(frame, 0);

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "TY");
	writeTranslationParameter(frame, 1);

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "TZ");
	writeTranslationParameter(frame, 2);

	stream->setPrecisionFormat(anglePrecision);

	// Writing rotations
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "RX");
	writeRotationParameter(frame, 0);

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "RY");
	writeRotationParameter(frame, 1);

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "RZ");
	writeRotationParameter(frame, 2);

	// Writing scale factor
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "SCALE");
	// the scale is unitless
	(*stream).writeString(obsWidth, "-");
	(*stream).writeDouble(obsWidth, lengthPrecision, frame.getProvScale());

	if (!frame.isScaleFixed())
	{
		(*stream).writeDouble(obsWidth, lengthPrecision, frame.getEstScale());
	}
	else
	{
		(*stream).writeString(obsWidth, "-");
	}
	// the scale is unitless
	(*stream).writeString(obsWidth, "-");

	if (!frame.isScaleFixed())
	{
		(*stream).writeDouble(obsWidth, lengthPrecision, frame.getEstimatedPrecisionScale());
		(*stream).writeString(obsWidth, "FALSE");
	}
	else
	{
		(*stream).writeString(obsWidth, "-");
		(*stream).writeString(obsWidth, "TRUE");
	}

	if (!frame.isFixed())
	{
		(*stream) << endl << endl;
		TDenseMatrix frameCovar = frame.getCovar();

		if (!frame.isRotationFixed(0) && !frame.isRotationFixed(1))
		{
			(*stream) << TABs;
			(*stream).writeStringLeft(nameWidth, "XY ROT. COVAR (GON²): ");
			(*stream) << frameCovar(3, 4) * RAD2GON << (separator);
		}

		if (!frame.isRotationFixed(1) && !frame.isRotationFixed(2))
		{
			(*stream) << TABs;
			(*stream).writeStringLeft(nameWidth, "YZ ROT. COVAR (GON²): ");
			(*stream) << frameCovar(4, 5) * RAD2GON << (separator);
		}

		if (!frame.isRotationFixed(0) && !frame.isRotationFixed(2))
		{
			(*stream) << TABs;
			(*stream).writeStringLeft(nameWidth, "XZ ROT. COVAR (GON²): ");
			(*stream) << frameCovar(3, 5) * RAD2GON << (separator);
		}

		(*stream) << endl;

		if (!frame.isTranslationFixed(0) && !frame.isTranslationFixed(1))
		{
			(*stream) << TABs;
			(*stream).writeStringLeft(nameWidth, "XY TRANS. COVAR (m²): ");
			(*stream) << frameCovar(0, 1) << (separator);
		}

		if (!frame.isTranslationFixed(1) && !frame.isTranslationFixed(2))
		{
			(*stream) << TABs;
			(*stream).writeStringLeft(nameWidth, "YZ TRANS. COVAR (m²): ");
			(*stream) << frameCovar(1, 2) << (separator);
		}

		if (!frame.isTranslationFixed(0) && !frame.isTranslationFixed(2))
		{
			(*stream) << TABs;
			(*stream).writeStringLeft(nameWidth, "XZ TRANS. COVAR (m²): ");
			(*stream) << frameCovar(0, 2) << (separator);
		}
	}
	(*stream) << endl;
}

void TFRAMEWriter::writePoints(TDataTreeIterator frameIt)
{
	bool localNode = false;
	bool errorEllipseActive = fProjectData->getConfig().errorEllipses.isActive();

	if (frameIt->get()->isROOTNode())
		localNode = false;
	else
		localNode = true;

	writePointType(pointCALA, frameIt, TSpatialStatus::kCala, localNode);

	writePointType(pointVXYZ, frameIt, TSpatialStatus::kVxyz, localNode);

	// Ellipsoid and Ellips data only in the root frame
	if (errorEllipseActive && !pointVXYZ.empty() && !localNode)
	{
		// write ellipsoidal error
		writeEllipsoidHeader();

		for (auto &it : pointVXYZ)
			writeEllipsoidData(it);
	}

	auto writeEllipse = [this, localNode, errorEllipseActive](std::list<AdjPointIter> &points) {
		if (errorEllipseActive && !points.empty() && !localNode)
		{
			// write ellips error
			writeEllipsHeader();
			for (auto &it : points)
				writeEllipsData(it);
		}
	};

	writePointType(pointVXY, frameIt, TSpatialStatus::kVxy, localNode);
	writeEllipse(pointVXY);

	writePointType(pointVXZ, frameIt, TSpatialStatus::kVxz, localNode);
	writeEllipse(pointVXZ);

	writePointType(pointVYZ, frameIt, TSpatialStatus::kVyz, localNode);
	writeEllipse(pointVYZ);

	writePointType(pointVZ, frameIt, TSpatialStatus::kVz, localNode);

	*getStream() << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void TFRAMEWriter::writePointType(const std::list<AdjPointIter> &lop, TDataTreeIterator frameIt, TSpatialStatus::ESpatialStatus type, bool localNode)
{
	if (!lop.empty())
	{
		// Tells if a header was already written (for local node).
		// In collections of points, we have points defined among whole project.
		// We want to write header only if we find out, that at least one point is defined in the particular sub-frame.
		bool headerWritten = false;

		std::string fReferentialName;
		if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
			fReferentialName = "SPHE";
		else if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
			fReferentialName = "RS2K";
		else if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
			fReferentialName = "LEP";
		else
			fReferentialName = "OLOC";

		if (!localNode)
			writeResultsPtsHeader(type, (int)lop.size(), fReferentialName, localNode);

		for (auto it(lop.begin()); it != lop.end(); ++it)
		{
			if (localNode)
			{
				AdjPointIter pIt = *it;
				if (pIt->getFrameTreePosition() == frameIt)
				{ // If the point was defined in this FRAME
					if (!headerWritten)
					{
						writeResultsPtsHeader(type, (int)lop.size(), fReferentialName, localNode);
						headerWritten = true;
					}
					writeResultsPtsData(*it, localNode);
				}
			}
			else
				writeResultsPtsData(*it, localNode);
		}
	}
}

void TFRAMEWriter::writeTranslationParameter(const TAdjustableHelmertTransformation &frameDef, int transl)
{
	assert3D(transl);
	TAStreamFormatter *stream = getStream();
	std::string separator = getSeparator();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int lengthPrecision = getLengthPrecision();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	// Write initial and calculated value
	(*stream).writeString(obsWidth, "(M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, frameDef.getProvTranslation(transl));

	if (!frameDef.isTranslationFixed(transl))
	{
		(*stream).writeDouble(obsWidth, lengthPrecision, frameDef.getEstTranslation(transl));
	}
	else
	{
		(*stream).writeString(obsWidth, "-");
	}

	(*stream).writeString(obsWidth, "(MM)");

	// Write the standard deviation after calculation if translation is variable and the status (fixed or variable)
	if (!frameDef.isTranslationFixed(transl))
	{
		(*stream).writeDouble(obsWidth, lengthResPrecision, frameDef.getEstimatedPrecisionTransl(transl).getMMetresValue());
		(*stream).writeString(obsWidth, "FALSE");
	}
	else
	{
		(*stream).writeString(obsWidth, "-");
		(*stream).writeString(obsWidth, "TRUE");
	}
	(*stream) << endl;
}

void TFRAMEWriter::writeRotationParameter(const TAdjustableHelmertTransformation &frameDef, int rot)
{
	assert3D(rot);
	TAStreamFormatter *stream = getStream();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int anglePrecision = getAnglePrecision();
	int angleResidualPrecision = std::max(getAngleResidualPrecision() - 4, 0);

	// Write initial and calculated value
	(*stream).writeString(obsWidth, "(GON)");

	(*stream).writeDouble(obsWidth, anglePrecision, frameDef.getProvRotation(rot).getGonsValue());

	if (!frameDef.isRotationFixed(rot))
	{
		(*stream).writeDouble(obsWidth, anglePrecision, frameDef.getEstRotation(rot).getGonsValue());
	}
	else
	{
		(*stream).writeString(obsWidth, "-");
	}

	(*stream).writeString(obsWidth, "(CC)");

	// Write the standard deviation after calculation if rotation is variable and the status (fixed or variable)
	if (!frameDef.isRotationFixed(rot))
	{
		(*stream).writeDouble(obsWidth, angleResidualPrecision, frameDef.getEstimatedPrecisionRot(rot).getSignedCCValue());
		(*stream).writeString(obsWidth, "FALSE");
	}
	else
	{
		(*stream).writeString(obsWidth, "-");
		(*stream).writeString(obsWidth, "TRUE");
	}
	(*stream) << endl;
}

void TFRAMEWriter::writeTSTNReliability(TDataTreeIterator frameIt)
{
	TAStreamFormatter *stream = getStream();
	TTSTNWriter tstnWriter(*stream, fProjectData->getConfig().histo.isActive());

	auto &tmeas = (*frameIt)->measurements;

	// ANGL
	bool isANGL = false;
	for (auto &itTSTN : tmeas.fTSTN)
	{
		for (auto const ItRoms : itTSTN->roms)
		{
			if (ItRoms->measANGL.size() > 0)
			{
				if (isANGL == false)
				{
					(*stream) << endl << "ANGL observations" << endl;
					tstnWriter.writeANGLReliabilityHeader();
					isANGL = true;
				}
				tstnWriter.writeANGLReliabilityData(itTSTN, fProjectData->getStatistics(), ItRoms->measANGL);
			}
		}
	}

	// ZEND
	bool isZEND = false;
	for (auto &itTSTN : tmeas.fTSTN)
	{
		for (auto const ItRoms : itTSTN->roms)
		{
			if (ItRoms->measZEND.size() > 0)
			{
				if (isZEND == false)
				{
					(*stream) << endl << "ZEND observations" << endl;
					tstnWriter.writeZENDReliabilityHeader();
					isZEND = true;
				}
				tstnWriter.writeZENDReliabilityData(itTSTN, fProjectData->getStatistics(), ItRoms->measZEND);
			}
		}
	}

	// DIST
	bool isDIST = false;
	for (auto &itTSTN : tmeas.fTSTN)
	{
		for (auto const ItRoms : itTSTN->roms)
		{
			if (ItRoms->measDIST.size() > 0)
			{
				if (isDIST == false)
				{
					(*stream) << endl << "DIST observations" << endl;
					tstnWriter.writeDISTReliabilityHeader();
					isDIST = true;
				}
				tstnWriter.writeDISTReliabilityData(itTSTN, fProjectData->getStatistics(), ItRoms->measDIST);
			}
		}
	}

	// PLR
	bool isPLR = false;
	for (auto &itTSTN : tmeas.fTSTN)
	{
		for (auto const ItRoms : itTSTN->roms)
		{
			if (ItRoms->measPLR3D.size() > 0)
			{
				if (isPLR == false)
				{
					(*stream) << endl << "PLR3D observations" << endl;
					tstnWriter.writePLRReliabilityHeader();
					isPLR = true;
				}
				tstnWriter.writePLRReliabilityData(itTSTN, fProjectData->getStatistics(), ItRoms->measPLR3D);
			}
		}
	}

	// ECTH
	bool isECTH = false;
	for (auto &itTSTN : tmeas.fTSTN)
	{
		for (auto const ItRoms : itTSTN->roms)
		{
			if (ItRoms->measECTH.size() > 0)
			{
				if (isECTH == false)
				{
					(*stream) << endl << "ECTH observations" << endl;
					tstnWriter.writeECTHReliabilityHeader();
					isECTH = true;
				}
				tstnWriter.writeECTHReliabilityData(itTSTN, fProjectData->getStatistics(), ItRoms->measECTH);
			}
		}
	}

	// ECTH
	bool isECDIR = false;
	for (auto &itTSTN : tmeas.fTSTN)
	{
		for (auto const &ItRoms : itTSTN->roms)
		{
			if (ItRoms->measECDIR.size() > 0)
			{
				if (isECDIR == false)
				{
					(*stream) << endl << "ECDIR observations" << endl;
					tstnWriter.writeECTHReliabilityHeader();
					isECDIR = true;
				}
				tstnWriter.writeECDIRReliabilityData(itTSTN, fProjectData->getStatistics(), ItRoms->measECDIR);
			}
		}
	}

	// DHOR
	bool isDHOR = false;
	for (auto &itTSTN : tmeas.fTSTN)
	{
		for (auto const ItRoms : itTSTN->roms)
		{
			if (ItRoms->measDHOR.size() > 0)
			{
				if (isDHOR == false)
				{
					(*stream) << endl << "DHOR observations" << endl;
					tstnWriter.writeDHORReliabilityHeader();
					isDHOR = true;
				}
				tstnWriter.writeDHORReliabilityData(itTSTN, fProjectData->getStatistics(), ItRoms->measDHOR);
			}
		}
	}
}

void TFRAMEWriter::writeCAMReliability(TDataTreeIterator frameIt)
{
	TAStreamFormatter *stream = getStream();
	TCAMWriter camWriter(*stream, fProjectData->getConfig().histo.isActive());

	auto &tmeas = (*frameIt)->measurements;

	// UVEC
	bool isuvec = false;
	for (auto &itCAM : tmeas.fCAM)
	{
		if (itCAM.measUVEC.size() > 0)
		{
			if (isuvec == false)
			{
				(*stream) << endl << "UVEC observations" << endl;
				camWriter.writeUVECReliabilityHeader();
				isuvec = true;
			}
			camWriter.writeUVECReliabilityData(itCAM, fProjectData->getStatistics());
		}
	}

	// UVD
	bool isuvd = false;
	for (auto &itCAM : tmeas.fCAM)
	{
		if (itCAM.measUVD.size() > 0)
		{
			if (isuvd == false)
			{
				(*stream) << endl << "UVD observations" << endl;
				camWriter.writeUVDReliabilityHeader();
				isuvd = true;
			}
			camWriter.writeUVDReliabilityData(itCAM, fProjectData->getStatistics());
		}
	}
}

void TFRAMEWriter::writeLEVELReliability(TDataTreeIterator frameIt)
{
	TAStreamFormatter *stream = getStream();
	TLEVELWriter levelWriter(*stream, fProjectData->getConfig().histo.isActive());

	auto &tmeas = (*frameIt)->measurements;

	// DLEV
	bool isdlev = false;
	for (auto &itLEV : tmeas.fLEVEL)
	{
		if (itLEV.measDLEV.size() > 0)
		{
			if (isdlev == false)
			{
				(*stream) << endl << "DLEV observations" << endl;
				levelWriter.writeReliabilityHeader();
				isdlev = true;
			}
			levelWriter.writeDLEVReliabilityData(itLEV, fProjectData->getStatistics());
		}
	}

	// Part of the for loop above probably

	// DHOR
	bool isdhor = false;
	for (auto &itLEV : tmeas.fLEVEL)
	{
		if (itLEV.hasDHOR)
		{
			if (isdhor == false)
			{
				(*stream) << endl << "DHOR observations from a DLEV observations" << endl;
				levelWriter.writeReliabilityHeader();
				isdhor = true;
			}
			levelWriter.writeDHORReliabilityData(itLEV, fProjectData->getStatistics());
		}
	}
}

void TFRAMEWriter::writeSCALEReliability(TDataTreeIterator frameIt)
{
	TAStreamFormatter *stream = getStream();
	TSCALEWriter scaleWriter(*stream, fProjectData->getConfig().histo.isActive());

	auto &tmeas = (*frameIt)->measurements;

	// ECHO
	bool isecho = false;
	for (auto itECH(tmeas.fECHO.begin()); itECH != tmeas.fECHO.end(); ++itECH)
	{
		if (itECH->measECHO.size() > 0)
		{
			if (isecho == false)
			{
				(*stream) << endl << "ECHO observations" << endl;
				scaleWriter.writeECHOReliabilityHeader();
				isecho = true;
			}
			scaleWriter.writeECHOReliabilityData(*itECH, fProjectData->getStatistics(), itECH->measECHO);
		}
	}

	// ECVE
	bool isecve = false;
	for (auto &itECVE : tmeas.fECVE)
	{
		if (itECVE.measECVE.size() > 0)
		{
			if (isecve == false)
			{
				(*stream) << endl << "ECVE observations" << endl;
				scaleWriter.writeECVEReliabilityHeader();
				isecve = true;
			}
			scaleWriter.writeECVEReliabilityData(itECVE, fProjectData->getStatistics(), itECVE.measECVE);
		}
	}

	// ECSP
	bool isecsp = false;
	for (auto &itECSP : tmeas.fECSP)
	{
		if (itECSP.measECSP.size() > 0)
		{
			if (isecsp == false)
			{
				(*stream) << endl << "ECSP observations" << endl;
				scaleWriter.writeECSPReliabilityHeader();
				isecsp = true;
			}
			scaleWriter.writeECSPReliabilityData(itECSP, fProjectData->getStatistics(), itECSP.measECSP);
		}
	}
}

/*
 * INCLY Reliability Report Writer
 * 
 * Writes reliability statistics and quality metrics for INCLY inclinometer measurements.
 * Generates comprehensive reports including measurement statistics, residuals analysis,
 * and quality indicators for the new arcsin-based mathematical model.
 * 
 * @param frameIt: Iterator pointing to the frame containing INCLY measurements
 */
void TFRAMEWriter::writeINCLReliability(TDataTreeIterator frameIt)
{
	// Get output stream and configure INCL writer with histogram settings
	TAStreamFormatter *stream = getStream();
	TINCLWriter inclWriter(*stream, fProjectData->getConfig().histo.isActive());

	// Access frame measurements
	auto &tmeas = (*frameIt)->measurements;

	// Process INCLY measurements and write reliability data
	bool isincly = false;
	for (auto &itINCLY : tmeas.fINCLY)
	{
		if (itINCLY.measINCLY.size() > 0)
		{
			// Write header only once per frame
			if (isincly == false)
			{
				(*stream) << endl << "INCLY observations" << endl;
				inclWriter.writeINCLYReliabilityHeader();
				isincly = true;
			}
			// Write reliability data for each round of measurements
			inclWriter.writeINCLYReliabilityData(itINCLY, fProjectData->getStatistics(), itINCLY.measINCLY);
		}
	}
}

/*
 * ROLLY Reliability Report Writer
 * 
 * Writes reliability statistics and quality metrics for ROLLY inclinometer measurements.
 * Generates comprehensive reports including measurement statistics, residuals analysis,
 * and quality indicators for the legacy atan2-based mathematical model.
 * 
 * @param frameIt: Iterator pointing to the frame containing ROLLY measurements
 */
void TFRAMEWriter::writeROLLYReliability(TDataTreeIterator frameIt)
{
	// Get output stream and configure INCL writer with histogram settings
	TAStreamFormatter *stream = getStream();
	TINCLWriter inclWriter(*stream, fProjectData->getConfig().histo.isActive());

	// Access frame measurements
	auto &tmeas = (*frameIt)->measurements;

	// Process ROLLY measurements and write reliability data
	bool isrolly = false;
	for (auto &itROLLY : tmeas.fROLLY)
	{
		if (itROLLY.measROLLY.size() > 0)
		{
			// Write header only once per frame
			if (isrolly == false)
			{
				(*stream) << endl << "ROLLY observations" << endl;
				inclWriter.writeROLLYReliabilityHeader();
				isrolly = true;
			}
			// Write reliability data for each round of measurements
			inclWriter.writeROLLYReliabilityData(itROLLY, fProjectData->getStatistics(), itROLLY.measROLLY);
		}
	}
}

void TFRAMEWriter::writeHLSRReliability(TDataTreeIterator frameIt)
{
	TAStreamFormatter *stream = getStream();
	THLSRWriter hlsrWriter(*stream, fProjectData->getConfig().histo.isActive());

	auto &tmeas = (*frameIt)->measurements;

	// ECWS
	bool isecws = false;
	for (auto &itECWS : tmeas.fECWS)
	{
		if (itECWS.measECWS.size() > 0)
		{
			if (isecws == false)
			{
				(*stream) << endl << "ECWS observations" << endl;
				hlsrWriter.writeECWSReliabilityHeader();
				isecws = true;
			}
			hlsrWriter.writeECWSReliabilityData(itECWS, fProjectData->getStatistics(), itECWS.measECWS);
		}
	}
}

void TFRAMEWriter::writeWPSRReliability(TDataTreeIterator frameIt)
{
	TAStreamFormatter *stream = getStream();
	TWPSRWriter wpsrWriter(*stream, fProjectData->getConfig().histo.isActive());

	auto &tmeas = (*frameIt)->measurements;

	// ECWI
	bool isecwi = false;
	for (auto &itECWI : tmeas.fECWI)
	{
		if (itECWI.measECWI.size() > 0)
		{
			if (isecwi == false)
			{
				(*stream) << endl << "ECWI observations" << endl;
				wpsrWriter.writeECWIReliabilityHeader();
				isecwi = true;
			}
			wpsrWriter.writeECWIReliabilityData(itECWI, fProjectData->getStatistics(), itECWI.measECWI);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// HEADER
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void TFRAMEWriter::writeResultsPtsHeader(const TSpatialStatus::ESpatialStatus status, const int ptNumber, const std::string &refSys, bool localFRAME)
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	int coordResWidth = getCoordResWidth();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(1);

	std::string title = "";
	if (status == TSpatialStatus::kCala)
	{
		title = "POINTS DE CALAGE";
	}
	if (status == TSpatialStatus::kVz)
	{
		title = "POINTS VARIABLES EN Z UNIQUEMENT";
	}
	if (status == TSpatialStatus::kVxy)
	{
		title = "POINTS INVARIABLES EN Z";
	}
	if (status == TSpatialStatus::kVxz)
	{
		title = "POINTS INVARIABLES EN Y";
	}
	if (status == TSpatialStatus::kVyz)
	{
		title = "POINTS INVARIABLES EN X";
	}
	if (status == TSpatialStatus::kVxyz)
	{
		title = "POINTS VARIABLES EN XYZ";
	}

	(*stream) << endl;

	(*stream) << title << separator;
	if (!localFRAME)
		(*stream) << "(NB. = " << ptNumber << ",  REFERENTIEL = " << refSys << " )";

	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// First line
	(*stream).writeString(3, "SFP"); // tell if point defined in subframe
	(*stream).writeString(nameWidth, "NOM"); // Nom
	(*stream).writeString(coordWidth, "X "); // X
	(*stream).writeString(coordWidth, "Y "); // Y
	(*stream).writeString(coordWidth, "Z "); // Z

	bool systemWithH = (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS
		|| fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine
		|| fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine);

	if (!localFRAME && systemWithH)
	{
		(*stream).writeString(coordWidth, "H "); // H
	}

	(*stream).writeString(coordResWidth, "SX "); // sigma X
	(*stream).writeString(coordResWidth, "SY "); // sigma Y
	(*stream).writeString(coordResWidth, "SZ "); // sigma Z

	(*stream).writeString(coordResWidth, "DX "); // X offset
	(*stream).writeString(coordResWidth, "DY "); // Y offset
	(*stream).writeString(coordResWidth, "DZ "); // Z offset
	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// second line : units
	(*stream).writeString(3, ""); // Star
	(*stream).writeString(nameWidth, ""); // Nom
	(*stream).writeString(coordWidth, "(M)"); // X units
	(*stream).writeString(coordWidth, "(M)"); // Y units
	(*stream).writeString(coordWidth, "(M)"); // Z units

	if (!localFRAME && systemWithH)
	{
		(*stream).writeString(coordWidth, "(M)"); // H units
	}

	(*stream).writeString(coordResWidth, "(MM)"); // sigma X units
	(*stream).writeString(coordResWidth, "(MM)"); // sigma Y units
	(*stream).writeString(coordResWidth, "(MM)"); // sigma Z units

	(*stream).writeString(coordResWidth, "(MM)"); // X offset units
	(*stream).writeString(coordResWidth, "(MM)"); // Y offset units
	(*stream).writeString(coordResWidth, "(MM)"); // Z offset units
	(*stream) << endl << endl;
}

void TFRAMEWriter::writeEllipsHeader()
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();

	*stream << endl << "ABSOLUTE ERROR ELLIPSES" << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// First line
	stream->writeStringLeft(nameWidth, "NOM");
	stream->writeString(coordWidth, "SEMI-MAJ.(A)");
	stream->writeString(coordWidth, "SEMI-MIN.(B)");
	stream->writeString(coordWidth, "ORIE. OF (A)");
	stream->writeStringLeft(nameWidth, "SYSTEME");

	*stream << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// second line : units
	stream->writeString(nameWidth, ""); // Nom
	stream->writeString(coordWidth, "(MM)"); // MINOR
	stream->writeString(coordWidth, "(MM)"); // MAJOR
	stream->writeString(coordWidth, "(GON)"); // ORIENTATION

	*stream << endl;

	return;
}

void TFRAMEWriter::writeEllipsoidHeader()
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();

	*stream << endl << "ABSOLUTE ERROR ELLIPSOIDS" << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// First line
	stream->writeStringLeft(nameWidth, "NOM");
	// Direction vectors: (+0.000, -0.000, +0.000) => 24
	const int vecwidth(24);
	stream->writeString(vecwidth, "DIRECTION X");
	stream->writeString(vecwidth, "DIRECTION Y");
	stream->writeString(vecwidth, "DIRECTION Z");
	stream->writeString(coordWidth, "LONGUEUR X");
	stream->writeString(coordWidth, "LONGUEUR Y");
	stream->writeString(coordWidth, "LONGUEUR Z");
	stream->writeStringLeft(nameWidth, "SYSTEME");
	*stream << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// second line : units
	stream->writeString(nameWidth, ""); // Nom
	stream->writeString(vecwidth, ""); // Direction
	stream->writeString(vecwidth, ""); // Direction
	stream->writeString(vecwidth, ""); // Direction
	stream->writeString(coordWidth, "(MM)"); // Axis length
	stream->writeString(coordWidth, "(MM)"); // Axis length
	stream->writeString(coordWidth, "(MM)"); // Axis length

	*stream << endl;

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void TFRAMEWriter::writeResultsPtsData(AdjPointIter pt, bool localFRAME)
{ // FILE.OUTPUT
	TAStreamFormatter *stream = getStream();

	TPointConverter converter(stream, pt->getReferenceFrame());
	std::string TABs = stream->getCurrSpaceExtended(1);

	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	int coordPrecision = this->getCoordPrecision();
	int coordResWidth = getCoordResWidth();
	// int				coordResidualPrecision = this->getCoordErrorPrecision();
	std::string separator = getSeparator();

	// int coordResPrecision = coordResidualPrecision > 3 ? (coordResidualPrecision - 3) : 0;

	if (!pt->getFrameTreePosition()->get()->isROOTNode() && !localFRAME)
		(*stream).writeStringLeft(3, "*");
	else
		(*stream).writeStringLeft(3, "");
	// name
	(*stream).writeString(nameWidth, pt->getName()); // Nom
	// coordinates
	stream->setWidthFormat(coordWidth);
	stream->setPrecisionFormat(coordPrecision);

	if (localFRAME)
	{ // Means that it is not ROOT!!!!!!

		TPositionVector estimatedValue = pt->getEstimatedValue();
		TPositionVector provisionalValue = pt->getProvisionalValue();
		TDenseMatrix covarianceMatrix = pt->getCovarianceMatrix();

		// Write point coordinates XYZ or H because it is a local frame
		stream->setLengthUnits(TLength::EUnits::kMetres);
		converter.write3Coordinates(coordWidth, coordPrecision, separator, estimatedValue);

		// Write point's estimated precision after calculation
		converter.writeCoordinateParam(pt->getSpatialStatus(), coordResWidth, coordPrecision, TLength::EUnits::kMillimetres, separator, pt->getXEstPrecision(),
			pt->getYEstPrecision(), pt->getZEstPrecision(), ""); /*sigma*/

		// Write DX, DY, DZ difference between provisional and estimated value
		converter.writeCoordinateParam(pt->getSpatialStatus(), coordResWidth, coordPrecision, TLength::EUnits::kMillimetres, separator,
			TLength(estimatedValue.getX() - provisionalValue.getX()), TLength(estimatedValue.getY() - provisionalValue.getY()),
			TLength(estimatedValue.getZ() - provisionalValue.getZ()), ""); /*offset*/
	}
	else
	{ // It is ROOT
		TDataTreeIterator root = fProjectData->getTree().begin();
		TRefSystemFactory::ERefFrame globalRef = fProjectData->getConfig().referential;

		TPositionVector provisionalValue = pt->getProvisionalValueInRoot();
		TPositionVector estimatedValue = pt->getEstimatedValueInRoot();
		TDenseMatrix covarianceMatrix = pt->getCovarianceMatrixInRoot();
		TSpatialStatus::ESpatialStatus status = pt->getSpatialStatus();

		stream->setLengthUnits(TLength::EUnits::kMetres);
		converter.write3Coordinates(coordWidth, coordPrecision, separator, estimatedValue);

		if (globalRef != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		{
			writeDouble(coordWidth, coordPrecision, pt->getEstimatedHeightInRoot());
			(*stream) << separator;
		}

		if (pt->getFrameTreePosition() != root)
		{
			// status = vxyz to write sigma when the point is CALA in its frame
			status = TSpatialStatus::kVxyz;
		}

		// status = vxyz to write sigma because with CALA, no sigma are writen
		converter.writeCoordinateParam(status, coordResWidth, coordPrecision, TLength::EUnits::kMillimetres, separator, TLength(sqrtq(covarianceMatrix(0, 0))),
			TLength(sqrtq(covarianceMatrix(1, 1))), TLength(sqrtq(covarianceMatrix(2, 2))), ""); /*sigma convert in root*/

		converter.writeCoordinateParam(pt->getSpatialStatus(), coordResWidth, coordPrecision, TLength::EUnits::kMillimetres, separator,
			TLength(estimatedValue.getX() - provisionalValue.getX()), TLength(estimatedValue.getY() - provisionalValue.getY()),
			TLength(estimatedValue.getZ() - provisionalValue.getZ()), ""); /*offset*/
	}

	(*stream) << endl; // end line
}

void TFRAMEWriter::writeEllipsData(AdjPointIter &pt)
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	int coordResWidth = getCoordResWidth();

	stream->writeStringLeft(nameWidth, pt->getName());
	stream->writeDouble(coordWidth, coordResWidth, pt->getErrorEllMajorAxis().getMMetresValue());
	stream->writeDouble(coordWidth, coordResWidth, pt->getErrorEllMinorAxis().getMMetresValue());
	stream->writeDouble(coordWidth, coordResWidth, pt->getErrorEllGis().getGonsValue());
	stream->writeStringLeft(nameWidth, pt->getFrameTreePosition().node->data->frame.getName());
	*stream << endl;
}

void TFRAMEWriter::writeEllipsoidData(AdjPointIter &pt)
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	int coordResWidth = getCoordResWidth();
	std::string sep = getSeparator();

	const auto &ell(pt->getErrorEllipsoid());

	const int vecwidth(24);
	char vecstr[32]; // format the vector output here and write as a string

	stream->writeStringLeft(nameWidth, pt->getName());
	sprintf(vecstr, "(% .3f  % .3f  % .3f)", ell.vx[0], ell.vx[1], ell.vx[2]);
	stream->writeString(vecwidth, vecstr);
	sprintf(vecstr, "(% .3f  % .3f  % .3f)", ell.vy[0], ell.vy[1], ell.vy[2]);
	stream->writeString(vecwidth, vecstr);
	sprintf(vecstr, "(% .3f  % .3f  % .3f)", ell.vz[0], ell.vz[1], ell.vz[2]);
	stream->writeString(vecwidth, vecstr);
	stream->writeDouble(coordWidth, coordResWidth, ell.lx * M2MM);
	stream->writeDouble(coordWidth, coordResWidth, ell.ly * M2MM);
	stream->writeDouble(coordWidth, coordResWidth, ell.lz * M2MM);
	stream->writeStringLeft(nameWidth, pt->getFrameTreePosition().node->data->frame.getName());
	*stream << endl;
}

void TFRAMEWriter::transfXYH2XYZ(TPositionVector &pv, const TRefSystemFactory::ERefFrame &rf)
{
	if (rf == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
		TXYH2CCS::XYHs2CCS(pv);
	else if (rf == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		TXYH2CCS::XYHg2000Machine2CCS(pv);
	else if (rf == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		TXYH2CCS::XYHg1985Machine2CCS(pv);
}

void TFRAMEWriter::transfXYZ2XYH(TPositionVector &pv, const TRefSystemFactory::ERefFrame &rf)
{
	if (rf == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
		TXYH2CCS::CCS2XYHs(pv);
	else if (rf == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		TXYH2CCS::CCS2XYHg2000Machine(pv);
	else if (rf == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		TXYH2CCS::CCS2XYHg1985Machine(pv);
}
