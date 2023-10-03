/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _READERBASE_H_
#define _READERBASE_H_

//STL
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
//LGC
#include <TLGCConfig.h>
#include <LGCAdjustableObjectCollection.h>
#include <TLGCData.h>

/*! 
	\ingroup InputFileReader
	\brief Abstract class for all the keywords.
*/
class TAKeyWord {
	public:
		/// The keyword to which the object reacts.
		const std::string key;

		/*!@name Construction/Destruction */
		//@{
			/// Constructor
			TAKeyWord(const std::string& word, TLGCData& project) :
				key(word),
				fparent(nullptr),
				proj(project) 
			{}

			/// Destructor
			virtual ~TAKeyWord() {}

		//@}

		/** React to a tokenized input line. Blanks and tabs are removed from the input 
		* and keyword markers (*) are a separate token. 
		* A comment symbol (%$) causes the end of tokenization, so comments are 
		* not tokenized and the comment symbol is included.
		*
		* Example:
		*   \t *DVER     P1 P2   \t  5 /1.2 $ This is a  DB_COMMENT
		* leads to
		* [*][DVER][P1][P2][5][/1.2][$ This is a  DB_COMMENT]
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line) = 0;

		/*!@name Currently not used functions - these functions are related to hierarchy of keywords, which was originally considered,
		but was later replaced by a list of prohibited keywords. This matter is still not completely resolved and therefore
		these methods still remain there if needed in the future
		*/
		//@{
		inline bool isChild(const std::string& keyword) {
			return std::find_if(fsiblings.cbegin(), fsiblings.cend(), [&](const TAKeyWord* kw) {
				return kw->key == keyword;
			}) != fsiblings.cend();
		}

		inline TAKeyWord& addChild(const TAKeyWord* sibling) {
			fsiblings.emplace_back(sibling);
			return *this;
		}
		
		inline bool hasParent() {
			return fparent != nullptr;
		}

		inline void setParent(TAKeyWord* parent) {
			assert(parent != nullptr);
			fparent = parent;
		}

		inline TAKeyWord* getParent() {
			if (fparent == nullptr)
				throw std::logic_error("Keyword " + key + " has no parent.");
			return fparent;
		}
		//@}

		std::string getKey() const {return key;} 
		
		/// This function returns true if the measurment key is allowed to appear after the current one, in the input file.
		inline bool isKeyWordAllowed(std::string keyword) const
		{
			if(allowed_keywords.size())
				return (std::find_if(allowed_keywords.cbegin(), allowed_keywords.cend(), [&](const std::string& kw) { return kw == keyword; }) 
						!= allowed_keywords.cend());
			return false;
		}

	private:
		TAKeyWord* fparent;
		std::vector<const TAKeyWord*> fsiblings;

	protected:
		TAKeyWord& operator=(const TAKeyWord&);

		/// Reference to the current project, used in all subclasses
		TLGCData&   proj;	

		/// Contains the keywords which are unauthorized after the current one.
		std::vector<std::string> allowed_keywords;
};


/*!
	\ingroup InputFileReader
	\brief Helps finding flags and options in tokenized lines. Provides functions to check for flags
	and extract parameters as a one-liner in the parsers.
*/
class TOptionHelper {

	public:
		TOptionHelper(std::vector<std::string>::const_iterator begin,
			         std::vector<std::string>::const_iterator end) :
			fbegin(begin), 
			fend(end){}

		bool has(const std::string& opt) const {
			return std::find(fbegin, fend, opt) != fend;
		}

		/// return the value of keyword opt as as string
		inline const std::string& getParam(const std::string& opt) const {
			auto it = get(opt);
			auto next = it+1;

			if (next == fend)
				throw std::runtime_error("Option " + opt + " expected a parameter.");

			return *next;
		}
		
		/// extract an option as a string with a default value
		inline const std::string& getParamS(const std::string& opt, const std::string& def) const {
			if (! has(opt)) return def;

			return getParam(opt);
		}		
		/// extract an option as a real (floating point) number with a default value
		inline TReal getParamR(const std::string& opt, TReal def= 0.0) const {
			if (! has(opt)) return def;

			return std::stor(getParam(opt));
		}

		/// extract an option as a real (floating point) number with a default value, string value given in mili-meters [mm], returned value is in meters [m]
		inline TReal getParamRmm2m(const std::string& opt, TReal def= 0.0) const {
			if (! has(opt)) return def;

			return (std::stor(getParam(opt)) * MM2M);
		}

		/// extract an option as a real (floating point) number with a default value, string value given in cc, returned value is in radians [rad]
		inline TReal getParamRcc2rad(const std::string& opt, TReal def= 0.0) const {
			if (! has(opt)) return def;

			return (std::stor(getParam(opt)) * CC2RAD);
		}

		/// extract an option as a real (floating point) number with a default value, string value given in microradians [urad], returned value is in radians [rad]
		inline TReal getParamRurad2rad(const std::string &opt, TReal def = 0.0) const
		{
			if (!has(opt))
				return def;

			return (std::stor(getParam(opt)) / pow(10,6));
		}

		/// extract an option as a real (floating point) number with a default value, string value given in gradians [gon], returned value is in radians [rad]
		inline TReal getParamRgon2rad(const std::string& opt, TReal def= 0.0) const {
			if (! has(opt)) return def;

			return (std::stor(getParam(opt)) * GON2RAD);
		}

		/// extract an option as an integer number with a default value
		inline int getParamI(const std::string& opt, int def= 0) const {
			if (! has(opt)) return def;

			return std::stoi(getParam(opt));
		}

	private:
		std::vector<std::string>::const_iterator fbegin;
		std::vector<std::string>::const_iterator fend;

		std::vector<std::string>::const_iterator get(const std::string& opt) const {
			auto it = std::find(fbegin, fend, opt);
			if (it == fend)
				throw std::runtime_error("Could not find option " + opt + ".");
			return it;
		}
};

#endif
