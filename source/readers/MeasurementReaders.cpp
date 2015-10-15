#include "MeasurementReaders.h"
#include <Measurements/TMeasurements.h>
#include <string>
#include <sstream>

TMeasurementKey::TMeasurementKey(TLGCData& project, const std::string& key) : 
			TAKeyWord(key, project),
			fpoints(project.points),
			finstruments(project.instruments),
			fscalars(project.scalars),
			fangles(project.angles),
			fplanes(project.planes){}

bool TMeasurementKey::updateDefaultTarget(const std::vector<const std::string>& tokens) 
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	//If TRGT keyword used,  target has to be updated
	if (firstline && tokens.size() == 4 && tokens.at(2) == "TRGT")
			currentTargetApplied = &finstruments.getDevice(getStation().targets, tokens.at(3));
	else if(firstline){
		//Line starts with '*', but TRGT keyword not used, i.e. taking default taget value either from ROM if TSTN used or the CAM default target
		if(fIsTSTN)
			currentTargetApplied = getROM().defaultTarget;
		else
			currentTargetApplied = &finstruments.getDevice(getStation().targets, getStation().defTarget);
	}
	return firstline;
}

void TKeyTSTN::parse(const std::vector<const std::string>& tokens, int line) 
{
	auto numTokens = tokens.size();
	using namespace LGC;

	if (numTokens < 4)
		throw std::runtime_error("Key *TSTN takes at least two arguments: Positioned point ID  and InstrumentID.");
	
	// Initialize the station
	TTSTN tstn(fpoints.getObject(tokens.at(2)),
		finstruments.getDevice(finstruments.fPOLAR, tokens.at(3)));
	tstn.line = line;

	// Parse the rest of the options
	TInstrumentData::TPOLAR& instrument(tstn.instrument);

	TOptionHelper opts(tokens.cbegin()+2, tokens.cend());
	stringstream ss;

	bool instrumentHeightFixed = opts.has("IHFIX");
	// Look for optional "IH" and "IHSE" flags only if the "IHFIX" flag used, ignore otherwise
	if(instrumentHeightFixed){
		instrument.instrHeight         = opts.getParamR("IH",   instrument.instrHeight);
		instrument.sigmaInstrHeight    = opts.getParamRmm2m("IHSE", instrument.sigmaInstrHeight); //value given in mili-meters [mm], returned value in meters [m]
	}

	tstn.rot3D = opts.has("ROT3D");
	//If station can rotate freely, we have two angles representing rotation around X a Y axis. Rotation around Z axis is made by the V0, which is Z-axis rotation.
	if (tstn.rot3D){
		tstn.rotX = &proj.angles.addObject(TAdjustableAngle(LGC::TAngle(LGC::TAngle::kGons, 0.0), false, "ROTX" + ss.str()));
		tstn.rotY = &proj.angles.addObject(TAdjustableAngle(LGC::TAngle(LGC::TAngle::kGons, 0.0), false, "ROTY" + ss.str()));
		//If ROT3D used, instrument height is fixed and is equal to 0
		instrumentHeightFixed = true;
		instrument.instrHeight = 0.0;
	}

	instrument.defTarget           = opts.getParamS("TRGT", instrument.defTarget);
	instrument.sigmaInstrCentering = opts.getParamRmm2m("ICSE", instrument.sigmaInstrCentering); //value given in mili-meters [mm], returned value in meters [m]

	//Add adjustable height of this station, NAME of the adjustable = "TSTN"  + frame_name + cam_instr_ID + sequence number of the TSTN in this FRAME 
	ss << proj.getCurrentNode().measurements.fTSTN.size();
	tstn.instrumentHeightAdjustable = &fscalars.addObject(TAdjustableScalar(TScalar(instrument.instrHeight),instrumentHeightFixed, "TSTN" + proj.getCurrentNode().frame.getName() + tstn.instrument.ID + ss.str()));

	//TSTN station used
	fIsTSTN = true;
	proj.getCurrentNode().measurements.fTSTN.emplace_back(tstn);

	if (opts.has("ROT3D") && opts.has("IHFIX"))
		throw std::runtime_error("ROT3D and IHFIX cannot be specified at the same time for TSTN.");
}

