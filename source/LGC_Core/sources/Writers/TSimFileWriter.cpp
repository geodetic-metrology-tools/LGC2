////////////////////////////////////////////////////////////////////
// TSimFileWriter.cpp : implementation class
// Write an LGC "input" file with simulated values for observations
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include "TSeparatedFormatTStream.h"
#include <TLGCData.h>
#include "TSimFileWriter.h"
//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TSimFileWriter::TSimFileWriter() : TAFileWriter()
{//default constructor
}


TSimFileWriter::TSimFileWriter(TAStreamFormatter* stream, const TLGCData* project) : TAFileWriter(stream, project), data(project)
{//constructor
}


TSimFileWriter::~TSimFileWriter()
{//destructor
}




//////////////////////////
// public member functions
//////////////////////////

void    TSimFileWriter::writeFile(const string error)
{//write error messages from project
	writeError(error);
}

//////////////////////////////////////////////////////////////////////
//write the sim obs lgc file used when there's no error in the project
//////////////////////////////////////////////////////////////////////
void	TSimFileWriter::writeFile()
{
	TAStreamFormatter* stream = getStream();
	//reformat the streamformatter
	stream->setLengthUnits(TLength::EUnits::kMetres);
	stream->setPrecisionFormat(fProjectData->getConfig().outPrecision.digits);
	
	writeHeader();
	writeInstrument();
	writeData(fProjectData->getTree().begin());

	(*stream) << "*END" << endl;

	return;
}

void	TSimFileWriter::writeHeader()
{
	TAStreamFormatter* stream = getStream();
	
	(*stream) << "*TITR" << endl;
	(*stream) << data->getConfig().title << endl;
	(*stream) << "DANS CE FICHIER, LES OBSERVATIONS SONT SIMULEES !" << endl;

	if (data->getConfig().referential == 106)
		(*stream) <<"*LEP" << endl;
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
		(*stream) << "*FAUT " << data->getConfig().faut.alpha << "  " << data->getConfig().faut.beta<< endl;

	if (data->getConfig().histo.isActive())
		(*stream) << "*HIST" << endl;

	if (data->getConfig().nodup.isActive())
		(*stream) << "*NODUP" << endl;

	if (data->getConfig().useApriori.isActive())
		(*stream) << "*APRI" << endl;

	if (data->getConfig().writeDefa.isActive())
		(*stream) << "*DEFA" << endl;

	if (data->getConfig().outPrecision.digits != 5 )
		(*stream) << "*PREC " << data->getConfig().outPrecision.digits <<  endl;

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


}

void	TSimFileWriter::writeInstrument()
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

	(*stream) << "*INSTR" << endl;
	for (auto& itCAMD : data->getInstruments().fCAMD)
	{
		(*stream) << "*CAMD " << itCAMD.second.ID << sep 
            << itCAMD.second.defTarget << sep
            << itCAMD.second.sigmaInstrCentering.getMMetresValue() << sep
            << endl;
		for (auto& itTarget : itCAMD.second.targets)
			(*stream) << itTarget.second.ID << sep
			<< itTarget.second.sigmaX*M2MM << sep
			<< itTarget.second.sigmaY*M2MM << sep
			<< itTarget.second.sigmaDist.getMMetresValue() << sep
			<< itTarget.second.sigmaTargetCentering.getMMetresValue() << endl;
	}

	for (auto& itPOLAR : data->getInstruments().fPOLAR)
	{
		(*stream) << "*POLAR " << itPOLAR.second.ID << sep 
			<< itPOLAR.second.defTarget << sep
			<< itPOLAR.second.instrHeight.getMetresValue() << sep
			<< itPOLAR.second.sigmaInstrHeight.getMMetresValue() << sep
			<< itPOLAR.second.sigmaInstrCentering.getMMetresValue() << sep
			<< itPOLAR.second.constAngle.getGonsValue() << sep
			<<endl;
		for (auto& itTarget : itPOLAR.second.targets)
			(*stream) << itTarget.second.ID << sep
			<< itTarget.second.sigmaAngl.getSignedCCValue() << sep
			<< itTarget.second.sigmaZenD.getSignedCCValue() << sep
			<< itTarget.second.sigmaDist.getMMetresValue() << sep
			<< itTarget.second.ppmDist.getMMetresValue() << sep
			<< itTarget.second.distCorrectionUnknown << sep
			<< itTarget.second.distCorrectionValue.getMetresValue() << sep
			<< itTarget.second.sigmaDCorr.getMMetresValue() << sep
			<< itTarget.second.sigmaTargetCentering.getMMetresValue() << sep
			<< itTarget.second.targetHt.getMetresValue() << sep
			<< itTarget.second.sigmaTargetHt.getMMetresValue() << sep
			<<endl;
	}

	for (auto& itEDM : data->getInstruments().fEDM)
	{
		(*stream) << "*EDM " << itEDM.second.ID << sep 
			<< itEDM.second.defTarget << sep
			<< itEDM.second.instrHeight.getMetresValue() << sep
			<< itEDM.second.sigmaInstrHeight.getMMetresValue() << sep
			<< itEDM.second.sigmaInstrCentering.getMMetresValue() << sep<<endl;
		for (auto& itTarget : itEDM.second.targets)
			(*stream) << itTarget.second.ID << sep
			<< itTarget.second.sigmaDSpt.getMMetresValue() << sep
			<< itTarget.second.ppmDSpt.getMMetresValue() << sep
			<< itTarget.second.distCorrectionUnknown << sep
			<< itTarget.second.distCorrectionValue.getMetresValue() << sep
			<< itTarget.second.sigmaDCorr.getMMetresValue() << sep
			<< itTarget.second.sigmaTargetCentering.getMMetresValue() << sep
			<< itTarget.second.targetHt.getMetresValue() << sep
			<< itTarget.second.sigmaTargetHt.getMMetresValue() << sep << endl;
	}

	for (auto& itLEVEL : data->getInstruments().fLEVEL)
	{
		(*stream) << "*LEVEL " << itLEVEL.second.ID << sep 
			<< itLEVEL.second.defStaffID << sep
			<< itLEVEL.second.collAngleUnknown << sep
			<< itLEVEL.second.collAngleValue.getGonsValue() << sep
			<<endl;
		for (auto& itTarget : itLEVEL.second.targets)
			(*stream) << itTarget.second.ID << sep
			<< itTarget.second.sigmaD.getMMetresValue() << sep
			<< itTarget.second.ppmD.getMMetresValue() << sep
			<< itTarget.second.distCorrectionValue.getMetresValue() << sep
			<< itTarget.second.sigmaDCorr.getMMetresValue() << sep
			<< itTarget.second.staffHt.getMetresValue() << sep
			<< itTarget.second.sigmaStaffHt.getMMetresValue() << sep
			<< endl;
	}

	for (auto& itSCALE : data->getInstruments().fSCALE)
	{
		(*stream) << "*SCALE " << itSCALE.second.ID << sep 
			<< itSCALE.second.sigmaD.getMMetresValue() << sep
			<< itSCALE.second.ppmD.getMMetresValue() << sep
			<< itSCALE.second.distCorrectionValue.getMetresValue() << sep
			<< itSCALE.second.sigmaDCorr.getMMetresValue() << sep
			<< itSCALE.second.sigmaInstrCentering.getMMetresValue() << sep
			<<endl;
	}		
}

