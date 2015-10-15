#ifndef _READERS_MEASUREMENTREADERS_H_
#define _READERS_MEASUREMENTREADERS_H_

#include <TInstrumentData.h>
#include "ReaderBase.h"


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
		static bool fIsTSTN;

		// returns if this was the first line beginning with a '*', 
		// NOT the success of the target update
		bool updateDefaultTarget(const std::vector<const std::string>& tokens);
		
		// returns a reference to the most recently added station 
		// (a modifyable copy of the object in the instruments records)
		inline TInstrumentData::TPOLAR& getStation() const {
			if (fIsTSTN)
				return proj.getCurrentNode().measurements.fTSTN.back().instrument;
			else
				return proj.getCurrentNode().measurements.fCAM.back().instrument;
		}

		// returns a reference to the current ROM
		inline TTSTN::TROM& getROM() const {
			return proj.getCurrentNode().measurements.fTSTN.back().roms.back();
		}

		// returns a reference to the current CAM
		inline TCAM& getCAM() const {
			return proj.getCurrentNode().measurements.fCAM.back();
		}

		//Currently used target, e.g. in ROM (V0 keyword) reading for *TSTN or taken from default for *CAM
		const TInstrumentData::TPOLAR::TTarget* currentTargetApplied;

	private:
		TMeasurementKey& operator=(const TMeasurementKey&);
};

class TKeyTSTN : public TMeasurementKey {
	public:
		TKeyTSTN(TLGCData& project) : 
		TMeasurementKey(project, "TSTN") {}
		
		//tokenize instrument data
		virtual void parse(const std::vector<const std::string>& tokens, int line);
};

class TKeyCAM : public TMeasurementKey {
	public:
		TKeyCAM(TLGCData& project) : 
		TMeasurementKey(project, "CAM") {}
		
		//tokenize instrument data
		virtual void parse(const std::vector<const std::string>& tokens, int line);
};

///////////////////////
//   TSTN MEAS       //
///////////////////////
class TKeyV0 : public TMeasurementKey {
	public:
		TKeyV0(TLGCData& project) : 
		TMeasurementKey(project, "V0") {}

		//tokenize V0 data
		virtual void parse(const std::vector<const std::string>& tokens, int) ;
};

class TKeyPLR3D : public TMeasurementKey {
	public:
		TKeyPLR3D(TLGCData& project) : 
		TMeasurementKey(project, "PLR3D") {}

		//tokenize PLR measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line);
};

class TKeyDIR3D : public TMeasurementKey {
	public:
		TKeyDIR3D(TLGCData& project) : 
		TMeasurementKey(project, "DIR3D") {}

		//tokenize DIR measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 
};

class TKeyANGL : public TMeasurementKey {
	public:
		TKeyANGL(TLGCData& project) : 
		TMeasurementKey(project, "ANGL") {}

		//tokenize ANGL measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 

};


class TKeyZEND : public TMeasurementKey {
	public:
		TKeyZEND(TLGCData& project) :  
		TMeasurementKey(project, "ZEND") {}

		//tokenize ZEND measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 
};

class TKeyDIST : public TMeasurementKey {
	public:
		TKeyDIST(TLGCData& project) : 
		TMeasurementKey(project, "DIST") {}

		//tokenize DIST measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line);
};

class TKeyECTH : public TMeasurementKey {
	public:
		TKeyECTH(TLGCData& project) : 
		TMeasurementKey(project, "ECTH") {}

		//tokenize ECTH measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 
	
	private:
		LGC::TAngle fObservedAngle;
		std::string fScaleInstID;

};

class TKeyDHOR : public TMeasurementKey {
	public:
		TKeyDHOR(TLGCData& project) : 
		TMeasurementKey(project, "DHOR") {}

		//tokenize DHOR measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 
};



// NON-Totalstation measurements

// Distance SPaTial
class TKeyDSPT : public TMeasurementKey {
	public:
		TKeyDSPT(TLGCData& project) : 
		TMeasurementKey(project, "DSPT") {}

		//tokenize DSPT measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 
};

// Vertical distance -- Geodetic Reference System
class TKeyDVER : public TMeasurementKey {
	public:
		TKeyDVER(TLGCData& project) : 
		TMeasurementKey(project, "DVER") {}

		//tokenize DVER measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 

		void storeEOLComment(){

		}
};

// DLEV
class TKeyDLEV : public TMeasurementKey {
	public:
		TKeyDLEV(TLGCData& project) : 
		TMeasurementKey(project, "DLEV")
		{}

		//tokenize DLEV measuremnent
		virtual void parse(const std::vector<const std::string>& tokens, int line); 
};




#endif
