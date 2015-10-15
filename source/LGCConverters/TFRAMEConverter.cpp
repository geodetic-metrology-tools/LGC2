#include "TFRAMEConverter.h"
#include "TTstnConverter.h"
#include "TPointConverter.h"
#include "TCamConverter.h"
#include "ContributionsGenerators\TLOR2LOR.h"
#include "refframetransformations\TXYH2CCS.h"
#include "TDirectTransformation.h"
#include "TInverseTransformation.h"


/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
TFRAMEConverter::TFRAMEConverter(TAStreamFormatter& stream, const TLGCData* data): TALGCConverter(stream), fProjectData(data)
{
	//Separate 
	for (AdjPointIter it(fProjectData->getPoints().objects.begin()); it != fProjectData->getPoints().objects.end(); ++it){	
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

TFRAMEConverter::~TFRAMEConverter(){}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void TFRAMEConverter::writeFRAMEAll(TDataTreeIterator frameIt){
	TAStreamFormatter*	stream = getStream();
	stream->setTreeDepth(frameIt->get()->ID.size() - 1); //Size of the ID is equal to the depth in the tree, which corresponds to the number o TABs to be used in formatting. Zero TABs for ROOT (depth 1).

	writeFRAMEHeader(frameIt->get()->frame.getName(), frameIt->get()->ID);

	writeFRAMEDefinition(*frameIt->get());
	
	writePoints(frameIt);

	TTstnConverter tstnConvert(*stream);
	TCamConverter camConvert(*stream);

	for(auto itTSTN(frameIt->get()->measurements.fTSTN.begin()); itTSTN != frameIt->get()->measurements.fTSTN.end(); ++itTSTN)
			tstnConvert.writeTSTNResults(*itTSTN);

	for(auto itCAM(frameIt->get()->measurements.fCAM.begin()); itCAM != frameIt->get()->measurements.fCAM.end(); ++itCAM)
			camConvert.writeCAMResults(*itCAM);
}

void TFRAMEConverter::writeFRAMEHeader(const std::string& name, const std::vector<int>& ID){
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


void TFRAMEConverter::writeFRAMEDefinition(const TTreeEntry& node){
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
	(*stream).writeDouble(obsWidth, lengthPrecision,node.frame.getScale());

	(*stream).writeString(obsWidth,	"");
	stream->setLengthUnits(TLength::kMillimetres);
	stream->setPrecisionFormat(lengthResidualPrecision);
	stream->setWidthFormat(obsResWidth);
	if(node.frame.hasScaleStandDev())
		(*stream)<<node.frame.getScaleStandDev()<<(separator);
	else
		(*stream).writeString(obsResWidth,	""); 
	


	if(!node.frame.isScaleFixed()){
		(*stream)<<node.frame.getEstimatedPrecisionScale()<<(separator); 
		(*stream).writeString(obsWidth,	"FALSE");
	}
	else{
		(*stream).writeString(obsResWidth,	""); 
		(*stream).writeString(obsWidth,	"TRUE"); 
	}

	(*stream)<<endl<<endl;

	if (!node.frame.isRotationFixed(0) && !node.frame.isRotationFixed(1))
	{
		(*stream)<<TABs;
		(*stream).writeStringLeft(nameWidth, "XY ROT. COVAR: "); 
		(*stream)<<node.frame.getXYCovarRot().getSLAngle()<<(separator);
	}

	if (!node.frame.isRotationFixed(1) && !node.frame.isRotationFixed(2))
	{
		(*stream)<<TABs;
		(*stream).writeStringLeft(nameWidth, "YZ ROT. COVAR: "); 
		(*stream)<<node.frame.getYZCovarRot().getSLAngle()<<(separator);
	}

	if (!node.frame.isRotationFixed(0) && !node.frame.isRotationFixed(2))
	{
		(*stream)<<TABs;
		(*stream).writeStringLeft(nameWidth, "XZ ROT. COVAR: "); 
		(*stream)<<node.frame.getXZCovarRot().getSLAngle()<<(separator);
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
	(*stream)<<endl<<endl;
}

void TFRAMEConverter::writePoints(TDataTreeIterator frameIt){
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
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void TFRAMEConverter::writePointType(const std::list<AdjPointIter>& lop, TDataTreeIterator frameIt, TSpatialStatus::ESpatialStatus type, bool localNode){
	if(!lop.empty()){
		//Tells if a header was already written (for local node).
		//In collections of points, we have points defined among whole project.
		//We want to write header only if we find out, that at least one point is defined in the particular sub-frame.
		bool headerWritten = false;	

		if(!localNode)
			writeResultsPtsHeader(type, lop.size(), fProjectData->getConfig().referential.typeName(), localNode);

		for(auto it(lop.begin()); it != lop.end(); ++it){
			if(localNode){
				AdjPointIter pIt = *it;
				if(pIt->getFrameTreePosition() == frameIt){ // If the point was defined in this FRAME
					if(!headerWritten){
						writeResultsPtsHeader(type, lop.size(), fProjectData->getConfig().referential.typeName(), localNode);
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


void TFRAMEConverter::writeTranslationParameter(const TAdjustableHelmertTransformation& frameDef, int transl){
	assert3D(transl);
	TAStreamFormatter*	stream = getStream();
	string				separator = getSeparator();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision =	getLengthPrecision();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	//Write initial and calculated value
	(*stream).writeString(obsWidth,	"(M)"); 
	(*stream).writeDouble(obsWidth, lengthPrecision, frameDef.getTranslation(transl));

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


void TFRAMEConverter::writeRotationParameter(const TAdjustableHelmertTransformation& frameDef, int rot){
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
	
	(*stream)<<frameDef.getAngle(rot).getSLAngle()<<(separator); 

	(*stream).writeString(obsWidth,	"(CC)");
	stream->setAngleUnits(TAngle::k100MicroGons);
	stream->setPrecisionFormat(angleResidualPrecision);
	stream->setWidthFormat(obsResWidth);

	//Write the initial standard deviation, if specified in the input file
	if(frameDef.hasRotationStandDev(rot))
		(*stream)<<frameDef.getRotationStandDev(rot).getSLAngle()<<(separator);  
	
	else
		(*stream).writeString(obsResWidth,	""); 
	
	//Write the standard deviation after calculation if rotation is variable and the status (fixed or variable)
	if(!frameDef.isRotationFixed(rot)){
		(*stream)<<frameDef.getEstimatedPrecisionRot(rot).getSLAngle()<<(separator);
		(*stream).writeString(obsWidth,	"FALSE");
	}
	else{
		(*stream).writeString(obsResWidth,	""); 
		(*stream).writeString(obsWidth,	"TRUE"); 
	}
	(*stream)<<endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//HEADER
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TFRAMEConverter::writeResultsPtsHeader(const TSpatialStatus::ESpatialStatus status, const int ptNumber, const string &refSys, bool isLocal)
{
	TAStreamFormatter*	stream = getStream();
	//TPointConverter converter (stream, getRefFrameForH());
	TPointConverter converter (stream, fProjectData->getConfig().referential.type()); 

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

	if (!isLocal)
		(*stream)<<TABs +"SFP = Sub-Frame Point; * = TRUE"<<endl;

	(*stream)<<TABs + title<<separator;
	if (!isLocal)
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
	if(!converter.isInLocalSystem())
	{
		(*stream).writeString( coordWidth,		"H");//H
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
	if(!converter.isInLocalSystem())
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
void	TFRAMEConverter::writeResultsPtsData(AdjPointIter pt, bool isLocal)
{//FILE.OUTPUT
	TAStreamFormatter*	stream = getStream();

	TPointConverter converter (stream, pt->getReferenceFrame());
	std::string        TABs = stream->getCurrSpaceExtended(1);

	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordPrecision = this->getCoordPrecision();
	int					coordResWidth = getCoordResWidth();
	int					coordResPrecision = this->getCoordErrorPrecision();
	string				separator = getSeparator();

	(*stream)<<TABs;
	if(!pt->getFrameTreePosition()->get()->isROOTNode() && !isLocal)
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

	if(isLocal){
		//Write point coordinates XYZ because it is a local frame
		converter.write3Coordinates(coordWidth, coordPrecision, separator, estimatedValue);

		//Write point's estimated precision after calculation
		converter.writeCoordinateParam(pt->getSpatialStatus(),
								coordResWidth,
								coordResPrecision > 3 ? (coordResPrecision - 3) : 0,
								separator,
								pt->getXEstPrecision()*LGC::M2MM,
								pt->getYEstPrecision()*LGC::M2MM,
								pt->getZEstPrecision()*LGC::M2MM,
								"");/*sigma*/

		//Write DX, DY, DZ difference between provisional and estimated value
		converter.writeCoordinateParam(pt->getSpatialStatus(),
								coordResWidth,
								coordResPrecision > 3 ? (coordResPrecision - 3) : 0,
								separator,
								(estimatedValue.getX() - provisionalValue.getX())*LGC::M2MM,
								(estimatedValue.getY() - provisionalValue.getY()) *LGC::M2MM,
								(estimatedValue.getZ() - provisionalValue.getZ()) *LGC::M2MM,
								"");/*offset*/
	}
	else{
		TDataTreeIterator root = fProjectData->getTree().begin();
		TLGCRefFrame::ERefs globalRef = fProjectData->getConfig().referential.type();

		TScalar ZProvisional = provisionalValue.getZ();
		TScalar ZEstimated = estimatedValue.getZ();

		//If point is defined in a sub-frame
		if(root != pt->getFrameTreePosition()){
			TLOR2LOR transfo = TLOR2LOR(pt->getFrameTreePosition(), fProjectData->getTree().begin(), "transfo");
			transfo.transform(provisionalValue);
			transfo.transform(estimatedValue);

			ZProvisional = provisionalValue.getZ();
			ZEstimated = estimatedValue.getZ();

			if(globalRef != TLGCRefFrame::kOLOC)
				transfUsingGeoid(provisionalValue, globalRef); 
		}

		if(globalRef!=TLGCRefFrame::kOLOC)
			transfUsingGeoid(estimatedValue, globalRef);

		converter.write3Coordinates(coordWidth, coordPrecision, separator, estimatedValue);

		if(pt->getFrameTreePosition() == root){
		//Write point's estimated precision after calculation
		converter.writeCoordinateParam(pt->getSpatialStatus(),
								coordResWidth,
								coordResPrecision > 3 ? (coordResPrecision - 3) : 0,
								separator,
								pt->getXEstPrecision()*LGC::M2MM,
								pt->getYEstPrecision()*LGC::M2MM,
								pt->getZEstPrecision()*LGC::M2MM,
								"");/*sigma*/
		}
		else{
				(*stream).writeString( coordResWidth, "");
				(*stream).writeString( coordResWidth, "");
				(*stream).writeString( coordResWidth, "");
		}


		converter.writeCoordinateParam(pt->getSpatialStatus(),
								coordResWidth,
								coordResPrecision > 3 ? (coordResPrecision - 3) : 0,
								separator,
								(estimatedValue.getX() - provisionalValue.getX())*LGC::M2MM,
								(estimatedValue.getY() - provisionalValue.getY()) *LGC::M2MM,
									(ZEstimated - ZProvisional)*LGC::M2MM,
									"");/*offset*/
	}

	(*stream)<<endl<<endl; //end line
}


void TFRAMEConverter::transfUsingGeoid(TPositionVector& pv, const TLGCRefFrame::ERefs& rf){
		if(rf == TLGCRefFrame::ERefs::kSPHE)
			TXYH2CCS::CCS2XYHs(pv);
		else if(rf == TLGCRefFrame::ERefs::kRS2K)
			TXYH2CCS::CCS2XYHg2000Machine(pv);
		else if (rf == TLGCRefFrame::ERefs::kLEP)
			TXYH2CCS::CCS2XYHg1985Machine(pv);
}