void	TSimFileWriter::writeData(TDataTreeIterator itTree)
{
	if (itTree == fProjectData->getTree().end())
		return;

	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

	writeFrameHeader(itTree);
	writePoint(itTree);
	writeMeasurement(itTree);

	//write the first child
	if (itTree.node->first_child) {
		writeData(itTree.node->first_child);

		//write the other child
		for (auto child = itTree.node->first_child; child != itTree.node->last_child; )
			writeData(child = child->next_sibling);
	}

	//root not need to be closed
	if (!itTree->get()->isROOTNode())
		(*stream) << "*ENDFRAME" << endl;
}

void TSimFileWriter::writeFrameHeader(TDataTreeIterator frameIt)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();
	stream->setPrecisionFormat(fProjectData->getConfig().outPrecision.digits);

	if ( !frameIt->get()->isROOTNode())
	{
		(*stream) << "*FRAME" << sep
			<< frameIt->get()->frame.getName() << sep
			<< frameIt->get()->frame.getProvTranslation(0).getMetresValue() << sep
			<< frameIt->get()->frame.getProvTranslation(1).getMetresValue() << sep
			<< frameIt->get()->frame.getProvTranslation(2).getMetresValue() << sep
			<< frameIt->get()->frame.getProvRotation(0).getGonsValue() << sep
			<< frameIt->get()->frame.getProvRotation(1).getGonsValue() << sep
			<< frameIt->get()->frame.getProvRotation(2).getGonsValue() << sep
			<< frameIt->get()->frame.getProvScale() << sep;
		
		if (frameIt->get()->frame.hasTranslStandDev(0))
			(*stream) << "STX" << sep << frameIt->get()->frame.getTranslationStandDev(0).getMMetresValue() << sep;
		else if (!frameIt->get()->frame.isTranslationFixed(0))
			(*stream) << "TX" << sep ;
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
			(*stream) << "SSCL" << sep << frameIt->get()->frame.getScaleStandDev()*M2MM << sep;
		else if (!frameIt->get()->frame.isScaleFixed())
			(*stream) << "SCL" << sep;

		(*stream) << endl;

	}
}

