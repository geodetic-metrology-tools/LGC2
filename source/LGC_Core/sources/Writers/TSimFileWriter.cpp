////////////////////////////////////////////////////////////////////
// TSimFileWriter.cpp : implementation class
// Write an LGC "input" file with simulated values for observations
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include "TSimFileWriter.h"

#include <TLGCData.h>

#include "TSeparatedFormatTStream.h"
//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TSimFileWriter::TSimFileWriter() : TAFileWriter()
{ // default constructor
}

TSimFileWriter::TSimFileWriter(TAStreamFormatter *stream, const TLGCData *project) : TAFileWriter(stream, project), data(project)
{ // constructor
	// set comments to write back in the input file
	stream->setComments(const_cast<TLGCData *>(project)->getComments());
}

TSimFileWriter::~TSimFileWriter()
{ // destructor
}

//////////////////////////
// public member functions
//////////////////////////

void TSimFileWriter::writeFile(const std::string error)
{ // write error messages from project
	writeError(error);
}

//////////////////////////////////////////////////////////////////////
// write the sim obs lgc file used when there's no error in the project
//////////////////////////////////////////////////////////////////////
void TSimFileWriter::writeFile()
{
	TAStreamFormatter *stream = getStream();
	// reformat the streamformatter
	stream->setLengthUnits(TLength::EUnits::kMetres);
	stream->setPrecisionFormat(fProjectData->getConfig().outPrecision.digits);

	writeHeader();
	writeInstrument();
	writeData(fProjectData->getTree().begin());

	(*stream) << "*END" << endl;

	return;
}

void TSimFileWriter::writeHeader()
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	(*stream) << "*TITR" << endl;
	(*stream) << data->getConfig().title << endl;
	(*stream) << "DANS CE FICHIER, LES OBSERVATIONS SONT SIMULEES !" << endl;

	if (data->getConfig().referential == 106)
		(*stream) << "*LEP" << endl;
	else if (data->getConfig().referential == 107)
		(*stream) << "*SPHE" << endl;
	else if (data->getConfig().referential == 104)
		(*stream) << "*RS2K" << endl;
	else
		(*stream) << "*OLOC" << endl;

	if (data->getConfig().allfixed.isActive())
		(*stream) << "*ALLFIXED" << endl;
	else if (data->getConfig().libre.isActive())
		(*stream) << "*LIBR" << endl;

	if (data->getConfig().covar.isActive())
		(*stream) << "*COVAR " << endl;

	if (data->getConfig().faut.isActive())
		(*stream) << "*FAUT " << data->getConfig().faut.alpha << "  " << data->getConfig().faut.beta << endl;

	if (data->getConfig().histo.isActive())
		(*stream) << "*HIST" << endl;

	if (data->getConfig().nodup.isActive())
		(*stream) << "*NODUP" << endl;

	if (data->getConfig().useApriori.isActive())
		(*stream) << "*APRI" << endl;
	if (!data->getConfig().fRelErrors.points.empty())
	{
		(*stream) << "*EREL" << endl;
		for (const auto &erelTuple : data->getConfig().fRelErrors.points)
		{
			(*stream) << erelTuple.getPoint1() << "  " << erelTuple.getPoint2() << "  " << erelTuple.getDestinationFrame() << endl;
		}
	}
    if (!data->getConfig().fRelErrors.frames.empty())
	{
		(*stream) << "*ERELFRAME" << endl;
		for (const auto &erelTuple : data->getConfig().fRelErrors.frames)
		{
			(*stream) << erelTuple.getFromFrame() << "  " << erelTuple.getToFrame() << endl;
		}
	}


	if (data->getConfig().writeDefa.isActive())
		(*stream) << "*DEFA" << endl;

	if (data->getConfig().outPrecision.digits != 5)
		(*stream) << "*PREC " << data->getConfig().outPrecision.digits << endl;

	if (data->getConfig().errorEllipses.isActive())
		(*stream) << "*PRES" << endl;

	if (data->getConfig().CustomOutputSeparatorPunch.isActive())
		(*stream) << "*FMTP SEP \"" << data->getConfig().CustomOutputSeparatorPunch.separator << "\"" << endl;

	if (data->getConfig().writePunch.isActive())
		if (data->getConfig().writePunch.fmode == TLGCConfig::TCoordOut::eMode::kE)
			(*stream) << "*PUNC E" << endl;
		else if (data->getConfig().writePunch.fmode == TLGCConfig::TCoordOut::eMode::kEE)
			(*stream) << "*PUNC EE" << endl;
		else if (data->getConfig().writePunch.fmode == TLGCConfig::TCoordOut::eMode::kH)
			(*stream) << "*PUNC H" << endl;
		else if (data->getConfig().writePunch.fmode == TLGCConfig::TCoordOut::eMode::kHN)
			(*stream) << "*PUNC HN" << endl;
		else if (data->getConfig().writePunch.fmode == TLGCConfig::TCoordOut::eMode::kHZ)
			(*stream) << "*PUNC HZ" << endl;
		else if (data->getConfig().writePunch.fmode == TLGCConfig::TCoordOut::eMode::kZ)
			(*stream) << "*PUNC Z" << endl;
		else if (data->getConfig().writePunch.fmode == TLGCConfig::TCoordOut::eMode::kZHN)
			(*stream) << "*PUNC ZHN" << endl;
		else if (data->getConfig().writePunch.fmode == TLGCConfig::TCoordOut::eMode::kT)
			(*stream) << "*PUNC T" << endl;
		else if (data->getConfig().writePunch.fmode == TLGCConfig::TCoordOut::eMode::kOUT1)
			(*stream) << "*PUNC OUT1" << endl;
		else
			(*stream) << "*PUNC" << endl;

	if (data->getConfig().writeJSON.isActive())
	{
		(*stream) << "*JSON";
		if (data->getConfig().writeJSON_COVAR.isActive())
			(*stream) << sep << "COVAR";
		(*stream) << "\n";
	}
}

