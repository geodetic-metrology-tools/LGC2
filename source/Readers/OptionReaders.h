#ifndef _READERS_OPTIONREADERS_H_
#define _READERS_OPTIONREADERS_H_

#include "ReaderBase.h"
#include "KeywordRights.h"


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
class TAOptionKey: public TAKeyWord {
	public:
		/// Constructor
		TAOptionKey(TLGCData& project, const std::string& key) : TAKeyWord(key, project),fconfig(project.getConfig()) {}

	protected:
		TLGCConfig& fconfig;
		TAOptionKey& operator=(const TAOptionKey&);
};

class TKeyTITR : public TAKeyWord {
	public:
		/// Constructor
		TKeyTITR(TLGCData& project, int nb_allowed_keywords = nb_allowed_titr, const char** keywords = allowed_TITR);
				
		/*!
			Titr does not parse anything, just a marker to move up the tree again and report.
			\throws runtime_error if the following lines are wrong
		*/
		virtual void parse(const std::vector<std::string>& tokens, int);
};

class TKeyOLOC : public TAOptionKey {
	public:
		//Constructor
		TKeyOLOC(TLGCData& project, int nb_allowed_keywords = nb_allowed_oloc, const char** keywords = allowed_OLOC) : TAOptionKey(project, OLOC)
		{			
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/// Tokenize and initialize options for the referential
		virtual void parse(const std::vector<std::string>&, int);
};

class TKeyRS2K : public TAOptionKey {
	public:
		///Constructor
		TKeyRS2K(TLGCData& project, int nb_allowed_keywords = nb_allowed_rs2k, const char** keywords = allowed_RS2K) : TAOptionKey(project, RS2K)
		{			
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		///Tokenize and initialize options for the referential
		virtual void parse(const std::vector<std::string>&, int);
};

class TKeyLEP : public TAOptionKey {
	public:
		///Constructor
		TKeyLEP(TLGCData& project, int nb_allowed_keywords = nb_allowed_lep, const char** keywords = allowed_LEP) : TAOptionKey(project, LEP)
		{			
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		///Tokenize and initialize options for the referential
		virtual void parse(const std::vector<std::string>&, int);
};

class TKeySPHE : public TAOptionKey {
	public:
		///Constructor
		TKeySPHE(TLGCData& project, int nb_allowed_keywords = nb_allowed_sphe, const char** keywords = allowed_SPHE) : TAOptionKey(project, SPHE)
		{			
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		///Tokenize and initialize options for the referential
		virtual void parse(const std::vector<std::string>&, int);
};



//////////////////
// Calc Options //
//////////////////

class TKeyALLFIXED : public TAOptionKey {
	public:
		///Constructor
		TKeyALLFIXED(TLGCData& project) : TAOptionKey(project, "ALLFIXED") {}

		///Tokenize and initialize options for the calculation
		virtual void parse(const std::vector<std::string>&, int);
};

class TKeyLIBR : public TAOptionKey {
	public:
		///Constructor
		TKeyLIBR(TLGCData& project) : TAOptionKey(project, "LIBR") {}

		///Tokenize and initialize options for the calculation
		virtual void parse(const std::vector<std::string>&, int);
};

class TKeyNODUP : public TAOptionKey {
	public:
		///Constructor
		TKeyNODUP(TLGCData& project) : TAOptionKey(project, "NODUP") {}

