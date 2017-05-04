#include "TFRAMEWriter.h"
#include "TTSTNWriter.h"
#include "TPointConverter.h"
#include "TCAMWriter.h"
#include "TLEVELWriter.h"
#include "TSCALEWriter.h"
#include "TEDMWriter.h"
#include "TOtherMeasurementsWriter.h"
#include "TLOR2LOR.h"
#include "TXYH2CCS.h"
#include "TDirectTransformation.h"
#include "TInverseTransformation.h"
#include "TAStreamFormatter.h"
#include "TSpatialStatus.h"
#include <TPointTransformer.h>
#include "TLGCObsSummary.h"

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
TFRAMEWriter::TFRAMEWriter(TAStreamFormatter& stream, const TLGCData* data): TALGCObjectWriter(stream), fProjectData(data)
{
	//Separate 
	for (AdjPointIter it(fProjectData->getPoints().cbegin()); it != fProjectData->getPoints().cend(); ++it){	
		if(it->getSpatialStatus() == TSpatialStatus::kCala)
			pointCALA.emplace_back(it);
		if(it->getSpatialStatus() == TSpatialStatus::kVxyz)
			pointVXYZ.emplace_back(it);
		if(it->getSpatialStatus() == TSpatialStatus::kVxy)
			pointVXY.emplace_back(it);
		if(it->getSpatialStatus() == TSpatialStatus::kVxz)
			pointVXZ.emplace_back(it);
		if(it->getSpatialStatus() == TSpatialStatus::kVyz)
			pointVYZ.emplace_back(it);
		if(it->getSpatialStatus() == TSpatialStatus::kVz)
			pointVZ.emplace_back(it);
	}
}

TFRAMEWriter::~TFRAMEWriter(){}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void TFRAMEWriter::writeFRAMEAll(TDataTreeIterator frameIt){
	TAStreamFormatter*	stream = getStream();
	std::string			TABs = stream->getCurrSpace();
	stream->setTreeDepth((int)frameIt->get()->ID.size() - 1); //Size of the ID is equal to the depth in the tree, which corresponds to the number o TABs to be used in formatting. Zero TABs for ROOT (depth 1).
	

	writeFRAMEHeader(frameIt->get()->frame.getName(), frameIt->get()->ID);

	if(frameIt->get()->frame.getName() != "ROOT")
		writeFRAMEDefinition(*frameIt->get());
	
	writePoints(frameIt);

	//legend of tables for the points in ROOT
	if(frameIt->get()->isROOTNode())
		(*stream)<<TABs +"SFP = Sub-Frame Point; * = TRUE"<<endl;
	(*stream)<<endl<<endl;


	//Start to write the measurements
	TTSTNWriter tstnWriter(*stream, fProjectData->getConfig().histo.isActive());
	tstnWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TCAMWriter camWriter(*stream, fProjectData->getConfig().histo.isActive());// no allfixed parameter
	TEDMWriter edmWriter(*stream, fProjectData->getConfig().histo.isActive());
	edmWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TSCALEWriter scaleWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TLEVELWriter levelWriter(*stream, fProjectData->getConfig().histo.isActive());
	levelWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TOtherMeasurentWriter otherMeasWriter(*stream, fProjectData->getConfig().histo.isActive());// no allfixed parameter

	//If PDOR
	if (frameIt->get()->measurements.fPDOR.isInitialised())
		otherMeasWriter.writePDORResults(frameIt->get()->measurements.fPDOR);

	//Summary
	(*stream) << TABs << "*** RESUME DES MESURES ***" << endl << endl;
	writeMeasurementsSummary(frameIt);
	if (fProjectData->getConfig().histo.isActive())
		writeHistogramme(frameIt);


	//Measures
	(*stream) << endl << endl<< TABs << "*** MESURES ***" << endl << endl;
	for(auto& itTSTN:frameIt->get()->measurements.fTSTN)
		tstnWriter.writeTSTNResults(itTSTN);

	for(auto& itCAM:frameIt->get()->measurements.fCAM)
		camWriter.writeCAMResults(itCAM);

	for(auto& itLEVEL:frameIt->get()->measurements.fLEVEL)
		levelWriter.writeLEVELResults(itLEVEL);

	//No instrument for DVER, so no loop to have each instrument.
	if (!frameIt->get()->measurements.fDVER.empty())
	{
		(*stream) << endl;
		otherMeasWriter.writeDVERResults(frameIt->get()->measurements.fDVER);
	}

	for (auto& itECHO : frameIt->get()->measurements.fECHO)
		scaleWriter.writeECHOResults(itECHO);
	
	for (auto& itECVE : frameIt->get()->measurements.fECVE)
		scaleWriter.writeECVEResults(itECVE);
	
	for (auto& itECSP : frameIt->get()->measurements.fECSP)
		scaleWriter.writeECSPResults(itECSP);

	for (auto& itORIE : frameIt->get()->measurements.fORIE)
	{
		(*stream) << endl;
		otherMeasWriter.writeORIEResults(itORIE.measORIE, *itORIE.instrumentPos);
	}

	if (!frameIt->get()->measurements.fRADI.empty())
	{
		(*stream) << endl;
		otherMeasWriter.writeRADIResults(frameIt->get()->measurements.fRADI);
	}

	if (!frameIt->get()->measurements.fCMM.empty())
	{
		(*stream) << endl;
		otherMeasWriter.writeCMMResults(frameIt->get()->measurements.fCMM);
	}

	for (auto& itEDM : frameIt->get()->measurements.fEDM)
		edmWriter.writeEDMResults(itEDM);
		
}

