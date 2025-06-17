#include "OptionReaders.h"

#include <chrono>

///////////////////////////////////////////////////////
// TKeyTITR
///////////////////////////////////////////////////////
TKeyTITR::TKeyTITR(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAKeyWord(TITR, project)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyTITR::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	if (!activeLine)
		throw std::runtime_error("*TITR cannot be deactivated");

	if (tokens.at(0) == "*" && tokens.size() < 3)
		throw std::runtime_error("Key *TITR expects only one argument");
}

//////////////////
// Referentiels //
//////////////////

void TKeyOLOC::parse(const std::vector<std::string> &, bool activeLine, int)
{
	if (!activeLine)
		throw std::runtime_error("Reference system keywords cannot be deactivated");

	// nothing to parse, using local cartesion coordinate system
	if (fconfig.referential == TRefSystemFactory::ERefFrame::kNotInGraph)
		fconfig.referential = TRefSystemFactory::ERefFrame::kLocalRefFrame;
	else
		throw std::runtime_error("Only one reference system option can be specified (either OLOC, RS2K, LEP or SPHE).");
}

void TKeyRS2K::parse(const std::vector<std::string> &, bool activeLine, int)
{
	if (!activeLine)
		throw std::runtime_error("Reference system keywords cannot be deactivated");

	// nothing to parse, using grid geoid
	if (fconfig.referential == TRefSystemFactory::ERefFrame::kNotInGraph)
		fconfig.referential = TRefSystemFactory::ERefFrame::kCernXYHg00Machine;
	else
		throw std::runtime_error("Only one reference system option can be specified (either OLOC, RS2K, LEP or SPHE).");
}

void TKeyLEP::parse(const std::vector<std::string> &, bool activeLine, int)
{
	if (!activeLine)
		throw std::runtime_error("Reference system keywords cannot be deactivated");

	// nothing to parse, using parabolic ellipsoid
	if (fconfig.referential == TRefSystemFactory::ERefFrame::kNotInGraph)
		fconfig.referential = TRefSystemFactory::ERefFrame::kCernXYHg85Machine;
	else
		throw std::runtime_error("Only one reference system option can be specified (either OLOC, RS2K, LEP or SPHE).");
}

void TKeySPHE::parse(const std::vector<std::string> &, bool activeLine, int)
{
	if (!activeLine)
		throw std::runtime_error("Reference system keywords cannot be deactivated");

	// nothing to parse, using spherical reference frame
	if (fconfig.referential == TRefSystemFactory::ERefFrame::kNotInGraph)
		fconfig.referential = TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS;
	else
		throw std::runtime_error("Only one reference system option can be specified (either OLOC, RS2K, LEP or SPHE).");
}

//////////////////
// Calc Options //
//////////////////

void TKeyALLFIXED::parse(const std::vector<std::string> &, bool activeLine, int)
{
	fconfig.allfixed = TLGCConfig::TBinaryOption(activeLine);

	LGCAdjustablePoint::setAllFixedParam(activeLine);
}

void TKeyLIBR::parse(const std::vector<std::string> &, bool activeLine, int)
{
	throw std::runtime_error("LIBR keyword is deprecated, please use *CONSI LIBR, see user guide.");
	// fconfig.libre = TLGCConfig::TBinaryOption(activeLine);
}

void TKeyCOVAR::parse(const std::vector<std::string> &, bool activeLine, int)
{
	fconfig.covar = TLGCConfig::TBinaryOption(activeLine);
}

void TKeyCHABA::parse(const std::vector<std::string> &, bool activeLine, int)
{
	fconfig.chaba = TLGCConfig::TBinaryOption(activeLine);
}

void TKeyNODUP::parse(const std::vector<std::string> &, bool activeLine, int)
{
	fconfig.nodup = TLGCConfig::TBinaryOption(activeLine);
}