void TSimFileWriter::writeInstrument()
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	(*stream) << "*INSTR" << endl;
	for (auto &itCAMD : data->getInstruments().fCAMD)
	{
		(*stream) << "*CAMD " << itCAMD.second->ID << sep << itCAMD.second->defTarget << sep << itCAMD.second->sigmaInstrCentering.getMMetresValue() << sep << endl;
		for (auto &itTarget : itCAMD.second->targets)
			(*stream) << itTarget.second->ID << sep << itTarget.second->sigmaX * M2MM << sep << itTarget.second->sigmaY * M2MM << sep
					  << itTarget.second->sigmaDist.getMMetresValue() << sep << itTarget.second->sigmaTargetCentering.getMMetresValue() << endl;
	}

	for (auto &itPOLAR : data->getInstruments().fPOLAR)
	{
		(*stream) << "*POLAR " << itPOLAR.second->ID << sep << itPOLAR.second->defTarget << sep << itPOLAR.second->instrHeight.getMetresValue() << sep
				  << itPOLAR.second->sigmaInstrHeight.getMMetresValue() << sep << itPOLAR.second->sigmaInstrCentering.getMMetresValue() << sep
				  << itPOLAR.second->constAngle.getGonsValue() << sep << endl;
		for (auto &itTarget : itPOLAR.second->targets)
			(*stream) << itTarget.second->ID << sep << itTarget.second->sigmaAngl.getSignedCCValue() << sep << itTarget.second->sigmaZenD.getSignedCCValue() << sep
					  << itTarget.second->sigmaDist.getMMetresValue() << sep << itTarget.second->ppmDist.getMMetresValue() << sep
					  << itTarget.second->distCorrectionUnknown << sep << itTarget.second->distCorrectionValue.getMetresValue() << sep
					  << itTarget.second->sigmaDCorr.getMMetresValue() << sep << itTarget.second->sigmaTargetCentering.getMMetresValue() << sep
					  << itTarget.second->targetHt.getMetresValue() << sep << itTarget.second->sigmaTargetHt.getMMetresValue() << sep << endl;
	}

	for (auto &itEDM : data->getInstruments().fEDM)
	{
		(*stream) << "*EDM " << itEDM.second->ID << sep << itEDM.second->defTarget << sep << itEDM.second->instrHeight.getMetresValue() << sep
				  << itEDM.second->sigmaInstrHeight.getMMetresValue() << sep << itEDM.second->sigmaInstrCentering.getMMetresValue() << sep << endl;
		for (auto &itTarget : itEDM.second->targets)
			(*stream) << itTarget.second->ID << sep << itTarget.second->sigmaDSpt.getMMetresValue() << sep << itTarget.second->ppmDSpt.getMMetresValue() << sep
					  << itTarget.second->distCorrectionUnknown << sep << itTarget.second->distCorrectionValue.getMetresValue() << sep
					  << itTarget.second->sigmaDCorr.getMMetresValue() << sep << itTarget.second->sigmaTargetCentering.getMMetresValue() << sep
					  << itTarget.second->targetHt.getMetresValue() << sep << itTarget.second->sigmaTargetHt.getMMetresValue() << sep << endl;
	}

	for (auto &itLEVEL : data->getInstruments().fLEVEL)
	{
		(*stream) << "*LEVEL " << itLEVEL.second->ID << sep << itLEVEL.second->defStaffID << sep << itLEVEL.second->collAngleUnknown << sep
				  << itLEVEL.second->collAngleValue.getGonsValue() << sep << endl;
		for (auto &itTarget : itLEVEL.second->targets)
			(*stream) << itTarget.second->ID << sep << itTarget.second->sigmaD.getMMetresValue() << sep << itTarget.second->ppmD.getMMetresValue() << sep
					  << itTarget.second->distCorrectionValue.getMetresValue() << sep << itTarget.second->sigmaDCorr.getMMetresValue() << sep
					  << itTarget.second->staffHt.getMetresValue() << sep << itTarget.second->sigmaStaffHt.getMMetresValue() << sep << endl;
	}

	for (auto &itSCALE : data->getInstruments().fSCALE)
	{
		(*stream) << "*SCALE " << itSCALE.second->ID << sep << itSCALE.second->sigmaD.getMMetresValue() << sep << itSCALE.second->ppmD.getMMetresValue() << sep
				  << itSCALE.second->distCorrectionValue.getMetresValue() << sep << itSCALE.second->sigmaDCorr.getMMetresValue() << sep
				  << itSCALE.second->sigmaInstrCentering.getMMetresValue() << sep << endl;
	}

	for (auto &itINCL : data->getInstruments().fINCL)
	{
		(*stream) << "*INCL " << itINCL.second->ID << sep << itINCL.second->sigmaAngl.getSignedCCValue() << sep << itINCL.second->angleCorrectionValue.getGonsValue()
				  << sep << itINCL.second->sigmaCorrectionValue.getSignedCCValue() << sep << itINCL.second->refAngleCorrectionValue.getGonsValue() << sep
				  << itINCL.second->refSigmaCorrectionValue.getSignedCCValue() << sep << endl;
	}

	for (auto &itHLSR : data->getInstruments().fHLSR)
	{
		(*stream) << "*HLSR " << itHLSR.second->ID << sep << itHLSR.second->sigmaDist.getMMetresValue() << sep << itHLSR.second->sigmaInstrHeight.getMMetresValue() << sep
				  << itHLSR.second->sigmaInstrCentering.getMMetresValue() << sep << endl;
	}

	for (auto &itWPSR : data->getInstruments().fWPSR)
	{
		(*stream) << "*WPSR " << itWPSR.second->ID << sep << itWPSR.second->sigmaX.getMMetresValue() << sep << itWPSR.second->sigmaZ.getMMetresValue() << sep
				  << itWPSR.second->sigmaCombinedX.getMMetresValue() << sep << itWPSR.second->sigmaCombinedZ.getMMetresValue() << sep << endl;
	}
}

void TSimFileWriter::writeData(TDataTreeIterator itTree)
{
	if (itTree == fProjectData->getTree().end())
		return;

	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	writeFrameHeader(itTree);
	writePoint(itTree);
	writeMeasurement(itTree);

	// write the first child
	if (itTree.node->first_child)
	{
		writeData(itTree.node->first_child);

		// write the other child
		for (auto child = itTree.node->first_child; child != itTree.node->last_child;)
			writeData(child = child->next_sibling);
	}

	// root not need to be closed
	if (!itTree->get()->isROOTNode())
		(*stream) << "*ENDFRAME" << endl;
}

void TSimFileWriter::writeFrameHeader(TDataTreeIterator frameIt)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();
	stream->setPrecisionFormat(fProjectData->getConfig().outPrecision.digits);

	if (!frameIt->get()->isROOTNode())
	{
		(*stream) << "*FRAME" << sep << frameIt->get()->frame.getName() << sep << frameIt->get()->frame.getProvTranslation(0).getMetresValue() << sep
				  << frameIt->get()->frame.getProvTranslation(1).getMetresValue() << sep << frameIt->get()->frame.getProvTranslation(2).getMetresValue() << sep
				  << frameIt->get()->frame.getProvRotation(0).getGonsValue() << sep << frameIt->get()->frame.getProvRotation(1).getGonsValue() << sep
				  << frameIt->get()->frame.getProvRotation(2).getGonsValue() << sep << frameIt->get()->frame.getProvScale() << sep;

		if (frameIt->get()->frame.hasTranslStandDev(0))
			(*stream) << "STX" << sep << frameIt->get()->frame.getTranslationStandDev(0).getMMetresValue() << sep;
		else if (!frameIt->get()->frame.isTranslationFixed(0))
			(*stream) << "TX" << sep;
		if (frameIt->get()->frame.hasTranslStandDev(1))
			(*stream) << "STY" << sep << frameIt->get()->frame.getTranslationStandDev(1).getMMetresValue() << sep;
		else if (!frameIt->get()->frame.isTranslationFixed(1))
			(*stream) << "TY" << sep;
		if (frameIt->get()->frame.hasTranslStandDev(2))
			(*stream) << "STZ" << sep << frameIt->get()->frame.getTranslationStandDev(2).getMMetresValue() << sep;
		else if (!frameIt->get()->frame.isTranslationFixed(2))
			(*stream) << "TZ" << sep;

		if (frameIt->get()->frame.hasRotationStandDev(0))
			(*stream) << "SRX" << sep << frameIt->get()->frame.getRotationStandDev(0).getSignedCCValue() << sep;
		else if (!frameIt->get()->frame.isRotationFixed(0))
			(*stream) << "RX" << sep;
		if (frameIt->get()->frame.hasRotationStandDev(1))
			(*stream) << "SRY" << sep << frameIt->get()->frame.getRotationStandDev(1).getSignedCCValue() << sep;
		else if (!frameIt->get()->frame.isRotationFixed(1))
			(*stream) << "RY" << sep;
		if (frameIt->get()->frame.hasRotationStandDev(2))
			(*stream) << "SRZ" << sep << frameIt->get()->frame.getRotationStandDev(2).getSignedCCValue() << sep;
		else if (!frameIt->get()->frame.isRotationFixed(2))
			(*stream) << "RZ" << sep;

		if (frameIt->get()->frame.hasScaleStandDev())
			(*stream) << "SSCL" << sep << frameIt->get()->frame.getScaleStandDev() * M2MM << sep;
		else if (!frameIt->get()->frame.isScaleFixed())
			(*stream) << "SCL" << sep;

		// check if it is part of a slave group
		for (LGCFrameConstraintGroup group : fProjectData->getSlaveGroups())
		{
			if (group.isPartOfGroup(frameIt->get()->frame.getName()))
			{
				(*stream) << "SLAVE" << sep;
				(*stream) << group.getGroupName() << sep;
				break;
			}
		}

		(*stream) << endl;
	}
}