void TFRAMEWriter::writeMeasurementsSummary(TDataTreeIterator frameIt){
	TAStreamFormatter*	stream = getStream();
	std::string			TABs = stream->getCurrSpace();
	int					nameWidth = getNameWidth();

	//Start to write the measurements
	TTSTNWriter tstnWriter(*stream, fProjectData->getConfig().histo.isActive());
	tstnWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TCAMWriter camWriter(*stream, fProjectData->getConfig().histo.isActive());// no allfixed parameter
	TSCALEWriter scaleWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TLEVELWriter levelWriter(*stream, fProjectData->getConfig().histo.isActive());
	levelWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TOtherMeasurentWriter otherMeasWriter(*stream, fProjectData->getConfig().histo.isActive());// no allfixed parameter
	TEDMWriter edmWriter(*stream, fProjectData->getConfig().histo.isActive());
	edmWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter

	//TSTN
	if (frameIt->get()->measurements.fTSTN.size() > 0)
	{
		//write ANGL summary
		bool headerAnglWriten = false;
		for (auto& it : frameIt->get()->measurements.fTSTN)
		{
			for (auto& itrom : it->roms)
			{
				if (itrom->measANGL.size() > 0){
					if (!headerAnglWriten)
					{
						(*stream) << TABs;
						(*stream).writeStringLeft(nameWidth, "ANGL");
						(*stream) << endl;
						tstnWriter.writeANGLHeaderSynthesis();
						headerAnglWriten = true;
					}
					tstnWriter.writeANGLResultsSynthesis(itrom->measANGL, it->instrumentPos, it->roms);
				}
			}
		}
		if (headerAnglWriten)
		{//write global mean
			TLGCObsSummary ANGLsummary;
			for (auto& it : frameIt->get()->measurements.fTSTN)
				for (auto& itrom : it->roms)
					for (auto& it : itrom->measANGL)
						ANGLsummary.addNewResidual(it.getAngleResidual().getSignedCCValue());

			tstnWriter.writeAngleResultsSummary(ANGLsummary, TABs);
		}
		

		//then ZEND 
		bool headerZendWriten = false;
		for (auto& it : frameIt->get()->measurements.fTSTN)
		{
			for (auto& itrom : it->roms)
			{
				if (itrom->measZEND.size() > 0){
					if (!headerZendWriten)
					{
						(*stream) << endl;
						(*stream) << TABs;
						(*stream).writeStringLeft(nameWidth, "ZEND");
						(*stream) << endl;
						tstnWriter.writeANGLHeaderSynthesis();
						headerZendWriten = true;
					}
					tstnWriter.writeZENDResultsSynthesis(itrom->measZEND, it->instrumentPos, it->roms);
				}
			}
		}
		if (headerZendWriten)
		{//write global mean
			TLGCObsSummary summary;
			for (auto& it : frameIt->get()->measurements.fTSTN)
				for (auto& itrom : it->roms)
					for (auto& it : itrom->measZEND)
						summary.addNewResidual(it.getAngleResidual().getSignedCCValue());

			tstnWriter.writeAngleResultsSummary(summary, TABs);
		}
		

		//DITS
		bool headerDistWriten = false;
		for (auto& it : frameIt->get()->measurements.fTSTN)
		{
			for (auto& itrom : it->roms)
			{
				if (itrom->measDIST.size() > 0){
					if (!headerDistWriten)
					{
						(*stream) << endl;
						(*stream) << TABs;
						(*stream).writeStringLeft(nameWidth, "DIST");
						(*stream) << endl;
						tstnWriter.writeDISTHeaderSynthesis();
						headerDistWriten = true;
					}
					tstnWriter.writeDISTResultsSynthesis(itrom->measDIST, it->instrumentPos, it->roms);
				}
			}
		}
		if (headerDistWriten)
		{//write global mean
			TLGCObsSummary summary;
			for (auto& it : frameIt->get()->measurements.fTSTN)
				for (auto& itrom : it->roms)
					for (auto& it : itrom->measDIST)
						summary.addNewResidual(it.getDistanceResidual().getMMetresValue());

			tstnWriter.writeDistanceResultsSummary(summary, TABs);
		}
		

		//DHOR
		bool headerDhorWriten = false;
		for (auto& it : frameIt->get()->measurements.fTSTN)
		{
			for (auto& itrom : it->roms)
			{
				if (itrom->measDHOR.size() > 0){
					if (!headerDhorWriten)
					{
						(*stream) << endl;
						(*stream) << TABs;
						(*stream).writeStringLeft(nameWidth, "DHOR");
						(*stream) << endl;
						tstnWriter.writeDISTHeaderSynthesis();
						headerDhorWriten = true;
					}
					tstnWriter.writeDISTResultsSynthesis(itrom->measDHOR, it->instrumentPos, it->roms, true);
				}
			}
		}
		if (headerDhorWriten)
		{//write global mean
			TLGCObsSummary summary;
			for (auto& it : frameIt->get()->measurements.fTSTN)
				for (auto& itrom : it->roms)
					for (auto& it : itrom->measDHOR)
						summary.addNewResidual(it.getDistanceResidual().getMMetresValue());

			tstnWriter.writeDistanceResultsSummary(summary, TABs);
		}
		

		//PLR3D
		bool headerPlrWriten = false;
		for (auto& it : frameIt->get()->measurements.fTSTN)
		{
			for (auto& itrom : it->roms)
			{
				if (itrom->measPLR3D.size() > 0){
					if (!headerPlrWriten)
					{
						(*stream) << endl;
						tstnWriter.writePLRRHeaderynthesis();
						headerPlrWriten = true;
					}
					tstnWriter.writePLRResultsSynthesis(itrom->measPLR3D, it->instrumentPos, it->roms);
				}
			}
		}
		if (headerPlrWriten)
		{//write global mean
			TPOLARObsSummary PLRsummary;
			for (auto& it : frameIt->get()->measurements.fTSTN)
				for (auto& itrom : it->roms)
					for (auto& itplr : itrom->measPLR3D)
					{
						PLRsummary.anglObsSum.addNewResidual(itplr.getAngleResidual(EPLR3DAngles::kANGL).getSignedCCValue());
						PLRsummary.zendObsSum.addNewResidual(itplr.getAngleResidual(EPLR3DAngles::kZEND).getSignedCCValue());
						PLRsummary.distObsSum.addNewResidual(itplr.getDistanceResidual().getMMetresValue());
					}

			(*stream) << "PLR3D: ANGL"<<endl;
			tstnWriter.writeAngleResultsSummary(PLRsummary.anglObsSum, TABs);
			(*stream) << "PLR3D: ZEND"<<endl;
			tstnWriter.writeAngleResultsSummary(PLRsummary.zendObsSum, TABs);
			(*stream) << "PLR3D: DIST"<<endl;
			tstnWriter.writeDistanceResultsSummary(PLRsummary.distObsSum, TABs);
		}

		//ECTH
		bool headerEcthWriten = false;
		for (auto& it : frameIt->get()->measurements.fTSTN)
		{
			for (auto& itrom : it->roms)
			{
				if (itrom->measECTH.size() > 0){
					if (!headerEcthWriten)
					{
						(*stream) << endl;
						(*stream) << TABs;
						(*stream).writeStringLeft(nameWidth, "ECTH");
						(*stream) << endl;
						tstnWriter.writeDISTHeaderSynthesis();
						headerEcthWriten = true;
					}
					tstnWriter.writeECTHResultsSynthesis(itrom->measECTH, it->instrumentPos, it->roms);
				}
			}
		}
		if (headerEcthWriten)
		{//write global mean
			TLGCObsSummary summary;
			for (auto& it : frameIt->get()->measurements.fTSTN)
				for (auto& itrom : it->roms)
					for (auto& it : itrom->measECTH)
						summary.addNewResidual(it.getDistanceResidual().getMMetresValue());

			tstnWriter.writeDistanceResultsSummary(summary, TABs);
		}
		

		//ECDIR
		bool headerEcdirWriten = false;
		for (auto& it : frameIt->get()->measurements.fTSTN)
		{
			for (auto& itrom : it->roms)
			{
				if (itrom->measECDIR.size() > 0){
					if (!headerEcdirWriten)
					{
						(*stream) << endl;
						(*stream) << TABs;
						(*stream).writeStringLeft(nameWidth, "ECDIR");
						(*stream) << endl;
						tstnWriter.writeDISTHeaderSynthesis();
						headerEcdirWriten = true;
					}
					tstnWriter.writeECDIRResultsSynthesis(itrom->measECDIR, it->instrumentPos, it->roms);
				}
			}
		}
		if (headerEcdirWriten)
		{//write global mean
			TLGCObsSummary summary;
			for (auto& it : frameIt->get()->measurements.fTSTN)
				for (auto& itrom : it->roms)
					for (auto& it : itrom->measECDIR)
						summary.addNewResidual(it.getDistanceResidual().getMMetresValue());

			tstnWriter.writeDistanceResultsSummary(summary, TABs);
		}
	}

	//BCAM
	if (frameIt->get()->measurements.fCAM.size() > 0)
	{
		//write UVD summary
		bool headerWriten = false;
		for (auto& it : frameIt->get()->measurements.fCAM)
		{
			if (it.measUVD.size() > 0)
			{
				if (!headerWriten)
				{
					(*stream) << endl;
					camWriter.writeUVDSynthesisHeader();
					headerWriten = true;
				}
				camWriter.writeUVDResultsSynthesis(it);
			}
		}
		if (headerWriten)
		{
			TUVDObsSummary summaryUVD;
			//write UVD summary
			for (auto& it : frameIt->get()->measurements.fCAM)
				for (auto& itUVD : it.measUVD)
				{
					summaryUVD.xVectorCompObsSum.addNewResidual(itUVD.getXCompVectorResidual()*M2MM);
					summaryUVD.yVectorCompObsSum.addNewResidual(itUVD.getYCompVectorResidual()*M2MM);
					summaryUVD.distObsSum.addNewResidual(itUVD.getDistanceResidual().getMMetresValue());
				}

			(*stream) << "UVD: XVEC" << endl;
			camWriter.writeUnitlessResultsSummary(summaryUVD.xVectorCompObsSum, TABs);
			(*stream) << "UVD: YVEC" << endl;
			camWriter.writeUnitlessResultsSummary(summaryUVD.yVectorCompObsSum, TABs);
			(*stream) << "UVD: DIST" << endl;
			camWriter.writeDistanceResultsSummary(summaryUVD.distObsSum, TABs);

		}

		//the UVEC when all UVD are written
		bool headerWriten2 = false;
		for (auto& it : frameIt->get()->measurements.fCAM)
		{
			if (it.measUVEC.size() > 0)
			{
				if (!headerWriten2)
				{
					(*stream) << endl;
					camWriter.writeUVECSynthesisHeader();
					headerWriten2 = true;
				}
				camWriter.writeUVECResultsSynthesis(it);
			}
		}
		if (headerWriten2)
		{
			TUVDObsSummary summaryUVEC;
			for (auto& it : frameIt->get()->measurements.fCAM)
				for (auto& itUVEC : it.measUVEC)
				{
					summaryUVEC.xVectorCompObsSum.addNewResidual(itUVEC.getXCompVectorResidual()*M2MM);
					summaryUVEC.yVectorCompObsSum.addNewResidual(itUVEC.getYCompVectorResidual()*M2MM);
				}

			(*stream) << "UVEC: XVEC" << endl;
			camWriter.writeUnitlessResultsSummary(summaryUVEC.xVectorCompObsSum, TABs);
			(*stream) << "UVEC: YVEC" << endl;
			camWriter.writeUnitlessResultsSummary(summaryUVEC.yVectorCompObsSum, TABs);
		}
	}

	//DLEV
	if (frameIt->get()->measurements.fLEVEL.size() > 0)
	{
		(*stream) << endl;
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "DLEV"); //instrument
		(*stream) << endl;
		levelWriter.writeLEVELSynthesisHeader();
		for (auto& itLEVEL : frameIt->get()->measurements.fLEVEL)
			levelWriter.writeLEVELResultsSynthesis(itLEVEL);
		//(*stream) << endl;

		//write global mean
		TLGCObsSummary DLEVsummary;
		if (frameIt->get()->measurements.fLEVEL.size() > 0)
			for (auto& itLEVEL : frameIt->get()->measurements.fLEVEL)
				for (auto& itDLEV : itLEVEL.measDLEV)
					DLEVsummary.addNewResidual(itDLEV.getDistanceResidual().getMMetresValue());
		levelWriter.writeDistanceResultsSummary(DLEVsummary, TABs);
	}

	//DVER
	if (frameIt->get()->measurements.fDVER.size() > 0)
	{
		(*stream) << endl;
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "DVER"); //instrument
		(*stream) << endl;
		otherMeasWriter.writeResultsSynthesisHeader();
		otherMeasWriter.writeDVERResultsSynthesis(frameIt->get()->measurements.fDVER);
		//(*stream) << endl;

		TLGCObsSummary DVERsummary;
		if (frameIt->get()->measurements.fDVER.size() > 0)
			for (auto const& ItDVER : frameIt->get()->measurements.fDVER)
				DVERsummary.addNewResidual(ItDVER.getDistanceResidual().getMMetresValue());
		otherMeasWriter.writeDistanceResultsSummary(DVERsummary,TABs);
	}

	//ECHO
	if (frameIt->get()->measurements.fECHO.size() > 0)
	{
		(*stream) << endl;
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ECHO"); //instrument
		(*stream) << endl;
		scaleWriter.writeSCALESynthesisHeader();
		for (auto& itECHO : frameIt->get()->measurements.fECHO)
			scaleWriter.writeECHOResultsSynthesis(itECHO);
		//(*stream) << endl;

		TLGCObsSummary ECHOsummary;
		if (frameIt->get()->measurements.fECHO.size() > 0)
			for (auto& itECHOPROM : frameIt->get()->measurements.fECHO)
				for (auto& itECHO : itECHOPROM.measECHO)
					ECHOsummary.addNewResidual(itECHO.getDistanceResidual().getMMetresValue());
		scaleWriter.writeDistanceResultsSummary(ECHOsummary, TABs);
	}

	//DSPT
	if (frameIt->get()->measurements.fECHO.size() > 0)
	{
		(*stream) << endl;
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "DSPT"); //instrument
		(*stream) << endl;

		TLGCObsSummary DSPTsummary;
		for (auto& it : frameIt->get()->measurements.fEDM)
			for (auto& itDSPT : it.measDSPT)
				DSPTsummary.addNewResidual(itDSPT.getDistanceResidual().getMMetresValue());
		edmWriter.writeDistanceResultsSummary(DSPTsummary, TABs);
	}

	//ECVE
	if (frameIt->get()->measurements.fECVE.size() > 0)
	{
		(*stream) << endl;
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ECVE"); //instrument
		(*stream) << endl;
		scaleWriter.writeSCALESynthesisHeader();
		for (auto& itECVE : frameIt->get()->measurements.fECVE)
			scaleWriter.writeECVEResultsSynthesis(itECVE);
		//(*stream) << endl;

		TLGCObsSummary summary;
		if (frameIt->get()->measurements.fECHO.size() > 0)
			for (auto& itECVEPROM : frameIt->get()->measurements.fECVE)
				for (auto& itECVE : itECVEPROM.measECVE)
					summary.addNewResidual(itECVE.getDistanceResidual().getMMetresValue());
		scaleWriter.writeDistanceResultsSummary(summary, TABs);
	}

	//ECSP
	if (frameIt->get()->measurements.fECSP.size() > 0)
	{
		(*stream) << endl;
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ECSP"); //instrument
		(*stream) << endl;
		scaleWriter.writeSCALESynthesisHeader();
		for (auto& itECSP : frameIt->get()->measurements.fECSP)
			scaleWriter.writeECSPResultsSynthesis(itECSP);
		//(*stream) << endl;

		TLGCObsSummary summary;
		if (frameIt->get()->measurements.fECHO.size() > 0)
			for (auto& itECSPPROM : frameIt->get()->measurements.fECSP)
				for (auto& itECSP : itECSPPROM.measECSP)
					summary.addNewResidual(itECSP.getDistanceResidual().getMMetresValue());
		scaleWriter.writeDistanceResultsSummary(summary, TABs);
	}
	
	//ORIE
	if (frameIt->get()->measurements.fORIE.size() > 0)
	{
		(*stream) << endl;
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "ORIE"); //instrument
		(*stream) << endl;
		otherMeasWriter.writeResultsSynthesisHeader();
		for (auto& itORIE : frameIt->get()->measurements.fORIE)
			otherMeasWriter.writeORIEResultsSynthesis(itORIE.measORIE,*itORIE.instrumentPos);
		//(*stream) << endl;

		TLGCObsSummary ORIEsummary;
		if (frameIt->get()->measurements.fORIE.size() > 0)
			for (auto const& It : frameIt->get()->measurements.fORIE)
				for (auto const& ItOrie : It.measORIE)
					ORIEsummary.addNewResidual(ItOrie.getAngleResidual().getSignedCCValue());
		otherMeasWriter.writeAngleResultsSummary(ORIEsummary, TABs);
	}
	
	//RADI
	if (frameIt->get()->measurements.fRADI.size() > 0)
	{
		(*stream) << endl;
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "RADI"); //instrument
		(*stream) << endl;
		otherMeasWriter.writeResultsSynthesisHeader();
		otherMeasWriter.writeRADIResultsSynthesis(frameIt->get()->measurements.fRADI);
		//(*stream) << endl;

		TLGCObsSummary RADIsummary;
		if (frameIt->get()->measurements.fRADI.size() > 0)
			for (auto const& It : frameIt->get()->measurements.fRADI)
				RADIsummary.addNewResidual(It.getResidual().getMMetresValue());
		otherMeasWriter.writeDistanceResultsSummary(RADIsummary,TABs);
	}

	//CMM
	if (frameIt->get()->measurements.fCMM.size() > 0)
	{
		(*stream) << endl;
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, "CMM"); //instrument
		(*stream) << endl;
		otherMeasWriter.writeResultsSynthesisHeader();
		otherMeasWriter.writeCMMResultsSynthesis(frameIt->get()->measurements.fCMM);

		TLGCObsSummary XCMMsummary;
		TLGCObsSummary YCMMsummary;
		TLGCObsSummary ZCMMsummary;
		if (frameIt->get()->measurements.fCMM.size() > 0)
			for (auto const& It : frameIt->get()->measurements.fCMM)
			{
				XCMMsummary.addNewResidual(It.getXResidual().getMMetresValue());
				YCMMsummary.addNewResidual(It.getYResidual().getMMetresValue());
				ZCMMsummary.addNewResidual(It.getZResidual().getMMetresValue());
			}
		otherMeasWriter.writeDistanceResultsSummary(XCMMsummary, TABs);
		otherMeasWriter.writeDistanceResultsSummary(YCMMsummary, TABs);
		otherMeasWriter.writeDistanceResultsSummary(ZCMMsummary, TABs);
	}
}