void TKeyCAM::parse(const std::vector<const std::string>& tokens, int line) 
{
	auto numTokens = tokens.size();
	
	if (numTokens < 4)
		throw std::runtime_error("Key *CAM takes at least two arguments: Positioned point ID  and InstrumentID.");
	
	// Initialize the station
	TCAM cam(fpoints.getObject(tokens.at(2)),
		finstruments.getDevice(finstruments.fPOLAR, tokens.at(3)));

	cam.line = line;

	// Parse the rest of the options
	TInstrumentData::TPOLAR& instrument(cam.instrument);

	TOptionHelper opts(tokens.cbegin()+2, tokens.cend());

	instrument.defTarget           = opts.getParamS("TRGT", instrument.defTarget);
	instrument.sigmaInstrCentering = opts.getParamRmm2m("ICSE", instrument.sigmaInstrCentering); //value given in mili-meters [mm], returned value in meters [m]
	
	//CAM station used
	fIsTSTN = false;
	proj.getCurrentNode().measurements.fCAM.emplace_back(cam);
}


///////////////////////
//   TSTN MEAS       //
///////////////////////
void TKeyV0::parse(const std::vector<const std::string>& tokens, int)
{
	TOptionHelper opts(tokens.cbegin()+2, tokens.cend());
	// get a reference to the modifyable station copy
	auto& stn(getStation());

	//*V0 can only occure below *TSTN
	if (!fIsTSTN)
		throw std::runtime_error("V0 keyword can not be used under *CAM, enabled only under *TSTN!");

	// Update the default target if specified or take one from TSTN (stn.defTarget)
	const auto& tgt(finstruments.getDevice(stn.targets, opts.getParamS("TRGT", stn.defTarget)));
	//Prepare a name of an adjustable angle (V0) = Frame name + V0 + numberOfAngle
	string angleName = proj.getCurrentNode().frame.getName() + "V0" + std::to_string(proj.angles.numObjects());
	// Create a new ROM (round of measurements) for the current station with the given default target, v0 is set to be zero
	TTSTN::TROM rom (tgt, &proj.angles.addObject(TAdjustableAngle(LGC::TAngle(LGC::TAngle::kGons, 0.0), false, angleName)));

	currentTargetApplied = &tgt;

	// set a constant orientation if defined
	if (opts.has("ACST"))
		rom.acst.set(LGC::TAngle::kGons, opts.getParamR("ACST"));	//Value in the input file given in GON
	else
		rom.acst.set(LGC::TAngle::kRadians, stn.constAngle); // Value stored in RAD
				
	// Add the ROM
	proj.getCurrentNode().measurements.fTSTN.back().roms.emplace_back(rom);
}

void TKeyPLR3D::parse(const std::vector<const std::string>& tokens, int line) 
{
	using namespace LGC;

	if (! updateDefaultTarget(tokens)) {
		//We get here if line does NOT start with a "*"
		if (tokens.size() < 4)
			throw std::runtime_error("A PLR3D measurement must have at least 4 entries: "
									 "The observed point and 3 measurement values.");
		
		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin()+4, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a station reference to update default values
		auto& stn(getStation());

		// update the default target if specified
		std::string currentTarget = currentTargetApplied->ID;				
		if (opts.has("TRGT"))
			currentTarget = opts.getParam("TRGT");

		//check whether currentTarget is between targets of this station, throw exception  otherwise
		currentTargetApplied = &finstruments.getDevice(stn.targets, currentTarget);	
		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(stn.targets, currentTarget));
		
		// set optional target modifications
		tgt.targetHt             = opts.getParamR("TH",   tgt.targetHt);
		tgt.sigmaTargetHt        = opts.getParamRmm2m("THSE", tgt.sigmaTargetHt);
		tgt.sigmaTargetCentering = opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering);
	
		// optionally change target sigmas
		tgt.sigmaAngl = opts.getParamRcc2rad("ASE", tgt.sigmaAngl);
		tgt.sigmaZenD = opts.getParamRcc2rad("ZSE", tgt.sigmaZenD);
		tgt.sigmaDist = opts.getParamRmm2m("DSE", tgt.sigmaDist);
		tgt.ppmDist   = opts.getParamRmm2m("PPM", tgt.ppmDist);
		
		// set measurement values
		TPLR3D plr(obspt, tgt);
		plr.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			plr.eolcomment = tokens.back();

		// set indices of LS matrices, PLR3D introduces 3 equations and 3 observations
		plr.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		plr.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex +=3;
		proj.fUEOIndices.OIndex +=3;
		proj.addToMeasurementNum(TMeasurementsGlobal::kPLR3D);

		proj.setCombinedCaseCalcUsed();   //PLR3D measurement processed, need to use Combined Case LS calculation

		if (! proj.getCurrentNode().measurements.fignoreValues) {
			plr.setAngle(LGC::TAngle(LGC::TAngle::kGons, std::stor(tokens.at(1))),kANGL);
			plr.setAngle(LGC::TAngle(LGC::TAngle::kGons, std::stor(tokens.at(2))),kZEND);
			plr.setScalar(std::stor(tokens.at(3)));
		}
		//Ad this PLR3D measurement either to TSTN ROM or to CAM measurements
		if(fIsTSTN)
			getROM().measPLR3D.emplace_back(plr);
		else
			getCAM().measPLR3D.emplace_back(plr);	
	}
}

