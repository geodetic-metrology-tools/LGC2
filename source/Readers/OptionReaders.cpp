#include "OptionReaders.h"

///////////////////////////////////////////////////////
// TKeyTITR
///////////////////////////////////////////////////////
TKeyTITR::TKeyTITR(TLGCData& project, int nb_allowed_keywords, const char** keywords): TAKeyWord(TITR, project)
{			
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyTITR::parse(const std::vector<std::string>& tokens, int) {
	if (tokens.size() != 3)
		throw std::runtime_error("Key *TITR expects only one argument");
}

//////////////////
// Referentiels //
//////////////////

void TKeyOLOC::parse(const std::vector<std::string>&, int) 
{
	// nothing to parse, using local cartesion coordinate system
	if(fconfig.referential == TRefSystemFactory::ERefFrame::kNotInGraph)
		fconfig.referential = TRefSystemFactory::ERefFrame::kLocalRefFrame;
	else
		throw std::runtime_error("Only one reference system option can be specified (either OLOC, RS2K, LEP or SPHE).");
}

void TKeyRS2K::parse(const std::vector<std::string>&, int) 
{
	// nothing to parse, using grid geoid
		if(fconfig.referential == TRefSystemFactory::ERefFrame::kNotInGraph)
			fconfig.referential = TRefSystemFactory::ERefFrame::kCernXYHg00Machine;
	else
		throw std::runtime_error("Only one reference system option can be specified (either OLOC, RS2K, LEP or SPHE).");
}

void TKeyLEP::parse(const std::vector<std::string>&, int) 
{
	// nothing to parse, using parabolic ellipsoid
	if(fconfig.referential == TRefSystemFactory::ERefFrame::kNotInGraph)
		fconfig.referential = TRefSystemFactory::ERefFrame::kCernXYHg85Machine;
	else
		throw std::runtime_error("Only one reference system option can be specified (either OLOC, RS2K, LEP or SPHE).");
}

void TKeySPHE::parse(const std::vector<std::string>&, int) {
	// nothing to parse, using spherical reference frame
	if(fconfig.referential == TRefSystemFactory::ERefFrame::kNotInGraph)
		fconfig.referential = TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS;
	else
		throw std::runtime_error("Only one reference system option can be specified (either OLOC, RS2K, LEP or SPHE).");
}



//////////////////
// Calc Options //
//////////////////

void TKeyALLFIXED::parse(const std::vector<std::string>&, int) {
	fconfig.allfixed = TLGCConfig::TBinaryOption(true);

	TAdjustablePoint::setAllFixedParam(fconfig.allfixed.isActiveRef());
}


void TKeyLIBR::parse(const std::vector<std::string>&, int) {
	fconfig.libre = TLGCConfig::TBinaryOption(true);
}


void TKeyNODUP::parse(const std::vector<std::string>&, int) {
	fconfig.nodup = TLGCConfig::TBinaryOption(true);
}


void TKeyPDOR::parse(const std::vector<std::string>& tokens, int) {
	const size_t numtok(tokens.size());
	if (numtok == 3)
		fconfig.pdor = TLGCConfig::TPDOR(tokens.at(2));
	else if (numtok == 4)
	{
		fconfig.pdor = TLGCConfig::TPDOR(tokens.at(2), TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kGons));
		fconfig.pdor.hasBearing = true;
	}
	else
		throw std::runtime_error("Keyword *PDOR takes either one or two arguments "
		                         "(Point name and optinal bearing), not " + std::to_string(numtok) + ".");
}


void TKeySIMU::parse(const std::vector<std::string>& tokens, int) {
	auto numTokens = tokens.size();

	if (numTokens == 3)
		fconfig.sim = TLGCConfig::TSimulation(std::stoi(tokens.at(2)));
	else
		throw std::runtime_error("*SIMU takes  1 argument, the number of simulation." );
}



////////////////////
// Output Options //
////////////////////

void TKeyAPRI::parse(const std::vector<std::string>&, int) {
	fconfig.useApriori = TLGCConfig::TBinaryOption(true);
}