		///Tokenize and initialize options for the calculation
		virtual void parse(const std::vector<std::string>&, int);
};

class TKeyPDOR : public TAOptionKey {
	public:
		///Constructor
		TKeyPDOR(TLGCData& project, int nb_allowed_keywords = nb_allowed_pdor, const char** keywords = allowed_PDOR) : TAOptionKey(project, PDOR) 
		{
			for (int i(0); i< nb_allowed_keywords; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		///Tokenize and initialize options for the calculation
		virtual void parse(const std::vector<std::string>& tokens, int);
};

class TKeySIMU : public TAOptionKey {
	public:
		///Constructor
		TKeySIMU(TLGCData& project, int nb_allowed_keywords = nb_allowed_simu, const char** keywords = allowed_SIMU) : TAOptionKey(project, SIMU)
		{			
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		///Tokenize and initialize options for the calculation
		virtual void parse(const std::vector<std::string>& tokens, int);
};



////////////////////
// Output Options //
////////////////////

class TKeyAPRI : public TAOptionKey {
	public:
		///Constructor
		TKeyAPRI(TLGCData& project, int nb_allowed_keywords = nb_allowed_apri, const char** keywords = allowed_APRI) : TAOptionKey(project, APRI) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);

		}

		///Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<std::string>&, int);
};

class TKeyEREL : public TAOptionKey {
	public:
		///Constructor
		TKeyEREL(TLGCData& project) : TAOptionKey(project, "EREL") {}

		///Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<std::string>& tokens, int);
};

struct TKeyFMTxHelper {
	TKeyFMTxHelper(TLGCConfig::TCustomOutputSep& co, const std::vector<std::string>& tokens);
};

class TKeyFMTO : public TAOptionKey {
	public:
		///Constructor
		TKeyFMTO(TLGCData& project) : TAOptionKey(project, "FMTO") {}

		///Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<std::string>& tokens, int);
};

class TKeyFMTP : public TAOptionKey {
	public:
		///Constructor
		TKeyFMTP(TLGCData& project) : TAOptionKey(project, "FMTP") {}

		//Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<std::string>& tokens, int);
};

class TKeyHIST : public TAOptionKey {
	public:
		///Constructor
		TKeyHIST(TLGCData& project) : TAOptionKey(project, "HIST") {}

		///Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<std::string>&, int);
};

class TKeyPREC : public TAOptionKey {
	public:
		///Constructor
		TKeyPREC(TLGCData& project, int nb_allowed_keywords = nb_allowed_prec, const char** keywords = allowed_PREC) : TAOptionKey(project, PREC) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);

		}

		///Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<std::string>& tokens, int);
};

class TKeyPRES : public TAOptionKey {
	public:
		///Constructor
		TKeyPRES(TLGCData& project) : TAOptionKey(project, "PRES") {}

		///Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<std::string>&, int);
};



/////////////////////////////
// Additional Output Files //
/////////////////////////////

class TKeyDEFA : public TAOptionKey {
	public:
		///Constructor
		TKeyDEFA(TLGCData& project) : TAOptionKey(project, "DEFA") {}

		///Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<std::string>&, int);
};

class TKeyFAUT : public TAOptionKey {
	public:
		///Constructor
		TKeyFAUT(TLGCData& project, int nb_allowed_keywords = nb_allowed_faut, const char** keywords = allowed_FAUT) : TAOptionKey(project, FAUT)
		{			
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		///Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<std::string>& tokens, int);
};

struct DiffOutHelper {
	DiffOutHelper(TLGCConfig::TCoordOut& out, const std::string& opt, const std::string& key);
};

class TKeyPUNC : public TAOptionKey {
	public:
		///Constructor
		TKeyPUNC(TLGCData& project, int nb_allowed_keywords = nb_allowed_punc, const char** keywords = allowed_PUNC) : TAOptionKey(project, PUNC)
		{			
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		///Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<std::string>& tokens, int);
};

class TKeyPLOT : public TAOptionKey {
	public:
		///Constructor
		TKeyPLOT(TLGCData& project) : TAOptionKey(project, "PLOT") {}

		///Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<std::string>& tokens, int);
};

class TKeySOBS : public TAOptionKey {
	public:
		///Constructor
		TKeySOBS(TLGCData& project) : TAOptionKey(project, "SOBS") {}

		///Tokenize and initialize options for the output processing
		virtual void parse(const std::vector<std::string>& tokens, int);
};

/*! @} End of Doxygen Groups*/
#endif