///write measurements summary
void TFRAMEWriter::writeHistogramme(TDataTreeIterator frameIt){
	TAStreamFormatter*	stream = getStream();
	std::string			TABs = stream->getCurrSpace();
	int					nameWidth = getNameWidth();

	//Start to write the measurements
	TTSTNWriter tstnWriter(*stream, fProjectData->getConfig().histo.isActive());
	tstnWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TCAMWriter camWriter(*stream, fProjectData->getConfig().histo.isActive());// no allfixed parameter
	TSCALEWriter scaleWriter(*stream, fProjectData->getConfig().histo.isActive()); // no allfixed parameter
	TLEVELWriter levelWriter(*stream, fProjectData->getConfig().histo.isActive());
	levelWriter.setAllfixed(fProjectData->getConfig().allfixed.isActive()); // to be able to write the allfixed parameter
	TOtherMeasurentWriter otherMeasWriter(*stream, fProjectData->getConfig().histo.isActive());// no allfixed parameter

	//TSTN
	if (frameIt->get()->measurements.fTSTN.size() > 0)
	{
		//write ANGL summary
		TLGCObsSummary ANGLsummary;
		for (auto& it : frameIt->get()->measurements.fTSTN)
			for (auto& itrom : it->roms)
				for (auto& it : itrom->measANGL)
					ANGLsummary.addNewResidual(it.getAngleResidual().getSignedCCValue());

		if (ANGLsummary.getNumberOfObs() >= 5)
		{
			(*stream) << endl;
			tstnWriter.writeHisto(ANGLsummary, "ANGL");
			
		}


		//then ZEND 
		TLGCObsSummary ZENDsummary;
		for (auto& it : frameIt->get()->measurements.fTSTN)
			for (auto& itrom : it->roms)
				for (auto& it : itrom->measZEND)
					ZENDsummary.addNewResidual(it.getAngleResidual().getSignedCCValue());

		if (ZENDsummary.getNumberOfObs() >= 5)
		{
			(*stream) << endl;
			tstnWriter.writeHisto(ZENDsummary, "ZEND");
		}


		//DITS
		TLGCObsSummary DISTsummary;
		for (auto& it : frameIt->get()->measurements.fTSTN)
			for (auto& itrom : it->roms)
				for (auto& it : itrom->measDIST)
					DISTsummary.addNewResidual(it.getDistanceResidual().getMMetresValue());

		if (DISTsummary.getNumberOfObs() >= 5)
		{
			(*stream) << endl;
			tstnWriter.writeHisto(DISTsummary, "DIST");
		}


		//DHOR
		TLGCObsSummary DHORsummary;
		for (auto& it : frameIt->get()->measurements.fTSTN)
			for (auto& itrom : it->roms)
				for (auto& it : itrom->measDHOR)
					DHORsummary.addNewResidual(it.getDistanceResidual().getMMetresValue());
		
		if (DHORsummary.getNumberOfObs() >= 5)
		{
			(*stream) << endl;
			tstnWriter.writeHisto(DHORsummary, "DHOR");
		}


		//PLR3D
		TPOLARObsSummary PLRsummary;
		for (auto& it : frameIt->get()->measurements.fTSTN)
			for (auto& itrom : it->roms)
				for (auto& itplr : itrom->measPLR3D)
			{
				PLRsummary.anglObsSum.addNewResidual(itplr.getAngleResidual(EPLR3DAngles::kANGL).getSignedCCValue());
				PLRsummary.zendObsSum.addNewResidual(itplr.getAngleResidual(EPLR3DAngles::kZEND).getSignedCCValue());
				PLRsummary.distObsSum.addNewResidual(itplr.getDistanceResidual().getMMetresValue());
			}

		if (PLRsummary.anglObsSum.getNumberOfObs() >= 5)
		{
			(*stream) << endl;
			tstnWriter.writeHisto(PLRsummary.anglObsSum, "PLR3D: ANGL");
			(*stream) << endl;
			tstnWriter.writeHisto(PLRsummary.zendObsSum, "PLR3D: ZEND");
			(*stream) << endl;
			tstnWriter.writeHisto(PLRsummary.distObsSum, "PLR3D: DIST");
		}


		//ECTH
		TLGCObsSummary ECTHsummary;
		for (auto& it : frameIt->get()->measurements.fTSTN)
			for (auto& itrom : it->roms)
				for (auto& it : itrom->measECTH)
					ECTHsummary.addNewResidual(it.getDistanceResidual().getMMetresValue());
		
		if (ECTHsummary.getNumberOfObs() >= 5)
		{
			(*stream) << endl;
			scaleWriter.writeHisto(ECTHsummary, "ECTH");
		}


		//ECDIR
		TLGCObsSummary ECDIRsummary;
		for (auto& it : frameIt->get()->measurements.fTSTN)
			for (auto& itrom : it->roms)
				for (auto& it : itrom->measECDIR)
					ECDIRsummary.addNewResidual(it.getDistanceResidual().getMMetresValue());
		
		if (ECDIRsummary.getNumberOfObs() >= 5)
		{
			(*stream) << endl;
			scaleWriter.writeHisto(ECDIRsummary, "ECDIR");
		}

	}

	//BCAM
	if (frameIt->get()->measurements.fCAM.size() > 0)
	{
		TUVDObsSummary summaryUVD;
		//write UVD summary
		for (auto& it : frameIt->get()->measurements.fCAM)
			for (auto& itUVD : it.measUVD)
			{
				summaryUVD.xVectorCompObsSum.addNewResidual(itUVD.getXCompVectorResidual()*M2MM);
				summaryUVD.yVectorCompObsSum.addNewResidual(itUVD.getYCompVectorResidual()*M2MM);
				summaryUVD.distObsSum.addNewResidual(itUVD.getDistanceResidual().getMMetresValue());
			}

		
		if (summaryUVD.distObsSum.getNumberOfObs() >= 5)
		{
			(*stream) << endl;
			scaleWriter.writeHisto(summaryUVD.xVectorCompObsSum, "UVD: XVEC");
			(*stream) << endl;
			scaleWriter.writeHisto(summaryUVD.yVectorCompObsSum, "UVD: YVEC");
			(*stream) << endl;
			scaleWriter.writeHisto(summaryUVD.distObsSum, "UVD: DIST");
		}

		//the UVEC when all UVD are written
		TUVECObsSummary summaryUVEC;
		for (auto& it : frameIt->get()->measurements.fCAM)
			for (auto& itUVD : it.measUVEC)
			{
				summaryUVEC.xVectorCompObsSum.addNewResidual(itUVD.getXCompVectorResidual()*M2MM);
				summaryUVEC.yVectorCompObsSum.addNewResidual(itUVD.getYCompVectorResidual()*M2MM);
			}


		if (summaryUVD.distObsSum.getNumberOfObs() >= 5)
		{
			(*stream) << endl;
			scaleWriter.writeHisto(summaryUVEC.xVectorCompObsSum, "UVEC: XVEC");
			(*stream) << endl;
			scaleWriter.writeHisto(summaryUVEC.yVectorCompObsSum, "UVEC: YVEC");
		}
	}

	//DLEV
	TLGCObsSummary DLEVsummary;
	if (frameIt->get()->measurements.fLEVEL.size() > 0)
		for (auto& itLEVEL : frameIt->get()->measurements.fLEVEL)
			for (auto& itDLEV : itLEVEL.measDLEV)
				DLEVsummary.addNewResidual(itDLEV.getDistanceResidual().getMMetresValue());

	if (DLEVsummary.getNumberOfObs() >= 5)
	{
		(*stream) << endl;
		levelWriter.writeHisto(DLEVsummary, "DLEV");
	}
	

	//DVER
	TLGCObsSummary DVERsummary;
	if (frameIt->get()->measurements.fDVER.size() > 0)
		for (auto const& ItDVER : frameIt->get()->measurements.fDVER)
			DVERsummary.addNewResidual(ItDVER.getDistanceResidual().getMMetresValue());
		
	if (DVERsummary.getNumberOfObs() >= 5)
	{
		(*stream) << endl;
		otherMeasWriter.writeHisto(DVERsummary, "DVER");
	}

	//ECHO
	TLGCObsSummary ECHOsummary;
	if (frameIt->get()->measurements.fECHO.size() > 0)
		for (auto& itECHOPROM : frameIt->get()->measurements.fECHO)
			for (auto& itECHO : itECHOPROM.measECHO)
				ECHOsummary.addNewResidual(itECHO.getDistanceResidual().getMMetresValue());

	if (ECHOsummary.getNumberOfObs() >= 5)
	{
		(*stream) << endl;
		scaleWriter.writeHisto(ECHOsummary, "ECHO");
	}

	//DSPT
	TLGCObsSummary DSPTsummary;
	for (auto& it : frameIt->get()->measurements.fEDM)	
		for (auto& itDSPT : it.measDSPT)
			DSPTsummary.addNewResidual(itDSPT.getDistanceResidual().getMMetresValue());

	if (DSPTsummary.getNumberOfObs() >= 5)
	{
		(*stream) << endl;
		tstnWriter.writeHisto(DSPTsummary, "DSPT");
	}

	//ECVE
	TLGCObsSummary ECVEsummary;
	if (frameIt->get()->measurements.fECVE.size() > 0)
		for (auto& itECVEROM : frameIt->get()->measurements.fECVE)
			for (auto& itECVE : itECVEROM.measECVE)
				ECVEsummary.addNewResidual(itECVE.getDistanceResidual().getMMetresValue());

	if (ECVEsummary.getNumberOfObs() >= 5)
	{
		(*stream) << endl;
		scaleWriter.writeHisto(ECVEsummary, "ECVE");
	}

	//ECSP
	TLGCObsSummary ECSPsummary;
	if (frameIt->get()->measurements.fECSP.size() > 0)
		for (auto& itECSPROM : frameIt->get()->measurements.fECSP)
			for (auto& itECSP : itECSPROM.measECSP)
			ECSPsummary.addNewResidual(itECSP.getDistanceResidual().getMMetresValue());

	if (ECSPsummary.getNumberOfObs() >= 5)
	{
		(*stream) << endl;
		scaleWriter.writeHisto(ECSPsummary, "ECSP");
	}

	//ORIE
	TLGCObsSummary ORIEsummary;
	if (frameIt->get()->measurements.fORIE.size() > 0)
		for (auto const& It : frameIt->get()->measurements.fORIE)
			for (auto const& ItOrie : It.measORIE)
				ORIEsummary.addNewResidual(ItOrie.getAngleResidual().getSignedCCValue());
		
	if (ORIEsummary.getNumberOfObs() >= 5)
	{
		(*stream) << endl;
		otherMeasWriter.writeHisto(ORIEsummary, "ORIE");
	}

	//RADI
	TLGCObsSummary RADIsummary;
	if (frameIt->get()->measurements.fRADI.size() > 0)
		for (auto const& It : frameIt->get()->measurements.fRADI)
			RADIsummary.addNewResidual(It.getResidual().getMMetresValue());

	if (RADIsummary.getNumberOfObs() >= 5)
	{
		(*stream) << endl;
		otherMeasWriter.writeHisto(RADIsummary, "RADI");
	}
}