void TSimFileWriter::writePoint(TDataTreeIterator frameIt)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();
	stream->setPrecisionFormat(data->getConfig().outPrecision.digits);

	bool firstCALA = true;
	bool firstPOIN = true;
	bool firstVXY = true;
	bool firstVXZ = true;
	bool firstVYZ = true;
	bool firstVZ = true;

	// lambda function to write the point coordinate
	auto writeXYZorH = [&](TAdjustablePoint const& fPoint) {
		if ((data->getConfig().referential == 106 || data->getConfig().referential == 107 || data->getConfig().referential == 104) && frameIt->get()->isROOTNode())
			(*stream) << fPoint.getName() << sep
			<< fPoint.getProvisionalValue().getX() << sep
			<< fPoint.getProvisionalValue().getY() << sep
			<< fPoint.getProvisionalValue().getH() << sep;
		else
			(*stream) << fPoint.getName() << sep
			<< fPoint.getProvisionalValue().getX() << sep
			<< fPoint.getProvisionalValue().getY() << sep
			<< fPoint.getProvisionalValue().getZ() << sep;
	};

	//write PDOR if we are in ROOT & PDOR is used
	if (frameIt->get()->isROOTNode())
		if (data->getConfig().pdor.isActive() && data->getConfig().pdor.hasBearing)
		{
			(*stream) << "*PDOR" << endl;
			(*stream) << data->getConfig().pdor.fptname
				<< sep << data->getConfig().pdor.fgis.getGonsValue()
				<< endl;
		}
		else if (data->getConfig().pdor.isActive())
		{
			(*stream) << "*PDOR" << endl;
			(*stream) << data->getConfig().pdor.fptname << endl;
		}
	 
	//Write point list
	for (auto& point : data->getPoints())
	{
		TSpatialStatus::ESpatialStatus status = point.getSpatialStatus();
		TDataTreeIterator posInTree = point.getFrameTreePosition();

		if (posInTree == frameIt)
		{
			switch (status)
			{
			case TSpatialStatus::ESpatialStatus::kCala:
				if (firstCALA)
				{
					firstCALA = false;
					(*stream) << "*CALA" << endl;
				}
				writeXYZorH(point);
				(*stream) << endl;
				break;

			case TSpatialStatus::ESpatialStatus::kVxyz:
				if (firstPOIN)
				{
					firstPOIN = false;
					(*stream) << "*POIN" << endl;
				}
				writeXYZorH(point);

				if (!frameIt->get()->isROOTNode())
					(*stream) << "SX"<<sep
					<< point.getStandDev(0) << sep
					<< "SY" << sep
					<< point.getStandDev(1) << sep
					<< "SZ" << sep
					<< point.getStandDev(2) << endl;
				else
				{
					if (point.hasStandDeviations())
						(*stream) << "SX" << sep
						<< point.getStandDev(0) << sep
						<< "SY" << sep
						<< point.getStandDev(1) << sep
						<< "SZ" << sep
						<< point.getStandDev(2) << endl;
					else
						(*stream) << endl;
				}
				break;

			case TSpatialStatus::ESpatialStatus::kVxy:
				if (firstVXY)
				{
					firstVXY = false;
					(*stream) << "*VXY" << endl;
				}
				writeXYZorH(point);
				(*stream)<< endl;
				break;

			case TSpatialStatus::ESpatialStatus::kVxz:
				if (firstVXZ)
				{
					firstVXZ = false;
					(*stream) << "*VXZ" << endl;
				}
				writeXYZorH(point);
				(*stream) << endl;
				break;

			case TSpatialStatus::ESpatialStatus::kVyz:

				if (firstVYZ)
				{
					firstVYZ = false;
					(*stream) << "*VYZ" << endl;
				}
				writeXYZorH(point);
				(*stream) << endl;
				break;

			case TSpatialStatus::ESpatialStatus::kVz:
				if (firstVZ)
				{
					firstCALA = false;
					(*stream) << "*VZ" << endl;
				}
				writeXYZorH(point);
				(*stream) << endl;
				break;
			}
		}

	}


}

void TSimFileWriter::writeMeasurement(TDataTreeIterator frameIt)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

	for (auto meas : frameIt->get()->measurements.fTSTN)
		writeTSTNMeas(meas);

	for (auto& meas : frameIt->get()->measurements.fCAM)
		writeCAMMeas(&meas);

	for (auto& meas : frameIt->get()->measurements.fEDM)
		writeEDMMeas(&meas);

	for (auto& meas : frameIt->get()->measurements.fLEVEL)
		writeLEVELMeas(&meas);

	if (!frameIt->get()->measurements.fDVER.empty())
	{
		(*stream) << "*DVER" << endl;
		for (auto& meas : frameIt->get()->measurements.fDVER)
			writeDVERMeas(&meas);
	}

	for (auto& meas : frameIt->get()->measurements.fECHO)
		writeECHOMeas(&meas);

	for (auto& meas : frameIt->get()->measurements.fECVE)
		writeECVEMeas(&meas);

	for (auto& meas : frameIt->get()->measurements.fECSP)
		writeECSPMeas(&meas);

	for (auto& meas : frameIt->get()->measurements.fORIE)
		writeORIEMeas(&meas);

	if (!frameIt->get()->measurements.fRADI.empty())
	{
		(*stream) << "*RADI" << endl;
		for (auto& meas : frameIt->get()->measurements.fRADI)
			writeRADIMeas(&meas);
	}

}


