#include "OptionReaders.h"

///////////////////////////////////////////////////////
// TKeyTITR
///////////////////////////////////////////////////////
TKeyTITR::TKeyTITR(TLGCData& project, int nb_allowed_keywords, const char** keywords): TAKeyWord(TITR, project)
{			
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyTITR::parse(const std::vector<std::string>& tokens, bool activeLine, int) {
    if(!activeLine)
        throw std::runtime_error("*TITR cannot be deactivated");

	if (tokens.at(0) == "*" && tokens.size() < 3)
		throw std::runtime_error("Key *TITR expects only one argument");
	
}

//////////////////
// Referentiels //
//////////////////

void TKeyOLOC::parse(const std::vector<std::string>&, bool activeLine, int) {
    if(!activeLine)
        throw std::runtime_error("Reference system keywords cannot be deactivated");

	// nothing to parse, using local cartesion coordinate system
	if(fconfig.referential == TRefSystemFactory::ERefFrame::kNotInGraph)
		fconfig.referential = TRefSystemFactory::ERefFrame::kLocalRefFrame;
	else
		throw std::runtime_error("Only one reference system option can be specified (either OLOC, RS2K, LEP or SPHE).");
}

void TKeyRS2K::parse(const std::vector<std::string>&, bool activeLine, int) {
    if(!activeLine)
        throw std::runtime_error("Reference system keywords cannot be deactivated");

	// nothing to parse, using grid geoid
		if(fconfig.referential == TRefSystemFactory::ERefFrame::kNotInGraph)
			fconfig.referential = TRefSystemFactory::ERefFrame::kCernXYHg00Machine;
	else
		throw std::runtime_error("Only one reference system option can be specified (either OLOC, RS2K, LEP or SPHE).");
}

void TKeyLEP::parse(const std::vector<std::string>&, bool activeLine, int) {
    if(!activeLine)
        throw std::runtime_error("Reference system keywords cannot be deactivated");

	// nothing to parse, using parabolic ellipsoid
	if(fconfig.referential == TRefSystemFactory::ERefFrame::kNotInGraph)
		fconfig.referential = TRefSystemFactory::ERefFrame::kCernXYHg85Machine;
	else
		throw std::runtime_error("Only one reference system option can be specified (either OLOC, RS2K, LEP or SPHE).");
}

void TKeySPHE::parse(const std::vector<std::string>&, bool activeLine, int) {
    if(!activeLine)
        throw std::runtime_error("Reference system keywords cannot be deactivated");

	// nothing to parse, using spherical reference frame
	if(fconfig.referential == TRefSystemFactory::ERefFrame::kNotInGraph)
		fconfig.referential = TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS;
	else
		throw std::runtime_error("Only one reference system option can be specified (either OLOC, RS2K, LEP or SPHE).");
}



//////////////////
// Calc Options //
//////////////////

void TKeyALLFIXED::parse(const std::vector<std::string>&, bool activeLine, int) {
	fconfig.allfixed = TLGCConfig::TBinaryOption(activeLine);

    LGCAdjustablePoint::setAllFixedParam(activeLine);
}


void TKeyLIBR::parse(const std::vector<std::string>&, bool activeLine, int) {
    fconfig.libre = TLGCConfig::TBinaryOption(activeLine);
}

void TKeyCOVAR::parse(const std::vector<std::string>&, bool activeLine, int) {
    fconfig.covar = TLGCConfig::TBinaryOption(activeLine);
}

void TKeyCHABA::parse(const std::vector<std::string>&, bool activeLine, int) {
	fconfig.chaba = TLGCConfig::TBinaryOption(activeLine);
}

void TKeyNODUP::parse(const std::vector<std::string>&, bool activeLine, int) {
    fconfig.nodup = TLGCConfig::TBinaryOption(activeLine);
}


void TKeyPDOR::parse(const std::vector<std::string>& tokens, bool activeLine, int) {
    if(activeLine && fconfig.allfixed.isActive())
        throw std::runtime_error("PDOR is not allowed with ALLFIXED option.");

	const size_t numtok(tokens.size());

	// this is a multi-line keyword, react just on the following calls
	if (numtok>1 && tokens.at(1) == "PDOR") {
		if (numtok > 2)
			throw std::runtime_error("PDOR not starting with the keyword line.");

        fconfig.pdor.setActive(activeLine);
		return;
	}

	// Points must occur in pairs
    if(numtok == 1){
        fconfig.pdor.fptname = tokens.at(0);
    }
	else if (numtok == 2)
	{
        fconfig.pdor.fptname = tokens.at(0);
        fconfig.pdor.fgis = TAngle(std::stor(tokens.at(1)), TAngle::EUnits::kGons);
		fconfig.pdor.hasBearing = true;
	}
	else
		throw std::runtime_error("Keyword *PDOR takes either one or two arguments "
		"(Point name and optinal bearing), not " + std::to_string(numtok) + ".");
}


void TKeySIMU::parse(const std::vector<std::string>& tokens, bool activeLine, int) {
	auto numTokens = tokens.size();

    if(numTokens >= 3){
        fconfig.sim = TLGCConfig::TSimulation(std::stoi(tokens.at(2)));
        fconfig.sim.setActive(activeLine);
    }
	else
		throw std::runtime_error("*SIMU takes  1 argument, the number of simulation." );
}



////////////////////
// Output Options //
////////////////////

void TKeyAPRI::parse(const std::vector<std::string>&, bool activeLine, int) {
    fconfig.useApriori = TLGCConfig::TBinaryOption(activeLine);
}


void TKeyEREL::parse(const std::vector<std::string>& tokens, bool activeLine, int) {
    auto numtokens(tokens.size());
	// this is a multi-line keyword, react just on the following calls
	if (tokens.at(1) == "EREL") { 
		if (numtokens > 2)
			throw std::runtime_error("Points for *EREL must occur as lines of point name pairs, "
			                         "not starting with the keyword line.");

        erelActive = activeLine;
		return;
	}

	// Points must occur in pairs
	if (numtokens != 2)
		throw std::runtime_error("Points for *EREL must occur as lines of point name pairs.");

	if(activeLine && erelActive) fconfig.erelPairs.push_back(std::make_pair(tokens.at(0), tokens.at(1)));
}


 TKeyFMTxHelper::TKeyFMTxHelper(TLGCConfig::TCustomOutputSep& co, const std::vector<std::string>& tokens, bool activeLine) {
	auto numTokens = tokens.size();

	if (numTokens == 4) {
		if (tokens.at(2) == "SEP")  {
			if (tokens.at(3).length() < 2)
				throw std::runtime_error("Separator string for " + tokens.at(1) + " invalid, did you forget the quotes?");

			co = TLGCConfig::TCustomOutputSep(tokens.at(3).substr(1, tokens.at(3).length()-2));
            co.setActive(activeLine);
		}
	}
	else if (numTokens == 3) {
		// Default behaviour: nothing to do
	}
	else {
		throw std::runtime_error("*" + tokens.at(1) + " expects SEP \"separator\"  ");
	}
}


 void TKeyFMTO::parse(const std::vector<std::string>& tokens, bool activeLine, int) {
	TKeyFMTxHelper(fconfig.CustomOutputSeparator, tokens, activeLine);
}


 void TKeyFMTP::parse(const std::vector<std::string>& tokens, bool activeLine, int) {
	TKeyFMTxHelper(fconfig.CustomOutputSeparatorPunch, tokens, activeLine);
}


 void TKeyHIST::parse(const std::vector<std::string>&, bool activeLine, int) {
    fconfig.histo = TLGCConfig::TBinaryOption(activeLine);

    // Set the usage of histogram in obsSummary
    TLGCObsSummary::createHistogram(activeLine);
}

 void TKeyPREC::parse(const std::vector<std::string>& tokens, bool activeLine, int) {
	if (tokens.size() != 3)
		throw std::runtime_error("*PREC expects exactly one integer argument.");
	if (std::stoi(tokens.at(2)) >= 0 && std::stoi(tokens.at(2)) <= 7)
		fconfig.outPrecision = TLGCConfig::TPrecision(std::stoi(tokens.at(2)));
	else
		throw std::runtime_error("*PREC expects interger between 0 and 7.");

    if(!activeLine) fconfig.outPrecision = TLGCConfig::TPrecision();
}

 void TKeyMICR::parse(const std::vector<std::string>& /*tokens*/, bool, int) {
	fconfig.outPrecision = TLGCConfig::TPrecision(6);
	auto& outputMessages(proj.getFileLogger());
	outputMessages.writeReportHeader("MICR should not be used. Use PREC option.");
}

 void TKeyCLIC::parse(const std::vector<std::string>& /*tokens*/, bool, int) {
	fconfig.outPrecision = TLGCConfig::TPrecision(6);
	auto& outputMessages(proj.getFileLogger());
	outputMessages.writeReportHeader("CLIC should not be used. Use PREC option.");
}

 void TKeyTOL::parse(const std::vector<std::string>& tokens, bool, int) {
	if (tokens.size() != 3)
		throw std::runtime_error("*TOL expects exactly one integer argument.");
	if (std::stoi(tokens.at(2)) >= 0 && std::stoi(tokens.at(2)) <= 6)
		fconfig.outPrecision = TLGCConfig::TPrecision(1 + std::stoi(tokens.at(2)));
	else
		throw std::runtime_error("*TOL expects interger between 0 and 6.");

	auto& outputMessages(proj.getFileLogger());
	outputMessages.writeReportHeader("TOL should not be used. Use PREC option.");
}

 void TKeyDIXI::parse(const std::vector<std::string>& /*tokens*/, bool, int) {
	fconfig.outPrecision = TLGCConfig::TPrecision(4);

	auto& outputMessages(proj.getFileLogger());
	outputMessages.writeReportHeader("DIXI should not be used. Use PREC option");
}

 void TKeyNOSPC::parse(const std::vector<std::string>& /*tokens*/, bool activeLine, int) {

	auto& outputMessages(proj.getFileLogger());
	outputMessages.writeReportHeader("NOSPC is used, it is an lgc1 option, so it is ignore in lgc2 output files");
}

 void TKeyPRES::parse(const std::vector<std::string>&, bool activeLine, int) {
     fconfig.errorEllipses = TLGCConfig::TBinaryOption(activeLine);
}


/////////////////////////////
// Additional Output Files //
/////////////////////////////

 void TKeyDEFA::parse(const std::vector<std::string>&, bool activeLine, int) {
     fconfig.writeDefa = TLGCConfig::TBinaryOption(activeLine);
}


 void TKeyFAUT::parse(const std::vector<std::string>& tokens, bool activeLine, int) {
	auto numTokens = tokens.size();

	if (numTokens == 2)
		fconfig.faut = TLGCConfig::TFautDetect(FAUT_DEF_ALPHA, FAUT_DEF_BETA);
	else if (numTokens == 4)
		fconfig.faut = TLGCConfig::TFautDetect(std::stor(tokens.at(2)), std::stor(tokens.at(3)));
	else
		throw std::runtime_error("*FAUT takes either 0 or 2 arguments but " + 
		                          std::to_string(numTokens) + " were supplied." );

    fconfig.faut.setActive(activeLine);
}


DiffOutHelper::	DiffOutHelper(TLGCConfig::TCoordOut& out, const std::string& opt, const std::string& key) {
	if (opt == "")
		out = TLGCConfig::TCoordOut(TLGCConfig::TCoordOut::kPLAIN);
	else if (opt == "E")
		out = TLGCConfig::TCoordOut(TLGCConfig::TCoordOut::kE);
	else if (opt == "EE")
		out = TLGCConfig::TCoordOut(TLGCConfig::TCoordOut::kEE);
	else if (opt == "H")
		out = TLGCConfig::TCoordOut(TLGCConfig::TCoordOut::kH);
	else if (opt == "Z")
		out = TLGCConfig::TCoordOut(TLGCConfig::TCoordOut::kZ);
	else if (opt == "HZ")
		out = TLGCConfig::TCoordOut(TLGCConfig::TCoordOut::kHZ);
	else if (opt == "HN")
		out = TLGCConfig::TCoordOut(TLGCConfig::TCoordOut::kHN);
	else if (opt == "ZHN")
		out = TLGCConfig::TCoordOut(TLGCConfig::TCoordOut::kZHN);
	else if (opt == "T")
		out = TLGCConfig::TCoordOut(TLGCConfig::TCoordOut::kT);
	else if (opt == "OUT1")
		out = TLGCConfig::TCoordOut(TLGCConfig::TCoordOut::kOUT1);
	else if (opt == "OUT3")
		out = TLGCConfig::TCoordOut(TLGCConfig::TCoordOut::kOUT3);
	else
		throw std::runtime_error("Unknown option \"" + opt + "\" for a keyword " + key + ".");
}


void TKeyPUNC::parse(const std::vector<std::string>& tokens, bool activeLine, int) {
	auto numtokens = tokens.size();

	if (numtokens == 2)
		DiffOutHelper(fconfig.writePunch, "", tokens.at(1));
	else if (numtokens == 3)
		DiffOutHelper(fconfig.writePunch, tokens.at(2), tokens.at(1));
	else
		throw std::runtime_error("Keyword *PUNC expects either a single argument or no argument.");

    fconfig.writePunch.setActive(activeLine);
}


void TKeyPLOT::parse(const std::vector<std::string>& tokens, bool activeLine, int) {
	auto numtokens = tokens.size();

	if (numtokens == 2)
		DiffOutHelper(fconfig.writePlot, "", tokens.at(1));
	else if (numtokens == 3)
		DiffOutHelper(fconfig.writePlot, tokens.at(2), tokens.at(1));
	else
		throw std::runtime_error("Keyword *PLOT expects either a single argument or no argument.");

    fconfig.writePlot.setActive(activeLine);
}


void TKeySOBS::parse(const std::vector<std::string>& tokens, bool activeLine, int) {
	auto numtokens = tokens.size();
	
	if (numtokens >= 2 )
        fconfig.sim.writeLGCFile = activeLine;
	else {
		throw std::runtime_error("Invalid argument for the keyword *SOBS. No argument needed");
	}
}