void TSimFileWriter::writePoint(TDataTreeIterator frameIt)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();
	stream->setPrecisionFormat(data->getConfig().outPrecision.digits);

	// lambda function to write the point coordinate
	auto writeXYZorH = [&](LGCAdjustablePoint const &fPoint) {
		if (!fPoint.isActive())
			(*stream) << DEACTIVATION_CHAR;

		if ((data->getConfig().referential == 106 || data->getConfig().referential == 107 || data->getConfig().referential == 104) && frameIt->get()->isROOTNode())
			(*stream) << fPoint.getName() << sep << fPoint.getProvisionalValue().getX() << sep << fPoint.getProvisionalValue().getY() << sep
					  << fPoint.getProvisionalHeightInRoot() << sep << fPoint.eolcomment << endl;
		else
			(*stream) << fPoint.getName() << sep << fPoint.getProvisionalValue().getX() << sep << fPoint.getProvisionalValue().getY() << sep
					  << fPoint.getProvisionalValue().getZ() << sep << fPoint.eolcomment << endl;
	};

	// write PDOR if we are in ROOT & PDOR is used
	if (frameIt->get()->isROOTNode())
	{
		const auto &pdor = data->getConfig().pdor;

		// Write pdor, if the point name is not an empty string
		// (to enable saving a deactivated pdor with existing
		// point name and possible bearing),
		// set activation status with deactivation character
		if (pdor.isActive() || pdor.fptname != "")
		{
			// Set inactive if necessary:
			if (!pdor.isActive())
				(*stream) << DEACTIVATION_CHAR;

			(*stream) << "*PDOR" << endl;

			// Set second line inactive if necessary:
			// (This has no effect on the reader, but for clarity...)
			if (!pdor.isActive())
				(*stream) << DEACTIVATION_CHAR;

			(*stream) << data->getConfig().pdor.fptname;

			if (pdor.hasBearing)
				(*stream) << sep << data->getConfig().pdor.fgis.getGonsValue();

			(*stream) << endl;
		}
	}

	// Write point list
	// define an unknown point type
	TSpatialStatus::ESpatialStatus previousPointType(TSpatialStatus::kUnknown);
	for (auto &point : data->getPoints())
	{
		TSpatialStatus::ESpatialStatus currentPointType = point.getSpatialStatus();
		TDataTreeIterator posInTree = point.getFrameTreePosition();

		if (posInTree == frameIt)
		{
			// check if it is a new point type, if so write type header
			if (previousPointType != currentPointType)
			{
				// write header
				switch (currentPointType)
				{
				case TSpatialStatus::ESpatialStatus::kCala:
					(*stream) << "*CALA" << endl;
					break;
				case TSpatialStatus::ESpatialStatus::kVxyz:
					(*stream) << "*POIN" << endl;
					break;
				case TSpatialStatus::ESpatialStatus::kVxy:
					(*stream) << "*VXY" << endl;
					break;
				case TSpatialStatus::ESpatialStatus::kVxz:
					(*stream) << "*VXZ" << endl;
					break;
				case TSpatialStatus::ESpatialStatus::kVyz:
					(*stream) << "*VYZ" << endl;
					break;
				case TSpatialStatus::ESpatialStatus::kVz:
					(*stream) << "*VZ" << endl;
					break;
				}
			}
			writeXYZorH(point);
			// copy current point status to point type for checking whether the next point has same type
			previousPointType = currentPointType;
		}
	}
}

void TSimFileWriter::writeMeasurement(TDataTreeIterator frameIt)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	for (auto meas : frameIt->get()->measurements.fTSTN)
		writeTSTNMeas(meas);

	for (auto &meas : frameIt->get()->measurements.fCAM)
		writeCAMMeas(&meas);

	for (auto &meas : frameIt->get()->measurements.fEDM)
		writeEDMMeas(&meas);

	for (auto &meas : frameIt->get()->measurements.fLEVEL)
		writeLEVELMeas(&meas);

	if (!frameIt->get()->measurements.fDVER.empty())
	{
		if (!frameIt->get()->measurements.dverActive)
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << "*DVER" << endl;
		for (auto &meas : frameIt->get()->measurements.fDVER)
			writeDVERMeas(&meas);
	}

	for (auto &meas : frameIt->get()->measurements.fECHO)
		writeECHOMeas(&meas);

	for (auto &meas : frameIt->get()->measurements.fECVE)
		writeECVEMeas(&meas);

	for (auto &meas : frameIt->get()->measurements.fECSP)
		writeECSPMeas(&meas);

	for (auto &meas : frameIt->get()->measurements.fORIE)
		writeORIEMeas(&meas);

	for (auto &meas : frameIt->get()->measurements.fECWS)
		writeECWSMeas(&meas);

	for (auto &meas : frameIt->get()->measurements.fECWI)
		writeECWIMeas(&meas);

	if (!frameIt->get()->measurements.fRADI.empty())
	{
		if (!frameIt->get()->measurements.radiActive)
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << "*RADI" << endl;
		for (auto &meas : frameIt->get()->measurements.fRADI)
			writeRADIMeas(&meas);
	}

	if (!frameIt->get()->measurements.fOBSXYZ.empty())
	{
		if (!frameIt->get()->measurements.obsxyzActive)
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << "*OBSXYZ" << endl;
		for (auto &meas : frameIt->get()->measurements.fOBSXYZ)
			writeOBSXYZMeas(&meas);
	}

	if (!frameIt->get()->measurements.fINCLY.empty())
	{
		for (auto &meas : frameIt->get()->measurements.fINCLY)
			writeINCLYMeas(&meas);
	}
}