void TSimFileWriter::writeCAMMeas(TCAM* meas)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

    auto edmDefInst = data->getInstruments().fCAMD.at(meas->instrument.ID);

    (*stream) << "*CAM" << sep
        << meas->instrumentPos->getName() << sep
        << meas->instrument.ID << sep;
	
    if(meas->instrument.sigmaInstrCentering != edmDefInst.sigmaInstrCentering)
        (*stream) << "ICSE" << sep
        << meas->instrument.sigmaInstrCentering.getMMetresValue() << sep;

    (*stream) << endl;

	if (!meas->measUVD.empty())
	{
		(*stream) << "*UVD" << endl;

		for (auto& uvd : meas->measUVD)
		{
			(*stream) << uvd.targetPos->getName() << sep
				<< uvd.getVectorValue().getX() << sep
				<< uvd.getVectorValue().getY() << sep
				<< uvd.getVectorValue().getZ() << sep
				<< uvd.getDistance().getMetresValue() << sep;

			if (uvd.target.ID != meas->instrument.targets.at(meas->instrument.defTarget).ID)
				(*stream) << "TRGT" << sep
				<< uvd.target.ID << sep;

			if (uvd.target.sigmaTargetCentering != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetCentering)
				(*stream) << "TCSE" << sep
				<< uvd.target.sigmaTargetCentering.getMMetresValue() << sep;
			
			if (uvd.target.sigmaX != meas->instrument.targets.at(meas->instrument.defTarget).sigmaX)
				(*stream) << "XSE" << sep
				<< uvd.target.sigmaX*M2MM<< sep;

			if (uvd.target.sigmaY != meas->instrument.targets.at(meas->instrument.defTarget).sigmaY)
				(*stream) << "YSE" << sep
				<< uvd.target.sigmaY*M2MM << sep;

            if(uvd.target.sigmaDist != meas->instrument.targets.at(meas->instrument.defTarget).sigmaDist)
                (*stream) << "DSE" << sep
                << uvd.target.sigmaDist.getMMetresValue() << sep;

			(*stream) << endl;
		}
	}
	
	if (!meas->measUVEC.empty())
	{
		(*stream) << "*UVEC" << endl;

		for (auto& uvec : meas->measUVEC)
		{
			(*stream) << uvec.targetPos->getName() << sep
				<< uvec.getVectorValue().getX() << sep
				<< uvec.getVectorValue().getY() << sep
				<< uvec.getVectorValue().getZ() << sep;
			
			if (uvec.target.ID != meas->instrument.targets.at(meas->instrument.defTarget).ID)
				(*stream) << "TRGT" << sep
				<< uvec.target.ID << sep;

			if (uvec.target.sigmaTargetCentering != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetCentering)
				(*stream) << "TCSE" << sep
				<< uvec.target.sigmaTargetCentering.getMMetresValue() << sep;

			if (uvec.target.sigmaX != meas->instrument.targets.at(meas->instrument.defTarget).sigmaX)
				(*stream) << "XSE" << sep
				<< uvec.target.sigmaX*M2MM << sep;

			if (uvec.target.sigmaY != meas->instrument.targets.at(meas->instrument.defTarget).sigmaY)
				(*stream) << "YSE" << sep
				<< uvec.target.sigmaY*M2MM << sep;

			(*stream) << endl;
		}
	}
}

void TSimFileWriter::writeDVERMeas(TDVER* meas)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

	//write the list of measurements
	(*stream) << meas->station->getName() << sep
		<< meas->targetPos->getName() << sep
		<< meas->getDistance().getMetresValue() << sep
		<< "OBSE" << sep
		<< meas->getObservedStDev().getMMetresValue() << sep;

	if (meas->getDistanceCorrection().getMetresValue() != 0)
		(*stream) << "DCOR" << sep
		<< meas->getDistanceCorrection().getMetresValue() << sep;

	(*stream) << endl;
}

void TSimFileWriter::writeECHOMeas(TECHOROM* meas)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

	auto scaleDefInst = data->getInstruments().fSCALE.at(meas->measECHO.front().target.ID);

	(*stream) << "*ECHO" << sep
		<< scaleDefInst.ID << endl;

	
	//write the list of measurements for the line
	for (auto& itECHO : meas->measECHO)
	{
		(*stream) << itECHO.targetPos->getName() << sep
			<< itECHO.getDistance().getMetresValue() << sep;

		if (itECHO.target.ID != scaleDefInst.ID)
			(*stream) << "SCALE" << sep
			<< itECHO.target.ID << sep;

		if (itECHO.target.sigmaD != scaleDefInst.sigmaD)
			(*stream) << "OBSE" << sep
			<< itECHO.target.sigmaD.getMMetresValue() << sep;

		if (itECHO.target.ppmD != scaleDefInst.ppmD)
			(*stream) << "PPM" << sep
			<< itECHO.target.ppmD.getMMetresValue() << sep;

		if (itECHO.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
			(*stream) << "ICSE" << sep
			<< itECHO.target.sigmaInstrCentering.getMMetresValue() << sep;

		(*stream) << endl;
	}
}

void TSimFileWriter::writeECVEMeas(TECVEROM* meas)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

	auto scaleDefInst = data->getInstruments().fSCALE.at(meas->measECVE.front().target.ID);

	(*stream) << "*ECVE" << sep
		<< scaleDefInst.ID << sep;
	
	//write point on the line if it is already defined
	for (auto& point : data->getPoints())
		if (point.getName() == meas->fMeasuredLine->getLinePoint()->getName())
			(*stream) << "PtLine" << sep << meas->fMeasuredLine->getLinePoint()->getName() << sep;
		
	(*stream) << endl;


	//write the list of measurements for the line
	for (auto& itECVE : meas->measECVE)
	{
		(*stream) << itECVE.targetPos->getName() << sep
			<< itECVE.getDistance().getMetresValue() << sep;

		if (itECVE.target.ID != scaleDefInst.ID)
			(*stream) << "SCALE" << sep
			<< itECVE.target.ID << sep;

		if (itECVE.target.sigmaD != scaleDefInst.sigmaD)
			(*stream) << "OBSE" << sep
			<< itECVE.target.sigmaD.getMMetresValue() << sep;

		if (itECVE.target.ppmD != scaleDefInst.ppmD)
			(*stream) << "PPM" << sep
			<< itECVE.target.ppmD.getMMetresValue() << sep;

		if (itECVE.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
			(*stream) << "ISCE" << sep
			<< itECVE.target.sigmaInstrCentering.getMMetresValue() << sep;

		(*stream) << endl;
	}

}