void TKeyPDOR::parse(const std::vector<std::string> &tokens, bool activeLine, int lineNumber)
{
	if (activeLine && fconfig.allfixed.isActive())
		throw std::runtime_error("PDOR is not allowed with ALLFIXED option.");

	const size_t numtok(tokens.size());

	// this is a multi-line keyword, react just on the following calls
	if (numtok > 1 && tokens.at(1) == "PDOR")
	{
		if (numtok > 2)
			throw std::runtime_error("PDOR not starting with the keyword line.");

		fconfig.pdor.setActive(activeLine);
		return;
	}

	// Either just a pointname or a pointname + bearing
	if (numtok == 1)
	{
		fconfig.pdor.fptname = tokens.at(0);
		fconfig.pdor.line = lineNumber;
	}
	else if (numtok == 2)
	{
		fconfig.pdor.fptname = tokens.at(0);
		fconfig.pdor.fgis = TAngle(std::stor(tokens.at(1)), TAngle::EUnits::kGons);
		fconfig.pdor.hasBearing = true;
		fconfig.pdor.line = lineNumber;
	}
	else
		throw std::runtime_error("Keyword *PDOR takes either one or two arguments "
								 "(Point name and optinal bearing), not "
			+ std::to_string(numtok) + ".");
}

void TKeySIMU::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	auto numTokens = tokens.size();

	// (VV) Use a time difference to create a seed (seedNumber) for the pseudo-random number generator/engine
	typedef std::chrono::high_resolution_clock myclock;
	myclock::time_point time1 = myclock::now(); // (VV) current timestamp
	myclock::time_point time0 = myclock::time_point::min(); // (VV) reference timestamp
	myclock::duration d = time1 - time0; // (VV) time difference
	int seedNumber = d.count();

	// (VV) If the last token starts with a comment character, the variable existComment becomes true
	const char fOfLastToken = tokens.back().at(0);
	bool existComment = false;
	if (fOfLastToken == '$' || fOfLastToken == '%')
	{
		existComment = true;
	}

	// (VV) If the tokens are more than five, e.g., "*SIMU N S %comment" or
	// (VV) if the tokens are four without a comment at the end, e.g., "*SIMU N S",
	// (VV) then get the number of the simulation N (tokens.at(2)) and the seed number S (tokens.at(3))
	if (numTokens >= 5 || (numTokens == 4 && !(fOfLastToken == '$' || fOfLastToken == '%')))
	{
		fconfig.sim = TLGCConfig::TSimulation(std::stoi(tokens.at(2)), std::stoi(tokens.at(3)),fconfig.sim.writeLGCFile);
		fconfig.sim.setActive(activeLine);
	}
	// (VV) If the tokens are four with a comment, e.g., "*SIMU N %comment" or
	// (VV) if the tokens are three without a comment at the end, e.g., "*SIMU N",
	// (VV) then get the number of the simulation N (tokens.at(2))
	else if (numTokens == 4 || (numTokens == 3 && !(fOfLastToken == '$' || fOfLastToken == '%')))
	{
		fconfig.sim = TLGCConfig::TSimulation(std::stoi(tokens.at(2)), seedNumber,fconfig.sim.writeLGCFile);
		fconfig.sim.setActive(activeLine);
	}
	// (VV) If the tokens are three with a comment, e.g., "*SIMU %comment" or
	// (VV) if the tokens are less than three, e.g., "*SIMU",
	// (VV) then throw an error message
	else
	{
		throw std::runtime_error("*SIMU accepts either 1 or 2 arguments but zero (0) arguments were provided.");
	}

}

////////////////////
// Output Options //
////////////////////

void TKeyAPRI::parse(const std::vector<std::string> &, bool activeLine, int)
{
	fconfig.useApriori = TLGCConfig::TBinaryOption(activeLine);
}

void TKeyEREL::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	auto numtokens(tokens.size());
	// this is a multi-line keyword, react just on the following calls
	if ((tokens.at(0) == "*") && (tokens.at(1) == "EREL"))
	{
		if (numtokens > 2)
		{
			throw std::runtime_error("*EREL accepts no additional arguments. The point pairs must follow on subsequent lines.");
		}
		erelActive = activeLine;
		return;
	}
	// Points must occur in pairs with optinal destination frame name
	if ((numtokens < 2) || (numtokens > 3))
	{
		throw std::runtime_error("The Relative error section only accepts pairs of points (for relative error expressed in Root) with an additional argument specifying "
								 "the destination frame name.");
	}

	// default destination frame is Root
	std::string destinationFrame{"ROOT"};
	if (numtokens == 3)
	{
		destinationFrame = tokens.at(2);
	}
	if (activeLine && erelActive)
	{
		TLSCalcRelativeErrorPoint newErelPair(tokens.at(0), tokens.at(1), destinationFrame);
		fconfig.fRelErrors.points.push_back(newErelPair);
	}
}

