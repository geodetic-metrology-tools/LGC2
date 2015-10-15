#ifndef _READERS_INSTRUMENTREADERS_H_
#define _READERS_INSTRUMENTREADERS_H_

#include <TInstrumentData.h>
#include "ReaderBase.h"
#include "Global.h"


namespace {
	template<typename T>
	void checkInstrument(int expectedTokens, T stations,
		                 const std::vector<const std::string>& tokens) {
		int numTokens = tokens.size();

		if(numTokens != expectedTokens)
			throw std::runtime_error("Keyword *" + tokens.at(1) + " expects " + std::to_string(expectedTokens-2) + " parameters.");
		
		// check if station exists
		if (stations.find(tokens.at(2)) != stations.end())
			throw std::runtime_error("Instrument " + tokens.at(2) + " already exists.");
	}
	template<typename T>
	void checkTarget(int expectedTokens, T targets,
		             const std::vector<const std::string>& tokens) {
		int numTokens = tokens.size();

		if(numTokens != expectedTokens)
			throw std::runtime_error("This target type expects " + std::to_string(expectedTokens) +  " parameters.");
				
		// check if target exists
		if (targets.find(tokens.at(0)) != targets.end())
			throw std::runtime_error("Target " + tokens.at(0) + " already exists.");
	}
}

// Abract class for the instrument keyword
class TAKeyINSTR : public TAKeyWord {
	public:
		/*! Constructor. 
		\ Initialize instrument, scalars and angles according to the instrument type
		\param key is the type of the instrument 
		\param project  is the container for the data 
		*/
		TAKeyINSTR(TLGCData& project, const std::string& key);

	protected:
		std::string currentStation; /*!< Name of the current station. */
		TInstrumentData& finstruments; /*!< Instrument data. */
		TAdjustableScalarCollection  &fscalars; /*!< Distance's collection. */
		TAdjustableAngleCollection  &fangles; /*!< Angle's collection. */

	private:
		// Overloading 
		TAKeyINSTR& operator=(const TAKeyINSTR&); 
};

class TKeyINSTR : public TAKeyWord {
	public:
		// Constructor
		TKeyINSTR(TLGCData& project);

		/*! virtual function. 
		\ To parse the instrument data: store the instrument data in the right units
		\ extract, check and store the station data 
		\ extract, check and store the target data 
		*/
		virtual void parse(const std::vector<const std::string>&, int);
};


/*!
 *  \addtogroup MeasurementReaders
 *  @{
 */
class TKeyPOLAR : public TAKeyINSTR {
	public:
		/*! 
			See \ref TAKeyINSTR::TAKeyINSTR
		*/
		TKeyPOLAR(TLGCData& project);

		/*! 
			See \ref TKeyINSTR::parse
		*/		
		virtual void parse(const std::vector<const std::string>& tokens, int);

		virtual const std::vector<const std::string>& parentKeys() const;

	private:
		TKeyPOLAR& operator=(const TKeyPOLAR&);
};

class TKeyEDM : public TAKeyINSTR {
	public:
		/*! 
			See \ref TAKeyINSTR::TAKeyINSTR
		*/
		TKeyEDM(TLGCData& project);

		/*! 
			See \ref TKeyINSTR::parse
		*/	
		virtual void parse(const std::vector<const std::string>& tokens, int);


		virtual const std::vector<const std::string>& parentKeys() const;
		
	private:
		TKeyEDM& operator=(const TKeyEDM&);
};

class TKeyLEVEL : public TAKeyINSTR {
	public:
		/*! 
			See \ref TAKeyINSTR::TAKeyINSTR
		*/
		TKeyLEVEL(TLGCData& project);

		/*! 
			See \ref TKeyINSTR::parse
		*/	
		virtual void parse(const std::vector<const std::string>& tokens, int); 

		virtual const std::vector<const std::string>& parentKeys() const;

	private:
		TKeyLEVEL& operator=(const TKeyLEVEL&);
};

class TKeySCALE : public TAKeyINSTR {
	public:
		/*! 
			See \ref TAKeyINSTR::TAKeyINSTR
		*/
		TKeySCALE(TLGCData& project);

		/*! 
			See \ref TKeyINSTR::parse
		*/	
		virtual void parse(const std::vector<const std::string>& tokens, int);

		virtual const std::vector<const std::string>& parentKeys() const;

	private:
		TKeySCALE& operator=(const TKeySCALE&);
};
/*! @} End of Doxygen Groups*/

#endif
