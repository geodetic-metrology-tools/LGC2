#ifndef _READERS_OPTIONREADERS_H_
#define _READERS_OPTIONREADERS_H_

#include "ReaderBase.h"


//////////////////
// Referentiels //
//////////////////

class TOptionKey: public TAKeyWord {
	public:
		//Constructor
		TOptionKey(TLGCData& project, const std::string& key) : TAKeyWord(key, project),fconfig(project.cfg) {}

	protected:
		TLGCConfig& fconfig;
		TOptionKey& operator=(const TOptionKey&);
};

class TKeyOLOC : public TOptionKey {
	public:
		//Constructor
		TKeyOLOC(TLGCData& project) : TOptionKey(project, "OLOC") {}

		//Tokenize and initialize options for the referential
		virtual void parse(const std::vector<const std::string>&, int);
};

class TKeyRS2K : public TOptionKey {
	public:
		//Constructor
		TKeyRS2K(TLGCData& project) : TOptionKey(project, "RS2K") {}

		//Tokenize and initialize options for the referential
		virtual void parse(const std::vector<const std::string>&, int);
};

class TKeyLEP : public TOptionKey {
	public:
		//Constructor
		TKeyLEP(TLGCData& project) : TOptionKey(project, "LEP") {}

		//Tokenize and initialize options for the referential
		virtual void parse(const std::vector<const std::string>&, int);
};

class TKeySPHE : public TOptionKey {
	public:
		//Constructor
		TKeySPHE(TLGCData& project) : TOptionKey(project, "SPHE") {}

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
		TKeyFAUT(TLGCData& project) : TOptionKey(project, "FAUT") {}

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