void TFRAMEWriter::writeFRAMESimu(TDataTreeIterator frameIt){
	TAStreamFormatter*	stream = getStream();
	std::string			TABs = stream->getCurrSpace();
	stream->setTreeDepth((int)frameIt->get()->ID.size() - 1); //Size of the ID is equal to the depth in the tree, which corresponds to the number o TABs to be used in formatting. Zero TABs for ROOT (depth 1).

	writeFRAMEHeader(frameIt->get()->frame.getName(), frameIt->get()->ID);

	if(frameIt->get()->frame.getName() != "ROOT")
		writeFRAMEDefinition(*frameIt->get());
	else
		writePoints(frameIt);  //Write points for SIMU only in ROOT, not in local nodes
		

	TTSTNWriter tstnWriter(*stream, fProjectData->getConfig().histo.isActive());
	TCAMWriter camWriter(*stream, fProjectData->getConfig().histo.isActive());
	TEDMWriter edmWriter(*stream, fProjectData->getConfig().histo.isActive());
	TSCALEWriter scaleWriter(*stream, fProjectData->getConfig().histo.isActive());
	TLEVELWriter levelWriter(*stream, fProjectData->getConfig().histo.isActive());
	TOtherMeasurentWriter otherMeasWriter(*stream, fProjectData->getConfig().histo.isActive());

	//If PDOR
	if (frameIt->get()->measurements.fPDOR.isInitialised())
		otherMeasWriter.writePDORResults(frameIt->get()->measurements.fPDOR);

	//Summary
	//(*stream) << endl << TABs << "*** MEASUREMENTS SUMMARY ***" << endl << endl;
	//writeMeasurementsSummary(frameIt);
	//if (fProjectData->getConfig().histo.isActive())
	//	writeHistogramme(frameIt);


	//Measures
	(*stream) << endl << endl << TABs << "*** MEASUREMENTS DATA ***" << endl << endl;
	for(auto& itTSTN:frameIt->get()->measurements.fTSTN)
		tstnWriter.writeTSTNResultsSIMU(itTSTN);

	for(auto& itCAM:frameIt->get()->measurements.fCAM)
		camWriter.writeCAMResultsSIMU(itCAM);

	for (auto& itLEVEL: frameIt->get()->measurements.fLEVEL)
		levelWriter.writeLEVELSIMUResults(itLEVEL);

	//No instrument for DVER, so no loop to have each instrument.
	if (!frameIt->get()->measurements.fDVER.empty()){
		(*stream) << TABs << "DVER" << endl << endl;
		otherMeasWriter.writeDVERSIMUResults(frameIt->get()->measurements.fDVER);
	}
	
	for(auto& itECHO:frameIt->get()->measurements.fECHO)
		scaleWriter.writeECHOSIMUResults(itECHO);
	
	for (auto& itECVE : frameIt->get()->measurements.fECVE)
		scaleWriter.writeECVESIMUResults(itECVE);
	
	for (auto& itECSP : frameIt->get()->measurements.fECSP)
		scaleWriter.writeECSPSIMUResults(itECSP);

	for (auto& itORIE : frameIt->get()->measurements.fORIE)
		otherMeasWriter.writeORIESIMUResults(itORIE);


	for (auto& itEDM : frameIt->get()->measurements.fEDM)
		edmWriter.writeEDMSIMUResults(itEDM);

}

