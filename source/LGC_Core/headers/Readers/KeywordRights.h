/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KEYWORD_AUTHORIZATIONS_H
#define KEYWORD_AUTHORIZATIONS_H
#pragma once

// STL
#include <map>
#include <set>
#include <string>

// ---------------------------------------------------------------------------
// Keyword string constants
// ---------------------------------------------------------------------------
// structural
inline const std::string END = "END";
inline const std::string ENDFRAME = "ENDFRAME";
inline const std::string FRAME = "FRAME";
inline const std::string INSTR = "INSTR";
inline const std::string TITR = "TITR";
// referentials
inline const std::string LEP = "LEP";
inline const std::string OLOC = "OLOC";
inline const std::string RS2K = "RS2K";
inline const std::string SPHE = "SPHE";
// options
inline const std::string ALLFIXED = "ALLFIXED";
inline const std::string APRI = "APRI";
inline const std::string CHABA = "CHABA";
inline const std::string CLIC = "CLIC";
inline const std::string CONSI = "CONSI";
inline const std::string COVAR = "COVAR";
inline const std::string DEFA = "DEFA";
inline const std::string DIXI = "DIXI";
inline const std::string EREL = "EREL";
inline const std::string ERELFRAME = "ERELFRAME";
inline const std::string FAUT = "FAUT";
inline const std::string FMTP = "FMTP";
inline const std::string HIST = "HIST";
inline const std::string JSON = "JSON";
inline const std::string LIBR = "LIBR";
inline const std::string LM = "LM";
inline const std::string MICR = "MICR";
inline const std::string NODUP = "NODUP";
inline const std::string NOSPC = "NOSPC";
inline const std::string PREC = "PREC";
inline const std::string PRES = "PRES";
inline const std::string PUNC = "PUNC";
inline const std::string SIMU = "SIMU";
inline const std::string SOBS = "SOBS";
inline const std::string TOL = "TOL";
// instruments
inline const std::string CAMD = "CAMD";
inline const std::string EDM = "EDM";
inline const std::string HLSR = "HLSR";
inline const std::string INCL = "INCL";
inline const std::string LEVEL = "LEVEL";
inline const std::string POLAR = "POLAR";
inline const std::string SCALE = "SCALE";
inline const std::string WPSR = "WPSR";
// points
inline const std::string CALA = "CALA";
inline const std::string PDOR = "PDOR";
inline const std::string POIN = "POIN";
inline const std::string VXY = "VXY";
inline const std::string VXZ = "VXZ";
inline const std::string VYZ = "VYZ";
inline const std::string VZ = "VZ";
// polar measurements (TSTN-based)
inline const std::string ANGL = "ANGL";
inline const std::string DHOR = "DHOR";
inline const std::string DIST = "DIST";
inline const std::string DMES = "DMES";
inline const std::string DTHE = "DTHE";
inline const std::string ECDIR = "ECDIR";
inline const std::string ECTH = "ECTH";
inline const std::string PLR3D = "PLR3D";
inline const std::string PLR3D_ANGL = "PLR3D:ANGL";
inline const std::string PLR3D_DIST = "PLR3D:DIST";
inline const std::string PLR3D_ZEND = "PLR3D:ZEND";
inline const std::string TSTN = "TSTN";
inline const std::string V0 = "V0";
inline const std::string ZEND = "ZEND";
inline const std::string ZENH = "ZENH";
inline const std::string ZENI = "ZENI";
// camera measurements
inline const std::string CAM = "CAM";
inline const std::string UVD = "UVD";
inline const std::string UVD_DIST = "UVD:DIST";
inline const std::string UVD_X = "UVD:X";
inline const std::string UVD_Y = "UVD:Y";
inline const std::string UVEC = "UVEC";
inline const std::string UVEC_X = "UVEC:X";
inline const std::string UVEC_Y = "UVEC:Y";
// standalone measurements
inline const std::string DLEV = "DLEV";
inline const std::string DSPT = "DSPT";
inline const std::string DVER = "DVER";
inline const std::string ECHO = "ECHO";
inline const std::string ECSP = "ECSP";
inline const std::string ECVE = "ECVE";
inline const std::string ECWI = "ECWI";
inline const std::string ECWS = "ECWS";
inline const std::string INCLY = "INCLY";
inline const std::string OBSXYZ = "OBSXYZ";
inline const std::string ORIE = "ORIE";
inline const std::string RADI = "RADI";
inline const std::string ROLLY = "ROLLY";

