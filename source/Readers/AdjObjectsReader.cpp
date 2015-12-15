#include "AdjObjectsReader.h"
//#include "TAffineTransformFactory.h"

///////////////////////////////////////////////////////
// TKeyFRAME
///////////////////////////////////////////////////////
int TKeyENDFRAME::fNumberOfFramesClosed = 0;

TKeyFRAME::TKeyFRAME(TLGCData& project, int nb_allowed_keywords, const char** keywords) : TAKeyWord(FRAME, project)
{			
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyFRAME::parse(const std::vector<std::string>& tokens, int line) {
	using namespace LGC;
	auto numTokens = tokens.size();
			
	// The asterisk and the keyword itself are already two tokens
	if (numTokens < 10)
		throw std::runtime_error("Key *FRAME takes at least 8 arguments: Name, tx, ty, tz, rx, ry, rz and scale.");

	// set translations, rotations and scale in the bitset to be fixed by default
	std::bitset<3> translations(std::string("111"));
	std::bitset<3> rotations(std::string("111"));
	std::bitset<1> scale(std::string("1"));

	TOptionHelper opts(tokens.cbegin()+2, tokens.cend());

	//If flag is used, change state of appropriate bit set and make the element of transl. rotation or scale variable (set 0)
	if (opts.has("TX") || opts.has("STX")) translations.set(0,0);
	if (opts.has("TY") || opts.has("STY")) translations.set(1,0);
	if (opts.has("TZ") || opts.has("STZ")) translations.set(2,0);
	if (opts.has("RX") || opts.has("SRX")) rotations.set(0,0);
	if (opts.has("RY") || opts.has("SRY")) rotations.set(1,0);
	if (opts.has("RZ") || opts.has("SRZ")) rotations.set(2,0);
	if (opts.has("SCL")|| opts.has("SSCL")) scale.set(0,0);

	const auto gon(TAngle::kGons);
	TransformParameters transfParam;
	transfParam.tX = TLength(std::stor(tokens[3])); // Translation along X
	// Create adjustable helmert transformation
	TAdjustableHelmertTransformation adjTrafo = TAdjustableHelmertTransformation( // Wrapper containing adjustable related information
			translations, // Bits telling which of the translations are fixed
			rotations,     // Bits telling which of the rotations around an axis are fixed
			scale,  // Bit telling whether scale is fixed
			tokens[2] //Transformation name
		);

	adjTrafo.setParam(    // The transformation itself
				TLength(std::stor(tokens[3])), // Translation along X
				TLength(std::stor(tokens[4])), // Translation along Y
				TLength(std::stor(tokens[5])), // Translation along Z
				TAngle(std::stor(tokens[6]), gon), // Rotation around X
				TAngle(std::stor(tokens[7]), gon), // Rotation around Y
				TAngle(std::stor(tokens[8]), gon), // Rotation around Z
				std::stor(tokens[9]) // SCALE factor
		);

	// remember the line of the frame definition		
	adjTrafo.getLine() = line;
			
	//If at least one of these options is used, create FRAME measurement
	if(opts.has("STX") || opts.has("STY") || opts.has("STZ") || opts.has("SRX") || opts.has("SRY") || opts.has("SRZ") || opts.has("SSCL") ){
		if(opts.has("STX")) adjTrafo.setTranslationStandDev(X, TLength(opts.getParamR("STX") , TLength::EUnits::kMillimetres)); //Value given in mili-metres, but stored in metres
		if(opts.has("STY")) adjTrafo.setTranslationStandDev(Y, TLength(opts.getParamR("STY") , TLength::EUnits::kMillimetres)); //Value given in mili-metres, but stored in metres
		if(opts.has("STZ")) adjTrafo.setTranslationStandDev(Z, TLength(opts.getParamR("STZ") , TLength::EUnits::kMillimetres)); //Value given in mili-metres, but stored in metres
		if(opts.has("SRX")) adjTrafo.setRotationStandDev(X, TAngle(opts.getParamR("SRX"), TAngle::kCCs)); //Value given in cc, stored in angle object
		if(opts.has("SRY")) adjTrafo.setRotationStandDev(Y, TAngle(opts.getParamR("SRY"), TAngle::kCCs));  //Value given in cc, stored in angle object
		if(opts.has("SRZ")) adjTrafo.setRotationStandDev(Z, TAngle(opts.getParamR("SRZ"), TAngle::kCCs));  //Value given in cc, stored in angle object
		if(opts.has("SSCL")) adjTrafo.setScaleStandDev(opts.getParamR("SSCL") * MM2M); //Value given in mili-metres, stored in metres
	}

	// Create a new level in the tree using the current transformation definition.
	proj.addChild(&adjTrafo);

	if(opts.has("RX") && opts.has("SRX"))
		throw std::runtime_error("Either \"RX\" flag or \"SRX\" option used, both are not allowed");
	if(opts.has("RY") && opts.has("SRY"))
		throw std::runtime_error("Either \"RY\" flag or \"SRY\" option used, both are not allowed");
	if(opts.has("RZ") && opts.has("SRZ"))
		throw std::runtime_error("Either \"RZ\" flag or \"SRZ\" option used, both are not allowed");
	if(opts.has("TX") && opts.has("STX"))
		throw std::runtime_error("Either \"TX\" flag or \"TRX\" option used, both are not allowed");
	if(opts.has("TY") && opts.has("STY"))
		throw std::runtime_error("Either \"TY\" flag or \"TRY\" option used, both are not allowed");
	if(opts.has("TZ") && opts.has("STZ"))
		throw std::runtime_error("Either \"TZ\" flag or \"TRZ\" option used, both are not allowed");
	if(opts.has("SCL") && opts.has("SSCL"))
		throw std::runtime_error("Either \"SCL\" flag or \"SSCL\" option used, both are not allowed");

	fNumberOfFramesOpened++;
}

///////////////////////////////////////////////////////
// TKeyENDFRAME
///////////////////////////////////////////////////////
int TKeyFRAME::fNumberOfFramesOpened = 0;
TKeyENDFRAME::TKeyENDFRAME(TLGCData& project, int nb_allowed_keywords, const char** keywords): TAKeyWord(ENDFRAME, project)
{			
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyENDFRAME::parse(const std::vector<std::string>& tokens, int) {
	if (tokens.size() != 2 &&  tokens[2].at(0) != '%')  // More then 2 and at the same time the third is not comment
		throw std::runtime_error("Key *ENDFRAME expects no arguments.");

	proj.moveUp();
	fNumberOfFramesClosed++;
}

///////////////////////////////////////////////////////
// TAPointKey
///////////////////////////////////////////////////////

TAPointKey::TAPointKey(TLGCData& project, const std::string& word):
	TAKeyWord(word, project),
	defaultErrmsg("Point entries need 4 parameters: ID and 3 coordinates (X,Y,H|Z)."),
	fconfig(proj.getConfig()),
	fpointAccess(proj.getPoints()) 
	{ }

size_t TAPointKey::findComment(const std::string& s) {
	for (size_t i = 0; i < strlen(INPUT_SEPERATOR_CHARS); i++) {
		size_t pos = s.find(INPUT_SEPERATOR_CHARS[i]);
		if (pos != std::string::npos) 
			return pos;
	}
	return std::string::npos;
}

void TAPointKey::parse(const std::vector<std::string>& tokens, int line) {
	using namespace LGC;

	auto numTokens = tokens.size();

	if (numTokens == 0 || tokens.at(0) == "*")
		return;

	size_t commentpos = findComment(tokens.at(0));
			
	if (commentpos != std::string::npos) {
		// this line is a comment, stpointore it and be done
		hdrcomment += std::string(tokens.at(0) + "\n");
		return;
	}

	if (numTokens < 4)
		throw std::runtime_error(defaultErrmsg);

	if (fpointAccess.doesObjectExist(tokens.at(0)))
		throw std::runtime_error("Point " + tokens.at(0) + " already exists.");
			
	// the conversion method stor throws on error
	auto& pt = insertPoint(tokens.at(0), std::stor(tokens.at(1)), std::stor(tokens.at(2)), std::stor(tokens.at(3)));
	pt.line = line;

	TOptionHelper opts(tokens.cbegin(), tokens.cend());

	//If point defined using POIN
	if(key == "POIN"){
		//If all 3 standard deviations are listed => store standard deviations
		if(opts.has("SX") && opts.has("SY") && opts.has("SZ"))
			pt.setStandardDeviations(opts.getParamR("SX") * MM2M, opts.getParamR("SY") * MM2M, opts.getParamR("SZ") * MM2M); //Standard deviations given in mili-meters, butstored in meters
		else if(!pt.getFrameTreePosition().node->data->isROOTNode())
			//POIN is not in a ROOT node and standard deviations are not provided => then it is an error
			proj.getFileLogger() << TFileLogger::e_logType::LOG_ERROR << "Line " + std::to_string(line) +  + " : point is defined using POIN in a sub-frame. Standard deviations are needed!";
		else if(opts.has("SX") || opts.has("SY") || opts.has("SZ"))
			//If only some of these options are set => standard deviations are not stored and warning is produced => warning 
			proj.getFileLogger() << TFileLogger::e_logType::LOG_WARNING << "Line " + std::to_string(line)  + " : point is defined using POIN, but not all standard errors specified (SX,SY,SZ)!";
	}
		
	//If last token starts with a comment chararcter, store it 
	const char fOfLastToken = tokens.back().at(0);
	if (fOfLastToken == '$' || fOfLastToken == '%')
		pt.eolcomment = tokens.back();

	// remove the newline of the last comment line
	auto lastchar(hdrcomment.length()-1);
	if (hdrcomment.length() > 0 && hdrcomment[lastchar] == '\n')
		hdrcomment[lastchar] = 0;
	// store the comment lines above the point
	pt.hdrcomment = hdrcomment;

	// reset the headercomment storage
	hdrcomment = "";
}

///////////////////////////////////////////////////////
// Specific points definitions
///////////////////////////////////////////////////////
TKeyCALA::TKeyCALA(TLGCData& project, int nb_allowed_keywords, const char** keywords) : TAPointKey(project, CALA) 
{			
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

TAdjustablePoint& TKeyCALA::insertPoint(const std::string& pointName, TReal x, TReal y, TReal z){
	proj.addToPointNum(TSpatialStatus::kCala);
	// TRUE, if it is a ROOT node
	if(proj.getCurrentNode().isROOTNode()){
		if(fconfig.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
			return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), true, true, true, pointName, fconfig.referential, proj.getCurrentPosition()));
		else
			return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k2DPlusH), true, true, true, pointName, fconfig.referential, proj.getCurrentPosition()));
	}
	else
		// If it is defined in a sub-frame the provisional values are given in XYZ coordinates relative to the subframe in which was the point defined
		return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), true, true, true, pointName, fconfig.referential, proj.getCurrentPosition()));
}