void TKeyDIR3D::parse(const std::vector<const std::string>& tokens, int line)
{
	using namespace LGC;
	if (! updateDefaultTarget(tokens)) {
		//We get here if line does NOT start with a "*"
		if (tokens.size() < 3)
			throw std::runtime_error("A DIR3D measurement must have at least 3 entries: "
									 "The observed point and 2 measurement values.");
		
		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin()+3, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a station reference to update default values
		auto& stn(getStation());

		// update the default target if specified
		std::string currentTarget = currentTargetApplied->ID;
		if (opts.has("TRGT"))
			currentTarget = opts.getParam("TRGT");

		currentTargetApplied = &finstruments.getDevice(stn.targets, currentTarget);

		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(stn.targets, currentTarget));
		
		// set optional target modifications
		tgt.targetHt             = opts.getParamR("TH",   tgt.targetHt);
		tgt.sigmaTargetHt        = opts.getParamRmm2m("THSE", tgt.sigmaTargetHt);
		tgt.sigmaTargetCentering = opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering);
	
		// optionally change target sigmas
		tgt.sigmaAngl = opts.getParamRcc2rad("ASE", tgt.sigmaAngl);
		tgt.sigmaZenD = opts.getParamRcc2rad("ZSE", tgt.sigmaZenD);

		// set measurement values
		TDIR3D dir(obspt, tgt);
		dir.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			dir.eolcomment = tokens.back();

		// set indices of LS matrices, DIR3D introduces 3 equations and 2 observations
		dir.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		dir.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex +=3;
		proj.fUEOIndices.OIndex +=2;
		proj.addToMeasurementNum(TMeasurementsGlobal::kDIR3D);

		proj.setCombinedCaseCalcUsed();   //DIR3D measurement processed, need to use Combined Case LS calculation

		if (! proj.getCurrentNode().measurements.fignoreValues) {
			dir.setAngle(LGC::TAngle(LGC::TAngle::kGons, std::stor(tokens.at(1))),kANGL);
			dir.setAngle(LGC::TAngle(LGC::TAngle::kGons, std::stor(tokens.at(2))),kZEND);
		}
		//Add this DIR3D measurement either to TSTN ROM or to CAM measurements
		if(fIsTSTN)
			getROM().measDIR3D.emplace_back(dir);
		else
			getCAM().measDIR3D.emplace_back(dir);
	}
}

void TKeyANGL::parse(const std::vector<const std::string>& tokens, int line)
{
	using namespace LGC;
	if(!fIsTSTN)
		throw std::runtime_error("ANGL measurement can only appear under *TSTN and *ROM keywords.");

	if (! updateDefaultTarget(tokens)) {
		if (tokens.size() < 2)
			throw std::runtime_error("An ANGL measurement must have at least 2 entries: "
									 "The observed point and the measured angle.");

		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin()+2, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a station reference to update default values
		auto& stn(getStation());			

		// update the default target if specified
		std::string currentTarget = currentTargetApplied->ID;
		if (opts.has("TRGT"))
			currentTarget = opts.getParam("TRGT");

		currentTargetApplied = &finstruments.getDevice(stn.targets, currentTarget);

		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(stn.targets, currentTarget));
		
		tgt.sigmaAngl = opts.getParamRcc2rad("OBSE",  tgt.sigmaAngl);
		tgt.sigmaTargetCentering = opts.getParamRmm2m("TCSE",  tgt.sigmaTargetCentering);
		
		// set measurement value
		TANGL angl(obspt, tgt);
		angl.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			angl.eolcomment = tokens.back();

		// set indices of LS matrices, ANGL introduces 1 equation and 1 observation, index of observation is not stored since it is not used, but need to be counted
		angl.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		angl.setFirstObservationIndex(proj.fUEOIndices.OIndex);

		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kANGL);

		if (! proj.getCurrentNode().measurements.fignoreValues) 
			angl.setAngle(LGC::TAngle(LGC::TAngle::kGons, std::stor(tokens.at(1))));

		getROM().measANGL.emplace_back(angl);
	}
}

