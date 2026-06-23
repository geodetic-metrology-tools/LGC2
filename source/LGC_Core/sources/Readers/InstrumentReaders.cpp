// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "InstrumentReaders.h"

TAInstrumentKey::TAInstrumentKey(TLGCData &project, const std::string &key) :
	TAKeyWord(key, project), finstruments(project.getInstruments()), flengths(project.getLength()), fangles(project.getAngles())
{
}

TKeyINSTR::TKeyINSTR(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAKeyWord(INSTR, project)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyINSTR::parse(const std::vector<std::string> &tokens, bool /*activeLine*/, int line)
{
	// Does not parse, just the root of its sub-keys
	if (tokens[0] != "*")
		throw std::runtime_error("INSTR must be follow by an instrument keyword ");
}

//////////////////////
// POLAR instrument //
//////////////////////
TKeyPOLAR::TKeyPOLAR(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAInstrumentKey(project, POLAR)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyPOLAR::parse(const std::vector<std::string> &tokens, bool /*activeLine*/, int line)
{
	using namespace LGC;

	if (tokens.at(0) == "*")
	{
		// This is the keyword line
		auto &polInstruments(finstruments.fPOLAR);
		checkInstrument(8, polInstruments, tokens);

		auto p = std::make_shared<TInstrumentData::TPOLAR>();
		p->ID = tokens.at(2);
		p->defTarget = tokens.at(3);
		p->instrHeight = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMetres);
		p->sigmaInstrHeight = TLength(std::stor(tokens[5]), TLength::EUnits::kMillimetres);
		p->sigmaInstrCentering = TLength(std::stor(tokens[6]), TLength::EUnits::kMillimetres);
		p->constAngle = TAngle(std::stor(tokens[7]), TAngle::EUnits::kGons);
		p->line = line;

		// store the new station
		polInstruments.insert(std::make_pair(tokens.at(2), p));
		currentStation = tokens.at(2);
	}
	else
	{
		// This is a target line
		auto &targets(finstruments.fPOLAR[currentStation]->targets);
		checkTarget(11, targets, tokens);

		auto t = std::make_shared<TInstrumentData::TPOLAR::TTarget>(TInstrumentData::TPOLAR::TTarget(tokens.at(0), TAngle(std::stor(tokens.at(1)), TAngle::EUnits::kCCs), // given in cc, transform to rad
			TAngle(std::stor(tokens.at(2)), TAngle::EUnits::kCCs), // given in cc, transform to rad
			TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
			TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
			std::stor(tokens[5]) != 0.0, TLength(std::stor(tokens[6]), TLength::EUnits::kMetres), TLength(std::stor(tokens[7]), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
			TLength(std::stor(tokens[8]), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
			TLength(std::stor(tokens[9]), TLength::EUnits::kMetres), TLength(std::stor(tokens[10]), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
			nullptr, line));

		// store the new target
		targets[tokens.at(0)] = t;
	}
}

const std::vector<std::string> &TKeyPOLAR::parentKeys() const
{
	static std::vector<std::string> p(1);
	p.push_back(INSTR);
	return p;
}

//////////////////////
// CAMD instrument //
//////////////////////
TKeyCAMD::TKeyCAMD(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAInstrumentKey(project, CAMD)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyCAMD::parse(const std::vector<std::string> &tokens, bool /*activeLine*/, int line)
{
	using namespace LGC;

	if (tokens.at(0) == "*")
	{
		// This is the keyword line
		auto &camInstruments(finstruments.fCAMD);
		checkInstrument(5, camInstruments, tokens);

		auto p = std::make_shared<TInstrumentData::TCAMD>(tokens.at(2), tokens.at(3), TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres), line);

		// store the new station
		camInstruments.insert(std::make_pair(tokens.at(2), p));
		currentStation = tokens.at(2);
	}
	else
	{
		// This is a target line
		auto &targets(finstruments.fCAMD[currentStation]->targets);
		checkTarget(5, targets, tokens);

		auto t = std::make_shared<TInstrumentData::TCAMD::TTarget>(TInstrumentData::TCAMD::TTarget(tokens.at(0),
			std::stor(tokens.at(1)) * VECCONV, // unitless
			std::stor(tokens.at(2)) * VECCONV, // unitless
			TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
			TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres), // conversion from mili-metres to metres
			line));

		// store the new target
		targets[tokens.at(0)] = t;
	}
}

const std::vector<std::string> &TKeyCAMD::parentKeys() const
{
	static std::vector<std::string> p(1);
	p.push_back("INSTR");
	return p;
}

////////////////////
// EDM instrument //
////////////////////
TKeyEDM::TKeyEDM(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAInstrumentKey(project, EDM)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyEDM::parse(const std::vector<std::string> &tokens, bool /*activeLine*/, int line)
{
	using namespace LGC;

	if (tokens.at(0) == "*")
	{
		// This is the keyword line
		auto &station(finstruments.fEDM);
		checkInstrument(7, station, tokens);

		auto e = std::make_shared<TInstrumentData::TEDM>(tokens.at(2), tokens.at(3), TLength(std::stor(tokens.at(4)), TLength::EUnits::kMetres),
			TLength(std::stor(tokens[5]), TLength::EUnits::kMillimetres), TLength(std::stor(tokens[6]), TLength::EUnits::kMillimetres), line);

		// store the new station
		station.insert(std::make_pair(tokens.at(2), e));
		currentStation = tokens.at(2);
	}
	else
	{
		// This is a target line
		auto &targets(finstruments.fEDM[currentStation]->targets);
		checkTarget(9, targets, tokens);

		auto t = std::make_shared<TInstrumentData::TEDM::TTarget>(TInstrumentData::TEDM::TTarget(tokens.at(0), TLength(std::stor(tokens.at(1)), TLength::EUnits::kMillimetres),
			TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres), std::stoi(tokens.at(3)) != 0, TLength(stor(tokens.at(4)), TLength::EUnits::kMetres),
			TLength(std::stor(tokens[5]), TLength::EUnits::kMillimetres), TLength(std::stor(tokens[6]), TLength::EUnits::kMillimetres),
			TLength(std::stor(tokens[7]), TLength::EUnits::kMetres), TLength(std::stor(tokens[8]), TLength::EUnits::kMillimetres), nullptr, line));

		// store the new target
		targets[tokens.at(0)] = t;
	}
}

const std::vector<std::string> &TKeyEDM::parentKeys() const
{
	static std::vector<std::string> p(1);
	p.push_back(INSTR);
	return p;
}

//////////////////////
// LEVEL instrument //
//////////////////////
TKeyLEVEL::TKeyLEVEL(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAInstrumentKey(project, LEVEL)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyLEVEL::parse(const std::vector<std::string> &tokens, bool /*activeLine*/, int line)
{
	using namespace LGC;

	if (tokens.at(0) == "*")
	{
		// This is the keyword line
		auto &station(finstruments.fLEVEL);

		// the token count includes *, the keyword and the parameters
		int expectedTokenCount = 8;
		checkInstrument(expectedTokenCount, station, tokens);

		currentStation = tokens.at(2);

		auto l = std::make_shared<TInstrumentData::TLEVEL>(tokens.at(2), tokens.at(3), TLength(std::stor(tokens.at(4)), TLength::EUnits::kMetres),
			TLength(std::stor(tokens.at(5)), TLength::EUnits::kMillimetres), std::stoi(tokens.at(6)) != 0, TAngle(std::stor(tokens[7]), TAngle::EUnits::kGons), nullptr, line);

		// store the new station
		station.insert(std::make_pair(tokens.at(2), l));
	}
	else
	{
		// This is a target line
		auto &targets(finstruments.fLEVEL[currentStation]->targets);

		// the token count includes *, the keyword and the parameters
		int expectedTokenCount = 10;
		checkTarget(expectedTokenCount, targets, tokens);

		auto t = std::make_shared<TInstrumentData::TLEVEL::TTarget>(TInstrumentData::TLEVEL::TTarget(tokens.at(0), TLength(std::stor(tokens.at(1)), TLength::EUnits::kMillimetres),
			TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres), TLength(std::stor(tokens.at(3)), TLength::EUnits::kMetres),
			TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres), TLength(std::stor(tokens.at(5)), TLength::EUnits::kMetres),
			TLength(std::stor(tokens.at(6)), TLength::EUnits::kMillimetres), TLength(std::stor(tokens.at(7)), TLength::EUnits::kMillimetres),
			TLength(std::stor(tokens.at(8)), TLength::EUnits::kMillimetres), TLength(std::stor(tokens.at(9)), TLength::EUnits::kMetres), line));

		// store the new target
		targets[tokens.at(0)] = t;
	}
}

const std::vector<std::string> &TKeyLEVEL::parentKeys() const
{
	static std::vector<std::string> p(1);
	p.push_back("INSTR");
	return p;
}

//////////////////////
// SCALE instrument //
//////////////////////
TKeySCALE::TKeySCALE(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAInstrumentKey(project, SCALE)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeySCALE::parse(const std::vector<std::string> &tokens, bool /*activeLine*/, int line)
{
	using namespace LGC;

	auto &scales(finstruments.fSCALE);
	checkInstrument(8, scales, tokens);

	auto s = std::make_shared<TInstrumentData::TSCALE>(TInstrumentData::TSCALE(tokens.at(2), TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres),
		TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres), TLength(std::stor(tokens[5]), TLength::EUnits::kMetres),
		TLength(std::stor(tokens[6]), TLength::EUnits::kMillimetres), TLength(std::stor(tokens[7]), TLength::EUnits::kMillimetres), line));

	// store the new scale
	scales.insert(std::make_pair(tokens.at(2), s));
}

const std::vector<std::string> &TKeySCALE::parentKeys() const
{
	static std::vector<std::string> p(1);
	p.push_back(INSTR);
	return p;
}

//////////////////////
// INCL  instrument //
//////////////////////
TKeyINCL::TKeyINCL(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAInstrumentKey(project, INCL)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyINCL::parse(const std::vector<std::string> &tokens, bool /*activeLine*/, int line)
{
	using namespace LGC;

	auto &incls(finstruments.fINCL);
	checkInstrument(9, incls, tokens);

	auto i = std::make_shared<TInstrumentData::TINCL>(TInstrumentData::TINCL(tokens.at(2), TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs),
		TAngle(std::stor(tokens.at(4)), TAngle::EUnits::kMicroRadians), TAngle(std::stor(tokens.at(5)), TAngle::EUnits::kGons), TAngle(std::stor(tokens.at(6)), TAngle::EUnits::kCCs),
		TAngle(std::stor(tokens.at(7)), TAngle::EUnits::kGons), TAngle(std::stor(tokens.at(8)), TAngle::EUnits::kCCs), line));

	// store the new inclinometer
	incls.insert(std::make_pair(tokens.at(2), i));
}

const std::vector<std::string> &TKeyINCL::parentKeys() const
{
	static std::vector<std::string> p(1);
	p.push_back(INSTR);
	return p;
}

//////////////////////
// HLSR instrument //
//////////////////////
TKeyHLSR::TKeyHLSR(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAInstrumentKey(project, HLSR)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyHLSR::parse(const std::vector<std::string> &tokens, bool /*activeLine*/, int line)
{
	using namespace LGC;

	auto &hlsrs(finstruments.fHLSR);
	checkInstrument(6, hlsrs, tokens);

	auto h = std::make_shared<TInstrumentData::THLSR>(TInstrumentData::THLSR(tokens.at(2), TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres),
		TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres), TLength(std::stor(tokens[5]), TLength::EUnits::kMillimetres), line));

	// store the new hlsr
	hlsrs.insert(std::make_pair(tokens.at(2), h));
}