void TKeyERELFRAME::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	auto numtokens(tokens.size());
	// this is a multi-line keyword, react just on the following calls
	if ((tokens.at(0) == "*") && (tokens.at(1) == "ERELFRAME"))
	{
		if (numtokens > 2)
		{
			throw std::runtime_error("*ERELFRAME accepts no additional arguments. The frame pairs must follow on subsequent lines.");
		}
		erelFrameActive = activeLine;
		return;
	}
	// Frames must occur in pairs
	if ((numtokens < 2) || (numtokens > 2))
	{
		throw std::runtime_error("The Relative Frame error section only accepts pairs of frames.");
	}

	if (activeLine && erelFrameActive)
	{
		fconfig.fRelErrors.frames.push_back(TLSCalcRelativeErrorFrame(tokens.at(0), tokens.at(1)));
	}
}

TKeyFMTxHelper::TKeyFMTxHelper(TLGCConfig::TCustomOutputSep &co, const std::vector<std::string> &tokens, bool activeLine)
{
	auto numTokens = tokens.size();

	if (numTokens == 4)
	{
		if (tokens.at(2) == "SEP")
		{
			if (tokens.at(3).length() < 2)
				throw std::runtime_error("Separator string for " + tokens.at(1) + " invalid, did you forget the quotes?");

			co = TLGCConfig::TCustomOutputSep(tokens.at(3).substr(1, tokens.at(3).length() - 2));
			co.setActive(activeLine);
		}
	}
	else if (numTokens == 3)
	{
		// Default behaviour: nothing to do
	}
	else
	{
		throw std::runtime_error("*" + tokens.at(1) + " expects SEP \"separator\"  ");
	}
}

void TKeyFMTO::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	TKeyFMTxHelper(fconfig.CustomOutputSeparator, tokens, activeLine);
}

void TKeyFMTP::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	TKeyFMTxHelper(fconfig.CustomOutputSeparatorPunch, tokens, activeLine);
}

void TKeyHIST::parse(const std::vector<std::string> &, bool activeLine, int)
{
	fconfig.histo = TLGCConfig::TBinaryOption(activeLine);

	// Set the usage of histogram in obsSummary
	TLGCObsSummary::createHistogram(activeLine);
}
void TKeyCONSI::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	// activate consistency check
	fconfig.consCheck = TLGCConfig::TBinaryOption(activeLine);
	// TODO: make sure this only gets executed ic line is active
	if (tokens.size() > 2)
	{
		// check if LIBR via automatic constraint detection or manual constraint specification is activated
		if (tokens.at(2) == "LIBR")
		{
			fconfig.useConsiLibr = TLGCConfig::TBinaryOption(activeLine);
			// LIBR mode active, either constraints are given or we try to find them automatically
			if (tokens.size() == 3)
			{
				// no constraints specified manually
				// activate the automatic constraint identification check
				// this is done in TDataAnalyzer
			}
			else
			{
				TOptionHelper opts(tokens.cbegin() + 2, tokens.cend());
				std::array<bool, 7> manualSpecifiedConstraints{opts.has("TX"), opts.has("TY"), opts.has("TZ"), opts.has("RX"), opts.has("RY"), opts.has("RZ"), opts.has("SCL")};
				// check if at least one constraint is specified
				bool hasUserDefinedConstraint = std::any_of(manualSpecifiedConstraints.begin(), manualSpecifiedConstraints.end(), [](bool value) { return value; });
				fconfig.hasManualConstraints = TLGCConfig::TBinaryOption(hasUserDefinedConstraint);

				if (hasUserDefinedConstraint)
				{
					fconfig.manualConstraints = manualSpecifiedConstraints;
				}
				else
				{
					throw std::runtime_error("Optional arguments allowed after *CONSI LIBR are TX,TY,TZ,RX,RY,RZ,SCL");
				}
			}
		}
		else
		{
			throw std::runtime_error("Unknown *CONSI option given : " + tokens.at(2));
		}
	}
}

void TKeyJSON::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	fconfig.writeJSON = TLGCConfig::TBinaryOption(activeLine);
	for (int i = 2; i < tokens.size(); i++)
	{
		if (tokens.at(i) == "COVAR")
			fconfig.writeJSON_COVAR = true;
	}
}

void TKeyPREC::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	if (tokens.size() != 3)
		throw std::runtime_error("*PREC expects exactly one integer argument.");
	if (std::stoi(tokens.at(2)) >= 0 && std::stoi(tokens.at(2)) <= 7)
		fconfig.outPrecision = TLGCConfig::TPrecision(std::stoi(tokens.at(2)));
	else
		throw std::runtime_error("*PREC expects interger between 0 and 7.");

	if (!activeLine)
		fconfig.outPrecision = TLGCConfig::TPrecision();
}