void TKeyZEND::parse(const std::vector<const std::string>& tokens, int line)
{	
	using namespace LGC;

	if(!fIsTSTN)
		throw std::runtime_error("ZEND measurement can only appear under *TSTN and *ROM keywords.");

	if (! updateDefaultTarget(tokens)) {
		if (tokens.size() < 2)
			throw std::runtime_error("A ZEND measurement must have at least 2 entries: "
									 "The observed point and the measured angle.");
		
		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin()+2, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a station reference to update default values
		auto& stn(getStation());

		// update the default target if specified
		std::string currentTarget = currentTargetApplied->ID;
		if (opts.has("TRGT"))
			currentTarget = opts.getParam("TRGT");

		currentTargetApplied = &finstruments.getDevice(stn.targets, currentTarget);

		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(stn.targets, currentTarget));
		
		// set optional target modifications
		tgt.sigmaZenD            = opts.getParamRcc2rad("OBSE", tgt.sigmaZenD);
		tgt.targetHt             = opts.getParamR("TH",   tgt.targetHt);
		tgt.sigmaTargetHt        = opts.getParamRmm2m("THSE", tgt.sigmaTargetHt);
		tgt.sigmaTargetCentering = opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering);

		// set measurement value
		TZEND zend(obspt, tgt);
		zend.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			zend.eolcomment = tokens.back();

		// set indices of LS matrices, ZEND introduces 1 equation and 1 observation, index of observation is not stored since it is not used, but need to be counted
		zend.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		zend.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kZEND);

		if (! proj.getCurrentNode().measurements.fignoreValues) 
			zend.setAngle(LGC::TAngle(LGC::TAngle::kGons, std::stor(tokens.at(1))));

		getROM().measZEND.emplace_back(zend);
	}
}

void TKeyDIST::parse(const std::vector<const std::string>& tokens, int line)
{
	if(!fIsTSTN)
		throw std::runtime_error("DIST measurement can only appear under *TSTN and *ROM keywords.");

	if (! updateDefaultTarget(tokens)) {
		if (tokens.size() < 2)
			throw std::runtime_error("A DIST measurement must have at least 2 entries: "
									 "The observed point and the measured distance.");
		
		// prepare the options analysis
	   TOptionHelper opts(tokens.cbegin()+2, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a station reference to update default values
		auto& stn(getStation());

		// update the default target if specified
		std::string currentTarget = currentTargetApplied->ID;
		if (opts.has("TRGT"))
			currentTarget = opts.getParam("TRGT");

		currentTargetApplied = &finstruments.getDevice(stn.targets, currentTarget);

		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(stn.targets, currentTarget));
		
		tgt.sigmaDist     = opts.getParamRmm2m("OBSE",   tgt.sigmaDist);
		tgt.ppmDist       = opts.getParamRmm2m("PPM",    tgt.ppmDist);
		tgt.targetHt      = opts.getParamR("TH",     tgt.targetHt);
		tgt.sigmaTargetHt        = opts.getParamRmm2m("THSE",   tgt.sigmaTargetHt);
		tgt.sigmaTargetCentering = opts.getParamRmm2m("TCSE",   tgt.sigmaTargetCentering);

		// Store  the measured value
		getROM().measDIST.emplace_back(
			TLINE(obspt, tgt, proj.getCurrentNode().measurements.fignoreValues?
												  NO_VALf : std::stor(tokens.at(1)))
		);

		//get a reference to the inserted measurement
	   auto& dist(getROM().measDIST.back());

		dist.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			dist.eolcomment = tokens.back();

		// set indices of LS matrices, DIST introduces 1 equation and 1 observation, index of observation is not stored since it is not used, but need to be counted
		dist.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		dist.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kDIST);
	}
}

