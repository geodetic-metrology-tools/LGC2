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
	TTSTNWriter tstnWriter(*stream);
	TCAMWriter camWriter(*stream);
	TEDMWriter edmWriter(*stream);
	TSCALEWriter scaleWriter(*stream);
	TLEVELWriter levelWriter(*stream);
	TOtherMeasurentWriter dverWriter(*stream);

	for(auto itTSTN(frameIt->get()->measurements.fTSTN.begin()); itTSTN != frameIt->get()->measurements.fTSTN.end(); ++itTSTN)
			tstnWriter.writeTSTNResults(*itTSTN);

	for(auto itCAM(frameIt->get()->measurements.fCAM.begin()); itCAM != frameIt->get()->measurements.fCAM.end(); ++itCAM)
			camWriter.writeCAMResults(*itCAM);

	for(auto itLEVEL(frameIt->get()->measurements.fLEVEL.begin()); itLEVEL != frameIt->get()->measurements.fLEVEL.end(); ++itLEVEL)
			levelWriter.writeLEVELResults(*itLEVEL);

	//No instrument for DVER, so no loop to have each instrument.
	if (! frameIt->get()->measurements.fDVER.empty())
		dverWriter.writeDVERResults(frameIt->get()->measurements.fDVER);

	for(auto itEDM(frameIt->get()->measurements.fEDM.begin()); itEDM != frameIt->get()->measurements.fEDM.end(); ++itEDM)
			edmWriter.writeEDMResults(*itEDM);

	for(auto itECHO(frameIt->get()->measurements.fECHO.begin()); itECHO != frameIt->get()->measurements.fECHO.end(); ++itECHO)
			scaleWriter.writeECHOResults(*itECHO);
}


void TFRAMEWriter::writeFRAMESimu(TDataTreeIterator frameIt){
	TAStreamFormatter*	stream = getStream();
   stream->setTreeDepth((int)frameIt->get()->ID.size() - 1); //Size of the ID is equal to the depth in the tree, which corresponds to the number o TABs to be used in formatting. Zero TABs for ROOT (depth 1).

	/*Do we want to write out FRAMES only if there is any measurement inside or summarize it each time??? (some parameters can be variable)*/
	/*Now: If there is at least one measurement or if it is a ROOT node, then write it!*/
	if(frameIt->get()->measurements.fTSTN.size() != 0 || frameIt->get()->measurements.fCAM.size() != 0 || frameIt->get()->isROOTNode()){
		writeFRAMEHeader(frameIt->get()->frame.getName(), frameIt->get()->ID);

		if(frameIt->get()->frame.getName() != "ROOT")
			writeFRAMEDefinition(*frameIt->get());

		//Write points for SIMU only in ROOT, not in local nodes
		if(frameIt->get()->isROOTNode())
			writePoints(frameIt);

		TTSTNWriter tstnWriter(*stream);
		TCAMWriter camWriter(*stream);
		TEDMWriter edmWriter(*stream);
		TSCALEWriter scaleWriter(*stream);
		TLEVELWriter levelWriter(*stream);


		for(auto itTSTN(frameIt->get()->measurements.fTSTN.begin()); itTSTN != frameIt->get()->measurements.fTSTN.end(); ++itTSTN)
			tstnWriter.writeTSTNResultsSIMU(*itTSTN);

		for(auto itCAM(frameIt->get()->measurements.fCAM.begin()); itCAM != frameIt->get()->measurements.fCAM.end(); ++itCAM)
			camWriter.writeCAMResultsSIMU(*itCAM);

		for(auto itLEVEL(frameIt->get()->measurements.fLEVEL.begin()); itLEVEL != frameIt->get()->measurements.fLEVEL.end(); ++itLEVEL)
			levelWriter.writeLEVELSIMUResults(*itLEVEL);

		for(auto itEDM(frameIt->get()->measurements.fEDM.begin()); itEDM != frameIt->get()->measurements.fEDM.end(); ++itEDM)
			edmWriter.writeEDMSIMUResults(*itEDM);
		
		for(auto itECHO(frameIt->get()->measurements.fECHO.begin()); itECHO != frameIt->get()->measurements.fECHO.end(); ++itECHO)
			scaleWriter.writeECHOSIMUResults(*itECHO);
	}
}