void TKeyMICR::parse(const std::vector<std::string> & /*tokens*/, bool, int)
{
	fconfig.outPrecision = TLGCConfig::TPrecision(6);
	auto &outputMessages(proj.getFileLogger());
	outputMessages.writeReportHeader("MICR should not be used. Use PREC option.");
}

void TKeyCLIC::parse(const std::vector<std::string> & /*tokens*/, bool, int)
{
	fconfig.outPrecision = TLGCConfig::TPrecision(6);
	auto &outputMessages(proj.getFileLogger());
	outputMessages.writeReportHeader("CLIC should not be used. Use PREC option.");
}

void TKeyTOL::parse(const std::vector<std::string> &tokens, bool, int)
{
	if (tokens.size() != 3)
		throw std::runtime_error("*TOL expects exactly one integer argument.");
	if (std::stoi(tokens.at(2)) >= 0 && std::stoi(tokens.at(2)) <= 6)
		fconfig.outPrecision = TLGCConfig::TPrecision(1 + std::stoi(tokens.at(2)));
	else
		throw std::runtime_error("*TOL expects interger between 0 and 6.");

	auto &outputMessages(proj.getFileLogger());
	outputMessages.writeReportHeader("TOL should not be used. Use PREC option.");
}

void TKeyDIXI::parse(const std::vector<std::string> & /*tokens*/, bool, int)
{
	fconfig.outPrecision = TLGCConfig::TPrecision(4);

	auto &outputMessages(proj.getFileLogger());
	outputMessages.writeReportHeader("DIXI should not be used. Use PREC option");
}

void TKeyNOSPC::parse(const std::vector<std::string> & /*tokens*/, bool, int)
{
	auto &outputMessages(proj.getFileLogger());
	outputMessages.writeReportHeader("NOSPC is used, it is an lgc1 option, so it is ignore in lgc2 output files");
}

void TKeyPRES::parse(const std::vector<std::string> &, bool activeLine, int)
{
	fconfig.errorEllipses = TLGCConfig::TBinaryOption(activeLine);
}

/////////////////////////////
// Additional Output Files //
/////////////////////////////

void TKeyDEFA::parse(const std::vector<std::string> &, bool activeLine, int)
{
	fconfig.writeDefa = TLGCConfig::TBinaryOption(activeLine);
}

void TKeyFAUT::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	auto numTokens = tokens.size();

	if (numTokens == 2)
		fconfig.faut = TLGCConfig::TFautDetect(FAUT_DEF_ALPHA, FAUT_DEF_BETA);
	else if (numTokens == 4)
		fconfig.faut = TLGCConfig::TFautDetect(std::stor(tokens.at(2)), std::stor(tokens.at(3)));
	else
		throw std::runtime_error("*FAUT takes either 0 or 2 arguments but " + std::to_string(numTokens) + " were supplied.");

	fconfig.faut.setActive(activeLine);
}

DiffOutHelper::DiffOutHelper(TLGCConfig::TCoordOut &out, const std::string &opt, const std::string &key)
{
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

void TKeyPUNC::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	auto numtokens = tokens.size();

	if (numtokens == 2)
		DiffOutHelper(fconfig.writePunch, "", tokens.at(1));
	else if (numtokens == 3)
		DiffOutHelper(fconfig.writePunch, tokens.at(2), tokens.at(1));
	else
		throw std::runtime_error("Keyword *PUNC expects either a single argument or no argument.");

	fconfig.writePunch.setActive(activeLine);
}

void TKeyPLOT::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	auto numtokens = tokens.size();

	if (numtokens == 2)
		DiffOutHelper(fconfig.writePlot, "", tokens.at(1));
	else if (numtokens == 3)
		DiffOutHelper(fconfig.writePlot, tokens.at(2), tokens.at(1));
	else
		throw std::runtime_error("Keyword *PLOT expects either a single argument or no argument.");

	fconfig.writePlot.setActive(activeLine);
}

void TKeySOBS::parse(const std::vector<std::string> &tokens, bool activeLine, int)
{
	auto numtokens = tokens.size();

	if (numtokens >= 2)
	{
		fconfig.sim.writeLGCFile = activeLine;
	}
	else
	{
		throw std::runtime_error("Invalid argument for the keyword *SOBS. No argument needed");
	}
}