/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _READERS_MEASUREMENTREADERS_H_
#define _READERS_MEASUREMENTREADERS_H_

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
class TAMeasurementKey : public TAKeyWord
{
public:
	TAMeasurementKey(TLGCData &project, const std::string &key);

protected:
	LGCAdjustablePointCollection const &fpoints;
	const TInstrumentData &finstruments;
	TAdjustableLengthCollection &flengths;
	TAdjustableAngleCollection &fangles;
	LGCAdjustablePlaneCollection &fplanes;
	LGCAdjustableLineCollection &flines;

	/// Returns if this was the first line beginning with a '*',  NOT the success of the target update
	// To be used for the TSTN measurement's keywords, such as PLR3D, ANGL, ZEND,...
	bool updateDefaultTargetTSTN(const std::vector<std::string> &tokens);

	/// returns a reference to the most recently added station (TSTN)
	// (a modifyable copy of the object in the instruments records)
	inline TInstrumentData::TPOLAR &getStation() const { return proj.getCurrentNode().measurements.fTSTN.back()->instrument; }

	/// returns a reference to the current ROM
	inline std::shared_ptr<TTSTN::TROM> getROM() const
	{
		if (proj.getCurrentNode().measurements.fTSTN.back()->roms.size())
			return proj.getCurrentNode().measurements.fTSTN.back()->roms.back();
		else
			throw std::range_error("TTSTN::ROM vector is emtpy");
	}

	/// returns a reference to the current CAM
	inline TCAM &getCAM() const { return proj.getCurrentNode().measurements.fCAM.back(); }

	/// default used target used in a ROM
	std::string defaultTargetApplied;

private:
	TAMeasurementKey &operator=(const TAMeasurementKey &);
};

class TKeyTSTN : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyTSTN(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(TSTN)) : TAMeasurementKey(project, TSTN)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

class TKeyCAM : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyCAM(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(CAM)) : TAMeasurementKey(project, CAM)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

///////////////////////
//   CAMERA MEAS       //
///////////////////////
class TKeyUVEC : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyUVEC(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(UVEC)) : TAMeasurementKey(project, UVEC)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

class TKeyUVD : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyUVD(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(UVD)) : TAMeasurementKey(project, UVD)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

///////////////////////
///////////////////////
//   TSTN MEAS       //
///////////////////////
class TKeyV0 : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyV0(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(V0)) : TAMeasurementKey(project, V0)
	{
		allowed_keywords = allowed_kw;
	}

	/// tokenize V0 data
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int);
};

class TKeyPLR3D : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyPLR3D(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(PLR3D)) : TAMeasurementKey(project, PLR3D)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

class TKeyANGL : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyANGL(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ANGL)) : TAMeasurementKey(project, ANGL)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process Zenith distance measurement
class TKeyZEND : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyZEND(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ZEND)) : TAMeasurementKey(project, ZEND)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process spatial distance measurement
class TKeyDIST : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDIST(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(DIST)) : TAMeasurementKey(project, DIST)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process ECTH measurement
class TKeyECTH : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECTH(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ECTH)) : TAMeasurementKey(project, ECTH)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	TAngle fObservedAngle;
	std::string fScaleInstID;
};
/// Keyword to process ECSP measurement
class TKeyECDIR : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECDIR(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ECDIR)) : TAMeasurementKey(project, ECDIR)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

private:
	TAngle fHorAngle;
	TAngle fVertAngle;
	std::string fScaleInstID;
};

/// Keyword to process ECSP measurement
class TKeyECSP : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECSP(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ECSP)) : TAMeasurementKey(project, ECSP)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process DHOR measurement
class TKeyDHOR : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDHOR(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(DHOR)) : TAMeasurementKey(project, DHOR)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

// NON-Totalstation measurements

/// Keyword to process Distance SPaTial
class TKeyDSPT : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDSPT(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(DSPT)) : TAMeasurementKey(project, DSPT)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process Vertical distance -- Geodetic Reference System
class TKeyDVER : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDVER(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(DVER)) : TAMeasurementKey(project, DVER)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

	TLength sigma;
};

/// Keyword to process DLEV measurement
class TKeyDLEV : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyDLEV(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(DLEV)) : TAMeasurementKey(project, DLEV)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process ECHO -- Offset to a vertical plane
class TKeyECHO : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECHO(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ECHO)) : TAMeasurementKey(project, ECHO)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process ECVE measurement
class TKeyECVE : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECVE(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ECVE)) : TAMeasurementKey(project, ECVE)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process ORIE -- Gyro-Theodolite Azimut
class TKeyORIE : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyORIE(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ORIE)) : TAMeasurementKey(project, ORIE)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process ORIE -- Gyro-Theodolite Azimut
class TKeyRADI : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyRADI(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(RADI)) : TAMeasurementKey(project, RADI)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);

	TLength sigma;
	TAngle constAngle;
};

/// Keyword to process OBSXYZ
class TKeyOBSXYZ : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyOBSXYZ(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(OBSXYZ)) : TAMeasurementKey(project, OBSXYZ)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process INCLY -- Inclinometer measurement around Y
class TKeyINCLY : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyINCLY(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(INCLY)) : TAMeasurementKey(project, INCLY)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process ROLLY -- Inclinometer measurement following ROLLY model
class TKeyROLLY : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyROLLY(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ROLLY)) : TAMeasurementKey(project, ROLLY)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process ECWS -- ECart-water surface measurements
class TKeyECWS : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECWS(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ECWS)) : TAMeasurementKey(project, ECWS)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};

/// Keyword to process ECWI -- ECart-WIre measurements
class TKeyECWI : public TAMeasurementKey
{
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECWI(TLGCData &project, const std::set<std::string> &allowed_kw = KeywordRights::lgc2Rights(ECWI)) : TAMeasurementKey(project, ECWI)
	{
		allowed_keywords = allowed_kw;
	}

	/*!
		\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

		/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
};
/*! @} End of Doxygen Groups*/

#endif