void TFRAMEWriter::writeFRAMEAllReliability(TDataTreeIterator frameIt){
	TAStreamFormatter*	stream = getStream();
	TEDMWriter edmWriter(*stream, fProjectData->getConfig().histo.isActive());
	TOtherMeasurentWriter otherMeasWriter(*stream, fProjectData->getConfig().histo.isActive());

	stream->setTreeDepth((int)frameIt->get()->ID.size() - 1); //Size of the ID is equal to the depth in the tree, which corresponds to the number o TABs to be used in formatting. Zero TABs for ROOT (depth 1).

	writeFRAMEHeader(frameIt->get()->frame.getName(), frameIt->get()->ID);

	if(frameIt->get()->frame.getName() != "ROOT")
		writeFRAMEDefinition(*frameIt->get());
	
	writeTSTNReliability(frameIt);
	writeCAMReliability(frameIt);
	writeLEVELReliability(frameIt);

	if (! frameIt->get()->measurements.fDVER.empty())
	{
		(*stream) << endl << "DVER observations" << endl;
		otherMeasWriter.writeDVERReliabilityHeader();
		otherMeasWriter.writeDVERReliabilityData(frameIt->get()->measurements.fDVER, fProjectData->getStatistics());
	}


	writeSCALEReliability(frameIt);

	bool  ORIEheaderWritten = false;
	for (auto& itORIE:frameIt->get()->measurements.fORIE)
	{
		if (!ORIEheaderWritten)
		{
			(*stream) << endl << "ORIE observations" << endl;
			otherMeasWriter.writeORIEReliabilityHeader();
			ORIEheaderWritten = true;
		}
		otherMeasWriter.writeORIEReliabilityData(itORIE.measORIE, fProjectData->getStatistics(), *itORIE.instrumentPos);
	}

	if (!frameIt->get()->measurements.fRADI.empty())
	{
		(*stream) << endl << "RADI observations" << endl;
		otherMeasWriter.writeRADIReliabilityHeader();
		otherMeasWriter.writeRADIReliabilityData(frameIt->get()->measurements.fRADI, fProjectData->getStatistics());
	}

	bool  EDMheaderWritten = false;
	for (auto& itEDM : frameIt->get()->measurements.fEDM)
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

void TFRAMEWriter::writeFRAMEHeader(const std::string& name, const std::vector<int>& ID){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	std::string			TABs = stream->getCurrSpace();
	int					obsResWidth = getObsResWidth();
	////////////////////////////////////////////////////////////
	//first line

	std::string nameID;
	for (std::vector<int>::const_iterator it = ID.begin(); it != ID.end(); ++it)
	{
		if (it == ID.begin())
			nameID += std::to_string(*it);
		else
			nameID += "_"+ std::to_string(*it);
	}


	(*stream)<<endl<<endl;
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"FRAME\t" + name + "  ID(" + nameID + ")");
	(*stream)<<endl;
	///////////////////////////////////////////////////////////////////////////////////
	//second line
	if ( ID.size() != 1 ) //Only for ROOT it is equal to 1 and in this case we do not want to write out the header
	{
		(*stream)<<TABs;
		(*stream).writeStringLeft(nameWidth, "PARAMETER"); 
		(*stream).writeString(obsWidth,	"UNIT"); 
		(*stream).writeString(obsWidth,	"INIT VAL");  
		(*stream).writeString(obsWidth,	"CALC VAL"); 
		(*stream).writeString(obsWidth,	"UNIT S"); 
		(*stream).writeString(obsResWidth,	"SIGMA"); 
		(*stream).writeString(obsResWidth,	"CSIGMA");
		(*stream).writeString(obsWidth,	"FIXED"); 
		(*stream)<<endl;
	}
}