TKeyPOIN::TKeyPOIN(TLGCData& project, int nb_allowed_keywords, const char** keywords) : TAPointKey(project, POIN)
{			
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

TAdjustablePoint& TKeyPOIN::insertPoint(const std::string& pointName, TReal x, TReal y, TReal z) {
	proj.addToPointNum(TSpatialStatus::kVxyz);
	if(proj.getCurrentNode().isROOTNode()){
		if(fconfig.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
			return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), false, false, false, pointName, fconfig.referential, proj.getCurrentPosition()));
		else
			return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k2DPlusH), false, false, false, pointName, fconfig.referential, proj.getCurrentPosition()));
	}
	else
		return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), false, false, false, pointName, fconfig.referential, proj.getCurrentPosition()));
}

TKeyVXY::TKeyVXY(TLGCData& project, int nb_allowed_keywords, const char** keywords) : TAPointKey(project, VXY)
{			
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

TAdjustablePoint& TKeyVXY::insertPoint(const std::string& pointName, TReal x, TReal y, TReal z) {
	proj.addToPointNum(TSpatialStatus::kVxy);
	if(proj.getCurrentNode().isROOTNode()){
		if(fconfig.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
			return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), false, false, true, pointName, fconfig.referential, proj.getCurrentPosition()));
		else
			return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k2DPlusH), false, false, true, pointName, fconfig.referential, proj.getCurrentPosition()));
	}
	else
		return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), false, false, true, pointName, fconfig.referential, proj.getCurrentPosition()));
}

