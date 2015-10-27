#include "InstrumentReaders.h"


TAInstrumentKey::TAInstrumentKey(TLGCData& project, const std::string& key) : 
	TAKeyWord(key, project),
	finstruments(project.getInstruments()), 
	flengths(project.getLength()), 
	fangles(project.getAngles()) 
	{}



TKeyINSTR::TKeyINSTR(TLGCData& project, int nb_allowed_keywords, const char** keywords) : TAKeyWord(INSTR, project)  
{
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyINSTR::parse(const std::vector<std::string>&, int) {
	// Does not parse, just the root of its sub-keys
}

//////////////////////
// POLAR instrument //
//////////////////////
TKeyPOLAR::TKeyPOLAR(TLGCData& project, int nb_allowed_keywords, const char** keywords): TAInstrumentKey(project, POLAR)
{
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyPOLAR::parse(const std::vector<std::string>& tokens, int) {
	using namespace LGC;

	if (tokens.at(0) == "*") { 
		// This is the keyword line		
		auto& polInstruments(finstruments.fPOLAR);
		checkInstrument(8, polInstruments, tokens);

		TInstrumentData::TPOLAR p;
		p.ID = tokens.at(2);
		p.defTarget = tokens.at(3);
		p.instrHeight = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMetres);
		p.sigmaInstrHeight = TLength(std::stor(tokens[5]), TLength::EUnits::kMillimetres);
		p.sigmaInstrCentering =  TLength(std::stor(tokens[6]), TLength::EUnits::kMillimetres);
		p.constAngle = TAngle(std::stor(tokens[7]), TAngle::EUnits::kGons);

		// store the new station
		polInstruments.insert(std::make_pair(tokens.at(2), p));
		currentStation = tokens.at(2);
	}
	else {
		// This is a target line		
		auto& targets(finstruments.fPOLAR[currentStation].targets);
		checkTarget(11, targets, tokens);				
				
		TInstrumentData::TPOLAR::TTarget t = {
					    tokens.at(0),
                   TAngle(std::stor(tokens.at(1)), TAngle::EUnits::kCCs), // given in cc, transform to rad
                   TAngle(std::stor(tokens.at(2)), TAngle::EUnits::kCCs), // given in cc, transform to rad
                   TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
                   TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
                   std::stor(tokens[5]) != 0.0,
				   TLength(std::stor(tokens[6]), TLength::EUnits::kMetres),
                   TLength(std::stor(tokens[7]), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
                   TLength(std::stor(tokens[8]), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
				   TLength(std::stor(tokens[9]), TLength::EUnits::kMetres),
                   TLength(std::stor(tokens[10]), TLength::EUnits::kMillimetres) // conversion from mili-metres to metres
		};

		// Add adjustable scalar into a global collection and store a pointer
		t.distCorrectionAdjustable = &flengths.addObject(TAdjustableLength(TLength(std::stor(tokens[6])),std::stoi(tokens[5])==0, currentStation + tokens.at(0)));

		// store the new target
		targets[tokens.at(0)] = t;
				
	}
}


const std::vector<std::string>& TKeyPOLAR::parentKeys() const {
	static std::vector<std::string> p(1);
	p.push_back(INSTR);
	return p;
}


//////////////////////
// CAMD instrument //
//////////////////////
TKeyCAMD::TKeyCAMD(TLGCData& project, int nb_allowed_keywords, const char** keywords): TAInstrumentKey(project, CAMD)
{
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyCAMD::parse(const std::vector<std::string>& tokens, int) {
	using namespace LGC;

	if (tokens.at(0) == "*") { 
		// This is the keyword line		
		auto& camInstruments(finstruments.fCAMD);
		checkInstrument(4, camInstruments, tokens);

		const TInstrumentData::TCAMD p = {
						tokens.at(2),
						tokens.at(3),
		};

		// store the new station
		camInstruments.insert(std::make_pair(tokens.at(2), p));
		currentStation = tokens.at(2);
	}
	else {
		// This is a target line		
		auto& targets(finstruments.fCAMD[currentStation].targets);
		checkTarget(5, targets, tokens);				
				
		TInstrumentData::TCAMD::TTarget t = {
					    tokens.at(0),
						std::stor(tokens.at(1))  * VECCONV , // unitless 
						std::stor(tokens.at(2))  * VECCONV , // unitless 
                  TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
                  TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
		};

		// store the new target
		targets[tokens.at(0)] = t;				
	}
}


const std::vector<std::string>& TKeyCAMD::parentKeys() const {
	static std::vector<std::string> p(1);
	p.push_back("INSTR");
	return p;
}



////////////////////
// EDM instrument //
////////////////////
TKeyEDM::TKeyEDM(TLGCData& project, int nb_allowed_keywords, const char** keywords) : TAInstrumentKey(project, EDM)
{
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyEDM::parse(const std::vector<std::string>& tokens, int) {
	using namespace LGC;

	if (tokens.at(0) == "*") {
		// This is the keyword line
		auto& station(finstruments.fEDM);
		checkInstrument(7, station, tokens);

		const TInstrumentData::TEDM e = {
				      tokens.at(2),
				      tokens.at(3),
                  TLength(std::stor(tokens.at(4)), TLength::EUnits::kMetres),
                  TLength(std::stor(tokens[5]), TLength::EUnits::kMillimetres),
                  TLength(std::stor(tokens[6]), TLength::EUnits::kMillimetres)
		};

		// store the new station
		station.insert(std::make_pair(tokens.at(2), e));
		currentStation = tokens.at(2);
	}
	else {
		// This is a target line
		auto& targets(finstruments.fEDM[currentStation].targets);
		checkTarget(9, targets, tokens);	

		TInstrumentData::TEDM::TTarget t = {
                  tokens.at(0),
                  TLength(std::stor(tokens.at(1)), TLength::EUnits::kMillimetres),
                  TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres),
                  std::stoi(tokens.at(3)) != 0,
                  TLength(stor(tokens.at(4)), TLength::EUnits::kMetres),
                  TLength(std::stor(tokens[5]), TLength::EUnits::kMillimetres),
                  TLength(std::stor(tokens[6]), TLength::EUnits::kMillimetres),
                  TLength(std::stor(tokens[7]), TLength::EUnits::kMetres),
                  TLength(std::stor(tokens[8]), TLength::EUnits::kMillimetres),
		};
	
		// Add adjustable scalar into a global collection and store a pointer
		t.distCorrectionAdjustable = &flengths.addObject(TAdjustableLength(TLength(std::stor(tokens[4])), std::stoi(tokens[3]) == 0, currentStation + tokens.at(0)));

		// store the new target
		targets[tokens.at(0)] = t;
	}
}

const std::vector<std::string>& TKeyEDM::parentKeys() const {
	static std::vector<std::string> p(1);
	p.push_back(INSTR);
	return p;
}

//////////////////////
// LEVEL instrument //
//////////////////////
TKeyLEVEL::TKeyLEVEL(TLGCData& project, int nb_allowed_keywords, const char** keywords) : TAInstrumentKey(project, LEVEL)
{
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyLEVEL::parse(const std::vector<std::string>& tokens, int){
	using namespace LGC;

	if (tokens.at(0) == "*") {
		// This is the keyword line
		auto& station(finstruments.fLEVEL);
		checkInstrument(6, station, tokens);

		TInstrumentData::TLEVEL l = {
         tokens.at(2),
         tokens.at(3),
         std::stoi(tokens.at(4)) != 0,
         TAngle(std::stor(tokens[5]), TAngle::EUnits::kGons)
		};

		currentStation = tokens.at(2);

		l.collAngleAdjustable = &fangles.addObject(TAdjustableAngle( TAngle(std::stor(tokens[5]), TAngle::kGons), std::stoi(tokens[4]) == 0, currentStation + "CollANGLE"));
		// store the new station
		station.insert(std::make_pair(tokens.at(2), l));
	}
	else {
		// This is a target line
		auto & targets(finstruments.fLEVEL[currentStation].targets);
		checkTarget(7, targets, tokens);		

		const TInstrumentData::TLEVEL::TTarget t = {
			          tokens.at(0),
			TLength(std::stor(tokens.at(1)), TLength::EUnits::kMillimetres),
         TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres),
         TLength(std::stor(tokens.at(3)), TLength::EUnits::kMetres),
         TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres),
         TLength(std::stor(tokens[5]), TLength::EUnits::kMetres),
         TLength(std::stor(tokens[6]), TLength::EUnits::kMillimetres),
		};

		// store the new target
		targets[tokens.at(0)] = t;
	}
}

const std::vector<std::string>& TKeyLEVEL::parentKeys() const {
	static std::vector<std::string> p(1);
	p.push_back("INSTR");
	return p;
}

//////////////////////
// SCALE instrument //
//////////////////////
TKeySCALE::TKeySCALE(TLGCData& project, int nb_allowed_keywords, const char** keywords) : TAInstrumentKey(project, SCALE)
{
	for(int i(0) ; i< nb_allowed_keywords ; i++)
		allowed_keywords.emplace_back(keywords[i]);
}
		
void TKeySCALE::parse(const std::vector<std::string>& tokens, int) {
	using namespace LGC;

	auto& scales(finstruments.fSCALE);
	checkInstrument(8, scales, tokens);

	const TInstrumentData::TSCALE s = {
					tokens.at(2),
               TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres),
               TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres),
               TLength(std::stor(tokens[5]), TLength::EUnits::kMetres),
               TLength(std::stor(tokens[6]), TLength::EUnits::kMillimetres),
               TLength(std::stor(tokens[7]), TLength::EUnits::kMillimetres)
	};

	// store the new scale
	scales.insert(std::make_pair(tokens.at(2), s));
}

const std::vector<std::string>& TKeySCALE::parentKeys() const {
	static std::vector<std::string> p(1);
	p.push_back(INSTR);
	return p;
}
