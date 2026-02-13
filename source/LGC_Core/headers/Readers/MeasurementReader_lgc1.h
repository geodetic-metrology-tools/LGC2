/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _READERS_MEASUREMENTREADERS_LGC1_H_
#define _READERS_MEASUREMENTREADERS_LGC1_H_

// LGC
#include <KeywordRights.h>
#include <ReaderBase.h>
#include <TInstrumentData.h>

/*!
 *  \addtogroup MeasurementReaders
 *  @{
 */

/*!
\brief This is an abstract base class for all keywords related to measurements, e.g. CAM, TSTN, PLR3D, ANGL, DVER, UVEC,...
*/
class TAMeasurementKey_lgc1 : public TAKeyWord
{
public:
	TAMeasurementKey_lgc1(TLGCData &project, const std::string &key);

protected:
	LGCAdjustablePointCollection const &fpoints;
	TInstrumentData &finstruments;
	TAdjustableLengthCollection &flengths;
	TAdjustableAngleCollection &fangles;
	LGCAdjustablePlaneCollection &fplanes;
	LGCAdjustableLineCollection &flines;

	/// returns a reference to the polar instrument
	inline TInstrumentData::TPOLAR &getPolarInstr()
	{
		if (finstruments.fPOLAR.size() == 0)
			createPolarInstrument();

		return *finstruments.fPOLAR["TSTNInstr"];
	}

	TInstrumentData::TEDM &getEDMInstr()
	{
		if (finstruments.fEDM.size() == 0)
			createEDMInstrument();

		return *finstruments.fEDM.begin()->second;
	}

	TInstrumentData::TEDM::TTarget &getEDMAdjTarget()
	{
		auto &tedm = getEDMInstr();
		if (tedm.targets.size() < 2)
		{
			// Create the adjustable target:
			auto adj_tgt = tedm.targets.begin()->second;
			adj_tgt->ID = "EDMAdjTgt";
			adj_tgt->distCorrectionUnknown = true;
			adj_tgt->distCorrectionAdjustable = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, "EDM_dcorr_adj"));

			// Add the new target to the instrument
			tedm.targets["EDMAdjTgt"] = adj_tgt;
		}

		return *tedm.targets.at("EDMAdjTgt");
	}

	// create a default TInstrumentData::TPOLAR
	void createPolarInstrument();
	// create a default EDM
	void createEDMInstrument();
	// create a TSTN
	void createTSTN(std::string stn, int line);
	// create a ROM in tstn
	void createROM(std::shared_ptr<TTSTN> tstn);

private:
	TAMeasurementKey_lgc1 &operator=(const TAMeasurementKey_lgc1 &);
};

///////////////////////
///////////////////////
//   TSTN MEAS       //
///////////////////////

class TKeyANGL_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyANGL_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(ANGL)) :
		TAMeasurementKey_lgc1(project, ANGL), currentTSTN(nullptr), currentROM(nullptr)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.
	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	TAngle sigmaANGL = TAngle(0.0);
	TAngle constanteANGL = TAngle(0.0);
	std::string currentStation = "";
	std::shared_ptr<TTSTN> currentTSTN;
	std::shared_ptr<TTSTN::TROM> currentROM;
};

/// Keyword to process Zenith distance measurement
class TKeyZENI_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyZENI_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(ZENI)) :
		TAMeasurementKey_lgc1(project, ZENI), currentTSTN(nullptr), currentROM(nullptr)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.
	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	TAngle sigmaZEND = TAngle(0.0);
	TLength IH = TLength(0.0);
	std::string currentStation = "";
	std::shared_ptr<TTSTN> currentTSTN;
	std::shared_ptr<TTSTN::TROM> currentROM;
};

/// Keyword to process Zenith distance measurement
class TKeyZENH_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyZENH_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(ZENH)) :
		TAMeasurementKey_lgc1(project, ZENH), currentTSTN(nullptr), currentROM(nullptr), firstmeas(true)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.
	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	TAngle sigmaZEND = TAngle(0.0);
	std::string currentStation = "";
	std::shared_ptr<TTSTN> currentTSTN;
	std::shared_ptr<TTSTN::TROM> currentROM;
	bool firstmeas;
};

