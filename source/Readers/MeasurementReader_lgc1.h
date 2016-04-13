#ifndef _READERS_MEASUREMENTREADERS_LGC1_H_
#define _READERS_MEASUREMENTREADERS_LGC1_H_

#include <TInstrumentData.h>
#include "ReaderBase.h"
#include "KeywordRights.h"

/*!
*  \addtogroup MeasurementReaders
*  @{
*/

/*!
\brief This is an abstract base class for all keywords related to measurements, e.g. CAM, TSTN, PLR3D, ANGL, DVER, UVEC,...
*/
class TAMeasurementKey_lgc1 : public TAKeyWord {
public:
	TAMeasurementKey_lgc1(TLGCData& project, const std::string& key);

protected:
	TAdjustablePointCollection const    &fpoints;
	TInstrumentData   &finstruments;
	TAdjustableLengthCollection&flengths;
	TAdjustableAngleCollection& fangles;
	TAdjustablePlaneCollection& fplanes;
	TAdjustableLineCollection& flines;
	const bool& fSIMUActive;

	/// returns a reference to the polar instrument
	inline TInstrumentData::TPOLAR& getPolarInstr() {
		//if (proj.getCurrentNode().measurements.fTSTN.size() == 0)
		if (finstruments.fPOLAR.size() == 0)
			createPolarInstrument();
		
		return finstruments.fPOLAR["TSTNInstr"];
	}

	//create a default TInstrumentData::TPOLAR
	void createPolarInstrument();
	//create a TSTN
	void createTSTN(string stn, int line);
	//create a ROM in tstn
	void createROM(TTSTN& tstn);


	///Currently used target, e.g. in ROM (V0 keyword) reading for *TSTN and also for *CAM
	std::string currentTargetApplied;

private:
	TAMeasurementKey_lgc1& operator=(const TAMeasurementKey_lgc1&);
};


///////////////////////
///////////////////////
//   TSTN MEAS       //
///////////////////////


class TKeyANGL_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyANGL_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_angl_lgc1, const char** keywords = allowed_ANGL_lgc1) :
		TAMeasurementKey_lgc1(project, ANGL),
		currentTSTN(nullptr),
		currentROM(nullptr)
		/*currentTSTN(proj.getCurrentNode().measurements.fTSTN.back()),
		currentROM(currentTSTN.roms.back())*/
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);

private:
	TAngle sigmaANGL, constanteANGL;
	string currentStation;
	//TTSTN& currentTSTN;
	//TTSTN::TROM& currentROM;
	TTSTN* currentTSTN;
	TTSTN::TROM* currentROM;
};

/// Keyword to process Zenith distance measurement
class TKeyZENI_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyZENI_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_zeni_lgc1, const char** keywords = allowed_ZENI_lgc1) :
		TAMeasurementKey_lgc1(project, ZENI),
		currentTSTN(nullptr),
		currentROM(nullptr)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);

private:
	TAngle sigmaZEND;
	TLength IH;
	string currentStation;
	//TTSTN& currentTSTN;
	//TTSTN::TROM& currentROM;
	TTSTN* currentTSTN;
	TTSTN::TROM* currentROM;
	
};

/// Keyword to process Zenith distance measurement
class TKeyZENH_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyZENH_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_zenh_lgc1, const char** keywords = allowed_ZENH_lgc1) :
		TAMeasurementKey_lgc1(project, ZENH),
		currentTSTN(nullptr),
		currentROM(nullptr)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);

private:
	TAngle sigmaZEND;
	string currentStation;
	//TTSTN& currentTSTN;
	//TTSTN::TROM& currentROM;
	TTSTN* currentTSTN;
	TTSTN::TROM* currentROM;
};

/// Keyword to process distance measurement
class TKeyDTHE_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDTHE_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_dthe_lgc1, const char** keywords = allowed_DTHE_lgc1) :
		TAMeasurementKey_lgc1(project, DTHE)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);
};

/// Keyword to process distance measurement
class TKeyDMES_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDMES_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_dmes_lgc1, const char** keywords = allowed_DMES_lgc1) :
		TAMeasurementKey_lgc1(project, DMES), fistrDMES(true)
	{
		adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, currentStation + "_adj"));
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);

private:
	bool fistrDMES;
	TLength sigma, ppm, dcorr;
	TAdjustableLength* adjDCorr;
	string currentStation;
};

/// Keyword to process ECTH measurement
class TKeyECTH_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECTH_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_ecth_lgc1, const char** keywords = allowed_ECTH_lgc1) :
		TAMeasurementKey_lgc1(project, ECTH)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);

private:
	TAngle fObservedAngle;
	
	bool fistrECTH;
	TLength sigma;
	TLength constante;
	string currentStation;

};

/// Keyword to process ECSP measurement
class TKeyECSP_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECSP_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_ecsp_lgc1, const char** keywords = allowed_ECSP_lgc1) :
		TAMeasurementKey_lgc1(project, ECSP)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);

private:
	TAngle fHorAngle;
	TAngle fVertAngle;

	bool fistrECSP = true;
	TLength sigma;
	TLength constante;
	string point1 = "";
	string point2 = "";
};

/// Keyword to process DHOR measurement
class TKeyDHOR_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDHOR_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_dhor_lgc1, const char** keywords = allowed_DHOR_lgc1) :
		TAMeasurementKey_lgc1(project, DHOR)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);
};



/// Keyword to process Vertical distance -- Geodetic Reference System
class TKeyDVER_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDVER_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_dver, const char** keywords = allowed_DVER) :
		TAMeasurementKey_lgc1(project, DVER)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);

private:
	TLength sigma;
	TLength dcorr;
};

/// Keyword to process DLEV measurement
class TKeyDLEV_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDLEV_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_dlev, const char** keywords = allowed_DLEV) :
		TAMeasurementKey_lgc1(project, DLEV)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);

private:
	bool fistrDLEV = true;
	TLength sigma = TLength(0.0);
	TLength dcorr = TLength(0.0);
	string currentStation = "";
};


/// Keyword to process ECHO -- Offset to a vertical plane
class TKeyECHO_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECHO_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_echo, const char** keywords = allowed_ECHO) :
		TAMeasurementKey_lgc1(project, ECHO)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);

private:
	bool firstECHO = true;
	TLength sigma;
	TLength constante;
	string encrage1 = "";
	string encrage2 = "";
};

/// Keyword to process ECVE measurement
class TKeyECVE_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECVE_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_ecve, const char** keywords = allowed_ECVE) :
		TAMeasurementKey_lgc1(project, ECVE)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);

private:
	bool fistrECVE = true;
	TLength sigma;
	TLength constante;
	string ptLine = "";
};

/// Keyword to process ORIE -- Gyro-Theodolite Azimut
class TKeyORIE_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyORIE_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_orie, const char** keywords = allowed_ORIE) :
		TAMeasurementKey_lgc1(project, ORIE)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);

private:
	string currentStation = "";
	bool fistrORIE = true;
	TAngle sigma;
	TAngle constante;
};

/// Keyword to process ORIE -- Gyro-Theodolite Azimut
class TKeyRADI_lgc1 : public TAMeasurementKey_lgc1 {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyRADI_lgc1(TLGCData& project, int nb_allowed_keywords = nb_allowed_radi, const char** keywords = allowed_RADI) :
		TAMeasurementKey_lgc1(project, RADI)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, int line);

private:
	TLength sigma = TLength(0.0);
};


/*! @} End of Doxygen Groups*/

#endif
