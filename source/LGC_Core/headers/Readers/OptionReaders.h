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
	TKeyTITR(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(TITR));

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
	TKeyOLOC(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(OLOC)) : TAOptionKey(project, OLOC)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the referential
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyRS2K : public TAOptionKey
{
public:
	/// Constructor
	TKeyRS2K(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(RS2K)) : TAOptionKey(project, RS2K)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the referential
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyLEP : public TAOptionKey
{
public:
	/// Constructor
	TKeyLEP(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(LEP)) : TAOptionKey(project, LEP)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the referential
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeySPHE : public TAOptionKey
{
public:
	/// Constructor
	TKeySPHE(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(SPHE)) : TAOptionKey(project, SPHE)
	{
		allowed_keywords = allowed_kw;
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
	TKeyALLFIXED(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ALLFIXED)) : TAOptionKey(project, ALLFIXED)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyLIBR : public TAOptionKey
{
public:
	/// Constructor
	TKeyLIBR(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(LIBR)) : TAOptionKey(project, "LIBR")
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyCOVAR : public TAOptionKey
{
public:
	/// Constructor
	TKeyCOVAR(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(COVAR)) : TAOptionKey(project, COVAR)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyCHABA : public TAOptionKey
{
public:
	/// Constructor
	TKeyCHABA(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(CHABA)) : TAOptionKey(project, CHABA)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyNODUP : public TAOptionKey
{
public:
	/// Constructor
	TKeyNODUP(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(NODUP)) : TAOptionKey(project, "NODUP")
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyPDOR : public TAOptionKey
{
public:
	/// Constructor
	TKeyPDOR(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(PDOR)) : TAOptionKey(project, PDOR)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the calculation
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeySIMU : public TAOptionKey
{
public:
	/// Constructor
	TKeySIMU(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(SIMU)) : TAOptionKey(project, SIMU)
	{
		allowed_keywords = allowed_kw;
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
	TKeyAPRI(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(APRI)) : TAOptionKey(project, APRI)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyEREL : public TAOptionKey
{
public:
	/// Constructor
	TKeyEREL(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(EREL)) : TAOptionKey(project, EREL)
	{
		allowed_keywords = allowed_kw;
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
	TKeyERELFRAME(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ERELFRAME)) : TAOptionKey(project, ERELFRAME)
	{
		allowed_keywords = allowed_kw;
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
	TKeyFMTP(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(FMTP)) : TAOptionKey(project, FMTP)
	{
		allowed_keywords = allowed_kw;
	}

	// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyHIST : public TAOptionKey
{
public:
	/// Constructor
	TKeyHIST(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(HIST)) : TAOptionKey(project, HIST)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};
class TKeyCONSI : public TAOptionKey
{
public:
	/// Constructor
	TKeyCONSI(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(CONSI)) : TAOptionKey(project, CONSI)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};
class TKeyLM: public TAOptionKey
{
public:
	/// Constructor
	TKeyLM(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(LM)) : TAOptionKey(project, LM)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyJSON : public TAOptionKey
{
public:
	/// Constructor
	TKeyJSON(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(JSON)) : TAOptionKey(project, JSON)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyPREC : public TAOptionKey
{
public:
	/// Constructor
	TKeyPREC(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(PREC)) : TAOptionKey(project, PREC)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyMICR : public TAOptionKey
{
public:
	/// Constructor
	TKeyMICR(TLGCData &project, const std::set<std::string> &allowed_kw) : TAOptionKey(project, MICR)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyCLIC : public TAOptionKey
{
public:
	/// Constructor
	TKeyCLIC(TLGCData &project, const std::set<std::string> &allowed_kw) : TAOptionKey(project, CLIC)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyDIXI : public TAOptionKey
{
public:
	/// Constructor
	TKeyDIXI(TLGCData &project, const std::set<std::string> &allowed_kw) : TAOptionKey(project, DIXI)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyTOL : public TAOptionKey
{
public:
	/// Constructor
	TKeyTOL(TLGCData &project, const std::set<std::string> &allowed_kw) : TAOptionKey(project, TOL)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyNOSPC : public TAOptionKey
{
public:
	/// Constructor
	TKeyNOSPC(TLGCData &project, const std::set<std::string> &allowed_kw) : TAOptionKey(project, NOSPC)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyPRES : public TAOptionKey
{
public:
	/// Constructor
	TKeyPRES(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(PRES)) : TAOptionKey(project, PRES)
	{
		allowed_keywords = allowed_kw;
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
	TKeyDEFA(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(DEFA)) : TAOptionKey(project, DEFA)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &, bool activeLine, int);
};

class TKeyFAUT : public TAOptionKey
{
public:
	/// Constructor
	TKeyFAUT(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(FAUT)) : TAOptionKey(project, FAUT)
	{
		allowed_keywords = allowed_kw;
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
	TKeyPUNC(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(PUNC)) : TAOptionKey(project, PUNC)
	{
		allowed_keywords = allowed_kw;
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
	TKeySOBS(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(SOBS)) : TAOptionKey(project, SOBS)
	{
		allowed_keywords = allowed_kw;
	}

	/// Tokenize and initialize options for the output processing
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

/*! @} End of Doxygen Groups*/
#endif