void TSimFileWriter::writeECSPMeas(TECSPROM* meas)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

	auto scaleDefInst = data->getInstruments().fSCALE.at(meas->measECSP.front().target.ID);

	(*stream) << "*ECSP" << sep
		<< meas->p1->getName() << sep 
		<< meas->p2->getName() << sep
		<< scaleDefInst.ID << sep;
	(*stream) << endl;

		for (auto& ecsp : meas->measECSP)
		{
				(*stream) << ecsp.targetPos->getName() << sep
					<< ecsp.getDistance().getMetresValue() << sep;

				if (ecsp.target.ID != scaleDefInst.ID)
					(*stream) << "SCALE" << sep
					<< ecsp.target.ID << sep;

				if (ecsp.target.sigmaD != scaleDefInst.sigmaD)
					(*stream) << "OBSE" << sep
					<< ecsp.target.sigmaD.getMMetresValue() << sep;

				if (ecsp.target.ppmD != scaleDefInst.ppmD)
					(*stream) << "PPM" << sep
					<< ecsp.target.ppmD.getMMetresValue() << sep;

				if (ecsp.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
					(*stream) << "ICSE" << sep
					<< ecsp.target.sigmaInstrCentering.getMMetresValue() << sep;

				(*stream) << endl;
			
		}
}

void TSimFileWriter::writeEDMMeas(TEDM* meas)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

    auto edmDefInst = data->getInstruments().fEDM.at(meas->instrument.ID);

    (*stream) << "*DSPT" << sep
        << meas->instrumentPos->getName() << sep
        << meas->instrument.ID << sep;

    if(meas->instrument.instrHeight != edmDefInst.instrHeight)
        (*stream) << "IH" << sep
		<< meas->instrument.instrHeight.getMetresValue() << sep;
    
    if(meas->instrument.sigmaInstrHeight != edmDefInst.sigmaInstrHeight)
        (*stream) << "IHSE" << sep
        << meas->instrument.sigmaInstrHeight.getMMetresValue() << sep;

    if(meas->instrument.sigmaInstrCentering != edmDefInst.sigmaInstrCentering)
        (*stream) << "ICSE" << sep
        << meas->instrument.sigmaInstrCentering.getMMetresValue() << sep;

    (*stream) << endl;

	//write the list of measurements
	for (auto& itDspt : meas->measDSPT)
	{
		(*stream) << itDspt.targetPos->getName() << sep
			<< itDspt.getDistance().getMetresValue() << sep;

		if (itDspt.target.ID != meas->instrument.defTarget)
			(*stream) << "TRGT" << sep
			<< itDspt.target.ID << sep;

		if (itDspt.target.sigmaDSpt != meas->instrument.targets.at(meas->instrument.defTarget).sigmaDSpt)
			(*stream) << "OBSE" << sep
			<< itDspt.target.sigmaDSpt.getMMetresValue() << sep;

		if (itDspt.target.ppmDSpt != meas->instrument.targets.at(meas->instrument.defTarget).ppmDSpt)
			(*stream) << "PPM" << sep
			<< itDspt.target.ppmDSpt.getMMetresValue() << sep;

		if (itDspt.target.targetHt != meas->instrument.targets.at(meas->instrument.defTarget).targetHt)
			(*stream) << "TH" << sep
			<< itDspt.target.targetHt.getMetresValue() << sep;

		if (itDspt.target.sigmaTargetHt != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetHt)
			(*stream) << "THSE" << sep
			<< itDspt.target.sigmaTargetHt.getMMetresValue() << sep;

		if (itDspt.target.sigmaTargetCentering != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetCentering)
			(*stream) << "TCSE" << sep
			<< itDspt.target.sigmaTargetCentering.getMMetresValue() << sep;

		(*stream) << endl;
	}
		
}

void TSimFileWriter::writeLEVELMeas(TLEVEL* meas)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

	(*stream) << "*DLEV" << sep
		<< meas->instrument.ID << sep;

	//write point on the line if it is already defined
	for (auto& point : data->getPoints())
		if (point.getName() == meas->fMeasuredPlane->getReferencePoint()->getName())
			(*stream) << "RefPt" << sep << meas->fMeasuredPlane->getReferencePoint()->getName() << sep;

	(*stream) << endl;

	//write measurement for the plane
	for (auto& itDLEV : meas->measDLEV)
	{
		(*stream) << itDLEV.targetPos->getName() << sep
			<< itDLEV.getDistance().getMetresValue() << sep;

		if (itDLEV.dhor.get())
			(*stream) << "DHOR" << sep
			<< itDLEV.dhor.get()->getDistance().getMetresValue() << sep
			<< "DSE" << sep
			<< itDLEV.dhor.get()->getDHORSigma().getMMetresValue() << sep;

		if (itDLEV.target.ID != meas->instrument.defStaffID)
			(*stream) << "TRGT" << sep
			<< itDLEV.target.ID << sep;

		if (itDLEV.target.sigmaD != meas->instrument.targets.at(meas->instrument.defStaffID).sigmaD)
			(*stream) << "OBSE" << sep
			<< itDLEV.target.sigmaD.getMMetresValue() << sep;

		if (itDLEV.target.ppmD != meas->instrument.targets.at(meas->instrument.defStaffID).ppmD)
			(*stream) << "PPM" << sep
			<< itDLEV.target.ppmD.getMMetresValue() << sep;

		if (itDLEV.target.staffHt != meas->instrument.targets.at(meas->instrument.defStaffID).staffHt)
			(*stream) << "TH" << sep
			<< itDLEV.target.staffHt.getMetresValue() << sep;

		if (itDLEV.target.sigmaStaffHt != meas->instrument.targets.at(meas->instrument.defStaffID).sigmaStaffHt)
			(*stream) << "THSE" << sep
			<< itDLEV.target.sigmaStaffHt.getMMetresValue() << sep;

		(*stream) << endl;
	}
}