void TSimFileWriter::writeCAMMeas(TCAM *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	auto edmDefInst = data->getInstruments().fCAMD.at(meas->instrument.ID);
	auto camDefTarget = meas->instrument.targets.at(meas->instrument.defTarget);

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << "*CAM" << sep << meas->instrumentPos->getName() << sep << meas->instrument.ID << sep;

	if (camDefTarget->ID != edmDefInst->defTarget)
		(*stream) << "TRGT" << sep << camDefTarget->ID << sep;

	if (meas->instrument.sigmaInstrCentering != edmDefInst->sigmaInstrCentering)
		(*stream) << "ICSE" << sep << meas->instrument.sigmaInstrCentering.getMMetresValue() << sep;

	(*stream) << endl;

	if (!meas->measUVD.empty())
	{
		auto romDefTarget = *camDefTarget;

		if (!meas->uvdActive)
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << "*UVD" << endl;

		for (auto &uvd : meas->measUVD)
		{
			if (!uvd.isActive())
				(*stream) << DEACTIVATION_CHAR;

			(*stream) << uvd.targetPos->getName() << sep << uvd.getVectorValue().getX() << sep << uvd.getVectorValue().getY() << sep << uvd.getVectorValue().getZ() << sep
					  << uvd.getDistance().getMetresValue() << sep;

			if (uvd.target.ID != romDefTarget.ID)
				(*stream) << "TRGT" << sep << uvd.target.ID << sep;

			if (uvd.target.sigmaTargetCentering != romDefTarget.sigmaTargetCentering)
				(*stream) << "TCSE" << sep << uvd.target.sigmaTargetCentering.getMMetresValue() << sep;

			if (uvd.target.sigmaX != romDefTarget.sigmaX)
				(*stream) << "XSE" << sep << uvd.target.sigmaX * M2MM << sep;

			if (uvd.target.sigmaY != romDefTarget.sigmaY)
				(*stream) << "YSE" << sep << uvd.target.sigmaY * M2MM << sep;

			if (uvd.target.sigmaDist != romDefTarget.sigmaDist)
				(*stream) << "DSE" << sep << uvd.target.sigmaDist.getMMetresValue() << sep;

			(*stream) << endl;

			// Update the rom def target if necessary
			if (uvd.target.ID != romDefTarget.ID)
				romDefTarget = uvd.target;
		}
	}

	if (!meas->measUVEC.empty())
	{
		auto romDefTarget = *camDefTarget;

		if (!meas->uvecActive)
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << "*UVEC" << endl;

		for (auto &uvec : meas->measUVEC)
		{
			if (!uvec.isActive())
				(*stream) << DEACTIVATION_CHAR;

			(*stream) << uvec.targetPos->getName() << sep << uvec.getVectorValue().getX() << sep << uvec.getVectorValue().getY() << sep << uvec.getVectorValue().getZ() << sep;

			if (uvec.target.ID != romDefTarget.ID)
				(*stream) << "TRGT" << sep << uvec.target.ID << sep;

			if (uvec.target.sigmaTargetCentering != romDefTarget.sigmaTargetCentering)
				(*stream) << "TCSE" << sep << uvec.target.sigmaTargetCentering.getMMetresValue() << sep;

			if (uvec.target.sigmaX != romDefTarget.sigmaX)
				(*stream) << "XSE" << sep << uvec.target.sigmaX * M2MM << sep;

			if (uvec.target.sigmaY != romDefTarget.sigmaY)
				(*stream) << "YSE" << sep << uvec.target.sigmaY * M2MM << sep;

			(*stream) << endl;

			// Update the rom def target if necessary
			if (uvec.target.ID != romDefTarget.ID)
				romDefTarget = uvec.target;
		}
	}
}

void TSimFileWriter::writeDVERMeas(TDVER *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	// write the list of measurements
	(*stream) << meas->station->getName() << sep << meas->targetPos->getName() << sep << meas->getDistance().getMetresValue() << sep << "OBSE" << sep
			  << meas->getObservedStDev().getMMetresValue() << sep;

	if (meas->getDistanceCorrection().getMetresValue() != 0)
		(*stream) << "DCOR" << sep << meas->getDistanceCorrection().getMetresValue() << sep;

	(*stream) << endl;
}