void TFRAMEWriter::writeFRAMEAllReliability(TDataTreeIterator frameIt){
	TAStreamFormatter*	stream = getStream();
	TEDMWriter edmWriter(*stream);
	TOtherMeasurentWriter dverWriter(*stream);

   stream->setTreeDepth((int)frameIt->get()->ID.size() - 1); //Size of the ID is equal to the depth in the tree, which corresponds to the number o TABs to be used in formatting. Zero TABs for ROOT (depth 1).

	writeFRAMEHeader(frameIt->get()->frame.getName(), frameIt->get()->ID);

	if(frameIt->get()->frame.getName() != "ROOT")
		writeFRAMEDefinition(*frameIt->get());
	
	writeTSTNReliability(frameIt);
	writeCAMReliability(frameIt);
	writeLEVELReliability(frameIt);
	writeSCALEReliability(frameIt);

	if (! frameIt->get()->measurements.fDVER.empty())
	{
		(*stream)<<"DVER observations"<<endl;
		dverWriter.writeDVERReliabilityHeader();
		dverWriter.writeDVERReliabilityData(frameIt->get()->measurements.fDVER, fProjectData->getStatistics());
	}

	
	bool  EDMheaderWritten = false;
	for(auto itEDM(frameIt->get()->measurements.fEDM.begin()); itEDM != frameIt->get()->measurements.fEDM.end(); ++itEDM)
	{
		if (!EDMheaderWritten)
		{
			(*stream)<<"DSPT observations"<<endl;
			edmWriter.writeReliabilityHeader();
			EDMheaderWritten = true;
		}
		edmWriter.writeReliabilityData(*itEDM, fProjectData->getStatistics());
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
	std::stringstream result;
	std::copy(ID.begin(), ID.end(), std::ostream_iterator<int>(result));

	(*stream)<<endl<<endl;
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,"FRAME\t" + name + "  ID(" + result.str() + ")");
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
			(*stream).writeStringLeft(nameWidth, "XY ROT. COVAR: "); 
			(*stream)<<node.frame.getXYCovarRot().getGonsValue()<<(separator);
		}

		if (!node.frame.isRotationFixed(1) && !node.frame.isRotationFixed(2))
		{
			(*stream)<<TABs;
			(*stream).writeStringLeft(nameWidth, "YZ ROT. COVAR: "); 
			(*stream)<<node.frame.getYZCovarRot().getGonsValue()<<(separator);
		}

		if (!node.frame.isRotationFixed(0) && !node.frame.isRotationFixed(2))
		{
			(*stream)<<TABs;
			(*stream).writeStringLeft(nameWidth, "XZ ROT. COVAR: "); 
			(*stream)<<node.frame.getXZCovarRot().getGonsValue()<<(separator);
		}
		
		(*stream)<<endl;

		if (!node.frame.isTranslationFixed(0) && !node.frame.isTranslationFixed(1))
		{		
			(*stream)<<TABs;
			(*stream).writeStringLeft(nameWidth, "XY TRANS. COVAR: "); 
			(*stream)<<node.frame.getXYCovarTransl()<<(separator);
		}

		if (!node.frame.isTranslationFixed(1) && !node.frame.isTranslationFixed(2))
		{
			(*stream)<<TABs;
			(*stream).writeStringLeft(nameWidth, "YZ TRANS. COVAR: "); 
			(*stream)<<node.frame.getYZCovarTransl()<<(separator);
		}
	
		if (!node.frame.isTranslationFixed(0) && !node.frame.isTranslationFixed(2))
		{
			(*stream)<<TABs;
			(*stream).writeStringLeft(nameWidth,  "XZ TRANS. COVAR: "); 
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
	writePointType(pointVXY, frameIt, TSpatialStatus::kVxy, localNode);
	writePointType(pointVXZ, frameIt, TSpatialStatus::kVxz, localNode);
	writePointType(pointVYZ, frameIt, TSpatialStatus::kVyz, localNode);
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
	int					lengthResidualPrecision = getLengthResidualPrecision();
	//Write initial and calculated value
	(*stream).writeString(obsWidth,	"(M)"); 
	(*stream).writeDouble(obsWidth, lengthPrecision, frameDef.getProvTranslation(transl));

	if(!frameDef.isTranslationFixed(transl)){
		(*stream).writeDouble(obsWidth, lengthPrecision, frameDef.getEstTranslation(transl));
	}
	else{
		(*stream).writeString(obsWidth,	""); 
	}

	stream->setLengthUnits(TLength::kMillimetres);
	stream->setPrecisionFormat(lengthResidualPrecision);
	stream->setWidthFormat(obsResWidth);

	(*stream).writeString(obsWidth,	"(MM)");
	//Write the initial standard deviation, if specified in the input file
	if(frameDef.hasTranslStandDev(transl))
		(*stream)<<frameDef.getTranslationStandDev(transl)<<(separator); 
	else
		(*stream).writeString(obsResWidth,	""); 

	//Write the standard deviation after calculation if translation is variable and the status (fixed or variable)
	if(!frameDef.isTranslationFixed(transl)){
		(*stream)<<frameDef.getEstimatedPrecisionTransl(transl)<<(separator); 
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
	string				separator = getSeparator();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResidualPrecision = getAngleResidualPrecision();

	stream->setAngleUnits(TAngle::kGons);
	stream->setPrecisionFormat(anglePrecision);
	stream->setWidthFormat(obsWidth);
	//Write initial and calculated value
	(*stream).writeString(obsWidth,	"(GON)");
	
	(*stream)<<frameDef.getProvRotation(rot).getGonsValue()<<(separator); 

	if(!frameDef.isRotationFixed(rot)){
		(*stream)<<frameDef.getEstRotation(rot).getGonsValue()<<(separator); 
	}
	else{
		(*stream).writeString(obsWidth,	""); 
	}

	(*stream).writeString(obsWidth,	"(CC)");
	stream->setAngleUnits(TAngle::k100MicroGons);
	stream->setPrecisionFormat(angleResidualPrecision);
	stream->setWidthFormat(obsResWidth);

	//Write the initial standard deviation, if specified in the input file
	if(frameDef.hasRotationStandDev(rot))
		(*stream)<<frameDef.getRotationStandDev(rot).getGonsValue()<<(separator);  
	
	else
		(*stream).writeString(obsResWidth,	""); 
	
	//Write the standard deviation after calculation if rotation is variable and the status (fixed or variable)
	if(!frameDef.isRotationFixed(rot)){
		(*stream)<<frameDef.getEstimatedPrecisionRot(rot).getGonsValue()<<(separator);
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
	TTSTNWriter tstnWriter(*stream);

	//ANGL
	bool isANGL = false;
	for(auto itTSTN(frameIt->get()->measurements.fTSTN.begin()); itTSTN != frameIt->get()->measurements.fTSTN.end(); ++itTSTN)
	{	
		for(auto const ItRoms : itTSTN->roms)
		{
			if(ItRoms.measANGL.size() > 0){
				
				if (isANGL== false)
				{
					(*stream)<<"ANGL observations"<<endl;
					tstnWriter.writeANGLReliabilityHeader();
					isANGL = true;
				}
				tstnWriter.writeANGLReliabilityData(*itTSTN, fProjectData->getStatistics(), ItRoms.measANGL);				
			}		
		}
	}

	//ZEND
	bool isZEND = false;
	for(auto itTSTN(frameIt->get()->measurements.fTSTN.begin()); itTSTN != frameIt->get()->measurements.fTSTN.end(); ++itTSTN)
	{	
		for(auto const ItRoms : itTSTN->roms)
		{	
			if(ItRoms.measZEND.size() > 0){
				if (isZEND == false)
				{
					(*stream)<<"ZEND observations"<<endl;
					tstnWriter.writeZENDReliabilityHeader();
					isZEND = true;
				}
				tstnWriter.writeZENDReliabilityData(*itTSTN, fProjectData->getStatistics(), ItRoms.measZEND);			
			}
		}
	}

	//DIST
	bool isDIST = false;
	for(auto itTSTN(frameIt->get()->measurements.fTSTN.begin()); itTSTN != frameIt->get()->measurements.fTSTN.end(); ++itTSTN)
	{
		for(auto const ItRoms : itTSTN->roms)
		{
			if(ItRoms.measDIST.size() > 0){
				
				if (isDIST==false)
				{
					(*stream)<<"DIST observations"<<endl;
					tstnWriter.writeDISTReliabilityHeader();
					isDIST = true;
				}
				tstnWriter.writeDISTReliabilityData(*itTSTN, fProjectData->getStatistics(), ItRoms.measDIST);
			}
		}
	}

	//PLR
	bool isPLR = false;
	for(auto itTSTN(frameIt->get()->measurements.fTSTN.begin()); itTSTN != frameIt->get()->measurements.fTSTN.end(); ++itTSTN)
	{	
		for(auto const ItRoms : itTSTN->roms)
		{
			if(ItRoms.measPLR3D.size() > 0){
				
				if (isPLR==false)
				{
					(*stream)<<"PLR observations"<<endl;
					tstnWriter.writePLRReliabilityHeader();
					isPLR = true;
				}
				tstnWriter.writePLRReliabilityData(*itTSTN, fProjectData->getStatistics(), ItRoms.measPLR3D);
			}
		}
	}

	//ECTH
	bool isECTH = false;
	for(auto itTSTN(frameIt->get()->measurements.fTSTN.begin()); itTSTN != frameIt->get()->measurements.fTSTN.end(); ++itTSTN)
	{
		for(auto const ItRoms : itTSTN->roms)
		{
			if(ItRoms.measECTH.size() > 0){
				
				if (isECTH==false)
				{
					(*stream)<<"ECTH observations"<<endl;
					tstnWriter.writeECTHReliabilityHeader();
					isECTH = true;
				}
				tstnWriter.writeECTHReliabilityData(*itTSTN, fProjectData->getStatistics(), ItRoms.measECTH);
			}
		}
	}

	//DHOR
	bool isDHOR = false;
	for(auto itTSTN(frameIt->get()->measurements.fTSTN.begin()); itTSTN != frameIt->get()->measurements.fTSTN.end(); ++itTSTN)
	{
		for(auto const ItRoms : itTSTN->roms)
		{
			if(ItRoms.measDHOR.size() > 0){
				
				if (isDHOR==false)
				{
					(*stream)<<"DHOR observations"<<endl;
					tstnWriter.writeDHORReliabilityHeader();
					isDHOR = true;
				}
				tstnWriter.writeDHORReliabilityData(*itTSTN, fProjectData->getStatistics(), ItRoms.measDHOR);
			}
		}
	}
}

void TFRAMEWriter::writeCAMReliability(TDataTreeIterator frameIt)
{
	TAStreamFormatter*	stream = getStream();
	TCAMWriter camWriter(*stream);


	//UVEC
	bool isuvec = false;
	for(auto itCAM(frameIt->get()->measurements.fCAM.begin()); itCAM != frameIt->get()->measurements.fCAM.end(); ++itCAM)
	{
		if(itCAM->measUVEC.size() > 0){
				
			if (isuvec==false)
			{
				(*stream)<<"UVEC observations"<<endl;
				camWriter.writeUVECReliabilityHeader();
				isuvec = true;
			}
			camWriter.writeUVECReliabilityData(*itCAM, fProjectData->getStatistics());
		}	
	}

	//UVD
	bool isuvd = false;
	for(auto itCAM(frameIt->get()->measurements.fCAM.begin()); itCAM != frameIt->get()->measurements.fCAM.end(); ++itCAM)
	{
		if(itCAM->measUVD.size() > 0){
				
			if (isuvd==false)
			{
				(*stream)<<"UVD observations"<<endl;
				camWriter.writeUVDReliabilityHeader();
				isuvd = true;
			}
			camWriter.writeUVDReliabilityData(*itCAM, fProjectData->getStatistics());
		}	
	}
		
}

void TFRAMEWriter::writeLEVELReliability(TDataTreeIterator frameIt)
{
	TAStreamFormatter*	stream = getStream();
	TLEVELWriter levelWriter(*stream);
	

	//DLEV
	bool isdlev = false;
	for(auto itLEV(frameIt->get()->measurements.fLEVEL.begin()); itLEV != frameIt->get()->measurements.fLEVEL.end(); ++itLEV)
	{
		if(itLEV->measDLEV.size() > 0){	
			if (isdlev==false)
			{
				(*stream)<<"DLEV observations"<<endl;
				levelWriter.writeReliabilityHeader();
				isdlev = true;
			}
			levelWriter.writeDLEVReliabilityData(*itLEV, fProjectData->getStatistics());
		}	
	}

//Part of the for loop above probably

	//DHOR
	bool isdhor = false;
	for(auto itLEV(frameIt->get()->measurements.fLEVEL.begin()); itLEV != frameIt->get()->measurements.fLEVEL.end(); ++itLEV)
	{
		if(itLEV->hasDHOR){
				
			if (isdhor==false)
			{
				(*stream)<<"DHOR observations from a DLEV observations"<<endl;
				levelWriter.writeReliabilityHeader();
				isdhor = true;
			}
			levelWriter.writeDHORReliabilityData(*itLEV, fProjectData->getStatistics());
		}	
	}
}

void TFRAMEWriter::writeSCALEReliability(TDataTreeIterator frameIt)
{
	TAStreamFormatter*	stream = getStream();
	TSCALEWriter scaleWriter(*stream);
	

	//ECHO
	bool isecho = false;
	for(auto itECH(frameIt->get()->measurements.fECHO.begin()); itECH != frameIt->get()->measurements.fECHO.end(); ++itECH)
	{
		if(itECH->measECHO.size() > 0){
				
			if (isecho==false)
			{
				(*stream)<<"DLEV observations"<<endl;
				scaleWriter.writeECHOReliabilityHeader();
				isecho = true;
			}
			scaleWriter.writeECHOReliabilityData(*itECH, fProjectData->getStatistics(),itECH->measECHO);
		}	
	}

	//ECSP
	//ECVE

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//HEADER
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TFRAMEWriter::writeResultsPtsHeader(const TSpatialStatus::ESpatialStatus status, const int ptNumber, const string &refSys, bool localFRAME)
{
	TAStreamFormatter*	stream = getStream();
	//TPointConverter converter (stream, getRefFrameForH());
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
		{ title = "POINTS VARIABLE EN Z UNIQUEMENT";}
	if(status == TSpatialStatus::kVxy)
		{ title = "POINTS INVARIABLE EN Z";}
	if(status == TSpatialStatus::kVxz)
		{ title = "POINTS INVARIABLE EN Y";}
	if(status == TSpatialStatus::kVyz)
		{ title = "POINTS INVARIABLE EN X";}
	if(status == TSpatialStatus::kVxyz)
		{ title = "POINTS VARIABLE EN XYZ";}

	(*stream)<<endl<<endl;

	(*stream)<<TABs + title<<separator;
	if (!localFRAME)
		(*stream)<<"(NB. = " << ptNumber << ",  REFERENTIEL = " << refSys << " )";

	(*stream)<<endl<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream)<<TABs;
	(*stream).writeString( 3, "SFP");//Star
	(*stream).writeString( nameWidth, "NOM");//Nom
	(*stream).writeString( coordWidth,"X ");//X
	(*stream).writeString( coordWidth,"Y ");//Y
	(*stream).writeString( coordWidth,"Z ");//Z

	if(!localFRAME && !converter.isInLocalSystem())
	{
		(*stream).writeString( coordWidth,"H ");//H
	}


	(*stream).writeString( coordResWidth,	"SX ");//sigma X
	(*stream).writeString( coordResWidth,	"SY ");//sigma Y
	(*stream).writeString( coordResWidth,	"SZ ");//sigma Z


	(*stream).writeString( coordResWidth,	"DX ");//X offset 
	(*stream).writeString( coordResWidth,	"DY ");//Y offset 
	(*stream).writeString( coordResWidth,	"DZ ");//Z offset
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream)<<TABs;
	(*stream).writeString( 3, "");//Star
	(*stream).writeString( nameWidth ,	"");//Nom
	(*stream).writeString( coordWidth,		"(M)");//X units
	(*stream).writeString( coordWidth,		"(M)");//Y units
	(*stream).writeString( coordWidth,		"(M)");//Z units

	if(!localFRAME && !converter.isInLocalSystem())
	{
		(*stream).writeString( coordWidth,		"(M)");//H units
	}

	(*stream).writeString( coordResWidth,	"(MM)");//sigma X units
	(*stream).writeString( coordResWidth,	"(MM)");//sigma Y units
	(*stream).writeString( coordResWidth,	"(MM)");//sigma Z units

	(*stream).writeString( coordResWidth,	"(MM)");//X offset units
	(*stream).writeString( coordResWidth,	"(MM)");//Y offset units
	(*stream).writeString( coordResWidth,	"(MM)");// Z offset units
	(*stream)<<endl<<endl;
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

	(*stream)<<TABs;
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
		//Write point coordinates XYZ because it is a local frame
		converter.write3Coordinates(coordWidth, coordPrecision, separator, estimatedValue);

		//Write point's estimated precision after calculation
		converter.writeCoordinateParam( pt->getSpatialStatus(),
										coordResWidth,
										coordResPrecision,
										separator,
                              pt->getXEstPrecision().getMMetresValue(),
                              pt->getYEstPrecision().getMMetresValue(),
                              pt->getZEstPrecision().getMMetresValue(),
										"");/*sigma*/

		//Write DX, DY, DZ difference between provisional and estimated value
		converter.writeCoordinateParam( pt->getSpatialStatus(),
										coordResWidth,
										coordResPrecision,
										separator,
										(estimatedValue.getX() - provisionalValue.getX()).getMMetresValue(),
                              (estimatedValue.getY() - provisionalValue.getY()).getMMetresValue(),
                              (estimatedValue.getZ() - provisionalValue.getZ()).getMMetresValue(),
										"");/*offset*/
	}
	else{//It is ROOT
		TDataTreeIterator root = fProjectData->getTree().begin();
		TRefSystemFactory::ERefFrame globalRef = fProjectData->getConfig().referential;


		//If point is defined in a sub-frame
		if(root != pt->getFrameTreePosition()){
			TLOR2LOR transfo = TLOR2LOR(pt->getFrameTreePosition(), fProjectData->getTree().begin(), "transfo");
			transfo.transform(provisionalValue);
			transfo.transform(estimatedValue);
		}
		else{
			/*If ROOT and not OLOC -> provisional is XYH, needs to be transformed to XYZ*/
			if(globalRef!=TRefSystemFactory::ERefFrame::kLocalRefFrame)
				transfXYH2XYZ(provisionalValue, globalRef);
		}

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
										coordResPrecision,
										separator,
										pt->getXEstPrecision()*M2MM,
										pt->getYEstPrecision()*M2MM,
										pt->getZEstPrecision()*M2MM,
										"");/*sigma*/
		}
		else{
				(*stream).writeString( coordResWidth, "");
				(*stream).writeString( coordResWidth, "");
				(*stream).writeString( coordResWidth, "");
		}


		converter.writeCoordinateParam( pt->getSpatialStatus(),
										coordResWidth,
										coordResPrecision,
										separator,
										(estimatedValue.getX() - provisionalValue.getX())*M2MM,
										(estimatedValue.getY() - provisionalValue.getY())*M2MM,
										(estimatedValue.getZ() - provisionalValue.getZ())*M2MM,
											"");/*offset*/
	}

	(*stream)<<endl; //end line
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