void TSimFileWriter::writeORIEMeas(TORIEROM* meas)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

    auto polarDefInst = data->getInstruments().fPOLAR.at(meas->instrument.ID);

	(*stream) << "*ORIE" << sep
		<< meas->instrumentPos->getName() << sep
		<< meas->instrument.ID << sep;

    if(meas->instrument.sigmaInstrCentering != polarDefInst.sigmaInstrCentering)
        (*stream) << "ICSE" << sep
        << meas->instrument.sigmaInstrCentering.getMMetresValue() << sep;

    if(meas->fConstantAngle.getGonsValue() != polarDefInst.constAngle.getGonsValue())
        (*stream) << "CST" << sep
        << meas->fConstantAngle.getGonsValue() << sep;

    (*stream) << endl;

	//write the list of measurements for the line
	for (auto& itORIE : meas->measORIE)
	{
		(*stream) << itORIE.targetPos->getName() << sep
			<< itORIE.getAngle().getGonsValue() << sep;

		if (itORIE.target.ID != meas->instrument.defTarget)
			(*stream) << "TRGT" << sep
			<< itORIE.target.ID << sep;

		if (itORIE.target.sigmaAngl != meas->instrument.targets.at(meas->instrument.defTarget).sigmaAngl)
			(*stream) << "OBSE" << sep
			<< itORIE.target.sigmaAngl .getSignedCCValue() << sep;

		if (itORIE.target.sigmaTargetCentering != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetCentering)
			(*stream) << "TCSE" << sep
			<< itORIE.target.sigmaTargetCentering.getMMetresValue() << sep;

		(*stream) << endl;
	}
}

void TSimFileWriter::writeRADIMeas(TRADI* meas)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

	(*stream) << meas->station->getName() << sep
		<< meas->getAngleCnstr().getGonsValue() << sep
		<< "SIGMA" << sep << meas->getObservedStDev().getMMetresValue()<< endl;


}