void TKeyEREL::parse(const std::vector<std::string>& tokens, int) {
    auto numtokens(tokens.size());
	// this is a multi-line keyword, react just on the following calls
	if (tokens.at(1) == "EREL") { 
		if (numtokens > 2)
			throw std::runtime_error("Points for *EREL must occur as lines of point name pairs, "
			                         "not starting with the keyword line.");
		return;
	}

	// Points must occur in pairs
	if (numtokens != 2)
		throw std::runtime_error("Points for *EREL must occur as lines of point name pairs.");

	fconfig.erelPairs.push_back(std::make_pair(tokens.at(0), tokens.at(1)));
}


 TKeyFMTxHelper::TKeyFMTxHelper(TLGCConfig::TCustomOutputSep& co, const std::vector<std::string>& tokens) {
	auto numTokens = tokens.size();

	if (numTokens == 4) {
		if (tokens.at(2) == "SEP")  {
			if (tokens.at(3).length() < 2)
				throw std::runtime_error("Separator string for " + tokens.at(1) + " invalid, did you forget the quotes?");

			co = TLGCConfig::TCustomOutputSep(tokens.at(3).substr(1, tokens.at(3).length()-2));
		}
	}
	else if (numTokens == 3) {
		// Default behaviour: nothing to do
	}
	else {
		throw std::runtime_error("*" + tokens.at(1) + " expects either one or two arguments.");
	}
}


void TKeyFMTO::parse(const std::vector<std::string>& tokens, int) {
	TKeyFMTxHelper(fconfig.CustomOutputSeparator, tokens);
}


void TKeyFMTP::parse(const std::vector<std::string>& tokens, int) {
	TKeyFMTxHelper(fconfig.CustomOutputSeparatorPunch, tokens);
}


void TKeyHIST::parse(const std::vector<std::string>&, int) {
	fconfig.histo = TLGCConfig::TBinaryOption(true);
}

void TKeyPREC::parse(const std::vector<std::string>& tokens, int) {
	if (tokens.size() != 3)
		throw std::runtime_error("*PREC expects exactly one integer argument.");
	if (std::stoi(tokens.at(2)) >= 0 && std::stoi(tokens.at(2)) <= 7)
		fconfig.outPrecision = TLGCConfig::TPrecision(std::stoi(tokens.at(2)));
	else
		throw std::runtime_error("*PREC expects interger between 0 and 7.");
}


void TKeyPRES::parse(const std::vector<std::string>&, int) {
	fconfig.errorEllipses = TLGCConfig::TBinaryOption(true);
}


/////////////////////////////
// Additional Output Files //
/////////////////////////////

void TKeyDEFA::parse(const std::vector<std::string>&, int) {
	fconfig.writeDefa = TLGCConfig::TBinaryOption(true);
}


void TKeyFAUT::parse(const std::vector<std::string>& tokens, int) {
	auto numTokens = tokens.size();

	if (numTokens == 2)
		fconfig.faut = TLGCConfig::TFautDetect(FAUT_DEF_ALPHA, FAUT_DEF_BETA);
	else if (numTokens == 4)
		fconfig.faut = TLGCConfig::TFautDetect(std::stor(tokens.at(2)), std::stor(tokens.at(3)));
	else
		throw std::runtime_error("*FAUT takes either 0 or 2 arguments but " + 
		                          std::to_string(numTokens) + " were supplied." );
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


void TKeyPUNC::parse(const std::vector<std::string>& tokens, int) {
	auto numtokens = tokens.size();

	if (numtokens == 2)
		DiffOutHelper(fconfig.writePunch, "", tokens.at(1));
	else if (numtokens == 3)
		DiffOutHelper(fconfig.writePunch, tokens.at(2), tokens.at(1));
	else
		throw std::runtime_error("Keyword *PUNC expects either a single argument or no argument.");
}


void TKeyPLOT::parse(const std::vector<std::string>& tokens, int) {
	auto numtokens = tokens.size();

	if (numtokens == 2)
		DiffOutHelper(fconfig.writePlot, "", tokens.at(1));
	else if (numtokens == 3)
		DiffOutHelper(fconfig.writePlot, tokens.at(2), tokens.at(1));
	else
		throw std::runtime_error("Keyword *PLOT expects either a single argument or no argument.");
}


void TKeySOBS::parse(const std::vector<std::string>& tokens, int) {
	auto numtokens = tokens.size();
	
	if (numtokens == 2 ) {
		fconfig.sim.writeLGCFile = true;
		fconfig.sim.newInfileHasMeasurements = true;
	}
	else {
		throw std::runtime_error("Invalid argument for the keyword *SOBS. No argument needed");
	}
}