/// Keyword to process distance measurement
class TKeyDTHE_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDTHE_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(DTHE)) :
		TAMeasurementKey_lgc1(project, DTHE), currentTSTN(nullptr), currentROM(nullptr), adjDCorr(nullptr)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	TLength sigmaDIST = TLength(0.0);
	TLength dcorr = TLength(0.0);
	TLength ppm = TLength(0.0);
	std::string currentStation = "";
	std::shared_ptr<TTSTN> currentTSTN;
	std::shared_ptr<TTSTN::TROM> currentROM;
	TAdjustableLength *adjDCorr;
};

/// Keyword to process distance measurement
class TKeyDMES_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDMES_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(DMES)) : TAMeasurementKey_lgc1(project, DMES)
	{
		// adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, currentStation + "_adj"));
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	std::string currentStation = "";
	TInstrumentData::TEDM romInstr;
	TInstrumentData::TEDM::TTarget romTarget;
};

/// Keyword to process ECTH measurement
class TKeyECTH_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECTH_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(ECTH)) :
		TAMeasurementKey_lgc1(project, ECTH), currentTSTN(nullptr), currentROM(nullptr), firstmeas(true)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	std::shared_ptr<TTSTN> currentTSTN;
	std::shared_ptr<TTSTN::TROM> currentROM;
	bool firstmeas;
	std::string currentStation = "";
	TLength sigma = TLength(0.0);
	TLength dcorr = TLength(0.0);
};

/// Keyword to process ECSP measurement
class TKeyECSP_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECSP_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(ECSP)) :
		TAMeasurementKey_lgc1(project, ECSP), firstmeas(true)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	bool firstmeas;
	std::string point1 = "";
	std::string point2 = "";
	TLength sigma = TLength(0.0);
	TLength dcorr = TLength(0.0);
};

/// Keyword to process DHOR measurement
class TKeyDHOR_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDHOR_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(DHOR)) :
		TAMeasurementKey_lgc1(project, DHOR), currentTSTN(nullptr), currentROM(nullptr), firstmeas(true)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	TLength sigmaDIST = TLength(0.0);
	TLength dcorr = TLength(0.0);
	TLength ppm = TLength(0.0);
	std::string currentStation = "";
	std::shared_ptr<TTSTN> currentTSTN;
	std::shared_ptr<TTSTN::TROM> currentROM;
	bool firstmeas;
};

/// Keyword to process Vertical distance -- Geodetic Reference System
class TKeyDVER_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDVER_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(DVER)) : TAMeasurementKey_lgc1(project, DVER)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	TLength sigma = TLength(0.0);
	TLength dcorr = TLength(0.0);
};

/// Keyword to process DLEV measurement
class TKeyDLEV_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDLEV_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(DLEV)) : TAMeasurementKey_lgc1(project, DLEV)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	bool fistrDLEV = true;
	TLength sigma = TLength(0.0);
	TLength dcorr = TLength(0.0);
	std::string currentStation = "";
};

/// Keyword to process ECHO -- Offset to a vertical plane
class TKeyECHO_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECHO_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(ECHO)) : TAMeasurementKey_lgc1(project, ECHO)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	bool firstECHO = true;
	TLength sigma = TLength(0.0);
	TLength constante = TLength(0.0);
	std::string encrage1 = "";
	std::string encrage2 = "";
};

/// Keyword to process ECVE measurement
class TKeyECVE_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECVE_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(ECVE)) : TAMeasurementKey_lgc1(project, ECVE)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	bool fistrECVE = true;
	TLength sigma = TLength(0.0);
	TLength constante = TLength(0.0);
	std::string ptLine = "";
};

/// Keyword to process ORIE -- Gyro-Theodolite Azimut
class TKeyORIE_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyORIE_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(ORIE)) : TAMeasurementKey_lgc1(project, ORIE)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	std::string currentStation = "";
	bool fistrORIE = true;
	TAngle sigma = TAngle(0.0);
	TAngle constante = TAngle(0.0);
};

/// Keyword to process RADI
class TKeyRADI_lgc1 : public TAMeasurementKey_lgc1
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyRADI_lgc1(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc1Rights(RADI)) : TAMeasurementKey_lgc1(project, RADI)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	TLength sigma = TLength(0.0);
};

/*! @} End of Doxygen Groups*/

#endif