void TSimFileWriter::writeTSTNMeas(shared_ptr<TTSTN> meas)
{
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();

    auto polarDefInst = data->getInstruments().fPOLAR.at(meas->instrument.ID);

    (*stream) << "*TSTN" << sep
        << meas->instrumentPos->getName() << sep
        << meas->instrument.ID << sep;

    if(meas->rot3D)
        (*stream) << "ROT3D" << sep;

    if(meas->instrumentHeightAdjustable)
    {
        if(meas->instrumentHeightAdjustable->isFixed())
        {
            (*stream) << "IHFIX" << sep;
            if(meas->instrument.instrHeight != 0)
				(*stream) << "IH" << sep << meas->instrument.instrHeight.getMetresValue() << sep;

            if(meas->instrument.sigmaInstrHeight != 0)
                (*stream) << "IHSE" << sep << meas->instrument.sigmaInstrHeight.getMMetresValue() << sep;
        }
    } else
        (*stream) << "IHFIX" << sep;

    if(meas->instrument.sigmaInstrCentering != polarDefInst.sigmaInstrCentering)
        (*stream) << "ICSE" << sep
        << meas->instrument.sigmaInstrCentering.getMMetresValue() << sep;

    (*stream) << endl;

	for (auto& rom : meas->roms)
	{
		(*stream) << "*V0" << sep;

		//if (rom->defaultTarget!=nullptr && rom->defaultTarget->ID != meas->instrument.defTarget)
		//	(*stream) << "TRGT" << sep << rom->defaultTarget->ID << sep;

		if (rom->acst != TAngle(0.0))
			(*stream) << "ACST" << sep << rom->acst.getGonsValue() << sep;
		(*stream) << endl;


		//ANGL
		if (!rom->measANGL.empty())
		{
			(*stream) << "*ANGL" << endl;
			for (auto& angl : rom->measANGL)
			{
				(*stream) << angl.targetPos->getName() << sep
					<< angl.getAngle().getGonsValue()<< sep;

				if (angl.target.ID != meas->instrument.defTarget)
					(*stream) << "TRGT" << sep
					<< angl.target.ID << sep;

				if (angl.target.sigmaAngl != meas->instrument.targets.at(meas->instrument.defTarget).sigmaAngl)
					(*stream) << "OBSE" << sep
					<< angl.target.sigmaAngl.getSignedCCValue() << sep;

				if (angl.target.sigmaTargetCentering != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetCentering)
					(*stream) << "TCSE" << sep
					<< angl.target.sigmaTargetCentering.getMMetresValue() << sep;

				(*stream) << endl;
			}
		}


		//ZEND
		if (!rom->measZEND.empty())
		{
			(*stream) << "*ZEND" << endl;
			for (auto& zend : rom->measZEND)
			{
				(*stream) << zend.targetPos->getName() << sep
					<< zend.getAngle().getGonsValue() << sep;

				if (zend.target.ID != meas->instrument.defTarget)
					(*stream) << "TRGT" << sep
					<< zend.target.ID << sep;

				if (zend.target.sigmaZenD != meas->instrument.targets.at(meas->instrument.defTarget).sigmaZenD)
					(*stream) << "OBSE" << sep
					<< zend.target.sigmaZenD.getSignedCCValue() << sep;

				if (zend.target.targetHt != meas->instrument.targets.at(meas->instrument.defTarget).targetHt)
					(*stream) << "TH" << sep
					<< zend.target.targetHt.getMetresValue() << sep;

				if (zend.target.sigmaTargetHt != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetHt)
					(*stream) << "THSE" << sep
					<< zend.target.sigmaTargetHt.getMMetresValue() << sep;

				if (zend.target.sigmaTargetCentering != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetCentering)
					(*stream) << "TCSE" << sep
					<< zend.target.sigmaTargetCentering.getMMetresValue() << sep;

				(*stream) << endl;
			}
		}

		//DIST
		if (!rom->measDIST.empty())
		{
			(*stream) << "*DIST" << endl;

			for (auto& dist : rom->measDIST)
			{
				(*stream) << dist.targetPos->getName() << sep
					<< dist.getDistance().getMetresValue() << sep;

				if (dist.target.ID != meas->instrument.defTarget)
					(*stream) << "TRGT" << sep
					<< dist.target.ID << sep;

				if (dist.target.sigmaDist != meas->instrument.targets.at(meas->instrument.defTarget).sigmaDist)
					(*stream) << "OBSE" << sep
					<< dist.target.sigmaDist.getMMetresValue() << sep;

				if (dist.target.ppmDist != meas->instrument.targets.at(meas->instrument.defTarget).ppmDist)
					(*stream) << "PPM" << sep
					<< dist.target.ppmDist.getMMetresValue() << sep;

				if (dist.target.targetHt != meas->instrument.targets.at(meas->instrument.defTarget).targetHt)
					(*stream) << "TH" << sep
					<< dist.target.targetHt.getMetresValue() << sep;

				if (dist.target.sigmaTargetHt != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetHt)
					(*stream) << "THSE" << sep
					<< dist.target.sigmaTargetHt.getMMetresValue() << sep;

				if (dist.target.sigmaTargetCentering != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetCentering)
					(*stream) << "TCSE" << sep
					<< dist.target.sigmaTargetCentering.getMMetresValue() << sep;
					
				(*stream) << endl;
			}
		}

		//DHOR
		if (!rom->measDHOR.empty())
		{
			(*stream) << "*DHOR" << endl;
			for (auto& dhor : rom->measDHOR)
			{
				(*stream) << dhor.targetPos->getName() << sep
					<< dhor.getDistance().getMetresValue() << sep;

				if (dhor.target.ID != meas->instrument.defTarget)
					(*stream) << "TRGT" << sep
					<< dhor.target.ID << sep;

				if (dhor.target.sigmaDist != meas->instrument.targets.at(meas->instrument.defTarget).sigmaDist)
					(*stream) << "OBSE" << sep
					<< dhor.target.sigmaDist.getMMetresValue() << sep;

				if (dhor.target.ppmDist != meas->instrument.targets.at(meas->instrument.defTarget).ppmDist)
					(*stream) << "PPM" << sep
					<< dhor.target.ppmDist.getMMetresValue() << sep;

				if (dhor.target.sigmaTargetCentering != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetCentering)
					(*stream) << "TCSE" << sep
					<< dhor.target.sigmaTargetCentering.getMMetresValue() << sep;

				(*stream) << endl;
			}
		}

		//PLR
		if (!rom->measPLR3D.empty())
		{
			(*stream) << "*PLR3D" << endl;

			for (auto& plr : rom->measPLR3D)
			{
				(*stream) << plr.targetPos->getName() << sep
					<< plr.getAngle(EPLR3DAngles::kANGL).getGonsValue() << sep
					<< plr.getAngle(EPLR3DAngles::kZEND).getGonsValue() << sep
					<< plr.getDistance() << sep;

				if (plr.target.ID != meas->instrument.defTarget)
					(*stream) << "TRGT" << sep
					<< plr.target.ID << sep;

				if (plr.target.targetHt != meas->instrument.targets.at(meas->instrument.defTarget).targetHt)
					(*stream) << "TH" << sep
					<< plr.target.targetHt.getMetresValue() << sep;

				if (plr.target.sigmaTargetHt != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetHt)
					(*stream) << "THSE" << sep
					<< plr.target.sigmaTargetHt.getMMetresValue() << sep;

				if (plr.target.sigmaTargetCentering != meas->instrument.targets.at(meas->instrument.defTarget).sigmaTargetCentering)
					(*stream) << "TCSE" << sep
					<< plr.target.sigmaTargetCentering.getMMetresValue() << sep;

				if (plr.target.sigmaAngl != meas->instrument.targets.at(meas->instrument.defTarget).sigmaAngl)
					(*stream) << "ASE" << sep
					<< plr.target.sigmaAngl.getSignedCCValue() << sep;

				if (plr.target.sigmaZenD != meas->instrument.targets.at(meas->instrument.defTarget).sigmaZenD)
					(*stream) << "ZSE" << sep
					<< plr.target.sigmaZenD.getSignedCCValue() << sep;

				if (plr.target.sigmaDist != meas->instrument.targets.at(meas->instrument.defTarget).sigmaDist)
					(*stream) << "DSE" << sep
					<< plr.target.sigmaDist.getMMetresValue() << sep;

				if (plr.target.ppmDist != meas->instrument.targets.at(meas->instrument.defTarget).ppmDist)
					(*stream) << "PPM" << sep
					<< plr.target.ppmDist.getMMetresValue() << sep;

				(*stream)<< endl;
			}
		}

		//ECTH
		if (!rom->measECTH.empty())
		{
			TAngle lecture = rom->measECTH.front().obsHorAngle;

			auto scaleDefInst = data->getInstruments().fSCALE.at(rom->measECTH.front().target.ID);

			(*stream) << "*ECTH" << sep
				<< rom->measECTH.front().obsHorAngle.getGonsValue() << sep
				<< rom->measECTH.front().target.ID << sep
				<< endl;

			for (auto& ecth : rom->measECTH)
			{
				if (ecth.obsHorAngle == lecture)
				{
					(*stream) << ecth.targetPos->getName() << sep
						<< ecth.getDistance().getMetresValue() << sep;
					
					if (ecth.target.ID != scaleDefInst.ID)
						(*stream) << "SCALE" << sep
						<< ecth.target.ID << sep;

					if (ecth.target.sigmaD != scaleDefInst.sigmaD)
						(*stream) << "OBSE" << sep
						<< ecth.target.sigmaD.getMMetresValue() << sep;

					if (ecth.target.ppmD != scaleDefInst.ppmD)
						(*stream) << "PPM" << sep
						<< ecth.target.ppmD.getMMetresValue() << sep;

					if (ecth.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
						(*stream) << "ICSE" << sep
						<< ecth.target.sigmaInstrCentering.getMMetresValue() << sep;
					
					(*stream) << endl;
				}
				else
				{
					lecture = ecth.obsHorAngle;

					(*stream) << "*ECTH" << sep
						<< ecth.obsHorAngle.getGonsValue() << sep
						<< ecth.target.ID << endl;

					(*stream) << ecth.targetPos->getName() << sep
						<< ecth.getDistance().getMetresValue() << sep;

					if (ecth.target.ID != scaleDefInst.ID)
						(*stream) << "SCALE" << sep
						<< ecth.target.ID << sep;

					if (ecth.target.sigmaD != scaleDefInst.sigmaD)
						(*stream) << "OBSE" << sep
						<< ecth.target.sigmaD.getMMetresValue() << sep;

					if (ecth.target.ppmD != scaleDefInst.ppmD)
						(*stream) << "PPM" << sep
						<< ecth.target.ppmD.getMMetresValue() << sep;

					if (ecth.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
						(*stream) << "ICSE" << sep
						<< ecth.target.sigmaInstrCentering.getMMetresValue() << sep;

					(*stream) << endl;
				}
			}
		}

		//ECDIR
		if (!rom->measECDIR.empty())
		{
			auto scaleDefInst = data->getInstruments().fSCALE.at(rom->measECDIR.front().target.ID);
			TAngle lectureHz = rom->measECDIR.front().obsHorAngle;
			TAngle lectureV = rom->measECDIR.front().obsVertAngle;

			(*stream) << "*ECDIR" << sep
				<< rom->measECDIR.front().obsHorAngle.getGonsValue() << sep
				<< rom->measECDIR.front().obsVertAngle.getGonsValue() << sep
				<< rom->measECDIR.front().target.ID << sep
				<< endl;

			for (auto& ecdir : rom->measECDIR)
			{
				if (ecdir.obsHorAngle == lectureHz && ecdir.obsVertAngle == lectureV)
				{
					(*stream) << ecdir.targetPos->getName() << sep
						<< ecdir.getDistance().getMetresValue() << sep;

					if (ecdir.target.ID != scaleDefInst.ID)
						(*stream) << "SCALE" << sep
						<< ecdir.target.ID << sep;

					if (ecdir.target.sigmaD != scaleDefInst.sigmaD)
						(*stream) << "OBSE" << sep
						<< ecdir.target.sigmaD.getMMetresValue() << sep;

					if (ecdir.target.ppmD != scaleDefInst.ppmD)
						(*stream) << "PPM" << sep
						<< ecdir.target.ppmD.getMMetresValue() << sep;

					if (ecdir.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
						(*stream) << "ICSE" << sep
						<< ecdir.target.sigmaInstrCentering.getMMetresValue() << sep;

					(*stream) << endl;
				}
				else
				{
					lectureHz = ecdir.obsHorAngle;
					lectureV = ecdir.obsVertAngle;
					(*stream) << "*ECDIR" << sep
						<< ecdir.obsHorAngle.getGonsValue() << sep
						<< ecdir.obsVertAngle.getGonsValue() << sep
						<< ecdir.target.ID << endl;

					(*stream) << ecdir.targetPos->getName() << sep
						<< ecdir.getDistance().getMetresValue() << sep;

					if (ecdir.target.ID != scaleDefInst.ID)
						(*stream) << "SCALE" << sep
						<< ecdir.target.ID << sep;

					if (ecdir.target.sigmaD != scaleDefInst.sigmaD)
						(*stream) << "OBSE" << sep
						<< ecdir.target.sigmaD.getMMetresValue() << sep;

					if (ecdir.target.ppmD != scaleDefInst.ppmD)
						(*stream) << "PPM" << sep
						<< ecdir.target.ppmD.getMMetresValue() << sep;

					if (ecdir.target.sigmaInstrCentering != scaleDefInst.sigmaInstrCentering)
						(*stream) << "ICSE" << sep
						<< ecdir.target.sigmaInstrCentering.getMMetresValue() << sep;

					(*stream) << endl;
				}
			}
		}
	}
}