void TFRAMEWriter::writeFRAMEDefinition(const TTreeEntry& node){
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	string				separator = getSeparator();
	int					lengthPrecision =	getLengthPrecision();
	int					anglePrecision = getAnglePrecision();
	std::string			TABs = stream->getCurrSpace();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	//set length parameters
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);

	//Writing translations
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "TX"); 
	writeTranslationParameter(node.frame, 0);

	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "TY"); 
	writeTranslationParameter(node.frame, 1);

	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "TZ"); 
	writeTranslationParameter(node.frame, 2);
	


	stream->setPrecisionFormat(anglePrecision);
	
	//Writing rotations
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"RX"); 
	writeRotationParameter(node.frame, 0);

	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "RY"); 
	writeRotationParameter(node.frame, 1);

	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "RZ"); 
	writeRotationParameter(node.frame, 2);

	//Writing scale factor
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "SCALE"); 
	(*stream).writeString(obsWidth,	""); 
	(*stream).writeDouble(obsWidth, lengthPrecision,node.frame.getProvScale());

	if(!node.frame.isScaleFixed()){
		(*stream).writeDouble(obsWidth, lengthPrecision,node.frame.getEstScale());
	}
	else{
		(*stream).writeString(obsWidth,	""); 
	}

	(*stream).writeString(obsWidth,	"");
	if(node.frame.hasScaleStandDev())
		(*stream).writeDouble(obsResWidth, lengthResidualPrecision,node.frame.getScaleStandDev()); //*M2MM????
	else
		(*stream).writeString(obsResWidth,	""); 
	


	if(!node.frame.isScaleFixed()){
		(*stream).writeDouble(obsWidth, lengthPrecision,node.frame.getEstimatedPrecisionScale());
		(*stream).writeString(obsWidth,	"FALSE");
	}
	else{
		(*stream).writeString(obsResWidth,	""); 
		(*stream).writeString(obsWidth,	"TRUE"); 
	}

	if(!node.frame.isFixed()){
		(*stream)<<endl<<endl;

		if (!node.frame.isRotationFixed(0) && !node.frame.isRotationFixed(1))
		{
			(*stream)<<TABs;
			(*stream).writeStringLeft(nameWidth, "XY ROT. COVAR (GON²): "); 
			(*stream)<<node.frame.getXYCovarRot()*RAD2GON<<(separator);
		}

		if (!node.frame.isRotationFixed(1) && !node.frame.isRotationFixed(2))
		{
			(*stream)<<TABs;
			(*stream).writeStringLeft(nameWidth, "YZ ROT. COVAR (GON²): "); 
			(*stream)<<node.frame.getYZCovarRot()*RAD2GON<<(separator);
		}

		if (!node.frame.isRotationFixed(0) && !node.frame.isRotationFixed(2))
		{
			(*stream)<<TABs;
			(*stream).writeStringLeft(nameWidth, "XZ ROT. COVAR (GON²): "); 
			(*stream)<<node.frame.getXZCovarRot()*RAD2GON<<(separator);
		}
		
		(*stream)<<endl;

		if (!node.frame.isTranslationFixed(0) && !node.frame.isTranslationFixed(1))
		{		
			(*stream)<<TABs;
			(*stream).writeStringLeft(nameWidth, "XY TRANS. COVAR (m²): "); 
			(*stream)<<node.frame.getXYCovarTransl()<<(separator);
		}

		if (!node.frame.isTranslationFixed(1) && !node.frame.isTranslationFixed(2))
		{
			(*stream)<<TABs;
			(*stream).writeStringLeft(nameWidth, "YZ TRANS. COVAR (m²): "); 
			(*stream)<<node.frame.getYZCovarTransl()<<(separator);
		}
	
		if (!node.frame.isTranslationFixed(0) && !node.frame.isTranslationFixed(2))
		{
			(*stream)<<TABs;
			(*stream).writeStringLeft(nameWidth,  "XZ TRANS. COVAR (m²): "); 
			(*stream)<<node.frame.getXZCovarTransl()<<(separator);
		}
	}
	(*stream)<<endl;
}

