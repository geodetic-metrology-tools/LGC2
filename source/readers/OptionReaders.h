#ifndef _READERS_OPTIONREADERS_H_
#define _READERS_OPTIONREADERS_H_

#include "ReaderBase.h"
#include "KeywordRights.h"


//////////////////
// Referentiels //
//////////////////

class TOptionKey: public TAKeyWord {
	public:
		//Constructor
		TOptionKey(TLGCData& project, const std::string& key) : TAKeyWord(key, project),fconfig(project.getConfig()) {}

	protected:
		TLGCConfig& fconfig;
		TOptionKey& operator=(const TOptionKey&);
};

class TKeyOLOC : public TOptionKey {
	public:
		//Constructor
		TKeyOLOC(TLGCData& project, int nb_allowed_keywords = nb_allowed_oloc, const char** keywords = allowed_OLOC) : TOptionKey(project, OLOC)
		{			
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//Tokenize and initialize options for the referential
		virtual void parse(const std::vector<const std::string>&, int);
};

class TKeyRS2K : public TOptionKey {
	public:
		//Constructor
		TKeyRS2K(TLGCData& project, int nb_allowed_keywords = nb_allowed_rs2k, const char** keywords = allowed_RS2K) : TOptionKey(project, RS2K)
		{			
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//Tokenize and initialize options for the referential
		virtual void parse(const std::vector<const std::string>&, int);
};

class TKeyLEP : public TOptionKey {
	public:
		//Constructor
		TKeyLEP(TLGCData& project, int nb_allowed_keywords = nb_allowed_lep, const char** keywords = allowed_LEP) : TOptionKey(project, LEP)
		{			
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//Tokenize and initialize options for the referential
		virtual void parse(const std::vector<const std::string>&, int);
};

class TKeySPHE : public TOptionKey {
	public:
		//Constructor
		TKeySPHE(TLGCData& project, int nb_allowed_keywords = nb_allowed_sphe, const char** keywords = allowed_SPHE) : TOptionKey(project, SPHE)
		{			
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//Tokenize and initialize options for the referential
		virtual void parse(const std::vector<const std::string>&, int);
};



//////////////////
// Calc Options //
//////////////////

class TKeyALLFIXED : public TOptionKey {
	public:
		//Constructor
		TKeyALLFIXED(TLGCData& project) : TOptionKey(project, "ALLFIXED") {}

		//Tokenize and initialize options for the calculation
		virtual void parse(const std::vector<const std::string>&, int);
};

class TKeyLIBR : public TOptionKey {
	public:
		//Constructor
		TKeyLIBR(TLGCData& project) : TOptionKey(project, "LIBR") {}

		//Tokenize and initialize options for the calculation
		virtual void parse(const std::vector<const std::string>&, int);
};

class TKeyNODUP : public TOptionKey {
	public:
		//Constructor
		TKeyNODUP(TLGCData& project) : TOptionKey(project, "NODUP") {}

		//Tokenize and initialize options for the calculation
		virtual void parse(const std::vector<const std::string>&, int);
};

class TKeyPDOR : public TOptionKey {
	public:
		//Constructor
		TKeyPDOR(TLGCData& project) : TOptionKey(project, "PDOR") {}

		//Tokenize and initialize options for the calculation
		virtual void parse(const std::vector<const std::string>& tokens, int);
};

class TKeySIMU : public TOptionKey {
	public:
		//Constructor
		TKeySIMU(TLGCData& project) : TOptionKey(project, "SIMU") {}

		//Tokenize and initialize options for the calculation
		virtual void parse(const std::vector<const std::string>& tokens, int);
};



////////////////////
// Output Options //
////////////////////

class TKeyAPRI : public TOptionKey {
	public:
		//Constructor
		TKeyAPRI(TLGCData& project) : TOptionKey(project, "APRI") {}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<const std::string>&, int);
};

class TKeyEREL : public TOptionKey {
	public:
		//Constructor
		TKeyEREL(TLGCData& project) : TOptionKey(project, "EREL") {}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<const std::string>& tokens, int);
};

struct TKeyFMTxHelper {
	TKeyFMTxHelper(TLGCConfig::TCustomOutputSep& co, const std::vector<const std::string>& tokens);
};

class TKeyFMTO : public TOptionKey {
	public:
		//Constructor
		TKeyFMTO(TLGCData& project) : TOptionKey(project, "FMTO") {}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<const std::string>& tokens, int);
};

class TKeyFMTP : public TOptionKey {
	public:
		//Constructor
		TKeyFMTP(TLGCData& project) : TOptionKey(project, "FMTP") {}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<const std::string>& tokens, int);
};

class TKeyHIST : public TOptionKey {
	public:
		//Constructor
		TKeyHIST(TLGCData& project) : TOptionKey(project, "HIST") {}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<const std::string>&, int);
};

class TKeyPREC : public TOptionKey {
	public:
		//Constructor
		TKeyPREC(TLGCData& project) : TOptionKey(project, "PREC") {}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<const std::string>& tokens, int);
};

class TKeyPRES : public TOptionKey {
	public:
		//Constructor
		TKeyPRES(TLGCData& project) : TOptionKey(project, "PRES") {}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<const std::string>&, int);
};



/////////////////////////////
// Additional Output Files //
/////////////////////////////

class TKeyDEFA : public TOptionKey {
	public:
		//Constructor
		TKeyDEFA(TLGCData& project) : TOptionKey(project, "DEFA") {}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<const std::string>&, int);
};

class TKeyFAUT : public TOptionKey {
	public:
		//Constructor
		TKeyFAUT(TLGCData& project, int nb_allowed_keywords = nb_allowed_faut, const char** keywords = allowed_FAUT) : TOptionKey(project, FAUT)
		{			
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<const std::string>& tokens, int);
};

struct DiffOutHelper {
	DiffOutHelper(TLGCConfig::TCoordOut& out, const std::string& opt, const std::string& key);
};

class TKeyPUNC : public TOptionKey {
	public:
		//Constructor
		TKeyPUNC(TLGCData& project) : TOptionKey(project, "PUNC") {}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<const std::string>& tokens, int);
};

class TKeyPLOT : public TOptionKey {
	public:
		//Constructor
		TKeyPLOT(TLGCData& project) : TOptionKey(project, "PLOT") {}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<const std::string>& tokens, int);
};

class TKeySOBS : public TOptionKey {
	public:
		//Constructor
		TKeySOBS(TLGCData& project) : TOptionKey(project, "SOBS") {}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<const std::string>& tokens, int);
};

#endif