void TKeyECTH::parse(const std::vector<const std::string>& tokens, int line)
{
	if(!fIsTSTN)
		throw std::runtime_error("ECTH measurement can only appear under *TSTN and *ROM keywords.");

	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline) {
		// look up station point and EDM ID
		if (tokens.size() < 4)
			throw std::runtime_error("An ECTH measurement must have two 2 entries: "
									 "The observed horizontal angle defining the reference plane and ID of a SCALE instrument.");
	
		fObservedAngle = LGC::TAngle(LGC::TAngle::kGons, std::stor(tokens.at(2)));
		fScaleInstID =  tokens.at(3);
	}
	else{
		if (tokens.size() < 2)
			throw std::runtime_error("An ECTH measurement must have at least 2 entries: "
									 "The observed point and the measured distance.");
		// prepare the options analysis
	   TOptionHelper opts(tokens.cbegin()+2, tokens.cend());

		// look up the stationed point
		const auto& stPoint(fpoints.getObject(tokens.at(0)));
	   
		//If SCALE used => update current SCALE instrument
	   if(opts.has("SCALE"))
		  fScaleInstID = opts.getParam("SCALE");

		// get a copy of  the specified instrument and update it
		auto scaleInstr(finstruments.getDevice(finstruments.fSCALE, fScaleInstID));

	   scaleInstr.sigmaD  = opts.getParamRmm2m("OBSE", scaleInstr.sigmaD);
	   scaleInstr.ppmD  = opts.getParamRmm2m("PPM", scaleInstr.ppmD);
	   scaleInstr.sigmaInstrCentering  = opts.getParamRmm2m("ICSE", scaleInstr.sigmaInstrCentering);
	    

	   // Store  the measured value
	   getROM().measECTH.emplace_back(TECTH(stPoint, scaleInstr, fObservedAngle, proj.getCurrentNode().measurements.fignoreValues?
												  NO_VALf : std::stor(tokens.at(1))));

	   //get a reference to the inserted measurement
	   auto& ecth(getROM().measECTH.back());

	   ecth.line = line;
	   	//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			ecth.eolcomment = tokens.back();

	   // set indices of LS matrices, ECTH introduces 1 equation and 1 observation
	   ecth.setFirstEquationIndex(proj.fUEOIndices.EIndex);
	   ecth.setFirstObservationIndex(proj.fUEOIndices.OIndex);
	   proj.fUEOIndices.EIndex++;
	   proj.fUEOIndices.OIndex++;
	   proj.addToMeasurementNum(TMeasurementsGlobal::kECTH);
	}
}

void TKeyDHOR::parse(const std::vector<const std::string>& tokens, int line)
{
	if(!fIsTSTN)
		throw std::runtime_error("DHOR measurement can only appear under *TSTN and *ROM keywords.");

	if (! updateDefaultTarget(tokens)) {
		if (tokens.size() < 2)
			throw std::runtime_error("A DHOR measurement must have at least 2 entries: "
									 "The observed point and the measured horizontal distance.");
		
		TOptionHelper opts(tokens.cbegin()+2, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a station reference to update default values
		auto& stn(getStation());

		// update the default target if specified
		std::string currentTarget = currentTargetApplied->ID;
		if (opts.has("TRGT"))
			currentTarget = opts.getParam("TRGT");

		currentTargetApplied = &finstruments.getDevice(stn.targets, currentTarget);

		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(stn.targets, currentTarget));

		tgt.sigmaDist = opts.getParamRmm2m("OBSE",   tgt.sigmaDist);
		tgt.ppmDist   = opts.getParamRmm2m("PPM",    tgt.ppmDist);
		tgt.sigmaTargetCentering = opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering);

		// Store the measured value
		getROM().measDHOR.emplace_back(
			TLINE(obspt, tgt, proj.getCurrentNode().measurements.fignoreValues?
												  NO_VALf : std::stor(tokens.at(1)))
		);

		//get a reference to the inserted measurement
		auto& dhor(getROM().measDHOR.back());
		dhor.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			dhor.eolcomment = tokens.back();

		// set indices of LS matrices, DHOR introduces 1 equation and 1 observation, index of observation is not stored since it is not used, but need to be counted
		dhor.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		dhor.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kDHOR);
	}
}