void TFRAMEWriter::writePoints(TDataTreeIterator frameIt){
	bool localNode = false;

	if(frameIt->get()->isROOTNode())
		localNode = false;
	else
		localNode = true;

	writePointType(pointCALA, frameIt, TSpatialStatus::kCala, localNode);
	
	writePointType(pointVXYZ, frameIt, TSpatialStatus::kVxyz, localNode);
	if (fProjectData->getConfig().errorEllipses.isActive() && (!pointVXYZ.empty()))
	{
		//write ellipsoidal error
		writeEllipsoidHeader();

		for (auto& it : pointVXYZ)
			writeEllipsoidData(it);
	}
	
	writePointType(pointVXY, frameIt, TSpatialStatus::kVxy, localNode);
	if (fProjectData->getConfig().errorEllipses.isActive() && (!pointVXY.empty()))
	{
		//write ellips error
		writeEllipsHeader();

		for (auto& it : pointVXY)
			writeEllipsData(it);
	}

	writePointType(pointVXZ, frameIt, TSpatialStatus::kVxz, localNode);
	if (fProjectData->getConfig().errorEllipses.isActive() && (!pointVXZ.empty()))
	{
		//write ellips error
		writeEllipsHeader();

		for (auto& it : pointVXZ)
			writeEllipsData(it);
	}
	
	writePointType(pointVYZ, frameIt, TSpatialStatus::kVyz, localNode);
	if (fProjectData->getConfig().errorEllipses.isActive() && (!pointVYZ.empty()))
	{
		//write ellips error
		writeEllipsHeader();

		for (auto& it : pointVYZ)
			writeEllipsData(it);
	}
	
	writePointType(pointVZ, frameIt, TSpatialStatus::kVz, localNode);


	*getStream()<<endl;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void TFRAMEWriter::writePointType(const std::list<AdjPointIter>& lop, TDataTreeIterator frameIt, TSpatialStatus::ESpatialStatus type, bool localNode){
	if(!lop.empty()){
		//Tells if a header was already written (for local node).
		//In collections of points, we have points defined among whole project.
		//We want to write header only if we find out, that at least one point is defined in the particular sub-frame.
		bool headerWritten = false;	

		string fReferentialName;
		if(fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
			fReferentialName = "SPHE";
		else if(fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
			fReferentialName = "RS2K";
		else if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
			fReferentialName = "LEP";
		else
			fReferentialName = "OLOC";

		if(!localNode)
         writeResultsPtsHeader(type, (int)lop.size(), fReferentialName, localNode);

		for(auto it(lop.begin()); it != lop.end(); ++it){
			if(localNode){
				AdjPointIter pIt = *it;
				if(pIt->getFrameTreePosition() == frameIt){ // If the point was defined in this FRAME
					if(!headerWritten){
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

void TFRAMEWriter::writeTranslationParameter(const TAdjustableHelmertTransformation& frameDef, int transl){
	assert3D(transl);
	TAStreamFormatter*	stream = getStream();
	string				separator = getSeparator();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision =	getLengthPrecision();
	int					lengthResPrecision = getLengthResidualPrecision();

	//Write initial and calculated value
	(*stream).writeString(obsWidth,	"(M)"); 
	(*stream).writeDouble(obsWidth, lengthPrecision, frameDef.getProvTranslation(transl));

	if(!frameDef.isTranslationFixed(transl)){
		(*stream).writeDouble(obsWidth, lengthPrecision, frameDef.getEstTranslation(transl));
	}
	else{
		(*stream).writeString(obsWidth,	""); 
	}

	(*stream).writeString(obsWidth,	"(MM)");
	//Write the initial standard deviation, if specified in the input file
	if(frameDef.hasTranslStandDev(transl))
		(*stream).writeDouble(obsResWidth, lengthResPrecision, frameDef.getTranslationStandDev(transl).getMMetresValue());
	else
		(*stream).writeString(obsResWidth,	""); 

	//Write the standard deviation after calculation if translation is variable and the status (fixed or variable)
	if(!frameDef.isTranslationFixed(transl)){
		(*stream).writeDouble(obsResWidth, lengthResPrecision, frameDef.getEstimatedPrecisionTransl(transl).getMMetresValue());
		(*stream).writeString(obsWidth,	"FALSE");
	}
	else{
		(*stream).writeString(obsResWidth,	""); 
		(*stream).writeString(obsWidth,	"TRUE"); 
	}
	(*stream)<<endl;
}

void TFRAMEWriter::writeRotationParameter(const TAdjustableHelmertTransformation& frameDef, int rot){
	assert3D(rot);
	TAStreamFormatter*	stream = getStream();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResidualPrecision = getAngleResidualPrecision();

	//Write initial and calculated value
	(*stream).writeString(obsWidth,	"(GON)");
	
	(*stream).writeDouble(obsWidth, anglePrecision, frameDef.getProvRotation(rot).getGonsValue());

	if(!frameDef.isRotationFixed(rot)){
		(*stream).writeDouble(obsWidth, anglePrecision, frameDef.getEstRotation(rot).getGonsValue());
	}
	else{
		(*stream).writeString(obsWidth,	""); 
	}

	(*stream).writeString(obsWidth,	"(CC)");

	//Write the initial standard deviation, if specified in the input file
	if(frameDef.hasRotationStandDev(rot))
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, frameDef.getRotationStandDev(rot).getSignedCCValue());
	
	else
		(*stream).writeString(obsResWidth,	""); 
	
	//Write the standard deviation after calculation if rotation is variable and the status (fixed or variable)
	if(!frameDef.isRotationFixed(rot)){
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, frameDef.getEstimatedPrecisionRot(rot).getSignedCCValue());
		(*stream).writeString(obsWidth,	"FALSE");
	}
	else{
		(*stream).writeString(obsResWidth,	""); 
		(*stream).writeString(obsWidth,	"TRUE"); 
	}
	(*stream)<<endl;
}

void TFRAMEWriter::writeTSTNReliability(TDataTreeIterator frameIt)
{
	TAStreamFormatter*	stream = getStream();
	TTSTNWriter tstnWriter(*stream, fProjectData->getConfig().histo.isActive());

	//ANGL
	bool isANGL = false;
	for(auto& itTSTN:frameIt->get()->measurements.fTSTN)
	{	
		for(auto const ItRoms : itTSTN->roms)
		{
			if(ItRoms->measANGL.size() > 0){
				
				if (isANGL== false)
				{
					(*stream) << endl << "ANGL observations" << endl;
					tstnWriter.writeANGLReliabilityHeader();
					isANGL = true;
				}
				tstnWriter.writeANGLReliabilityData(itTSTN, fProjectData->getStatistics(), ItRoms->measANGL);				
			}		
		}
	}

	//ZEND
	bool isZEND = false;
	for(auto& itTSTN:frameIt->get()->measurements.fTSTN)
	{	
		for(auto const ItRoms : itTSTN->roms)
		{	
			if(ItRoms->measZEND.size() > 0){
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

	//DIST
	bool isDIST = false;
	for(auto& itTSTN:frameIt->get()->measurements.fTSTN)
	{
		for(auto const ItRoms : itTSTN->roms)
		{
			if(ItRoms->measDIST.size() > 0){
				
				if (isDIST==false)
				{
					(*stream) << endl << "DIST observations" << endl;
					tstnWriter.writeDISTReliabilityHeader();
					isDIST = true;
				}
				tstnWriter.writeDISTReliabilityData(itTSTN, fProjectData->getStatistics(), ItRoms->measDIST);
			}
		}
	}

	//PLR
	bool isPLR = false;
	for(auto& itTSTN:frameIt->get()->measurements.fTSTN)
	{	
		for(auto const ItRoms : itTSTN->roms)
		{
			if(ItRoms->measPLR3D.size() > 0){
				
				if (isPLR==false)
				{
					(*stream) << endl << "PLR3D observations" << endl;
					tstnWriter.writePLRReliabilityHeader();
					isPLR = true;
				}
				tstnWriter.writePLRReliabilityData(itTSTN, fProjectData->getStatistics(), ItRoms->measPLR3D);
			}
		}
	}

	//ECTH
	bool isECTH = false;
	for(auto& itTSTN:frameIt->get()->measurements.fTSTN)
	{
		for(auto const ItRoms : itTSTN->roms)
		{
			if(ItRoms->measECTH.size() > 0){
				
				if (isECTH==false)
				{
					(*stream) << endl << "ECTH observations" << endl;
					tstnWriter.writeECTHReliabilityHeader();
					isECTH = true;
				}
				tstnWriter.writeECTHReliabilityData(itTSTN, fProjectData->getStatistics(), ItRoms->measECTH);
			}
		}
	}

	//DHOR
	bool isDHOR = false;
	for(auto& itTSTN:frameIt->get()->measurements.fTSTN)
	{
		for(auto const ItRoms : itTSTN->roms)
		{
			if(ItRoms->measDHOR.size() > 0){
				
				if (isDHOR==false)
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
	TAStreamFormatter*	stream = getStream();
	TCAMWriter camWriter(*stream, fProjectData->getConfig().histo.isActive());


	//UVEC
	bool isuvec = false;
	for(auto& itCAM:frameIt->get()->measurements.fCAM)
	{
		if(itCAM.measUVEC.size() > 0){
				
			if (isuvec==false)
			{
				(*stream) << endl << "UVEC observations" << endl;
				camWriter.writeUVECReliabilityHeader();
				isuvec = true;
			}
			camWriter.writeUVECReliabilityData(itCAM, fProjectData->getStatistics());
		}	
	}

	//UVD
	bool isuvd = false;
	for(auto& itCAM:frameIt->get()->measurements.fCAM)
	{
		if(itCAM.measUVD.size() > 0){
				
			if (isuvd==false)
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
	TAStreamFormatter*	stream = getStream();
	TLEVELWriter levelWriter(*stream, fProjectData->getConfig().histo.isActive());
	

	//DLEV
	bool isdlev = false;
	for(auto& itLEV:frameIt->get()->measurements.fLEVEL)
	{
		if(itLEV.measDLEV.size() > 0){	
			if (isdlev==false)
			{
				(*stream) << endl << "DLEV observations" << endl;
				levelWriter.writeReliabilityHeader();
				isdlev = true;
			}
			levelWriter.writeDLEVReliabilityData(itLEV, fProjectData->getStatistics());
		}	
	}

//Part of the for loop above probably

	//DHOR
	bool isdhor = false;
	for(auto& itLEV:frameIt->get()->measurements.fLEVEL)
	{
		if(itLEV.hasDHOR){
				
			if (isdhor==false)
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
	TAStreamFormatter*	stream = getStream();
	TSCALEWriter scaleWriter(*stream, fProjectData->getConfig().histo.isActive());
	

	//ECHO
	bool isecho = false;
	for(auto itECH(frameIt->get()->measurements.fECHO.begin()); itECH != frameIt->get()->measurements.fECHO.end(); ++itECH)
	{
		if(itECH->measECHO.size() > 0){
				
			if (isecho==false)
			{
				(*stream) << endl << "ECHO observations" << endl;
				scaleWriter.writeECHOReliabilityHeader();
				isecho = true;
			}
			scaleWriter.writeECHOReliabilityData(*itECH, fProjectData->getStatistics(),itECH->measECHO);
		}	
	}

	//ECVE
	bool isecve = false;
	for (auto& itECVE:frameIt->get()->measurements.fECVE)
	{
		if (itECVE.measECVE.size() > 0){

			if (isecve == false)
			{
				(*stream) << endl << "ECVE observations" << endl;
				scaleWriter.writeECVEReliabilityHeader();
				isecve = true;
			}
			scaleWriter.writeECVEReliabilityData(itECVE, fProjectData->getStatistics(), itECVE.measECVE);
		}
	}

	//ECSP
	bool isecsp = false;
	for (auto& itECSP : frameIt->get()->measurements.fECSP)
	{
		if (itECSP.measECSP.size() > 0){

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


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//HEADER
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TFRAMEWriter::writeResultsPtsHeader(const TSpatialStatus::ESpatialStatus status, const int ptNumber, const string &refSys, bool localFRAME)
{
	TAStreamFormatter*	stream = getStream();
	TPointConverter converter (stream, fProjectData->getConfig().referential); 

	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();
	string				separator = getSeparator();
	std::string        TABs = stream->getCurrSpaceExtended(1);

	string title ="";
	if(status == TSpatialStatus::kCala)
		{ title = "POINTS DE CALAGE";}
	if(status == TSpatialStatus::kVz)
		{ title = "POINTS VARIABLES EN Z UNIQUEMENT";}
	if(status == TSpatialStatus::kVxy)
		{ title = "POINTS INVARIABLE EN Z";}
	if(status == TSpatialStatus::kVxz)
		{ title = "POINTS INVARIABLE EN Y";}
	if(status == TSpatialStatus::kVyz)
		{ title = "POINTS INVARIABLE EN X";}
	if(status == TSpatialStatus::kVxyz)
		{ title = "POINTS VARIABLE EN XYZ";}

	(*stream)<<endl;

	(*stream)<<title<<separator;
	if (!localFRAME)
		(*stream)<<"(NB. = " << ptNumber << ",  REFERENTIEL = " << refSys << " )";

	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).writeString( 3, "SFP");//tell if point defined in subframe
	(*stream).writeString( nameWidth, "NOM");//Nom
	(*stream).writeString( coordWidth,"X ");//X
	(*stream).writeString( coordWidth,"Y ");//Y
	(*stream).writeString( coordWidth,"Z ");//Z

	bool systemWithH = (fProjectData->getConfig().referential==TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS ||
		fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine||
		fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine);

	if(!localFRAME && systemWithH)
	{
		(*stream).writeString( coordWidth,"H ");//H
	}


	(*stream).writeString( coordResWidth,	"SX ");//sigma X
	(*stream).writeString( coordResWidth,	"SY ");//sigma Y
	(*stream).writeString( coordResWidth,	"SZ ");//sigma Z


	(*stream).writeString(coordResWidth, "DX ");//X offset 
	(*stream).writeString(coordResWidth, "DY ");//Y offset 
	(*stream).writeString(coordResWidth, "DZ ");//Z offset
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).writeString( 3, "");//Star
	(*stream).writeString( nameWidth ,	"");//Nom
	(*stream).writeString( coordWidth,		"(M)");//X units
	(*stream).writeString( coordWidth,		"(M)");//Y units
	(*stream).writeString( coordWidth,		"(M)");//Z units

	if (!localFRAME && systemWithH)
	{
		(*stream).writeString( coordWidth,		"(M)");//H units
	}

	(*stream).writeString( coordResWidth,	"(MM)");//sigma X units
	(*stream).writeString( coordResWidth,	"(MM)");//sigma Y units
	(*stream).writeString( coordResWidth,	"(MM)");//sigma Z units

	(*stream).writeString(coordResWidth, "(MM)");//X offset units
	(*stream).writeString(coordResWidth, "(MM)");//Y offset units
	(*stream).writeString(coordResWidth, "(MM)");// Z offset units
	(*stream)<<endl<<endl;
}

void TFRAMEWriter::writeEllipsHeader()
{
	TAStreamFormatter*	stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	*stream << endl<<"ABSOLUTE ERROR ELLIPSES" << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	stream->writeStringLeft(nameWidth, "NOM");
	stream->writeString(coordWidth, "SEMI-MAJ.(A)");
	stream->writeString(coordWidth, "SEMI-MIN.(B)");
	stream->writeString(coordWidth, "ORIE. OF (A)");

	*stream << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	stream->writeString(nameWidth, "");//Nom
	stream->writeString(coordWidth, "(MM)");// MINOR
	stream->writeString(coordWidth, "(MM)");// MAJOR
	stream->writeString(coordWidth, "(GON)");// ORIENTATION

	*stream << endl;

	return;
}

void TFRAMEWriter::writeEllipsoidHeader()
{
	TAStreamFormatter*	stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	*stream << endl<< "ABSOLUTE ERROR ELLIPSOIDS" << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	stream->writeStringLeft(nameWidth, "NOM");
	// Direction vectors: (+0.000, -0.000, +0.000) => 24
	const int vecwidth(24);
	stream->writeString(vecwidth, "DIRECTION X");
	stream->writeString(vecwidth, "DIRECTION Y");
	stream->writeString(vecwidth, "DIRECTION Z");
	stream->writeString(coordWidth, "LONGUEUR X");
	stream->writeString(coordWidth, "LONGUEUR Y");
	stream->writeString(coordWidth, "LONGUEUR Z");

	*stream << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	stream->writeString(nameWidth, "");//Nom
	stream->writeString(vecwidth, "");//Direction
	stream->writeString(vecwidth, "");//Direction
	stream->writeString(vecwidth, "");//Direction
	stream->writeString(coordWidth, "(MM)");// Axis length
	stream->writeString(coordWidth, "(MM)");// Axis length
	stream->writeString(coordWidth, "(MM)");// Axis length

	*stream  << endl;

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TFRAMEWriter::writeResultsPtsData(AdjPointIter pt, bool localFRAME)
{//FILE.OUTPUT
	TAStreamFormatter*	stream = getStream();

	TPointConverter converter (stream, pt->getReferenceFrame());
	std::string        TABs = stream->getCurrSpaceExtended(1);

	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordPrecision = this->getCoordPrecision();
	int					coordResWidth = getCoordResWidth();
	int					coordResidualPrecision = this->getCoordErrorPrecision();
	string				separator = getSeparator();

	int coordResPrecision = coordResidualPrecision > 3 ? (coordResidualPrecision - 3) : 0;

	if(!pt->getFrameTreePosition()->get()->isROOTNode() && !localFRAME)
		(*stream).writeStringLeft( 3, "*");
	else
		(*stream).writeStringLeft( 3, "");
	//name
	(*stream).writeString( nameWidth, pt->getName());//Nom
	//coordinates
	stream->setWidthFormat(coordWidth);
	stream->setPrecisionFormat(coordPrecision);
	TPositionVector estimatedValue = pt->getEstimatedValue();
	TPositionVector provisionalValue = pt->getProvisionalValue();

	if(localFRAME){ //Means that it is not ROOT!!!!!!
		//Write point coordinates XYZ or H because it is a local frame
		stream->setLengthUnits(TLength::EUnits::kMetres);
		converter.write3Coordinates(coordWidth, coordPrecision, separator, estimatedValue);

		//Write point's estimated precision after calculation
		converter.writeCoordinateParam( pt->getSpatialStatus(),
										coordResWidth,
										coordPrecision,
										TLength::EUnits::kMillimetres,
										separator,
										pt->getXEstPrecision(),
										pt->getYEstPrecision(),
										pt->getZEstPrecision(),
										"");/*sigma*/

		//Write DX, DY, DZ difference between provisional and estimated value
		converter.writeCoordinateParam( pt->getSpatialStatus(),
										coordResWidth,
										coordPrecision,
										TLength::EUnits::kMillimetres,
										separator,
										TLength(estimatedValue.getX() - provisionalValue.getX()),
										TLength(estimatedValue.getY() - provisionalValue.getY()),
										TLength(estimatedValue.getZ() - provisionalValue.getZ()),
										"");/*offset*/
	}
	else{//It is ROOT
		TDataTreeIterator root = fProjectData->getTree().begin();
		TRefSystemFactory::ERefFrame globalRef = fProjectData->getConfig().referential;

		TFreeVector sigmaRoot;
		//If point is defined in a sub-frame
		if(root != pt->getFrameTreePosition()){
			TLOR2LOR transfo = TLOR2LOR(pt->getFrameTreePosition(), fProjectData->getTree().begin(), "transfo");
			//transform sigma in root
			sigmaRoot = pt->transformSigmaInRoot(*pt, fProjectData);
			//transform coordinates in root
			transfo.transform(provisionalValue);
			transfo.transform(estimatedValue);
		}
		else{
			/*If ROOT and not OLOC -> provisional is XYH, needs to be transformed to XYZ*/
			if(globalRef!=TRefSystemFactory::ERefFrame::kLocalRefFrame)
				transfXYH2XYZ(provisionalValue, globalRef);
		}

		stream->setLengthUnits(TLength::EUnits::kMetres);
		converter.write3Coordinates(coordWidth, coordPrecision, separator, estimatedValue);

		if(globalRef!=TRefSystemFactory::ERefFrame::kLocalRefFrame){
			TPositionVector forHCalc(estimatedValue);
			transfXYZ2XYH(forHCalc, globalRef);

			writeDouble(coordWidth,coordPrecision, forHCalc.getH().getMetresValue());
			(*stream)<<separator;
		}

		if(pt->getFrameTreePosition() == root){
		//Write point's estimated precision after calculation
		converter.writeCoordinateParam( pt->getSpatialStatus(),
										coordResWidth,
										coordPrecision,
										TLength::EUnits::kMillimetres,
										separator,
										pt->getXEstPrecision(),
										pt->getYEstPrecision(),
										pt->getZEstPrecision(),
										"");/*sigma*/
		}
		else{
			//status = vxyz to write sigma because with CALA, no sigma are writen
			converter.writeCoordinateParam(TSpatialStatus::kVxyz,
				coordResWidth,
				coordPrecision,
				TLength::EUnits::kMillimetres,
				separator,
				sigmaRoot.getX(),
				sigmaRoot.getY(),
				sigmaRoot.getZ(),
				"");/*sigma convert in root*/
		}


		converter.writeCoordinateParam( pt->getSpatialStatus(),
										coordResWidth,
										coordPrecision,
										TLength::EUnits::kMillimetres,
										separator,
										TLength(estimatedValue.getX() - provisionalValue.getX()),
										TLength(estimatedValue.getY() - provisionalValue.getY()),
										TLength(estimatedValue.getZ() - provisionalValue.getZ()),
											"");/*offset*/
	}

	(*stream)<<endl; //end line
}

void TFRAMEWriter::writeEllipsData(AdjPointIter& pt)
{
	TAStreamFormatter*	stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();

	stream->writeStringLeft(nameWidth, pt->getName());
	stream->writeDouble(coordWidth, coordResWidth, pt->getErrorEllMajorAxis().getMMetresValue());
	stream->writeDouble(coordWidth, coordResWidth, pt->getErrorEllMinorAxis().getMMetresValue());
	stream->writeDouble(coordWidth, coordResWidth, pt->getErrorEllGis().getGonsValue());
	*stream << endl;
}

void TFRAMEWriter::writeEllipsoidData(AdjPointIter& pt)
{
	TAStreamFormatter*	stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();
	string				sep = getSeparator();

	const auto& ell(pt->getErrorEllipsoid());

	const int vecwidth(24);
	char vecstr[32]; // format the vector output here and write as a string

	stream->writeStringLeft(nameWidth, pt->getName());
	sprintf(vecstr, "(% .3f  % .3f  % .3f)", ell.vx[0], ell.vx[1], ell.vx[2]);
	stream->writeString(vecwidth, vecstr);
	sprintf(vecstr, "(% .3f  % .3f  % .3f)", ell.vy[0], ell.vy[1], ell.vy[2]);
	stream->writeString(vecwidth, vecstr);
	sprintf(vecstr, "(% .3f  % .3f  % .3f)", ell.vz[0], ell.vz[1], ell.vz[2]);
	stream->writeString(vecwidth, vecstr);
	stream->writeDouble(coordWidth, coordResWidth, ell.lx*M2MM);
	stream->writeDouble(coordWidth, coordResWidth, ell.ly*M2MM);
	stream->writeDouble(coordWidth, coordResWidth, ell.lz*M2MM);
	*stream << endl;
}


void TFRAMEWriter::transfXYH2XYZ(TPositionVector& pv, const TRefSystemFactory::ERefFrame& rf){
		if(rf == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
			TXYH2CCS::XYHs2CCS(pv);
		else if(rf == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
			TXYH2CCS::XYHg2000Machine2CCS(pv);
		else if (rf == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
			TXYH2CCS::XYHg1985Machine2CCS(pv);
}

void TFRAMEWriter::transfXYZ2XYH(TPositionVector& pv, const TRefSystemFactory::ERefFrame& rf){
		if(rf == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
			TXYH2CCS::CCS2XYHs(pv);
		else if(rf == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
			TXYH2CCS::CCS2XYHg2000Machine(pv);
		else if (rf == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
			TXYH2CCS::CCS2XYHg1985Machine(pv);
}

