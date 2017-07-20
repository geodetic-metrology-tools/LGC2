#ifndef _READERS_MEASUREMENTREADERS_H_
#define _READERS_MEASUREMENTREADERS_H_

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

//LGC
#include <TInstrumentData.h>
#include <ReaderBase.h>
#include <KeywordRights.h>

/*!
 *  \addtogroup MeasurementReaders
 *  @{
 */

/*! 
	\brief This is an abstract base class for all keywords related to measurements, e.g. CAM, TSTN, PLR3D, ANGL, DVER, UVEC,...
*/
class TAMeasurementKey : public TAKeyWord {
	public:
		TAMeasurementKey(TLGCData& project, const std::string& key);

	protected:
		LGCAdjustablePointCollection const    &fpoints;
		const TInstrumentData   &finstruments;
		TAdjustableLengthCollection&flengths;
		TAdjustableAngleCollection& fangles;
		LGCAdjustablePlaneCollection& fplanes;
		LGCAdjustableLineCollection& flines;
		const bool fSIMUActive;

		/// Returns if this was the first line beginning with a '*',  NOT the success of the target update
		// To be used for the TSTN measurement's keywords, such as PLR3D, ANGL, ZEND,...
		bool updateDefaultTargetTSTN(const std::vector<std::string>& tokens);
	

		/// returns a reference to the most recently added station (TSTN)
		// (a modifyable copy of the object in the instruments records)
		inline TInstrumentData::TPOLAR& getStation() const {
			return proj.getCurrentNode().measurements.fTSTN.back()->instrument;
		}

		/// returns a reference to the current ROM
		inline shared_ptr<TTSTN::TROM> getROM() const {
			if(proj.getCurrentNode().measurements.fTSTN.back()->roms.size())
				return proj.getCurrentNode().measurements.fTSTN.back()->roms.back();
			else
				throw std::range_error("TTSTN::ROM vector is emtpy");
		}

		/// returns a reference to the current CAM
		inline TCAM& getCAM() const {
			return proj.getCurrentNode().measurements.fCAM.back();
		}

		///Currently used target, e.g. in ROM (V0 keyword) reading for *TSTN and also for *CAM
		std::string currentTargetApplied;

	private:
		TAMeasurementKey& operator=(const TAMeasurementKey&);
};

class TKeyTSTN : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyTSTN(TLGCData& project, int nb_allowed_keywords = nb_allowed_tstn, const char** keywords = allowed_TSTN) : 
		TAMeasurementKey(project, TSTN) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}
		
		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line);
};

class TKeyCAM : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyCAM(TLGCData& project, int nb_allowed_keywords = nb_allowed_cam, const char** keywords = allowed_CAM) : 
		TAMeasurementKey(project, CAM)  
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}
		
		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line);
};


///////////////////////
//   CAMERA MEAS       //
///////////////////////
class TKeyUVEC : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyUVEC(TLGCData& project, int nb_allowed_keywords = nb_allowed_uvec, const char** keywords = allowed_UVEC) : 
		TAMeasurementKey(project, UVEC) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}
		
		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line);
};

class TKeyUVD : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyUVD(TLGCData& project, int nb_allowed_keywords = nb_allowed_uvd, const char** keywords = allowed_UVD) : 
		TAMeasurementKey(project, UVD) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}
		
		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line);
};

///////////////////////
///////////////////////
//   TSTN MEAS       //
///////////////////////
class TKeyV0 : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyV0(TLGCData& project, int nb_allowed_keywords = nb_allowed_v0, const char** keywords = allowed_V0) : 
		TAMeasurementKey(project, V0) 
		{
			requiredAdjustableVo = true;

			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/// tokenize V0 data
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int);

		bool requiredAdjustableVo;
		/// function used only for V0 keyword. The polar instrument has an unknown orientation angle which can be determinate only if ANGL, PLR3D, ECTH or ECDIR measurement are used 
		void setRequiredAdjVo(bool b){ requiredAdjustableVo = b; }
};

class TKeyPLR3D : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyPLR3D(TLGCData& project, int nb_allowed_keywords = nb_allowed_plr3d, const char** keywords = allowed_PLR3D) : 
		TAMeasurementKey(project, PLR3D) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line);
};

class TKeyANGL : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyANGL(TLGCData& project, int nb_allowed_keywords = nb_allowed_angl, const char** keywords = allowed_ANGL) : 
		TAMeasurementKey(project, ANGL) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line); 

};

/// Keyword to process Zenith distance measurement
class TKeyZEND : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyZEND(TLGCData& project, int nb_allowed_keywords = nb_allowed_zend, const char** keywords = allowed_ZEND) :  
		TAMeasurementKey(project, ZEND) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line); 
};