const std::vector<std::string> &TKeyHLSR::parentKeys() const
{
	static std::vector<std::string> p{INSTR};
	return p;
}

//////////////////////
// WPSR instrument //
//////////////////////
TKeyWPSR::TKeyWPSR(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAInstrumentKey(project, WPSR)
{
	for (int i(0); i < nb_allowed_keywords; i++)
		allowed_keywords.emplace_back(keywords[i]);
}

void TKeyWPSR::parse(const std::vector<std::string> &tokens, bool /*activeLine*/, int line)
{
	using namespace LGC;

	auto &wpsrs(finstruments.fWPSR);
	checkInstrument(7, wpsrs, tokens);

	auto h = std::make_shared<TInstrumentData::TWPSR>(TInstrumentData::TWPSR(tokens.at(2), TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres),
		TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres), TLength(std::stor(tokens.at(5)), TLength::EUnits::kMillimetres),
		TLength(std::stor(tokens.at(6)), TLength::EUnits::kMillimetres), TLength(0), TLength(0), TLength(0), TLength(0), TLength(0), line));

	// store the new wpsr
	wpsrs.insert(std::make_pair(tokens.at(2), h));
}

const std::vector<std::string> &TKeyWPSR::parentKeys() const
{
	static std::vector<std::string> p{INSTR};
	return p;
}