void TSimFileWriter::writeECHOMeas(TECHOROM *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	auto scaleDefInst = *data->getInstruments().fSCALE.at(meas->measECHO.front().target.ID);

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << "*ECHO" << sep << scaleDefInst.ID << endl;

	// write the list of measurements for the line
	for (auto &itECHO : meas->measECHO)
	{
		if (!itECHO.isActive())
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << itECHO.targetPos->getName() << sep << itECHO.getDistance().getMetresValue() << sep;

		if (itECHO.target.ID != scaleDefInst.ID)
			(*stream) << "SCALE" << sep << itECHO.target.ID << sep;

		if (itECHO.target.sigmaD != scaleDefInst.sigmaD)
			(*stream) << "OBSE" << sep << itECHO.target.sigmaD.getMMetresValue() << sep;

		if (itECHO.target.ppmD != scaleDefInst.ppmD)
			(*stream) << "PPM" << sep << itECHO.target.ppmD.getMMetresValue() << sep;

		if (itECHO.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
			(*stream) << "ICSE" << sep << itECHO.target.sigmaInstrCentering.getMMetresValue() << sep;

		(*stream) << endl;

		// Update the default target if necessary
		if (itECHO.target.ID != scaleDefInst.ID)
			scaleDefInst = itECHO.target;
	}
}

void TSimFileWriter::writeECVEMeas(TECVEROM *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	auto scaleDefInst = *data->getInstruments().fSCALE.at(meas->measECVE.front().target.ID);

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << "*ECVE" << sep << scaleDefInst.ID << sep;

	// write point on the line if it is already defined
	for (auto &point : data->getPoints())
		if (point.getName() == meas->fMeasuredLine->getLinePoint()->getName())
			(*stream) << "PtLine" << sep << meas->fMeasuredLine->getLinePoint()->getName() << sep;

	(*stream) << endl;

	// write the list of measurements for the line
	for (auto &itECVE : meas->measECVE)
	{
		if (!itECVE.isActive())
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << itECVE.targetPos->getName() << sep << itECVE.getDistance().getMetresValue() << sep;

		if (itECVE.target.ID != scaleDefInst.ID)
			(*stream) << "SCALE" << sep << itECVE.target.ID << sep;

		if (itECVE.target.sigmaD != scaleDefInst.sigmaD)
			(*stream) << "OBSE" << sep << itECVE.target.sigmaD.getMMetresValue() << sep;

		if (itECVE.target.ppmD != scaleDefInst.ppmD)
			(*stream) << "PPM" << sep << itECVE.target.ppmD.getMMetresValue() << sep;

		if (itECVE.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
			(*stream) << "ISCE" << sep << itECVE.target.sigmaInstrCentering.getMMetresValue() << sep;

		(*stream) << endl;

		// Update the default target if necessary
		if (itECVE.target.ID != scaleDefInst.ID)
			scaleDefInst = itECVE.target;
	}
}

void TSimFileWriter::writeECSPMeas(TECSPROM *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	auto scaleDefInst = *data->getInstruments().fSCALE.at(meas->measECSP.front().target.ID);

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << "*ECSP" << sep << meas->p1->getName() << sep << meas->p2->getName() << sep << scaleDefInst.ID << sep;
	(*stream) << endl;

	for (auto &ecsp : meas->measECSP)
	{
		if (!ecsp.isActive())
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << ecsp.targetPos->getName() << sep << ecsp.getDistance().getMetresValue() << sep;

		if (ecsp.target.ID != scaleDefInst.ID)
			(*stream) << "SCALE" << sep << ecsp.target.ID << sep;

		if (ecsp.target.sigmaD != scaleDefInst.sigmaD)
			(*stream) << "OBSE" << sep << ecsp.target.sigmaD.getMMetresValue() << sep;

		if (ecsp.target.ppmD != scaleDefInst.ppmD)
			(*stream) << "PPM" << sep << ecsp.target.ppmD.getMMetresValue() << sep;

		if (ecsp.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
			(*stream) << "ICSE" << sep << ecsp.target.sigmaInstrCentering.getMMetresValue() << sep;

		(*stream) << endl;

		// Update the default target if necessary
		if (ecsp.target.ID != scaleDefInst.ID)
			scaleDefInst = ecsp.target;
	}
}

void TSimFileWriter::writeEDMMeas(TEDM *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	auto edmDefInst = *data->getInstruments().fEDM.at(meas->instrument.ID);
	auto romDefTarget = *meas->instrument.targets.at(meas->instrument.defTarget);

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << "*DSPT" << sep << meas->instrumentPos->getName() << sep << meas->instrument.ID << sep;

	if (meas->instrument.defTarget != edmDefInst.defTarget)
		(*stream) << "TRGT" << sep << romDefTarget.ID << sep;

	if (meas->instrument.instrHeight != edmDefInst.instrHeight)
		(*stream) << "IH" << sep << meas->instrument.instrHeight.getMetresValue() << sep;

	if (meas->instrument.sigmaInstrHeight != edmDefInst.sigmaInstrHeight)
		(*stream) << "IHSE" << sep << meas->instrument.sigmaInstrHeight.getMMetresValue() << sep;

	if (meas->instrument.sigmaInstrCentering != edmDefInst.sigmaInstrCentering)
		(*stream) << "ICSE" << sep << meas->instrument.sigmaInstrCentering.getMMetresValue() << sep;

	(*stream) << endl;

	// write the list of measurements
	for (auto &itDspt : meas->measDSPT)
	{
		if (!itDspt.isActive())
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << itDspt.targetPos->getName() << sep << itDspt.getDistance().getMetresValue() << sep;

		if (itDspt.target.ID != romDefTarget.ID)
			(*stream) << "TRGT" << sep << itDspt.target.ID << sep;

		if (itDspt.target.sigmaDSpt != romDefTarget.sigmaDSpt)
			(*stream) << "OBSE" << sep << itDspt.target.sigmaDSpt.getMMetresValue() << sep;

		if (itDspt.target.ppmDSpt != romDefTarget.ppmDSpt)
			(*stream) << "PPM" << sep << itDspt.target.ppmDSpt.getMMetresValue() << sep;

		if (itDspt.target.targetHt != romDefTarget.targetHt)
			(*stream) << "TH" << sep << itDspt.target.targetHt.getMetresValue() << sep;

		if (itDspt.target.sigmaTargetHt != romDefTarget.sigmaTargetHt)
			(*stream) << "THSE" << sep << itDspt.target.sigmaTargetHt.getMMetresValue() << sep;

		if (itDspt.target.sigmaTargetCentering != romDefTarget.sigmaTargetCentering)
			(*stream) << "TCSE" << sep << itDspt.target.sigmaTargetCentering.getMMetresValue() << sep;

		(*stream) << endl;

		// Update the rom def target if necessary
		if (itDspt.target.ID != romDefTarget.ID)
			romDefTarget = itDspt.target;
	}
}

void TSimFileWriter::writeLEVELMeas(TLEVEL *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	auto romDefStaff = *meas->instrument.targets.at(meas->instrument.defStaffID);

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << "*DLEV" << sep << meas->instrument.ID << sep;

	// write point on the line if it is already defined
	if (meas->fMeasuredPlane->getReferencePoint())
		for (auto &point : data->getPoints())
			if (point.getName() == meas->fMeasuredPlane->getReferencePoint()->getName())
				(*stream) << "RefPt" << sep << meas->fMeasuredPlane->getReferencePoint()->getName() << sep;

	(*stream) << endl;

	// write measurement for the plane
	for (auto &itDLEV : meas->measDLEV)
	{
		if (!itDLEV.isActive())
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << itDLEV.targetPos->getName() << sep << itDLEV.getDistance().getMetresValue() << sep;

		if (itDLEV.dhor.get())
			(*stream) << "DHOR" << sep << itDLEV.dhor.get()->getDistance().getMetresValue() << sep << "DSE" << sep << itDLEV.dhor.get()->getDHORSigma().getMMetresValue() << sep;

		if (itDLEV.target.ID != romDefStaff.ID)
			(*stream) << "TRGT" << sep << itDLEV.target.ID << sep;

		if (itDLEV.target.sigmaD != romDefStaff.sigmaD)
			(*stream) << "OBSE" << sep << itDLEV.target.sigmaD.getMMetresValue() << sep;

		if (itDLEV.target.ppmD != romDefStaff.ppmD)
			(*stream) << "PPM" << sep << itDLEV.target.ppmD.getMMetresValue() << sep;

		if (itDLEV.target.staffHt != romDefStaff.staffHt)
			(*stream) << "TH" << sep << itDLEV.target.staffHt.getMetresValue() << sep;

		if (itDLEV.target.sigmaStaffHt != romDefStaff.sigmaStaffHt)
			(*stream) << "THSE" << sep << itDLEV.target.sigmaStaffHt.getMMetresValue() << sep;

		(*stream) << endl;

		// Update the rom def target if necessary
		if (itDLEV.target.ID != romDefStaff.ID)
			romDefStaff = itDLEV.target;
	}
}

void TSimFileWriter::writeORIEMeas(TORIEROM *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	auto polarDefInst = *data->getInstruments().fPOLAR.at(meas->instrument.ID);
	auto romDefTarget = *meas->instrument.targets.at(meas->instrument.defTarget);

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << "*ORIE" << sep << meas->instrumentPos->getName() << sep << meas->instrument.ID << sep;

	if (romDefTarget.ID != polarDefInst.defTarget)
		(*stream) << "TRGT" << sep << romDefTarget.ID << sep;

	if (meas->instrument.sigmaInstrCentering != polarDefInst.sigmaInstrCentering)
		(*stream) << "ICSE" << sep << meas->instrument.sigmaInstrCentering.getMMetresValue() << sep;

	if (meas->fConstantAngle.getGonsValue() != polarDefInst.constAngle.getGonsValue())
		(*stream) << "CST" << sep << meas->fConstantAngle.getGonsValue() << sep;

	(*stream) << endl;

	// write the list of measurements for the line
	for (auto &itORIE : meas->measORIE)
	{
		if (!itORIE.isActive())
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << itORIE.targetPos->getName() << sep << itORIE.getAngle().getGonsValue() << sep;

		if (itORIE.target.ID != romDefTarget.ID)
			(*stream) << "TRGT" << sep << itORIE.target.ID << sep;

		if (itORIE.target.sigmaAngl != romDefTarget.sigmaAngl)
			(*stream) << "OBSE" << sep << itORIE.target.sigmaAngl.getSignedCCValue() << sep;

		if (itORIE.target.sigmaTargetCentering != romDefTarget.sigmaTargetCentering)
			(*stream) << "TCSE" << sep << itORIE.target.sigmaTargetCentering.getMMetresValue() << sep;

		(*stream) << endl;

		// Update the rom def target if necessary
		if (itORIE.target.ID != romDefTarget.ID)
			romDefTarget = itORIE.target;
	}
}

void TSimFileWriter::writeRADIMeas(TRADI *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << meas->station->getName() << sep << meas->getAngleCnstr().getGonsValue() << sep << "SIGMA" << sep << meas->getObservedStDev().getMMetresValue() << endl;
}

void TSimFileWriter::writeOBSXYZMeas(TOBSXYZ *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << meas->station->getName() << sep << meas->obsValue.getX() << sep << meas->obsValue.getY() << sep << meas->obsValue.getZ() << sep
			  << meas->getXObservedStDev().getMMetresValue() << sep << meas->getYObservedStDev().getMMetresValue() << sep << meas->getZObservedStDev().getMMetresValue() << endl;
}

void TSimFileWriter::writeECWSMeas(TECWSROM *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	auto hlsrDefInst = *data->getInstruments().fHLSR.at(meas->measECWS.front().target.ID);

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << "*ECWS" << sep << hlsrDefInst.ID << sep << meas->sigmaWS.getMMetresValue() << sep << "WSID" << sep << meas->romName << endl;
	;

	// write the list of measurements for the line
	for (auto &itECWS : meas->measECWS)
	{
		if (!itECWS.isActive())
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << itECWS.targetPos->getName() << sep << itECWS.getDistance().getMetresValue() << sep;

		if (itECWS.target.ID != hlsrDefInst.ID)
			(*stream) << "INSTR" << sep << itECWS.target.ID << sep;

		// Update the default target if necessary, to do before updating other values to avoid unecessary values.
		if (itECWS.target.ID != hlsrDefInst.ID)
			hlsrDefInst = *data->getInstruments().fHLSR.at(itECWS.target.ID);

		if (itECWS.target.sigmaDist != hlsrDefInst.sigmaDist)
			(*stream) << "OBSE" << sep << itECWS.target.sigmaDist.getMMetresValue() << sep;

		if (itECWS.target.sigmaInstrHeight != hlsrDefInst.sigmaInstrHeight)
			(*stream) << "IHSE" << sep << itECWS.target.sigmaInstrHeight.getMMetresValue() << sep;

		if (itECWS.target.sigmaInstrCentering != hlsrDefInst.sigmaInstrCentering)
			(*stream) << "ICSE" << sep << itECWS.target.sigmaInstrCentering.getMMetresValue() << sep;

		if (itECWS.target.sigmaWS != meas->sigmaWS)
			(*stream) << "WSSE" << sep << itECWS.target.sigmaWS.getMMetresValue() << sep;

		(*stream) << endl;
	}
}

void TSimFileWriter::writeECWIMeas(TECWIROM *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	auto wpsrDefInst = *data->getInstruments().fWPSR.at(meas->measECWI.front().target.ID);

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << "*ECWI" << sep << wpsrDefInst.ID << sep << meas->sagAdjustable->getProvisionalValue().getMetresValue() << sep << meas->sigmaWire.getMMetresValue() << sep
			  << meas->anchorPtFirst->getName() << sep << meas->anchorPtSecond->getName() << sep << "WIID" << sep << meas->romName; 
	if (meas->sagfix)
	{
		(*stream) << sep << "SAGFIX";
		// use SAGSE flag only if the precision is not zero
		if (fabs(meas->instrument.sigmaSagWire) > EPSILON)
		{
			(*stream) << sep << "SAGSE" << sep << meas->instrument.sigmaSagWire.getMMetresValue();
		}
	}
	(*stream) << endl;

	// write the list of measurements for the line
	for (auto &itECWI : meas->measECWI)
	{
		if (!itECWI.isActive())
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << itECWI.targetPos->getName() << sep << itECWI.getDistance(EECWIDistances::kX).getMetresValue() << sep
				  << itECWI.getDistance(EECWIDistances::kZ).getMetresValue() << sep;

		if (itECWI.target.ID != wpsrDefInst.ID)
			(*stream) << "INSTR" << sep << itECWI.target.ID << sep;

		// Update the default target if necessary, to do before updating other values to avoid unecessary values.
		if (itECWI.target.ID != wpsrDefInst.ID)
			wpsrDefInst = *data->getInstruments().fWPSR.at(itECWI.target.ID);

		if (itECWI.target.sigmaX != wpsrDefInst.sigmaX)
			(*stream) << "XSE" << sep << itECWI.target.sigmaX.getMMetresValue() << sep;

		if (itECWI.target.sigmaZ != wpsrDefInst.sigmaZ)
			(*stream) << "ZSE" << sep << itECWI.target.sigmaZ.getMMetresValue() << sep;

		if (itECWI.target.sigmaInstrCenteringX != wpsrDefInst.sigmaInstrCenteringX)
			(*stream) << "XICSE" << sep << itECWI.target.sigmaInstrCenteringX.getMMetresValue() << sep;

		if (itECWI.target.sigmaInstrCenteringZ != wpsrDefInst.sigmaInstrCenteringZ)
			(*stream) << "ZICSE" << sep << itECWI.target.sigmaInstrCenteringZ.getMMetresValue() << sep;

		(*stream) << endl;
	}
}

void TSimFileWriter::writeTSTNMeas(std::shared_ptr<TTSTN> meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	auto polarDefInst = *data->getInstruments().fPOLAR.at(meas->instrument.ID);
	auto tstnDefTarget = *meas->instrument.targets.at(meas->instrument.defTarget);

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << "*TSTN" << sep << meas->instrumentPos->getName() << sep << meas->instrument.ID << sep;

	if (meas->rot3D)
		(*stream) << "ROT3D" << sep;
	else
	{
		if (meas->ihfix)
		{
			(*stream) << "IHFIX" << sep;
			if (meas->instrument.instrHeight != polarDefInst.instrHeight)
				(*stream) << "IH" << sep << meas->instrument.instrHeight.getMetresValue() << sep;

			if (meas->instrument.sigmaInstrHeight != polarDefInst.sigmaInstrHeight)
				(*stream) << "IHSE" << sep << meas->instrument.sigmaInstrHeight.getMMetresValue() << sep;
		}
	}
	if (tstnDefTarget.ID != polarDefInst.defTarget)
		(*stream) << "TRGT" << sep << tstnDefTarget.ID << sep;

	if (meas->instrument.sigmaInstrCentering != polarDefInst.sigmaInstrCentering)
		(*stream) << "ICSE" << sep << meas->instrument.sigmaInstrCentering.getMMetresValue() << sep;

	(*stream) << endl;

	for (auto &rom : meas->roms)
	{
		auto v0DefTarget = *meas->instrument.targets.at(rom->defaultTargetId);

		if (!rom->isActive())
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << "*V0" << sep;

		if (v0DefTarget.ID != tstnDefTarget.ID)
			(*stream) << "TRGT" << sep << v0DefTarget.ID << sep;

		if (rom->acst != TAngle(0.0))
			(*stream) << "ACST" << sep << rom->acst.getGonsValue() << sep;
		(*stream) << endl;

		// ANGL
		if (!rom->measANGL.empty())
		{
			auto romDefTarget = v0DefTarget;

			if (!rom->anglActive)
				(*stream) << DEACTIVATION_CHAR;

			(*stream) << "*ANGL" << endl;
			for (auto &angl : rom->measANGL)
			{
				if (!angl.isActive())
					(*stream) << DEACTIVATION_CHAR;

				(*stream) << angl.targetPos->getName() << sep << angl.getAngle().getGonsValue() << sep;

				if (angl.target.ID != romDefTarget.ID)
					(*stream) << "TRGT" << sep << angl.target.ID << sep;

				if (angl.target.sigmaAngl != romDefTarget.sigmaAngl)
					(*stream) << "OBSE" << sep << angl.target.sigmaAngl.getSignedCCValue() << sep;

				if (angl.target.sigmaTargetCentering != romDefTarget.sigmaTargetCentering)
					(*stream) << "TCSE" << sep << angl.target.sigmaTargetCentering.getMMetresValue() << sep;

				(*stream) << endl;

				// Update the rom def target if necessary
				if (angl.target.ID != romDefTarget.ID)
					romDefTarget = *meas->instrument.targets.at(angl.target.ID);
			}
		}

		// ZEND
		if (!rom->measZEND.empty())
		{
			auto romDefTarget = v0DefTarget;

			if (!rom->zendActive)
				(*stream) << DEACTIVATION_CHAR;

			(*stream) << "*ZEND" << endl;
			for (auto &zend : rom->measZEND)
			{
				if (!zend.isActive())
					(*stream) << DEACTIVATION_CHAR;

				(*stream) << zend.targetPos->getName() << sep << zend.getAngle().getGonsValue() << sep;

				if (zend.target.ID != romDefTarget.ID)
					(*stream) << "TRGT" << sep << zend.target.ID << sep;

				if (zend.target.sigmaZenD != romDefTarget.sigmaZenD)
					(*stream) << "OBSE" << sep << zend.target.sigmaZenD.getSignedCCValue() << sep;

				if (zend.target.targetHt != romDefTarget.targetHt)
					(*stream) << "TH" << sep << zend.target.targetHt.getMetresValue() << sep;

				if (zend.target.sigmaTargetHt != romDefTarget.sigmaTargetHt)
					(*stream) << "THSE" << sep << zend.target.sigmaTargetHt.getMMetresValue() << sep;

				if (zend.target.sigmaTargetCentering != romDefTarget.sigmaTargetCentering)
					(*stream) << "TCSE" << sep << zend.target.sigmaTargetCentering.getMMetresValue() << sep;

				(*stream) << endl;

				// Update the rom def target if necessary
				if (zend.target.ID != romDefTarget.ID)
					romDefTarget = *meas->instrument.targets.at(zend.target.ID);
			}
		}

		// DIST
		if (!rom->measDIST.empty())
		{
			auto romDefTarget = v0DefTarget;

			if (!rom->distActive)
				(*stream) << DEACTIVATION_CHAR;

			(*stream) << "*DIST" << endl;

			for (auto &dist : rom->measDIST)
			{
				if (!dist.isActive())
					(*stream) << DEACTIVATION_CHAR;

				(*stream) << dist.targetPos->getName() << sep << dist.getDistance().getMetresValue() << sep;

				if (dist.target.ID != romDefTarget.ID)
					(*stream) << "TRGT" << sep << dist.target.ID << sep;

				if (dist.target.sigmaDist != romDefTarget.sigmaDist)
					(*stream) << "OBSE" << sep << dist.target.sigmaDist.getMMetresValue() << sep;

				if (dist.target.ppmDist != romDefTarget.ppmDist)
					(*stream) << "PPM" << sep << dist.target.ppmDist.getMMetresValue() << sep;

				if (dist.target.targetHt != romDefTarget.targetHt)
					(*stream) << "TH" << sep << dist.target.targetHt.getMetresValue() << sep;

				if (dist.target.sigmaTargetHt != romDefTarget.sigmaTargetHt)
					(*stream) << "THSE" << sep << dist.target.sigmaTargetHt.getMMetresValue() << sep;

				if (dist.target.sigmaTargetCentering != romDefTarget.sigmaTargetCentering)
					(*stream) << "TCSE" << sep << dist.target.sigmaTargetCentering.getMMetresValue() << sep;

				(*stream) << endl;

				// Update the rom def target if necessary
				if (dist.target.ID != romDefTarget.ID)
					romDefTarget = *meas->instrument.targets.at(dist.target.ID);
			}
		}

		// DHOR
		if (!rom->measDHOR.empty())
		{
			auto romDefTarget = v0DefTarget;

			if (!rom->dhorActive)
				(*stream) << DEACTIVATION_CHAR;

			(*stream) << "*DHOR" << endl;
			for (auto &dhor : rom->measDHOR)
			{
				if (!dhor.isActive())
					(*stream) << DEACTIVATION_CHAR;

				(*stream) << dhor.targetPos->getName() << sep << dhor.getDistance().getMetresValue() << sep;

				if (dhor.target.ID != romDefTarget.ID)
					(*stream) << "TRGT" << sep << dhor.target.ID << sep;

				if (dhor.target.sigmaDist != romDefTarget.sigmaDist)
					(*stream) << "OBSE" << sep << dhor.target.sigmaDist.getMMetresValue() << sep;

				if (dhor.target.ppmDist != romDefTarget.ppmDist)
					(*stream) << "PPM" << sep << dhor.target.ppmDist.getMMetresValue() << sep;

				if (dhor.target.sigmaTargetCentering != romDefTarget.sigmaTargetCentering)
					(*stream) << "TCSE" << sep << dhor.target.sigmaTargetCentering.getMMetresValue() << sep;

				(*stream) << endl;

				// Update the rom def target if necessary
				if (dhor.target.ID != romDefTarget.ID)
					romDefTarget = *meas->instrument.targets.at(dhor.target.ID);
			}
		}

		// PLR
		if (!rom->measPLR3D.empty())
		{
			auto romDefTarget = v0DefTarget;

			if (!rom->plrActive)
				(*stream) << DEACTIVATION_CHAR;

			(*stream) << "*PLR3D" << endl;

			for (auto &plr : rom->measPLR3D)
			{
				if (!plr.isActive())
					(*stream) << DEACTIVATION_CHAR;

				(*stream) << plr.targetPos->getName() << sep << plr.getAngle(EPLR3DAngles::kANGL).getGonsValue() << sep
						  << plr.getAngle(EPLR3DAngles::kZEND).getGonsValue() << sep << plr.getDistance() << sep;

				if (plr.target.ID != romDefTarget.ID)
					(*stream) << "TRGT" << sep << plr.target.ID << sep;

				if (plr.target.targetHt != romDefTarget.targetHt)
					(*stream) << "TH" << sep << plr.target.targetHt.getMetresValue() << sep;

				if (plr.target.sigmaTargetHt != romDefTarget.sigmaTargetHt)
					(*stream) << "THSE" << sep << plr.target.sigmaTargetHt.getMMetresValue() << sep;

				if (plr.target.sigmaTargetCentering != romDefTarget.sigmaTargetCentering)
					(*stream) << "TCSE" << sep << plr.target.sigmaTargetCentering.getMMetresValue() << sep;

				if (plr.target.sigmaAngl != romDefTarget.sigmaAngl)
					(*stream) << "ASE" << sep << plr.target.sigmaAngl.getSignedCCValue() << sep;

				if (plr.target.sigmaZenD != romDefTarget.sigmaZenD)
					(*stream) << "ZSE" << sep << plr.target.sigmaZenD.getSignedCCValue() << sep;

				if (plr.target.sigmaDist != romDefTarget.sigmaDist)
					(*stream) << "DSE" << sep << plr.target.sigmaDist.getMMetresValue() << sep;

				if (plr.target.ppmDist != romDefTarget.ppmDist)
					(*stream) << "PPM" << sep << plr.target.ppmDist.getMMetresValue() << sep;

				(*stream) << endl;

				// Update the rom def target if necessary
				if (plr.target.ID != romDefTarget.ID)
					romDefTarget = *meas->instrument.targets.at(plr.target.ID);
			}
		}

		// ECTH
		if (!rom->measECTH.empty())
		{
			TAngle lecture = rom->measECTH.front().obsHorAngle;

			auto scaleDefInst = *data->getInstruments().fSCALE.at(rom->measECTH.front().target.ID);

			if (!rom->ecthActive)
				(*stream) << DEACTIVATION_CHAR;

			(*stream) << "*ECTH" << sep << rom->measECTH.front().obsHorAngle.getGonsValue() << sep << rom->measECTH.front().target.ID << sep << endl;

			for (auto &ecth : rom->measECTH)
			{
				if (ecth.obsHorAngle == lecture)
				{
					if (!ecth.isActive())
						(*stream) << DEACTIVATION_CHAR;

					(*stream) << ecth.targetPos->getName() << sep << ecth.getDistance().getMetresValue() << sep;

					if (ecth.target.ID != scaleDefInst.ID)
						(*stream) << "SCALE" << sep << ecth.target.ID << sep;

					if (ecth.target.sigmaD != scaleDefInst.sigmaD)
						(*stream) << "OBSE" << sep << ecth.target.sigmaD.getMMetresValue() << sep;

					if (ecth.target.ppmD != scaleDefInst.ppmD)
						(*stream) << "PPM" << sep << ecth.target.ppmD.getMMetresValue() << sep;

					if (ecth.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
						(*stream) << "ICSE" << sep << ecth.target.sigmaInstrCentering.getMMetresValue() << sep;

					(*stream) << endl;
				}
				else
				{
					lecture = ecth.obsHorAngle;

					if (!rom->ecthActive)
						(*stream) << DEACTIVATION_CHAR;

					(*stream) << "*ECTH" << sep << ecth.obsHorAngle.getGonsValue() << sep << ecth.target.ID << endl;

					if (!ecth.isActive())
						(*stream) << DEACTIVATION_CHAR;

					(*stream) << ecth.targetPos->getName() << sep << ecth.getDistance().getMetresValue() << sep;

					if (ecth.target.ID != scaleDefInst.ID)
						(*stream) << "SCALE" << sep << ecth.target.ID << sep;

					if (ecth.target.sigmaD != scaleDefInst.sigmaD)
						(*stream) << "OBSE" << sep << ecth.target.sigmaD.getMMetresValue() << sep;

					if (ecth.target.ppmD != scaleDefInst.ppmD)
						(*stream) << "PPM" << sep << ecth.target.ppmD.getMMetresValue() << sep;

					if (ecth.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
						(*stream) << "ICSE" << sep << ecth.target.sigmaInstrCentering.getMMetresValue() << sep;

					(*stream) << endl;
				}

				// Update the default target if necessary
				if (ecth.target.ID != scaleDefInst.ID)
					scaleDefInst = ecth.target;
			}
		}

		// ECDIR
		if (!rom->measECDIR.empty())
		{
			auto scaleDefInst = *data->getInstruments().fSCALE.at(rom->measECDIR.front().target.ID);
			TAngle lectureHz = rom->measECDIR.front().obsHorAngle;
			TAngle lectureV = rom->measECDIR.front().obsVertAngle;

			if (!rom->ecdirActive)
				(*stream) << DEACTIVATION_CHAR;

			(*stream) << "*ECDIR" << sep << rom->measECDIR.front().obsHorAngle.getGonsValue() << sep << rom->measECDIR.front().obsVertAngle.getGonsValue() << sep
					  << rom->measECDIR.front().target.ID << sep << endl;

			for (auto &ecdir : rom->measECDIR)
			{
				if (ecdir.obsHorAngle == lectureHz && ecdir.obsVertAngle == lectureV)
				{
					if (!ecdir.isActive())
						(*stream) << DEACTIVATION_CHAR;

					(*stream) << ecdir.targetPos->getName() << sep << ecdir.getDistance().getMetresValue() << sep;

					if (ecdir.target.ID != scaleDefInst.ID)
						(*stream) << "SCALE" << sep << ecdir.target.ID << sep;

					if (ecdir.target.sigmaD != scaleDefInst.sigmaD)
						(*stream) << "OBSE" << sep << ecdir.target.sigmaD.getMMetresValue() << sep;

					if (ecdir.target.ppmD != scaleDefInst.ppmD)
						(*stream) << "PPM" << sep << ecdir.target.ppmD.getMMetresValue() << sep;

					if (ecdir.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
						(*stream) << "ICSE" << sep << ecdir.target.sigmaInstrCentering.getMMetresValue() << sep;

					(*stream) << endl;
				}
				else
				{
					if (!rom->ecdirActive)
						(*stream) << DEACTIVATION_CHAR;

					lectureHz = ecdir.obsHorAngle;
					lectureV = ecdir.obsVertAngle;
					(*stream) << "*ECDIR" << sep << ecdir.obsHorAngle.getGonsValue() << sep << ecdir.obsVertAngle.getGonsValue() << sep << ecdir.target.ID << endl;

					if (!ecdir.isActive())
						(*stream) << DEACTIVATION_CHAR;

					(*stream) << ecdir.targetPos->getName() << sep << ecdir.getDistance().getMetresValue() << sep;

					if (ecdir.target.ID != scaleDefInst.ID)
						(*stream) << "SCALE" << sep << ecdir.target.ID << sep;

					if (ecdir.target.sigmaD != scaleDefInst.sigmaD)
						(*stream) << "OBSE" << sep << ecdir.target.sigmaD.getMMetresValue() << sep;

					if (ecdir.target.ppmD != scaleDefInst.ppmD)
						(*stream) << "PPM" << sep << ecdir.target.ppmD.getMMetresValue() << sep;

					if (ecdir.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
						(*stream) << "ICSE" << sep << ecdir.target.sigmaInstrCentering.getMMetresValue() << sep;

					(*stream) << endl;
				}

				// Update the default target if necessary
				if (ecdir.target.ID != scaleDefInst.ID)
					scaleDefInst = ecdir.target;
			}
		}
	}
}

void TSimFileWriter::writeINCLYMeas(TINCLYROM *meas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	auto inclDefInst = *data->getInstruments().fINCL.at(meas->measINCLY.front().target.ID);

	if (!meas->isActive())
		(*stream) << DEACTIVATION_CHAR;

	(*stream) << "*INCLY" << sep << inclDefInst.ID << endl;

	// write the list of measurements for the line
	for (auto &itINCLY : meas->measINCLY)
	{
		if (!itINCLY.isActive())
			(*stream) << DEACTIVATION_CHAR;

		(*stream) << itINCLY.targetPos->getName() << sep << itINCLY.getAngle().getGonsValue() << sep;

		if (itINCLY.target.ID != inclDefInst.ID)
			(*stream) << "INSTR" << sep << itINCLY.target.ID << sep;

		// Update the default target if necessary, to do before updating other values to avoid unecessary values.
		if (itINCLY.target.ID != inclDefInst.ID)
			inclDefInst = *data->getInstruments().fINCL.at(itINCLY.target.ID);

		if (itINCLY.target.sigmaAngl != inclDefInst.sigmaAngl)
			(*stream) << "OBSE" << sep << itINCLY.target.sigmaAngl.getSignedCCValue() << sep;

		if (itINCLY.target.sigmaPpm != inclDefInst.sigmaPpm)
			(*stream) << "PPM" << sep << itINCLY.target.sigmaPpm.getMicroRadiansValue() << sep;

		if (itINCLY.target.angleCorrectionValue != inclDefInst.angleCorrectionValue)
			(*stream) << "AC" << sep << itINCLY.target.angleCorrectionValue.getGonsValue() << sep;

		if (itINCLY.target.sigmaCorrectionValue != inclDefInst.sigmaCorrectionValue)
			(*stream) << "ACSE" << sep << itINCLY.target.sigmaCorrectionValue.getSignedCCValue() << sep;

		if (itINCLY.target.refAngleCorrectionValue != inclDefInst.refAngleCorrectionValue)
			(*stream) << "RF" << sep << itINCLY.target.refAngleCorrectionValue.getGonsValue() << sep;

		if (itINCLY.target.refSigmaCorrectionValue != inclDefInst.refSigmaCorrectionValue)
			(*stream) << "RFSE" << sep << itINCLY.target.refSigmaCorrectionValue.getSignedCCValue() << sep;

		(*stream) << endl;
	}
}