/// Keyword to process spatial distance measurement
class TKeyDIST : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyDIST(TLGCData& project, int nb_allowed_keywords = nb_allowed_dist, const char** keywords = allowed_DIST) : 
		TAMeasurementKey(project, DIST) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line);
};

/// Keyword to process ECTH measurement
class TKeyECTH : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyECTH(TLGCData& project, int nb_allowed_keywords = nb_allowed_ecth, const char** keywords = allowed_ECTH) : 
		TAMeasurementKey(project, ECTH) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line); 
	
	private:
		TAngle fObservedAngle;
		std::string fScaleInstID;

};
/// Keyword to process ECSP measurement
class TKeyECDIR : public TAMeasurementKey {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECDIR(TLGCData& project, int nb_allowed_keywords = nb_allowed_ecdir, const char** keywords = allowed_ECDIR) :
		TAMeasurementKey(project, ECDIR)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line);

private:
	TAngle fHorAngle;
	TAngle fVertAngle;
	std::string fScaleInstID;
};

/// Keyword to process ECSP measurement
class TKeyECSP : public TAMeasurementKey {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECSP(TLGCData& project, int nb_allowed_keywords = nb_allowed_ecsp, const char** keywords = allowed_ECSP) :
		TAMeasurementKey(project, ECSP)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line);

};

/// Keyword to process DHOR measurement
class TKeyDHOR : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyDHOR(TLGCData& project, int nb_allowed_keywords = nb_allowed_dhor, const char** keywords = allowed_DHOR) : 
		TAMeasurementKey(project, DHOR) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line); 
};


// NON-Totalstation measurements

/// Keyword to process Distance SPaTial
class TKeyDSPT : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyDSPT(TLGCData& project, int nb_allowed_keywords = nb_allowed_dspt, const char** keywords = allowed_DSPT) : 
		TAMeasurementKey(project, DSPT)
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line); 
};

/// Keyword to process Vertical distance -- Geodetic Reference System
class TKeyDVER : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyDVER(TLGCData& project, int nb_allowed_keywords = nb_allowed_dver, const char** keywords = allowed_DVER) : 
		TAMeasurementKey(project, DVER)
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line); 

		TLength sigma;
};

/// Keyword to process DLEV measurement
class TKeyDLEV : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyDLEV(TLGCData& project, int nb_allowed_keywords = nb_allowed_dlev, const char** keywords = allowed_DLEV) : 
		TAMeasurementKey(project, DLEV)
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line); 
};


/// Keyword to process ECHO -- Offset to a vertical plane
class TKeyECHO : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyECHO(TLGCData& project, int nb_allowed_keywords = nb_allowed_echo, const char** keywords = allowed_ECHO) : 
		TAMeasurementKey(project, ECHO)
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line); 
};

/// Keyword to process ECVE measurement
class TKeyECVE : public TAMeasurementKey {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyECVE(TLGCData& project, int nb_allowed_keywords = nb_allowed_ecve, const char** keywords = allowed_ECVE) :
		TAMeasurementKey(project, ECVE)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line);

};

/// Keyword to process ORIE -- Gyro-Theodolite Azimut
class TKeyORIE : public TAMeasurementKey {
	public:
		/// Constructor, the list of allowed keywords is filled
		TKeyORIE(TLGCData& project, int nb_allowed_keywords = nb_allowed_orie, const char** keywords = allowed_ORIE) : 
		TAMeasurementKey(project, ORIE)
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		/*!
			\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

			/throws Exception if the keyword is not used correctly.
		*/
		virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line); 
};

/// Keyword to process ORIE -- Gyro-Theodolite Azimut
class TKeyRADI : public TAMeasurementKey {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyRADI(TLGCData& project, int nb_allowed_keywords = nb_allowed_radi, const char** keywords = allowed_RADI) :
		TAMeasurementKey(project, RADI)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line);

	TLength sigma;
};


/// Keyword to process OBSXYZ
class TKeyOBSXYZ : public TAMeasurementKey {
public:
	/// Constructor, the list of allowed keywords is filled
	TKeyOBSXYZ(TLGCData& project, int nb_allowed_keywords = nb_allowed_OBSXYZ, const char** keywords = allowed_OBSXYZ) :
		TAMeasurementKey(project, OBSXYZ)
	{
		for (int i(0); i< nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}

	/*!
	\brief Processes the tokenized line (tokens) of the input file, creates and fills the respective classes to store the data.

	/throws Exception if the keyword is not used correctly.
	*/
	virtual void parse(const std::vector<std::string>& tokens, bool activeLine, int line);
};

/*! @} End of Doxygen Groups*/

#endif
