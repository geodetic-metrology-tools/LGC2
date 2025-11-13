/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _READERS_OPTIONREADERS_H_
#define _READERS_OPTIONREADERS_H_

// LGC
#include <KeywordRights.h>
#include <ReaderBase.h>

//////////////////
// Referentiels //
//////////////////

/*!
 *  \addtogroup OptionReaders
 *  @{
 */

/*!
	\brief This is an abstract base class for all the options, which can appear in the input file, e.g. OLOC, RS2K, ALLFIXED, PUNC, PLOT,...
*/
class TAOptionKey : public TAKeyWord
{
public:
	/// Constructor
	TAOptionKey(TLGCData &project, const std::string &key) : TAKeyWord(key, project), fconfig(project.getConfig()) {}

protected:
	TLGCConfig &fconfig;
	TAOptionKey &operator=(const TAOptionKey &);
};

class TKeyTITR : public TAKeyWord
{
public:
	/// Constructor
	TKeyTITR(TLGCData &project, int nb_allowed_keywords = nb_allowed_titr, const char **keywords = allowed_TITR);

	/*!
		Titr does not parse anything, just a marker to move up the tree again and report.
		\throws runtime_error if the following lines are wrong
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyOLOC : public TAOptionKey
{
public:
	// Constructor
	TKeyOLOC(TLGCData &project, int nb_allowed_keywords = nb_allowed_oloc, const char **keywords = allowed_OLOC) : TAOptionKey(project, OLOC)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the referential
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyRS2K : public TAOptionKey
{
public:
	/// Constructor
	TKeyRS2K(TLGCData &project, int nb_allowed_keywords = nb_allowed_rs2k, const char **keywords = allowed_RS2K) : TAOptionKey(project, RS2K)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the referential
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyLEP : public TAOptionKey
{
public:
	/// Constructor
	TKeyLEP(TLGCData &project, int nb_allowed_keywords = nb_allowed_lep, const char **keywords = allowed_LEP) : TAOptionKey(project, LEP)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the referential
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeySPHE : public TAOptionKey
{
public:
	/// Constructor
	TKeySPHE(TLGCData &project, int nb_allowed_keywords = nb_allowed_sphe, const char **keywords = allowed_SPHE) : TAOptionKey(project, SPHE)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the referential
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

//////////////////
// Calc Options //
//////////////////

class TKeyALLFIXED : public TAOptionKey
{
public:
	/// Constructor
	TKeyALLFIXED(TLGCData &project, int nb_allowed_keywords = nb_allowed_allfixed, const char **keywords = allowed_ALLFIXED) : TAOptionKey(project, ALLFIXED)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyLIBR : public TAOptionKey
{
public:
	/// Constructor
	TKeyLIBR(TLGCData &project, int nb_allowed_keywords = nb_allowed_libr, const char **keywords = allowed_LIBR) : TAOptionKey(project, "LIBR")
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyCOVAR : public TAOptionKey
{
public:
	/// Constructor
	TKeyCOVAR(TLGCData &project, int nb_allowed_keywords = nb_allowed_covar, const char **keywords = allowed_COVAR) : TAOptionKey(project, COVAR)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyCHABA : public TAOptionKey
{
public:
	/// Constructor
	TKeyCHABA(TLGCData &project, int nb_allowed_keywords = nb_allowed_chaba, const char **keywords = allowed_CHABA) : TAOptionKey(project, CHABA)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyNODUP : public TAOptionKey
{
public:
	/// Constructor
	TKeyNODUP(TLGCData &project, int nb_allowed_keywords = nb_allowed_nodup, const char **keywords = allowed_NODUP) : TAOptionKey(project, "NODUP")
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyPDOR : public TAOptionKey
{
public:
	/// Constructor
	TKeyPDOR(TLGCData &project, int nb_allowed_keywords = nb_allowed_pdor, const char **keywords = allowed_PDOR) : TAOptionKey(project, PDOR)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeySIMU : public TAOptionKey
{
public:
	/// Constructor
	TKeySIMU(TLGCData &project, int nb_allowed_keywords = nb_allowed_simu, const char **keywords = allowed_SIMU) : TAOptionKey(project, SIMU)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

////////////////////
// Output Options //
////////////////////

class TKeyAPRI : public TAOptionKey
{
public:
	/// Constructor
	TKeyAPRI(TLGCData &project, int nb_allowed_keywords = nb_allowed_apri, const char **keywords = allowed_APRI) : TAOptionKey(project, APRI)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyEREL : public TAOptionKey
{
public:
	/// Constructor
	TKeyEREL(TLGCData &project, int nb_allowed_keywords = nb_allowed_erel, const char **keywords = allowed_EREL) : TAOptionKey(project, EREL)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);

private:
	bool erelActive{false};
};

class TKeyERELFRAME : public TAOptionKey
{
public:
	/// Constructor
	TKeyERELFRAME(TLGCData &project, int nb_allowed_keywords = nb_allowed_erel, const char **keywords = allowed_ERELFRAME) : TAOptionKey(project, ERELFRAME)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);

private:
	bool erelFrameActive{false};
};

struct TKeyFMTxHelper
{
	TKeyFMTxHelper(TLGCConfig::TCustomOutputSep &co, const std::vector<std::string> &tokens, bool lineActive);
};

class TKeyFMTO : public TAOptionKey
{
public:
	/// Constructor
	TKeyFMTO(TLGCData &project) : TAOptionKey(project, "FMTO") {}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyFMTP : public TAOptionKey
{
public:
	/// Constructor
	TKeyFMTP(TLGCData &project, int nb_allowed_keywords = nb_allowed_fmtp, const char **keywords = allowed_FMTP) : TAOptionKey(project, FMTP)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyHIST : public TAOptionKey
{
public:
	/// Constructor
	TKeyHIST(TLGCData &project, int nb_allowed_keywords = nb_allowed_hist, const char **keywords = allowed_HIST) : TAOptionKey(project, HIST)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};
class TKeyCONSI : public TAOptionKey
{
public:
	/// Constructor
	TKeyCONSI(TLGCData &project, int nb_allowed_keywords = nb_allowed_cons_check, const char **keywords = allowed_CONSI) : TAOptionKey(project, CONSI)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};
class TKeyLM: public TAOptionKey
{
public:
	/// Constructor
	TKeyLM(TLGCData &project, int nb_allowed_keywords = nb_allowed_lm, const char **keywords = allowed_LM) : TAOptionKey(project, LM)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyJSON : public TAOptionKey
{
public:
	/// Constructor
	TKeyJSON(TLGCData &project, int nb_allowed_keywords = nb_allowed_json, const char **keywords = allowed_JSON) : TAOptionKey(project, JSON)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyPREC : public TAOptionKey
{
public:
	/// Constructor
	TKeyPREC(TLGCData &project, int nb_allowed_keywords = nb_allowed_prec, const char **keywords = allowed_PREC) : TAOptionKey(project, PREC)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyMICR : public TAOptionKey
{
public:
	/// Constructor
	TKeyMICR(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAOptionKey(project, MICR)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyCLIC : public TAOptionKey
{
public:
	/// Constructor
	TKeyCLIC(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAOptionKey(project, CLIC)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyDIXI : public TAOptionKey
{
public:
	/// Constructor
	TKeyDIXI(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAOptionKey(project, DIXI)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyTOL : public TAOptionKey
{
public:
	/// Constructor
	TKeyTOL(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAOptionKey(project, TOL)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyNOSPC : public TAOptionKey
{
public:
	/// Constructor
	TKeyNOSPC(TLGCData &project, int nb_allowed_keywords, const char **keywords) : TAOptionKey(project, NOSPC)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyPRES : public TAOptionKey
{
public:
	/// Constructor
	TKeyPRES(TLGCData &project, int nb_allowed_keywords = nb_allowed_pres, const char **keywords = allowed_PRES) : TAOptionKey(project, PRES)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

/////////////////////////////
// Additional Output Files //
/////////////////////////////

class TKeyDEFA : public TAOptionKey
{
public:
	/// Constructor
	TKeyDEFA(TLGCData &project, int nb_allowed_keywords = nb_allowed_defa, const char **keywords = allowed_DEFA) : TAOptionKey(project, DEFA)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyFAUT : public TAOptionKey
{
public:
	/// Constructor
	TKeyFAUT(TLGCData &project, int nb_allowed_keywords = nb_allowed_faut, const char **keywords = allowed_FAUT) : TAOptionKey(project, FAUT)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

struct DiffOutHelper
{
	DiffOutHelper(TLGCConfig::TCoordOut &out, const std::string &opt, const std::string &key);
};

class TKeyPUNC : public TAOptionKey
{
public:
	/// Constructor
	TKeyPUNC(TLGCData &project, int nb_allowed_keywords = nb_allowed_punc, const char **keywords = allowed_PUNC) : TAOptionKey(project, PUNC)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyPLOT : public TAOptionKey
{
public:
	/// Constructor
	TKeyPLOT(TLGCData &project) : TAOptionKey(project, "PLOT") {}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeySOBS : public TAOptionKey
{
public:
	/// Constructor
	TKeySOBS(TLGCData &project, int nb_allowed_keywords = nb_allowed_sobs, const char **keywords = allowed_SOBS) : TAOptionKey(project, SOBS)
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

/*! @} End of Doxygen Groups*/
#endif