///////////////////////
//   NON TSTN MEAS   //
///////////////////////
void TKeyDSPT::parse(const std::vector<const std::string>& tokens, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	TOptionHelper opts(tokens.cbegin(), tokens.cend());

	if (firstline) {
		// look up station point and EDM ID
		if (tokens.size() < 4)
			throw std::runtime_error("Not enough entries for a DSPT station, "
			                         "needs stationed point and instrument ID.");

		
		// Initialize the station
		TEDM edm(fpoints.getObject(tokens.at(2)),
			finstruments.getDevice(finstruments.fEDM, tokens.at(3)));
		
		// get a reference to modify the default values for this station
		auto& instrument(edm.instrument);
		
		instrument.defTarget   = opts.getParamS("TRGT", instrument.defTarget);
		instrument.instrHeight = opts.getParamR("IH",   instrument.instrHeight);
		instrument.sigmaInstrHeight    = opts.getParamRmm2m("IHSE", instrument.sigmaInstrHeight);
		instrument.sigmaInstrCentering = opts.getParamRmm2m("ICSE", instrument.sigmaInstrCentering);

		//Store adjustable scalar for an instrument height in a global collection, and pointer to this object in edm measurement, height is assumed to be variable
		stringstream ss;
		ss << proj.getCurrentNode().measurements.fEDM.size();
		edm.instrumentHeightAdjustable = &fscalars.addObject(TAdjustableScalar(TScalar(instrument.instrHeight),false, proj.getCurrentNode().frame.getName() + edm.instrument.ID + ss.str()));

		proj.getCurrentNode().measurements.fEDM.emplace_back(edm);
		proj.getCurrentNode().measurements.fEDM.back().line = line;
	} 
	else {
		if (tokens.size() < 2)
			throw std::runtime_error("A DSPT measurement must have at least 2 entries: "
									 "The observed point and the measured distance.");
		
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a station reference to update default values
		TInstrumentData::TEDM& instrument = proj.getCurrentNode().measurements.fEDM.back().instrument;

		// update the default target if specified
		instrument.defTarget  = opts.getParamS("TRGT", instrument.defTarget);
		// get a copy of  the specified target and update it
		TInstrumentData::TEDM::TTarget tgt(finstruments.getDevice(instrument.targets, instrument.defTarget));
		
		tgt.sigmaDSpt     = opts.getParamRmm2m("OBSE", tgt.sigmaDSpt);
		tgt.ppmDSpt       = opts.getParamRmm2m("PPM",  tgt.ppmDSpt);
		tgt.targetHt      = opts.getParamR("TH",   tgt.targetHt);
		tgt.sigmaTargetHt       = opts.getParamRmm2m("THSE", tgt.sigmaTargetHt);
		tgt.sigmaTargetCentering = opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering);

		// Store  the measured value
		proj.getCurrentNode().measurements.fEDM.back().measDSPT.emplace_back(
			TDSPT(obspt, tgt, proj.getCurrentNode().measurements.fignoreValues?
												 NO_VALf : std::stor(tokens.at(1)))
		);

		//get a reference to the inserted measurement
		auto& dpst(proj.getCurrentNode().measurements.fEDM.back().measDSPT.back());
		dpst.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			dpst.eolcomment = tokens.back();

		// set indices of LS matrices, DSPT introduces 1 equation and 1 observation, index of observation is not stored since it is not used, but need to be counted
		dpst.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		dpst.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kDSPT);
	}
}

void TKeyDVER::parse(const std::vector<const std::string>& tokens, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");

	//On first line nothing appears so far: to be discussed
	if (firstline) {}  
	else {
		TOptionHelper opts(tokens.cbegin()+2, tokens.cend());

		if (tokens.size() < 3)
			throw std::runtime_error("A DVER measurement must have at least 3 entries: "
									 "Two points and the measured distance.");

		// Store  the measured value
		proj.getCurrentNode().measurements.fDVER.emplace_back(
			TDVER(fpoints.getObject(tokens.at(0)), fpoints.getObject(tokens.at(1)), proj.getCurrentNode().measurements.fignoreValues?
												 NO_VALf : std::stor(tokens.at(2)))
			);

		auto& dver(proj.getCurrentNode().measurements.fDVER.back());

		dver.setObservedStDev(opts.getParamRmm2m("OBSE", proj.getCurrentNode().measurements.fDVER.back().getObservedStDev()));
		dver.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			dver.eolcomment = tokens.back();


		dver.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		dver.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kDVER);
	}
}