namespace KeywordRights
{

// ---------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------
inline std::set<std::string> unite(std::initializer_list<std::set<std::string>> sets)
{
	std::set<std::string> result;
	for (auto &s : sets)
		result.insert(s.begin(), s.end());
	return result;
}

inline std::set<std::string> minus(const std::set<std::string> &a, const std::set<std::string> &b)
{
	std::set<std::string> result;
	for (auto &e : a)
		if (b.count(e) == 0)
			result.insert(e);
	return result;
}

// ---------------------------------------------------------------------------
// Named sets
// ---------------------------------------------------------------------------
// referentials
const std::set<std::string> referentials = {LEP, OLOC, RS2K, SPHE};
// options
const std::set<std::string> options = {ALLFIXED, APRI, CHABA, CONSI, COVAR, DEFA, EREL, ERELFRAME, FAUT, FMTP, HIST, JSON, LIBR, LM, NODUP, PREC, PRES, PUNC, SIMU, SOBS};
// instruments
const std::set<std::string> instruments = {CAMD, EDM, HLSR, INCL, LEVEL, POLAR, SCALE, WPSR};
// points
const std::set<std::string> points = {CALA, POIN, VXY, VXZ, VYZ, VZ};
// polar measurements (TSTN-based)
const std::set<std::string> polarMeas = {ANGL, DHOR, DIST, ECDIR, ECTH, PLR3D, ZEND};
// camera measurements
const std::set<std::string> camMeas = {UVD, UVEC};
// standalone measurements (context restrictions enforced in TDataAnalyzer)
const std::set<std::string> standaloneMeas = {DLEV, DSPT, DVER, ECHO, ECSP, ECVE, ECWI, ECWS, INCLY, OBSXYZ, ORIE, RADI, ROLLY};
// structural keywords in measurement context
const std::set<std::string> measCommon = {CAM, END, ENDFRAME, FRAME, TSTN};

// ---------------------------------------------------------------------------
// LGC2 rights: built from 16 composition rules
// ---------------------------------------------------------------------------
inline std::map<std::string, std::set<std::string>> buildLGC2Rights()
{
	std::map<std::string, std::set<std::string>> r;

	// TITR -> referentials
	r[TITR] = referentials;

	// Referentials -> options + {INSTR}
	auto refSucc = unite({options, {INSTR}});
	for (auto &kw : referentials)
		r[kw] = refSucc;

	// Options -> (options - {self}) + {INSTR}
	auto optBase = unite({options, {INSTR}});
	for (auto &kw : options)
	{
		auto s = optBase;
		s.erase(kw);
		r[kw] = s;
	}

	// INSTR -> instruments + points + standaloneMeas + {FRAME, PDOR}
	r[INSTR] = unite({instruments, points, standaloneMeas, {FRAME, PDOR}});

	// Instruments -> instruments + points + standaloneMeas + {TSTN, CAM, FRAME, PDOR}
	auto instrSucc = unite({instruments, points, standaloneMeas, {TSTN, CAM, FRAME, PDOR}});
	for (auto &kw : instruments)
		r[kw] = instrSucc;

	// Points -> (points - {self}) + standaloneMeas + measCommon
	auto ptBase = unite({points, standaloneMeas, measCommon});
	for (auto &kw : points)
	{
		auto s = ptBase;
		s.erase(kw);
		r[kw] = s;
	}

	// PDOR -> points + standaloneMeas + measCommon
	r[PDOR] = unite({points, standaloneMeas, measCommon});

	// FRAME -> points + standaloneMeas + {CAM, TSTN, FRAME, ENDFRAME}
	r[FRAME] = unite({points, standaloneMeas, {CAM, TSTN, FRAME, ENDFRAME}});

	// ENDFRAME -> points + standaloneMeas + measCommon
	r[ENDFRAME] = unite({points, standaloneMeas, measCommon});

	// TSTN -> {V0}
	r[TSTN] = {V0};

	// V0 -> polarMeas
	r[V0] = polarMeas;

	// Polar measurements -> polarMeas + standaloneMeas + measCommon
	auto polarSucc = unite({polarMeas, standaloneMeas, measCommon});
	for (auto &kw : polarMeas)
		r[kw] = polarSucc;

	// CAM -> camMeas
	r[CAM] = camMeas;

	// Cam measurements -> camMeas + standaloneMeas + measCommon
	auto camSucc = unite({camMeas, standaloneMeas, measCommon});
	for (auto &kw : camMeas)
		r[kw] = camSucc;

	// Standalone measurements -> standaloneMeas + measCommon
	auto standaloneSucc = unite({standaloneMeas, measCommon});
	for (auto &kw : standaloneMeas)
		r[kw] = standaloneSucc;

	// END -> {}
	r[END] = {};

	return r;
}

// ---------------------------------------------------------------------------
// LGC1 rights
// ---------------------------------------------------------------------------
inline std::map<std::string, std::set<std::string>> buildLGC1Rights()
{
	std::map<std::string, std::set<std::string>> r;

	// Measurements
	const std::set<std::string> lgc1Meas = {ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END};
	for (auto &kw : {ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, ECSP, DLEV, DVER, ECHO, ECVE, ORIE, RADI})
		r[kw] = lgc1Meas;

	// Points (LGC1 measurements reachable from points)
	const std::set<std::string> lgc1PtMeas = {DTHE, DLEV, DVER, ECHO, ECVE, ORIE, RADI, DMES, ANGL, ZENI, ZENH, ECTH, DHOR, END};
	const std::set<std::string> lgc1PtTypes = {CALA, POIN, VXY, VXZ, VYZ, VZ};
	for (auto &kw : lgc1PtTypes)
	{
		auto s = unite({lgc1PtMeas, lgc1PtTypes});
		s.erase(kw);
		r[kw] = s;
	}
	r[PDOR] = unite({lgc1PtMeas, lgc1PtTypes});

	// Options (LGC1)
	const std::set<std::string> lgc1OptCommon = {DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, FMTP, NODUP, SOBS, HIST, EREL};
	const std::set<std::string> lgc1OptExtra = {MICR, CLIC, DIXI, TOL};
	const std::set<std::string> lgc1PtAndRef = {CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR};

	// Large option group: NOSPC + common + extra + pt&ref
	auto lgc1OptLargeBase = unite({{NOSPC}, lgc1OptCommon, lgc1OptExtra, lgc1PtAndRef});
	for (auto &kw : {DEFA, APRI, PRES, FAUT, PUNC, FMTP, NODUP, SOBS, HIST, EREL, NOSPC})
	{
		auto s = lgc1OptLargeBase;
		s.erase(kw);
		r[kw] = s;
	}

	// Smaller option group (PREC, MICR, CLIC, DIXI, TOL): no extra (MICR/CLIC/DIXI/TOL), no PREC
	auto lgc1OptSmallBase = unite({{NOSPC}, lgc1OptCommon, lgc1PtAndRef});
	lgc1OptSmallBase.erase(PREC);
	for (auto &kw : {PREC, MICR, CLIC, DIXI, TOL})
		r[kw] = lgc1OptSmallBase;

	// SIMU, LIBR, ALLFIXED (no self, no counterpart where applicable)
	auto lgc1SimuBase = unite({{NOSPC}, lgc1OptCommon, lgc1OptExtra, lgc1PtAndRef});
	lgc1SimuBase.erase(SIMU);
	lgc1SimuBase.erase(ALLFIXED);
	r[SIMU] = lgc1SimuBase;

	auto lgc1LibrBase = unite({{NOSPC}, lgc1OptCommon, lgc1OptExtra, lgc1PtAndRef});
	lgc1LibrBase.erase(LIBR);
	lgc1LibrBase.erase(ALLFIXED);
	r[LIBR] = lgc1LibrBase;

	auto lgc1AllfixedBase = unite({{NOSPC}, lgc1OptCommon, lgc1OptExtra, lgc1PtAndRef});
	lgc1AllfixedBase.erase(ALLFIXED);
	lgc1AllfixedBase.erase(SIMU);
	lgc1AllfixedBase.erase(LIBR);
	r[ALLFIXED] = lgc1AllfixedBase;

	// Referentials (LGC1): no INSTR, no FRAME, no ERELFRAME, no COVAR/CHABA/CONSI/JSON/LM, no referentials
	auto lgc1RefSucc = unite({lgc1OptCommon, lgc1OptExtra, minus(lgc1PtAndRef, referentials)});
	for (auto &kw : referentials)
		r[kw] = lgc1RefSucc;

	return r;
}

// ---------------------------------------------------------------------------
// Accessors with static caching
// ---------------------------------------------------------------------------
inline const std::map<std::string, std::set<std::string>> &lgc2RightSets()
{
	static const auto rights = buildLGC2Rights();
	return rights;
}

inline const std::map<std::string, std::set<std::string>> &lgc1RightSets()
{
	static const auto rights = buildLGC1Rights();
	return rights;
}

inline const std::set<std::string> &lgc2Rights(const std::string &kw) { return lgc2RightSets().at(kw); }
inline const std::set<std::string> &lgc1Rights(const std::string &kw) { return lgc1RightSets().at(kw); }

} // namespace KeywordRights

#endif