TKeyVXZ::TKeyVXZ(TLGCData& project, int nb_allowed_keywords, const char** keywords) : TAPointKey(project, VXZ) 
{			
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

TAdjustablePoint& TKeyVXZ::insertPoint(const std::string& pointName, TReal x, TReal y, TReal z) {
	proj.addToPointNum(TSpatialStatus::kVxz);
	if(proj.getCurrentNode().isROOTNode()){
		if(fconfig.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
			return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), false, true, false, pointName, fconfig.referential, proj.getCurrentPosition()));
		else
			return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k2DPlusH), false, true, false, pointName, fconfig.referential, proj.getCurrentPosition()));
	}
	else
		return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), false, true, false, pointName, fconfig.referential, proj.getCurrentPosition()));
}

TKeyVYZ::TKeyVYZ(TLGCData& project, int nb_allowed_keywords, const char** keywords) : TAPointKey(project, VYZ) 
{			
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

TAdjustablePoint& TKeyVYZ::insertPoint(const std::string& pointName, TReal x, TReal y, TReal z) {
	proj.addToPointNum(TSpatialStatus::kVyz);
	if(proj.getCurrentNode().isROOTNode()){
		if(fconfig.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
			return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), true, false, false, pointName, fconfig.referential, proj.getCurrentPosition()));
		else
			return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k2DPlusH), true, false, false, pointName, fconfig.referential, proj.getCurrentPosition()));
	}
	else
		return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), true, false, false, pointName, fconfig.referential, proj.getCurrentPosition()));
}


TKeyVZ::TKeyVZ(TLGCData& project, int nb_allowed_keywords, const char** keywords) : TAPointKey(project, VZ) 
{			
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

TAdjustablePoint& TKeyVZ::insertPoint(const std::string& pointName, TReal x, TReal y, TReal z) {
	proj.addToPointNum(TSpatialStatus::kVz);
	if(proj.getCurrentNode().isROOTNode()){
		if(fconfig.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame)
			return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), true, true, false, pointName, fconfig.referential, proj.getCurrentPosition()));
		else
			return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k2DPlusH), true, true, false, pointName, fconfig.referential, proj.getCurrentPosition()));
	}
	else
		return fpointAccess.addObject(TAdjustablePoint(TPositionVector(x, y, z, TCoordSysFactory::ECoordSys::k3DCartesian), true, true, false, pointName, fconfig.referential, proj.getCurrentPosition()));
}