void TKeyDLEV::parse(const std::vector<const std::string>& tokens, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	TOptionHelper opts(tokens.cbegin()+2, tokens.cend());  
	
	if (firstline) {
		if (tokens.size() < 3)
			throw std::runtime_error("A DLEV group of measurements must have at least 1 entry, the levelling instrument ID");

		const std::bitset<3> planeNormalVectLock(std::string("111")); //fixed state of the plane's normal vector
		stringstream ss;
		ss << proj.getCurrentNode().measurements.fLEVEL.size();
		const std::string& name = "DLEV" + ss.str(); //name of the measured adjustable plane

		if(opts.has("RefPt")){
			std::string  rpName = opts.getParamS("RefPt", "NULL"); 
			
			if (!fpoints.doesObjectExist(rpName))
				throw std::runtime_error("Point" +  rpName + "used as reference point in DLEV measurement, must be declared before used");

			fplanes.addObject(TAdjustablePlane(&fpoints.getObject(rpName), TScalar(0.0), TFreeVector(0,0,1,TCoordSysFactory::k3DCartesian), planeNormalVectLock,name));
		}
		else
			fplanes.addObject(TAdjustablePlane::createUninitialized(name));

		TLEVEL level(fplanes.back(),finstruments.getDevice(finstruments.fLEVEL, tokens.at(2)));
		level.line = line;

		proj.getCurrentNode().measurements.fLEVEL.emplace_back(level); //add new measurement
	}
	else{
		if (tokens.size() < 2)
			throw std::runtime_error("A DLEV measurement must have at least 2 entries: stationed point ID and observed vertical distance");

		const auto& tgtfPoint(fpoints.getObject(tokens.at(0)));
		TLEVEL& levelGrOfMeas = proj.getCurrentNode().measurements.fLEVEL.back();
		levelGrOfMeas.instrument.defStaffID = opts.getParamS("TRGT",  levelGrOfMeas.instrument.defStaffID); //Overrides current target (staff) ID, current value

		TInstrumentData::TLEVEL::TTarget tgt = finstruments.getDevice(levelGrOfMeas.instrument.targets, levelGrOfMeas.instrument.defStaffID);

		tgt.sigmaD = opts.getParamRmm2m("OBSE", tgt.sigmaD);
		tgt.ppmD = opts.getParamRmm2m("PPM", tgt.ppmD);
		tgt.staffHt = opts.getParamR("TH", tgt.staffHt); //Vertical offset of the staff == height
		tgt.sigmaStaffHt = opts.getParamRmm2m("THSE", tgt.sigmaStaffHt);


		TDLEV dlev(tgtfPoint, tgt, std::stor(tokens.at(1)));

		TReal horDist  = opts.getParamR("DHOR",  NO_VALf);
		if(!_isnan(horDist)){
			 dlev.setHorizontalDistance(horDist);
			 //Horizontal distance given, DHOR measurement is introduced
			 TDHOR dhor(tgtfPoint, tgt, horDist);
			 dhor.setFirstEquationIndex(proj.fUEOIndices.EIndex);
			 dhor.setFirstEquationIndex(proj.fUEOIndices.OIndex);

			 levelGrOfMeas.measDHOR.emplace_back(dhor);

		     proj.fUEOIndices.EIndex++;
			 proj.fUEOIndices.OIndex++;
			 proj.addToMeasurementNum(TMeasurementsGlobal::kDLEV);
		}

		TReal horDistSigma  = opts.getParamRmm2m("DSE",  NO_VALf);

		if(!_isnan(horDistSigma))
			 dlev.setHorizontalDistanceSigma(horDistSigma);

		dlev.setEquationIndex(proj.fUEOIndices.EIndex);
		dlev.setObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;

		dlev.line = line;
		levelGrOfMeas.measDLEV.emplace_back(dlev);
	}
}

