/*
© Copyright CERN 2000-2024. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _READERS_INSTRUMENTREADERS_H_
#define _READERS_INSTRUMENTREADERS_H_

// LGC
#include <Global.h>
#include <KeywordRights.h>
#include <ReaderBase.h>
#include <TInstrumentData.h>

namespace
{
template<typename T>
void checkInstrument(int expectedTokens, T stations, const std::vector<std::string> &tokens)
{
	int numTokens = (int)tokens.size();

	if (numTokens != expectedTokens)
		throw std::runtime_error("Keyword *" + tokens.at(1) + " expects " + std::to_string(expectedTokens - 2) + " parameters.");

	// check if station exists
	if (stations.find(tokens.at(2)) != stations.end())
		throw std::runtime_error("Instrument " + tokens.at(2) + " already exists.");
}
template<typename T>
void checkTarget(int expectedTokens, T targets, const std::vector<std::string> &tokens)
{
	int numTokens = (int)tokens.size();

	if (numTokens != expectedTokens)
		throw std::runtime_error("This target type expects " + std::to_string(expectedTokens) + " parameters.");

	// check if target exists
	if (targets.find(tokens.at(0)) != targets.end())
		throw std::runtime_error("Target " + tokens.at(0) + " already exists.");
}
} // namespace

/*!
 *  \addtogroup InstrumentReaders
 *  @{
 */
/*!
	\brief This is an abstract base class for all the instrument keywords, e.g. INSTR, POLAR, SCALE,...
*/
class TAInstrumentKey : public TAKeyWord
{
public:
	/*! Constructor.
	\ Initialize instrument, scalars and angles according to the instrument type
	\param key is the type of the instrument
	\param project  is the container for the data
	*/
	TAInstrumentKey(TLGCData &project, const std::string &key);

protected:
	std::string currentStation; /*!< Name of the current station. */
	TInstrumentData &finstruments; /*!< Instrument data. */
	TAdjustableLengthCollection &flengths; /*!< Collection of all the adjustable scalars. */
	TAdjustableAngleCollection &fangles; /*!<  Collection of all the adjustable angles. */

private:
	// Overloading
	TAInstrumentKey &operator=(const TAInstrumentKey &);
};

/// Keyword, which starts the instrument section of the input file.
class TKeyINSTR : public TAKeyWord
{
public:
	// Constructor
	TKeyINSTR(TLGCData &project, int nb_allowed_keywords = nb_allowed_instr, const char **keywords = allowed_INSTR);

	/*! virtual function.
	\brief  To parse the instrument data: store the instrument data in the right units

	\ extract, check and store the station data
	\ extract, check and store the target data
	*/
	virtual void parse(const std::vector<std::string> &, bool, int);
};

class TKeyPOLAR : public TAInstrumentKey
{
public:
	/*!
		See \ref TAInstrumentKey::TAInstrumentKey
	*/
	TKeyPOLAR(TLGCData &project, int nb_allowed_keywords = nb_allowed_polar, const char **keywords = allowed_POLAR);

	/*!
		See \ref TKeyINSTR::parse
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);

	virtual const std::vector<std::string> &parentKeys() const;

private:
	TKeyPOLAR &operator=(const TKeyPOLAR &);
};

class TKeyCAMD : public TAInstrumentKey
{
public:
	/*!
		See \ref TAInstrumentKey::TAInstrumentKey
	*/
	TKeyCAMD(TLGCData &project, int nb_allowed_keywords = nb_allowed_camd, const char **keywords = allowed_CAMD);

	/*!
		See \ref TKeyINSTR::parse
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);

	virtual const std::vector<std::string> &parentKeys() const;

private:
	TKeyPOLAR &operator=(const TKeyPOLAR &);
};

class TKeyEDM : public TAInstrumentKey
{
public:
	/*!
		See \ref TAInstrumentKey::TAInstrumentKey
	*/
	TKeyEDM(TLGCData &project, int nb_allowed_keywords = nb_allowed_edm, const char **keywords = allowed_EDM);

	/*!
		See \ref TKeyINSTR::parse
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);

	virtual const std::vector<std::string> &parentKeys() const;

private:
	TKeyEDM &operator=(const TKeyEDM &);
};

class TKeyLEVEL : public TAInstrumentKey
{
public:
	/*!
		See \ref TAInstrumentKey::TAInstrumentKey
	*/
	TKeyLEVEL(TLGCData &project, int nb_allowed_keywords = nb_allowed_level, const char **keywords = allowed_LEVEL);

	/*!
		See \ref TKeyINSTR::parse
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);

	virtual const std::vector<std::string> &parentKeys() const;

private:
	TKeyLEVEL &operator=(const TKeyLEVEL &);
};

class TKeySCALE : public TAInstrumentKey
{
public:
	/*!
		See \ref TAInstrumentKey::TAInstrumentKey
	*/
	TKeySCALE(TLGCData &project, int nb_allowed_keywords = nb_allowed_scale, const char **keywords = allowed_SCALE);

	/*!
		See \ref TKeyINSTR::parse
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);

	virtual const std::vector<std::string> &parentKeys() const;

private:
	TKeySCALE &operator=(const TKeySCALE &);
};

class TKeyINCL : public TAInstrumentKey
{
public:
	/*!
		See \ref TAInstrumentKey::TAInstrumentKey
	*/
	TKeyINCL(TLGCData &project, int nb_allowed_keywords = nb_allowed_incl, const char **keywords = allowed_INCL);

	// overloading
	TKeyINCL(const TKeyINCL &) = delete;
	TKeyINCL &operator=(const TKeyINCL &) = delete;

	TKeyINCL(TKeyINCL &&) = default;
	TKeyINCL &operator=(TKeyINCL &&) = default;

	/*!
		See \ref TKeyINSTR::parse
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);

	virtual const std::vector<std::string> &parentKeys() const;
};

class TKeyHLSR : public TAInstrumentKey
{
public:
	/*!
		See \ref TAInstrumentKey::TAInstrumentKey
	*/
	TKeyHLSR(TLGCData &project, int nb_allowed_keywords = nb_allowed_hlsr, const char **keywords = allowed_HLSR);

	/*!
		See \ref TKeyINSTR::parse
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);

	virtual const std::vector<std::string> &parentKeys() const;

private:
	TKeyHLSR &operator=(const TKeyHLSR &);
};

class TKeyWPSR : public TAInstrumentKey
{
public:
	/*!
		See \ref TAInstrumentKey::TAInstrumentKey
	*/
	TKeyWPSR(TLGCData &project, int nb_allowed_keywords = nb_allowed_wpsr, const char **keywords = allowed_WPSR);

	/*!
		See \ref TKeyINSTR::parse
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);

	virtual const std::vector<std::string> &parentKeys() const;

private:
	TKeyWPSR &operator=(const TKeyWPSR &);
};

/*! @} End of Doxygen Groups*/

#endif
