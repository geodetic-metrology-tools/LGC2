#ifndef _READERS_MEASUREMENTREADERS_H_
#define _READERS_MEASUREMENTREADERS_H_

#include <TInstrumentData.h>
#include "ReaderBase.h"
#include "KeywordRights.h"


class TMeasurementKey : public TAKeyWord {
	public:
		TMeasurementKey(TLGCData& project, const std::string& key);

	protected:
		TAdjustablePointCollection const    &fpoints;
		const TInstrumentData   &finstruments;
		TAdjustableScalarCollection&fscalars;
		TAdjustableAngleCollection& fangles;
		TAdjustablePlaneCollection& fplanes;

		//Tells whether the measurement is made by TSTN or not (currently need for PLR3D and DIR3D for identifiying whether are used under *TSTN or *CAM keyword), switcher
		//static bool fIsTSTN;

		// returns if this was the first line beginning with a '*',  NOT the success of the target update
		// To be used for the TSTN measurements (PLR3D, ANGL, ...)
		bool updateDefaultTargetTSTN(const std::vector<const std::string>& tokens);
	

		// returns a reference to the most recently added station (TSTN)
		// (a modifyable copy of the object in the instruments records)
		inline TInstrumentData::TPOLAR& getStation() const {
				return proj.getCurrentNode().measurements.fTSTN.back().instrument;
		}

		// returns a reference to the current ROM
		inline TTSTN::TROM& getROM() const {
			return proj.getCurrentNode().measurements.fTSTN.back().roms.back();
		}

		// returns a reference to the current CAM
		inline TCAM& getCAM() const {
			return proj.getCurrentNode().measurements.fCAM.back();
		}

		//Currently used target, e.g. in ROM (V0 keyword) reading for *TSTN and also for *CAM
		std::string currentTargetApplied;

	private:
		TMeasurementKey& operator=(const TMeasurementKey&);
};

class TKeyTSTN : public TMeasurementKey {
	public:
		TKeyTSTN(TLGCData& project, int nb_allowed_keywords = nb_allowed_tstn, const char** keywords = allowed_TSTN) : 
		TMeasurementKey(project, TSTN) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}
		
		//tokenize instrument data
		virtual void parse(const std::vector<const std::string>& tokens, int line);
};

class TKeyCAM : public TMeasurementKey {
	public:
		TKeyCAM(TLGCData& project, int nb_allowed_keywords = nb_allowed_cam, const char** keywords = allowed_CAM) : 
		TMeasurementKey(project, CAM)  
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}
		
		//tokenize instrument data
		virtual void parse(const std::vector<const std::string>& tokens, int line);
};


///////////////////////
//   CAMERA MEAS       //
///////////////////////
class TKeyUVEC : public TMeasurementKey {
	public:
		TKeyUVEC(TLGCData& project, int nb_allowed_keywords = nb_allowed_uvec, const char** keywords = allowed_UVEC) : 
		TMeasurementKey(project, UVEC) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}
		
		//tokenize instrument data
		virtual void parse(const std::vector<const std::string>& tokens, int line);
};


class TKeyUVD : public TMeasurementKey {
	public:
		TKeyUVD(TLGCData& project, int nb_allowed_keywords = nb_allowed_uvd, const char** keywords = allowed_UVD) : 
		TMeasurementKey(project, UVD) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}
		
		//tokenize instrument data
		virtual void parse(const std::vector<const std::string>& tokens, int line);
};



///////////////////////
///////////////////////
//   TSTN MEAS       //
///////////////////////
class TKeyV0 : public TMeasurementKey {
	public:
		TKeyV0(TLGCData& project, int nb_allowed_keywords = nb_allowed_v0, const char** keywords = allowed_V0) : 
		TMeasurementKey(project, V0) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//tokenize V0 data
		virtual void parse(const std::vector<const std::string>& tokens, int) ;
};

class TKeyPLR3D : public TMeasurementKey {
	public:
		TKeyPLR3D(TLGCData& project, int nb_allowed_keywords = nb_allowed_plr3d, const char** keywords = allowed_PLR3D) : 
		TMeasurementKey(project, PLR3D) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//tokenize PLR measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line);
};

class TKeyANGL : public TMeasurementKey {
	public:
		TKeyANGL(TLGCData& project, int nb_allowed_keywords = nb_allowed_angl, const char** keywords = allowed_ANGL) : 
		TMeasurementKey(project, ANGL) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//tokenize ANGL measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 

};


class TKeyZEND : public TMeasurementKey {
	public:
		TKeyZEND(TLGCData& project, int nb_allowed_keywords = nb_allowed_zend, const char** keywords = allowed_ZEND) :  
		TMeasurementKey(project, ZEND) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//tokenize ZEND measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 
};

class TKeyDIST : public TMeasurementKey {
	public:
		TKeyDIST(TLGCData& project, int nb_allowed_keywords = nb_allowed_dist, const char** keywords = allowed_DIST) : 
		TMeasurementKey(project, DIST) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//tokenize DIST measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line);
};

class TKeyECTH : public TMeasurementKey {
	public:
		TKeyECTH(TLGCData& project, int nb_allowed_keywords = nb_allowed_ecth, const char** keywords = allowed_ECTH) : 
		TMeasurementKey(project, ECTH) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//tokenize ECTH measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 
	
	private:
		LGC::TAngle fObservedAngle;
		std::string fScaleInstID;

};

class TKeyDHOR : public TMeasurementKey {
	public:
		TKeyDHOR(TLGCData& project, int nb_allowed_keywords = nb_allowed_dhor, const char** keywords = allowed_DHOR) : 
		TMeasurementKey(project, DHOR) 
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//tokenize DHOR measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 
};


// NON-Totalstation measurements

// Distance SPaTial
class TKeyDSPT : public TMeasurementKey {
	public:
		TKeyDSPT(TLGCData& project, int nb_allowed_keywords = nb_allowed_dspt, const char** keywords = allowed_DSPT) : 
		TMeasurementKey(project, DSPT)
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//tokenize DSPT measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 
};

// Vertical distance -- Geodetic Reference System
class TKeyDVER : public TMeasurementKey {
	public:
		TKeyDVER(TLGCData& project, int nb_allowed_keywords = nb_allowed_dver, const char** keywords = allowed_DVER) : 
		TMeasurementKey(project, DVER)
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//tokenize DVER measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 

		void storeEOLComment(){

		}
};

// DLEV
class TKeyDLEV : public TMeasurementKey {
	public:
		TKeyDLEV(TLGCData& project, int nb_allowed_keywords = nb_allowed_dlev, const char** keywords = allowed_DLEV) : 
		TMeasurementKey(project, DLEV)
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//tokenize DLEV measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 
};


//ECHO -- Offset to a vertical plane
class TKeyECHO : public TMeasurementKey {
	public:
		TKeyECHO(TLGCData& project, int nb_allowed_keywords = nb_allowed_echo, const char** keywords = allowed_ECHO) : 
		TMeasurementKey(project, ECHO)
		{
			for(int i(0) ; i< nb_allowed_keywords ; i++)
				allowed_keywords.emplace_back(keywords[i]);
		}

		//tokenize DLEV measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 
};


#endif
