#include "MeasurementReaders.h"
#include <TMeasurements.h>
#include <string>
#include <sstream>

const TReal lengthTolerance = 1e-5;

namespace{
    bool isNumber(const std::string &str){
        try{
            std::stod(str);
        } catch(...){
            return false;
        }
        return true;
    }
}

TAMeasurementKey::TAMeasurementKey(TLGCData& project, const std::string& key) : 
			TAKeyWord(key, project),
			fpoints(project.getPoints()),
			finstruments(project.getInstruments()),
			flengths(project.getLength()),
			fangles(project.getAngles()),
			fplanes(project.getPlanes()),
			flines(project.getLines())
			{}

bool TAMeasurementKey::updateDefaultTargetTSTN(const std::vector<std::string>& tokens) 
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*"); //If true, then it is a first line of the measurement definition, i.e. PLR3D, ZEND, DIST,etc.
	
	try{
		//If TRGT keyword used,  target has to be updated.
		if (firstline && tokens.size() == 4 && tokens.at(2) == "TRGT")
			currentTargetApplied = tokens.at(3);
		else if(firstline) //Line starts with '*', but TRGT keyword not used, i.e. taking default taget value from ROM
			currentTargetApplied = getROM()->defaultTargetId;
	}
	catch(std::exception e) { }

	return firstline;
}

void TKeyTSTN::parse(const std::vector<std::string>& tokens, bool activeLine, int line) 
{
	auto numTokens = tokens.size();
	using namespace LGC;

	if (numTokens < 4)
		throw std::runtime_error("Key *TSTN takes at least two arguments: Positioned point ID  and InstrumentID.");
	
	// Initialize the station
	std::shared_ptr<TTSTN> tstn = std::make_shared<TTSTN>(fpoints.getObject(tokens.at(2)),
		finstruments.getDevice(finstruments.fPOLAR, tokens.at(3)));
	tstn->line = line;
    tstn->setActive(activeLine);

	// Parse the rest of the options
	TInstrumentData::TPOLAR& instrument(tstn->instrument);

	TOptionHelper opts(tokens.cbegin()+3, tokens.cend());

	tstn->ihfix = opts.has("IHFIX");
	// Look for optional "IH" and "IHSE" flags only if the "IHFIX" flag used, ignore otherwise
	if(tstn->ihfix){
		instrument.instrHeight    = TLength(opts.getParamR("IH",   instrument.instrHeight));
      instrument.sigmaInstrHeight = TLength(opts.getParamRmm2m("IHSE", instrument.sigmaInstrHeight));
	}

	tstn->rot3D = opts.has("ROT3D");

	instrument.defTarget           = opts.getParamS("TRGT", instrument.defTarget);
	instrument.sigmaInstrCentering = TLength(opts.getParamRmm2m("ICSE", instrument.sigmaInstrCentering));

	//emplace this station
	proj.getCurrentNode().measurements.fTSTN.emplace_back(tstn);

	if (tstn->ihfix && tstn->rot3D)
		throw std::runtime_error("ROT3D and IHFIX cannot be specified at the same time for TSTN.");
}

void TKeyCAM::parse(const std::vector<std::string>& tokens, bool activeLine, int line) 
{
	auto numTokens = tokens.size();
	
	if (numTokens < 4)
		throw std::runtime_error("Key *CAM takes at least two arguments: Positioned point ID  and InstrumentID.");
	
	// Initialize the station
	TCAM cam(fpoints.getObject(tokens.at(2)),
		finstruments.getDevice(finstruments.fCAMD, tokens.at(3)));

	cam.line = line;
    cam.setActive(activeLine);

	// Parse the rest of the options
	TInstrumentData::TCAMD& instrument(cam.instrument);

	TOptionHelper opts(tokens.cbegin()+3, tokens.cend());

	instrument.defTarget           = opts.getParamS("TRGT", instrument.defTarget);
	instrument.sigmaInstrCentering = TLength(opts.getParamR("ICSE", instrument.sigmaInstrCentering), TLength::EUnits::kMillimetres); //value given in mili-meters [mm], returned value in meters [m]
	
	//emplace this Camera
	proj.getCurrentNode().measurements.fCAM.emplace_back(cam);
}

///////////////////////
//   CAMERA MEAS       //
///////////////////////
void TKeyUVEC::parse(const std::vector<std::string>& tokens, bool activeLine, int line) 
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");

	if (firstline){ //If this is the line starting with *UVEC, where only default target can be changed, use the default from the CAM definition or override it
		TOptionHelper opts(tokens.cbegin()+1, tokens.cend());
		currentTargetApplied = opts.getParamS("TRGT", getCAM().instrument.defTarget);
        getCAM().uvecActive = getCAM().isActive() && activeLine; // Active only if station is active as well
	}
	else{//Get here, if the line does NOT start with a "*", it means that is the concrete measurement 
        bool hasAllParams = (tokens.size() > 3) && isNumber(tokens.at(1)) && isNumber(tokens.at(2)) && isNumber(tokens.at(3));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("A UVEC measurement must have at least 4 entries: "
									 "The observed point and 3 components of the unit vector.");
		
		// prepare the options analysis, i.e. searching for optional keywords
		TOptionHelper opts(tokens.cbegin()+1, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a camera reference to update default values
		auto& camera(getCAM());

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for(auto& point : camera.measUVEC)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("An UVEC measurement is duplicated");

		
		
		std::string currentTarget = currentTargetApplied; //Take the current target, which is used
		// Overwrite the target if specified and update the 'currentTargetApplied' to be used for upcoming measurements
		if (opts.has("TRGT")){
			currentTarget = opts.getParam("TRGT");
			currentTargetApplied = currentTarget;
		}

		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(camera.instrument.targets, currentTarget));	//If not found throws exception, that the target was not found
		
		tgt.sigmaTargetCentering = TLength(opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering));

		// optionally change target sigmas
		tgt.sigmaX = opts.getParamRmm2m("XSE", tgt.sigmaX );
		tgt.sigmaY = opts.getParamRmm2m("YSE", tgt.sigmaY );
		
		// set measurement value
		TUVEC uvec(obspt, tgt);
		uvec.line = line;
        uvec.setActive(getCAM().uvecActive && activeLine); // Active only if ROM is active as well

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			uvec.eolcomment = tokens.back();

		// Measured unit vector save, if it is not a simulation
		if (hasAllParams){
			TFreeVector vectorMeasurement(std::stor(tokens.at(1)),std::stor(tokens.at(2)),std::stor(tokens.at(3)),TCoordSysFactory::k3DCartesian);

			if(isZero(vectorMeasurement.getZ().getMetresValue()))
				throw std::runtime_error("Input of UVEC measurement is not correct: "
										 "Z coordinate of the unit vector can not be ZERO or close to it.");

         TReal vectorLength = vectorMeasurement.length().getMetresValue();

			if(!((1-lengthTolerance)<vectorLength && (1+lengthTolerance)>vectorLength)) //If vectorLength is not 1 with a given tolerance
				throw std::runtime_error("UVEC measurement input values are not correct: "
										 "Given vector is not a unit vector.");

			uvec.setVectorMeasurement(vectorMeasurement);
		}

		//Store this UVEC measurement
		getCAM().measUVEC.emplace_back(uvec);
	}
}


void TKeyUVD::parse(const std::vector<std::string>& tokens, bool activeLine, int line) 
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");

	if (firstline){ //If this is the line starting with *UVD, where only default target can be changed
		TOptionHelper opts(tokens.cbegin()+1, tokens.cend());
		currentTargetApplied = opts.getParamS("TRGT", getCAM().instrument.defTarget);
        getCAM().uvdActive = getCAM().isActive() && activeLine; // Active only if station is active as well
    }
	else{//Enter if the line does NOT start with a "*"
        bool hasAllParams = (tokens.size() > 4) && isNumber(tokens.at(1)) && isNumber(tokens.at(2)) && isNumber(tokens.at(3)) && isNumber(tokens.at(4));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("A UVD measurement must have at least 5 entries: "
									 "The observed point, 3 components of the unit vector and the spatial distance.");
		
		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin()+1, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a camera reference to update default values
		auto& camera(getCAM());

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : camera.measUVD)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("An UVD measurement is duplicated");
		
		std::string currentTarget = currentTargetApplied; //Take the current target, which is used
		// Overwrite the target if specified and update the 'currentTargetApplied' to be used for upcoming measurements
		if (opts.has("TRGT")){
			currentTarget = opts.getParam("TRGT");
			currentTargetApplied = currentTarget;
		}

		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(camera.instrument.targets, currentTarget));	//If not found throws exception, that the target was not found
		
		// optionally change target sigmas
		tgt.sigmaX = opts.getParamRmm2m("XSE", tgt.sigmaX);
		tgt.sigmaY = opts.getParamRmm2m("YSE", tgt.sigmaY);
		tgt.sigmaDist = TLength(opts.getParamRmm2m("DSE", tgt.sigmaDist));
		tgt.sigmaTargetCentering = TLength(opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering));
		// set measurement value
		TUVD uvd(obspt, tgt);
		uvd.line = line;
        uvd.setActive(getCAM().uvdActive && activeLine); // Active only if ROM is active as well

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			uvd.eolcomment = tokens.back();

		//Measured unit vector and distance save if it is not a simulation
		if (hasAllParams) {
			TFreeVector vectorMeasurement(std::stor(tokens.at(1)),std::stor(tokens.at(2)),std::stor(tokens.at(3)),TCoordSysFactory::k3DCartesian);

         if(isZero(vectorMeasurement.getZ().getMetresValue()))
				throw std::runtime_error("UVD measurement input values are not correct: "
										 "Z coordinate of the unit vector can not be ZERO or close to it.");

         TReal vectorLength = vectorMeasurement.length().getMetresValue();

			if(!((1-lengthTolerance)<vectorLength && (1+lengthTolerance)>vectorLength)) //If vectorLength is not 1 with a given tolerance
				throw std::runtime_error("UVD measurement input values are not correct: "
										 "Given vector is not a unit vector.");

			uvd.setVectorMeasurement(vectorMeasurement);
			uvd.setDistance(TLength(std::stor(tokens.at(4))));
		}

		proj.setCombinedCaseCalcUsed();
		//Store this UVEC measurement
		getCAM().measUVD.emplace_back(uvd);
	}
}


///////////////////////
//   TSTN MEAS       //
///////////////////////
void TKeyV0::parse(const std::vector<std::string>& tokens, bool activeLine, int)
{
	TOptionHelper opts(tokens.cbegin()+2, tokens.cend());
	// get a reference to the modifyable station copy
	auto& stn(getStation());

    // Update the default target, if specified, or take one from TSTN (stn.defTarget):
    // (NB. use the following function to get the standard error message)
    auto tgtId = finstruments.getDevice(stn.targets, opts.getParamS("TRGT", stn.defTarget)).ID;

    // Create the ROM:
	std::shared_ptr<TTSTN::TROM> rom = std::make_shared<TTSTN::TROM>(tgtId, nullptr);
    rom->setActive(proj.getCurrentNode().measurements.fTSTN.back()->isActive() && activeLine); // Active only if station is active as well

	// set a constant orientation if defined
	if (opts.has("ACST"))
		rom->acst.setGonsValue(opts.getParamR("ACST"));	//Value in the input file given in GON
	else
		rom->acst.setRadiansValue(stn.constAngle); // Value stored in RAD
				
	// Add the ROM
	proj.getCurrentNode().measurements.fTSTN.back()->roms.emplace_back(rom);
}

void TKeyPLR3D::parse(const std::vector<std::string>& tokens, bool activeLine, int line) 
{
	using namespace LGC;

    // If first line, update the active status of the PLR3D rom:
    if(updateDefaultTargetTSTN(tokens))
        getROM()->plrActive = getROM()->isActive() && activeLine; // Active only if ROM is active as well

    // Else handle the measurement line:
    else {
		//We get here if line does NOT start with a "*"
        bool hasAllParams = (tokens.size() > 3) && isNumber(tokens.at(1)) && isNumber(tokens.at(2)) && isNumber(tokens.at(3));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("A PLR3D measurement must have at least 4 entries: "
									 "The observed point and 3 measurement values.");
		
		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin()+1, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a station reference to update default values
		auto& stn(getStation());

		//Use the current target (either from V0 or from the previous measurement)
		std::string currentTarget = currentTargetApplied;
		// Overwrite the target if specified and update the 'currentTargetApplied' to be used for upcoming measurement
		if (opts.has("TRGT")){
			currentTarget = opts.getParam("TRGT");
			currentTargetApplied = currentTarget;
		}

		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(stn.targets, currentTarget));

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : getROM()->measPLR3D)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("A PLR3D measurement is duplicated");
		
		// set optional target modifications
		tgt.targetHt             = TLength(opts.getParamR("TH", tgt.targetHt));
		tgt.sigmaTargetHt        = TLength(opts.getParamRmm2m("THSE", tgt.sigmaTargetHt));
		tgt.sigmaTargetCentering = TLength(opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering));
	
		// optionally change target sigmas
		tgt.sigmaAngl = TAngle(opts.getParamRcc2rad("ASE", tgt.sigmaAngl));
		tgt.sigmaZenD = TAngle(opts.getParamRcc2rad("ZSE", tgt.sigmaZenD));
		tgt.sigmaDist = TLength(opts.getParamRmm2m("DSE", tgt.sigmaDist));
		tgt.ppmDist   = TLength(opts.getParamRmm2m("PPM", tgt.ppmDist));
		
		// set measurement values
		TPLR3D plr(obspt, tgt);
		plr.line = line;
        plr.setActive(getROM()->plrActive && activeLine); // Active only if ROM is active as well

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			plr.eolcomment = tokens.back();

		proj.setCombinedCaseCalcUsed();   //PLR3D measurement processed, need to use Combined Case LS calculation


		if (hasAllParams) { //Store value if it is not a simulation
			
			// (VV) Here we will check whether the angle measurements are performed 
			// (VV) in the first face (left circle) or in the second face (right circle) of the theodolite (total station, tracker).
			// (VV) The tokens arleady contain the angle measurements in radians in the range (-pi, pi), therefore,
			// (VV) the zenith angle in the first face is positive and the zenith anlge in the second face is negative.

			// (VV) If the zenith angle is positive (or equal to zero) then just copy the angle values.
			if (TAngle(std::stor(tokens.at(2)), TAngle::kGons) >= TAngle(0, TAngle::kGons)) { // 
				plr.setAngle(TAngle(std::stor(tokens.at(1)), TAngle::kGons), kANGL);
				plr.setAngle(TAngle(std::stor(tokens.at(2)), TAngle::kGons), kZEND);
			}
			// (VV) If the zenith angle is negative (right circle) then we store the equivalent values of the left circle.
			else {
				plr.setAngle(TAngle(std::stor(tokens.at(1)), TAngle::kGons) + TAngle(200.0, TAngle::kGons), kANGL);  // (VV) either -200 or +200 has the same result.
				plr.setAngle(TAngle(400.0, TAngle::kGons) - TAngle(std::stor(tokens.at(2)), TAngle::kGons), kZEND);
			}
			plr.setDistance(TLength(std::stor(tokens.at(3))));
		}
		//Ad this PLR3D measurement to TSTN's ROM 
		getROM()->measPLR3D.emplace_back(plr);

	}
}


void TKeyANGL::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
	using namespace LGC;

    // If first line, update the active status of the ANGL rom:
    if(updateDefaultTargetTSTN(tokens))
        getROM()->anglActive = getROM()->isActive() && activeLine; // Active only if ROM is active as well

    // Else handle the measurement line:
    else {
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("An ANGL measurement must have at least 2 entries: "
									 "The observed point and the measured angle.");

		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin() + 1, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a station reference to update default values
		auto& stn(getStation());

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : getROM()->measANGL)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("A ANGL measurement is duplicated");

		//Use the current target (either from V0 or from the previous measurement)
		std::string currentTarget = currentTargetApplied;
		// Overwrite the target if specified and update the 'currentTargetApplied' to be used for upcoming measurement
		if (opts.has("TRGT")){
			currentTarget = opts.getParam("TRGT");
			currentTargetApplied = currentTarget;
		}

		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(stn.targets, currentTarget));
		
      tgt.sigmaAngl = TAngle(opts.getParamRcc2rad("OBSE", tgt.sigmaAngl));
      tgt.sigmaTargetCentering = TLength(opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering));
		
		// set measurement value
		TANGL angl(obspt, tgt);
		angl.line = line;
        angl.setActive(getROM()->anglActive && activeLine); // Active only if ROM is active as well

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			angl.eolcomment = tokens.back();

		if (hasAllParams)
			angl.setAngle(TAngle(std::stor(tokens.at(1)), TAngle::kGons));

		getROM()->measANGL.emplace_back(angl);
	}
}

void TKeyZEND::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{	
	using namespace LGC;

    // If first line, update the active status of the ZEND rom:
    if(updateDefaultTargetTSTN(tokens))
        getROM()->zendActive = getROM()->isActive() && activeLine; // Active only if ROM is active as well

    // Else handle the measurement line:
    else {
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("A ZEND measurement must have at least 2 entries: "
									 "The observed point and the measured angle.");
		
		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin()+1, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a station reference to update default values
		auto& stn(getStation());

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : getROM()->measZEND)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("A ZEND measurement is duplicated");

		//Use the current target (either from V0 or from the previous measurement)
		std::string currentTarget = currentTargetApplied;
		// Overwrite the target if specified and update the 'currentTargetApplied' to be used for upcoming measurement
		if (opts.has("TRGT")){
			currentTarget = opts.getParam("TRGT");
			currentTargetApplied = currentTarget;
		}

		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(stn.targets, currentTarget));
		
		// set optional target modifications
		tgt.sigmaZenD            = TAngle(opts.getParamRcc2rad("OBSE", tgt.sigmaZenD));
		tgt.targetHt           = TLength(opts.getParamR("TH",   tgt.targetHt));
		tgt.sigmaTargetHt        = TLength(opts.getParamRmm2m("THSE", tgt.sigmaTargetHt));
		tgt.sigmaTargetCentering = TLength(opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering));

		// set measurement value
		TZEND zend(obspt, tgt);
		zend.line = line;
        zend.setActive(getROM()->zendActive && activeLine); // Active only if ROM is active as well

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			zend.eolcomment = tokens.back();

		
		if (hasAllParams) {

			// (VV) Here we will check whether the angle measurements are performed 
			// (VV) in the first face (left circle) or in the second face (right circle) of the theodolite (total station, tracker).
			// (VV) The tokens arleady contain the angle measurements in radians in the range (-pi, pi), therefore,
			// (VV) the zenith angle in the first face is positive and the zenith anlge in the second face is negative.

			// (VV) If the zenith angle is positive (or equal to zero) then just copy the angle values.
			if (TAngle(std::stor(tokens.at(1)), TAngle::kGons) >= TAngle(0, TAngle::kGons)) {
				zend.setAngle(TAngle(std::stor(tokens.at(1)), TAngle::kGons));
			}
			// (VV) If the zenith angle is negative (right circle) then we store the equivalent values of the left circle.
			else {
				zend.setAngle(TAngle(400.0, TAngle::kGons) - TAngle(std::stor(tokens.at(1)), TAngle::kGons));
			}
		}
		getROM()->measZEND.emplace_back(zend);
		
	}
}

void TKeyDIST::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
    // If first line, update the active status of the DIST rom:
    if(updateDefaultTargetTSTN(tokens))
        getROM()->distActive = getROM()->isActive() && activeLine; // Active only if ROM is active as well

    // Else handle the measurement line:
    else {
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("A DIST measurement must have at least 2 entries: "
									 "The observed point and the measured distance.");
		
		// prepare the options analysis
	   TOptionHelper opts(tokens.cbegin()+1, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a station reference to update default values
		auto& stn(getStation());

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : getROM()->measDIST)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("A DIST measurement is duplicated");

		//Use the current target (either from V0 or from the previous measurement)
		std::string currentTarget = currentTargetApplied;
		// Overwrite the target if specified and update the 'currentTargetApplied' to be used for upcoming measurement
		if (opts.has("TRGT")){
			currentTarget = opts.getParam("TRGT");
			currentTargetApplied = currentTarget;
		}

		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(stn.targets, currentTarget));
		
      tgt.sigmaDist            = TLength(opts.getParamRmm2m("OBSE", tgt.sigmaDist));
      tgt.ppmDist              = TLength(opts.getParamRmm2m("PPM", tgt.ppmDist));
      tgt.targetHt             = TLength(opts.getParamR("TH", tgt.targetHt));
      tgt.sigmaTargetHt        = TLength(opts.getParamRmm2m("THSE", tgt.sigmaTargetHt));
      tgt.sigmaTargetCentering = TLength(opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering));

		// Store  the measured value
		getROM()->measDIST.emplace_back(
         TLINE(obspt, tgt, TLength(!hasAllParams ? NO_VALf : std::stor(tokens.at(1))))
		);

		//get a reference to the inserted measurement
	   auto& dist(getROM()->measDIST.back());

		dist.line = line;
        dist.setActive(getROM()->distActive && activeLine); // Active only if ROM is active as well

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			dist.eolcomment = tokens.back();

	}
}

void TKeyECTH::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline) {
		// look up station point
		if (tokens.size() < 4)
			throw std::runtime_error("An ECTH measurement must have 2 entries: "
									 "The observed horizontal angle defining the reference plane and ID of a SCALE instrument.");
	
		fObservedAngle = TAngle(std::stor(tokens.at(2)), TAngle::kGons);
		fScaleInstID =  tokens.at(3);

		//The SCALE instrument is only the default one used, it is not stored in TECTH because it is specific for each observation
		currentTargetApplied = finstruments.getDevice(finstruments.fSCALE, fScaleInstID).ID;
        
        // Update the activation status of the ECTH rom:
        getROM()->ecthActive = getROM()->isActive() && activeLine; // Active only if ROM is active as well
	}
	else{
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("An ECTH measurement must have at least 2 entries: "
									 "The observed point and the measured distance.");
		// prepare the options analysis
	   TOptionHelper opts(tokens.cbegin()+1, tokens.cend());

		// look up the stationed point
		const auto& stPoint(fpoints.getObject(tokens.at(0)));

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : getROM()->measECTH)
				if (stPoint.getName() == point.targetPos->getName())
					throw std::runtime_error("A ECTH measurement is duplicated");
	   

		// Overwrite the target if specified and update the 'currentTargetApplied' to be used for upcoming measurement
		currentTargetApplied = opts.getParamS("SCALE", currentTargetApplied); //If SCALE is used then change ID of CurrentTargetApplied for the following measurements.
		TInstrumentData::TSCALE scaleInstr = finstruments.getDevice(finstruments.fSCALE, currentTargetApplied); //Throws exception if instrument not found, catched on the top level

		scaleInstr.sigmaD = TLength(opts.getParamRmm2m("OBSE", scaleInstr.sigmaD));
		scaleInstr.ppmD = TLength(opts.getParamRmm2m("PPM", scaleInstr.ppmD));
		scaleInstr.sigmaInstrCentering = TLength(opts.getParamRmm2m("ICSE", scaleInstr.sigmaInstrCentering));

	   // Store  the measured value
	   getROM()->measECTH.emplace_back(TECTH(stPoint, scaleInstr, fObservedAngle, TLength(!hasAllParams ? NO_VALf : std::stor(tokens.at(1)))));

	   //get a reference to the inserted measurement
	   auto& ecth(getROM()->measECTH.back());

	   ecth.line = line;
       ecth.setActive(getROM()->ecthActive && activeLine); // Active only if ROM is active as well

	   	//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			ecth.eolcomment = tokens.back();
	}
}

void TKeyECDIR::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline) {
		// look up station point
		if (tokens.size() < 5)
			throw std::runtime_error("An ECDIR measurement must have 3 entries: "
			"The horizontal and vertical angles defining the line and ID of a SCALE instrument.");

		fScaleInstID = tokens.at(4);
		fHorAngle = TAngle(std::stor(tokens.at(2)), TAngle::kGons);
		fVertAngle = TAngle(std::stor(tokens.at(3)), TAngle::kGons);

		//The SCALE instrument is only the default one used, it is not stored in TECTH because it is specific for each observation
		currentTargetApplied = finstruments.getDevice(finstruments.fSCALE, fScaleInstID).ID;

        // Update the activation status of the ECDIR rom:
        getROM()->ecdirActive = getROM()->isActive() && activeLine; // Active only if ROM is active as well
	}
	else{
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("An ECTH measurement must have at least 2 entries: "
			"The observed point and the measured distance.");
		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin() + 1, tokens.cend());

		// look up the stationed point
		const auto& stPoint(fpoints.getObject(tokens.at(0)));

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : getROM()->measECDIR)
				if (stPoint.getName() == point.targetPos->getName())
					throw std::runtime_error("A ECDIR measurement is duplicated");


		// Overwrite the target if specified and update the 'currentTargetApplied' to be used for upcoming measurement
		currentTargetApplied = opts.getParamS("SCALE", currentTargetApplied); //If SCALE is used then change ID of CurrentTargetApplied for the following measurements.
		TInstrumentData::TSCALE scaleInstr = finstruments.getDevice(finstruments.fSCALE, currentTargetApplied); //Throws exception if instrument not found, catched on the top level

		scaleInstr.sigmaD = TLength(opts.getParamRmm2m("OBSE", scaleInstr.sigmaD));
		scaleInstr.ppmD = TLength(opts.getParamRmm2m("PPM", scaleInstr.ppmD));
		scaleInstr.sigmaInstrCentering = TLength(opts.getParamRmm2m("ICSE", scaleInstr.sigmaInstrCentering));

		// Store  the measured value
		getROM()->measECDIR.emplace_back(TECDIR(stPoint, scaleInstr, fHorAngle, fVertAngle, TLength(!hasAllParams ? NO_VALf : std::stor(tokens.at(1)))));

		//get a reference to the inserted measurement
		auto& ecdir(getROM()->measECDIR.back());

		ecdir.line = line;
        ecdir.setActive(getROM()->ecdirActive && activeLine); // Active only if ROM is active as well

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			ecdir.eolcomment = tokens.back();
	}
}

void TKeyDHOR::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
    // If first line, update the active status of the DHOR rom:
    if(updateDefaultTargetTSTN(tokens))
        getROM()->dhorActive = getROM()->isActive() && activeLine; // Active only if ROM is active as well

    // Else handle the measurement line:
    else {
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("A DHOR measurement must have at least 2 entries: "
									 "The observed point and the measured horizontal distance.");
		
		TOptionHelper opts(tokens.cbegin()+1, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));
		// get a station reference to update default values
		auto& stn(getStation());

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : getROM()->measDHOR)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("A DHOR measurement is duplicated");

		//Use the current target (either from V0 or from the previous measurement)
		std::string currentTarget = currentTargetApplied;
		// Overwrite the target if specified and update the 'currentTargetApplied' to be used for upcoming measurement
		if (opts.has("TRGT")){
			currentTarget = opts.getParam("TRGT");
			currentTargetApplied = currentTarget;
		}

		// get a copy of  the specified target and update it
		auto tgt(finstruments.getDevice(stn.targets, currentTarget));

      tgt.sigmaDist = TLength(opts.getParamRmm2m("OBSE", tgt.sigmaDist));
      tgt.ppmDist = TLength(opts.getParamRmm2m("PPM", tgt.ppmDist));
      tgt.sigmaTargetCentering = TLength(opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering));

		// Store the measured value
		getROM()->measDHOR.emplace_back(
         TLINE(obspt, tgt, TLength(!hasAllParams ? NO_VALf : std::stor(tokens.at(1))))
		);

		//get a reference to the inserted measurement
		auto& dhor(getROM()->measDHOR.back());
		dhor.line = line;
        dhor.setActive(getROM()->dhorActive && activeLine); // Active only if ROM is active as well

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			dhor.eolcomment = tokens.back();
	}
}

///////////////////////
//   NON TSTN MEAS   //
///////////////////////
void TKeyDSPT::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	//TOptionHelper opts(tokens.cbegin(), tokens.cend());

	if (firstline) {
		// look up station point and EDM ID
		if (tokens.size() < 4)
			throw std::runtime_error("Not enough entries for a DSPT station, "
			                         "needs stationed point and instrument ID.");

		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin() + 3, tokens.cend());
		// Initialize the station
		TEDM edm(fpoints.getObject(tokens.at(2)),
			finstruments.getDevice(finstruments.fEDM, tokens.at(3)));
        edm.line = line;
        edm.setActive(activeLine);
		
		// get a reference to modify the default values for this station
		auto& instrument(edm.instrument);

        instrument.defTarget = opts.getParamS("TRGT", instrument.defTarget);
        instrument.instrHeight = TLength(opts.getParamR("IH", instrument.instrHeight));
        instrument.sigmaInstrHeight = TLength(opts.getParamRmm2m("IHSE", instrument.sigmaInstrHeight));
        instrument.sigmaInstrCentering = TLength(opts.getParamRmm2m("ICSE", instrument.sigmaInstrCentering));

		proj.getCurrentNode().measurements.fEDM.emplace_back(edm);
	} 
	else {
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("A DSPT measurement must have at least 2 entries: "
									 "The observed point and the measured distance.");
		
		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin() + 1, tokens.cend());
		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(0)));

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : proj.getCurrentNode().measurements.fEDM.back().measDSPT)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("A DSPT measurement is duplicated");

		// get a station reference to update default values
		TInstrumentData::TEDM& instrument = proj.getCurrentNode().measurements.fEDM.back().instrument;

		// update the default target if specified
		instrument.defTarget  = opts.getParamS("TRGT", instrument.defTarget);
		// get a copy of  the specified target and update it
		TInstrumentData::TEDM::TTarget tgt(finstruments.getDevice(instrument.targets, instrument.defTarget));
		
      tgt.sigmaDSpt = TLength(opts.getParamRmm2m("OBSE", tgt.sigmaDSpt));
      tgt.ppmDSpt = TLength(opts.getParamRmm2m("PPM", tgt.ppmDSpt));
      tgt.targetHt = TLength(opts.getParamR("TH", tgt.targetHt));
      tgt.sigmaTargetHt = TLength(opts.getParamRmm2m("THSE", tgt.sigmaTargetHt));
      tgt.sigmaTargetCentering = TLength(opts.getParamRmm2m("TCSE", tgt.sigmaTargetCentering));

		// Store  the measured value
		proj.getCurrentNode().measurements.fEDM.back().measDSPT.emplace_back(
         TDSPT(obspt, tgt, TLength(!hasAllParams ? NO_VALf : std::stor(tokens.at(1))))
		);
		
		//get a reference to the inserted measurement
        auto& dspt(proj.getCurrentNode().measurements.fEDM.back().measDSPT.back());
        dspt.line = line;
        dspt.setActive(proj.getCurrentNode().measurements.fEDM.back().isActive() && activeLine); // Active only if station active as well

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
            dspt.eolcomment = tokens.back();
	}
}

void TKeyDVER::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");

	//On first line nothing appears so far: to be discussed
	if (firstline) {
		if (tokens.size() > 2)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);

        // Update the activation status of the DVER rom:
        proj.getCurrentNode().measurements.dverActive = activeLine;
	}
	else {
        bool hasAllParams = (tokens.size() > 2) && isNumber(tokens.at(2));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("A DVER measurement must have at least 3 entries: "
									 "Two points and the measured distance.");

		TOptionHelper opts(tokens.cbegin()+2, tokens.cend());

		// Store  the measured value
		proj.getCurrentNode().measurements.fDVER.emplace_back(
         TDVER(fpoints.getObject(tokens.at(0)), fpoints.getObject(tokens.at(1)), TLength(!hasAllParams ? NO_VALf : std::stor(tokens.at(2))))
			);

		auto& dver(proj.getCurrentNode().measurements.fDVER.back());


		if (opts.has("OBSE"))
			dver.setObservedStDev(TLength(opts.getParamRmm2m("OBSE", proj.getCurrentNode().measurements.fDVER.back().getObservedStDev())));
		else
			dver.setObservedStDev(sigma);

      
		dver.setDistanceCorrection(TLength(opts.getParamR("DCOR", proj.getCurrentNode().measurements.fDVER.back().getDistanceCorrection())));

		dver.line = line;
        dver.setActive(proj.getCurrentNode().measurements.dverActive && activeLine); // Active only if ROM active as well

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			dver.eolcomment = tokens.back();

	}
}

void TKeyDLEV::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	
	if (firstline) {
		if (tokens.size() < 3)
			throw std::runtime_error("A DLEV group of measurements must have at least 1 entry, the levelling instrument ID");


		TOptionHelper opts(tokens.cbegin()+1, tokens.cend());

        const LGCAdjustablePoint* refPt = nullptr;

		if(opts.has("RefPt")){
			std::string rpName = opts.getParamS("RefPt", "NULL"); 
			
			if (!fpoints.doesObjectExist(rpName))
				throw std::runtime_error("Point" +  rpName + "used as reference point in DLEV measurement, must be declared before used");

            refPt = &fpoints.getObject(rpName);
		}

        TLEVEL level(refPt, finstruments.getDevice(finstruments.fLEVEL, tokens.at(2)));
        level.line = line;
        level.setActive(activeLine);

		proj.getCurrentNode().measurements.fLEVEL.emplace_back(level); //add new measurement
	}
	else{
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("A DLEV measurement must have at least 2 entries: stationed point ID and observed vertical distance");

		TOptionHelper opts(tokens.cbegin()+1, tokens.cend());

		const auto& tgtfPoint(fpoints.getObject(tokens.at(0)));
		TLEVEL& levelGrOfMeas = proj.getCurrentNode().measurements.fLEVEL.back();

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : levelGrOfMeas.measDLEV)
				if (tgtfPoint.getName() == point.targetPos->getName())
					throw std::runtime_error("A DLEV measurement is duplicated");

		levelGrOfMeas.instrument.defStaffID = opts.getParamS("TRGT",  levelGrOfMeas.instrument.defStaffID); //Overrides current target (staff) ID, current value

		TInstrumentData::TLEVEL::TTarget tgt = finstruments.getDevice(levelGrOfMeas.instrument.targets, levelGrOfMeas.instrument.defStaffID);

      tgt.sigmaD       = TLength(opts.getParamRmm2m("OBSE", tgt.sigmaD));
      tgt.ppmD         = TLength(opts.getParamRmm2m("PPM", tgt.ppmD));
      tgt.staffHt      = TLength(opts.getParamR("TH", tgt.staffHt)); //Vertical offset of the staff == height
      tgt.sigmaStaffHt = TLength(opts.getParamRmm2m("THSE", tgt.sigmaStaffHt));

		// Store  the dlev measured value
      TDLEV dlev(tgtfPoint, tgt, TLength(!hasAllParams ? NO_VALf : std::stor(tokens.at(1))));

		// Store  the dhor measured value if keyword is used
		if (opts.has("DHOR"))
		{
			if(!hasAllParams)
            dlev.dhor = std::make_shared<TDLEV::TDHOR>(TDLEV::TDHOR(tgtfPoint, tgt, TLength(NO_VALf)));
			else
			   //If horizontal distance is given, DHOR measurement is introduced.
            dlev.dhor = std::make_shared<TDLEV::TDHOR>(TDLEV::TDHOR(tgtfPoint, tgt, TLength(opts.getParamR("DHOR", NO_VALf))));

			dlev.dhor->line = line;
			TReal horDistSigma  = opts.getParamRmm2m("DSE",  NO_VALf);
			if(!isnotanumber(horDistSigma))
            dlev.dhor->setDHORSigma(TLength(horDistSigma));
			else
				throw std::runtime_error("If DHOR distance is provided, standard deviation (DSE) needs to be assigned!");

		}

		dlev.line = line;
        dlev.setActive(levelGrOfMeas.isActive() && activeLine); // Active only if station active as well

		levelGrOfMeas.measDLEV.emplace_back(dlev);
	}
}

void TKeyECHO::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{

	bool firstline(tokens.size() > 0 && tokens.at(0) == "*"); 
	if(firstline){
		if (tokens.size() < 3 )
			throw std::runtime_error("ECHO measurement must have at least 1 entry, the SCALE instrument ID");

		TECHOROM echoRom(nullptr);
		echoRom.line = line;
        echoRom.setActive(activeLine);
      
		proj.getCurrentNode().measurements.fECHO.emplace_back(echoRom); //add new round of measurement

		//The SCALE instrument is only the default one used, it is not stored in TECHOROM because it is specific for each observation
		currentTargetApplied = finstruments.getDevice(finstruments.fSCALE, tokens.at(2)).ID;
	}
	else{
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("ECHO measurement must have at least 2 entries: stationed point ID and observed horizontal offset");

		/*This is a position of station point from which the plane is measured in the ECHO class it has a 'traget' name, since the abstract class is used. Bit confusing to be improved. */
		const auto& stationPoint(fpoints.getObject(tokens.at(0)));

		TOptionHelper opts(tokens.cbegin()+1, tokens.cend()); 
		currentTargetApplied = opts.getParamS("SCALE", currentTargetApplied); //If SCALE is used then change ID of CurrentTargetApplied for the following measurements.

		TInstrumentData::TSCALE instr = finstruments.getDevice(finstruments.fSCALE, currentTargetApplied); //Throws exception if instrument not found, catched on the top level

		instr.sigmaD = TLength(opts.getParamRmm2m("OBSE", instr.sigmaD));
		instr.ppmD = TLength(opts.getParamRmm2m("PPM", instr.ppmD));
		instr.sigmaInstrCentering = TLength(opts.getParamRmm2m("ICSE", instr.sigmaInstrCentering));
		
		// Store  the measured value
		TECHO echo(stationPoint, instr, TLength(!hasAllParams ? NO_VALf : std::stor(tokens.at(1))));

        TECHOROM& echoROMLatest = proj.getCurrentNode().measurements.fECHO.back();

		echo.line = line;
        echo.setActive(echoROMLatest.isActive() && activeLine); // Active only if ROM active as well

		echoROMLatest.measECHO.emplace_back(echo);


		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : echoROMLatest.measECHO)
				if (stationPoint.getName() == point.targetPos->getName())
					throw std::runtime_error("An ECHO measurement is duplicated");
	}
}

void TKeyECVE::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline) {
		// look up station point
		if (tokens.size() < 3)
			throw std::runtime_error("An ECVE measurement must have 1 entries: "
			"The ID of a SCALE instrument.");

		TOptionHelper opts(tokens.cbegin() + 1, tokens.cend());

        LGCAdjustablePoint const * ptLine = nullptr;

		if (opts.has("PtLine")){
			std::string  rpName = opts.getParamS("PtLine", "NULL");

			if (!fpoints.doesObjectExist(rpName))
				throw std::runtime_error("Point" + rpName + "used as reference point in ECVE measurement, must be declared before used");

            ptLine = &fpoints.getObject(rpName);
		}

		 //The line will be initialized in TDataAnalyzer class, when checked for consistency
		TECVEROM ecveRom(ptLine);

		ecveRom.line = line;
        ecveRom.setActive(activeLine);

		proj.getCurrentNode().measurements.fECVE.emplace_back(ecveRom); //add new round of measurement

		//The SCALE instrument is only the default one used, it is not stored in TECVEROM because it is specific for each observation
		currentTargetApplied = finstruments.getDevice(finstruments.fSCALE, tokens.at(2)).ID;
	}
	else{
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("An ECVE measurement must have at least 2 entries: "
			"The stationned point and the measured distance.");
		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin() + 1, tokens.cend());

		// look up the stationed point
		const auto& stationPoint(fpoints.getObject(tokens.at(0)));


		// Overwrite the target if specified and update the 'currentTargetApplied' to be used for upcoming measurement
		currentTargetApplied = opts.getParamS("SCALE", currentTargetApplied); //If SCALE is used then change ID of CurrentTargetApplied for the following measurements.
		TInstrumentData::TSCALE scaleInstr = finstruments.getDevice(finstruments.fSCALE, currentTargetApplied); //Throws exception if instrument not found, catched on the top level

		scaleInstr.sigmaD = TLength(opts.getParamRmm2m("OBSE", scaleInstr.sigmaD));
		scaleInstr.ppmD = TLength(opts.getParamRmm2m("PPM", scaleInstr.ppmD));
		scaleInstr.sigmaInstrCentering = TLength(opts.getParamRmm2m("ICSE", scaleInstr.sigmaInstrCentering));

		// Store  the measured value
		TECVE ecve(stationPoint, scaleInstr, TLength(!hasAllParams ? NO_VALf : std::stor(tokens.at(1))));

        TECVEROM& ecveROMLatest = proj.getCurrentNode().measurements.fECVE.back();
        
        ecve.line = line;
        ecve.setActive(ecveROMLatest.isActive() && activeLine); // Active only if ROM active as well

		ecveROMLatest.measECVE.emplace_back(ecve);

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : ecveROMLatest.measECVE)
				if (stationPoint.getName() == point.targetPos->getName())
					throw std::runtime_error("An ECVE measurement is duplicated");

	}
}

void TKeyECSP::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline) {
		// look up station point
		if (tokens.size() < 5)
			throw std::runtime_error("An ECSP measurement must have 3 entries: "
			"The 2 points defining the line and ID of a SCALE instrument.");


		const std::string& name = "ECSPLINE" + std::to_string(proj.getCurrentNode().measurements.fECSP.size()); //name of the measured adjustable line
		if (!fpoints.doesObjectExist(tokens.at(2)))
			throw std::runtime_error("the first point on the line doesn't exit");
		if (!fpoints.doesObjectExist(tokens.at(3)))
			throw std::runtime_error("the second point on the line doesn't exit");
		TECSPROM ecspRom(name, fpoints.getObject(tokens.at(2)), fpoints.getObject(tokens.at(3)));

		ecspRom.line = line; 
        ecspRom.setActive(activeLine);

		proj.getCurrentNode().measurements.fECSP.emplace_back(ecspRom); //add new round of measurement

		//The SCALE instrument is only the default one used, it is not stored in TECSPROM because it is specific for each observation
		currentTargetApplied = finstruments.getDevice(finstruments.fSCALE, tokens.at(4)).ID;
	}
	else{
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("An ECSP measurement must have at least 2 entries: "
			"The observed point and the measured distance.");
		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin() + 1, tokens.cend());

		// look up the stationed point
		const auto& stationPoint(fpoints.getObject(tokens.at(0)));

		// Overwrite the target if specified and update the 'currentTargetApplied' to be used for upcoming measurement
		currentTargetApplied = opts.getParamS("SCALE", currentTargetApplied); //If SCALE is used then change ID of CurrentTargetApplied for the following measurements.
		TInstrumentData::TSCALE scaleInstr = finstruments.getDevice(finstruments.fSCALE, currentTargetApplied); //Throws exception if instrument not found, catched on the top level

		scaleInstr.sigmaD = TLength(opts.getParamRmm2m("OBSE", scaleInstr.sigmaD));
		scaleInstr.ppmD = TLength(opts.getParamRmm2m("PPM", scaleInstr.ppmD));
		scaleInstr.sigmaInstrCentering = TLength(opts.getParamRmm2m("ICSE", scaleInstr.sigmaInstrCentering));

		// Store  the measured value
		TECSP ecsp(stationPoint, scaleInstr, TLength(!hasAllParams ? NO_VALf : std::stor(tokens.at(1))));

        TECSPROM& ecspROMLatest = proj.getCurrentNode().measurements.fECSP.back();
        
        ecsp.line = line;
        ecsp.setActive(ecspROMLatest.isActive() && activeLine); // Active only if ROM active as well

		ecspROMLatest.measECSP.emplace_back(ecsp);

		proj.setCombinedCaseCalcUsed(); //Combined case used

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : ecspROMLatest.measECSP)
				if (stationPoint.getName() == point.targetPos->getName())
					throw std::runtime_error("An ECSP measurement is duplicated");
	}
	// auto& debug = proj.getCurrentNode().measurements;
}

void TKeyORIE::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{

	bool firstline(tokens.size() > 0 && tokens.at(0) == "*"); 
	if(firstline){
		if (tokens.size() < 4 )
			throw std::runtime_error("ORIE measurement must have at least 2 entries, the Station point and the Polar instrument ID");

		TORIEROM orieROM(fpoints.getObject(tokens.at(2)), finstruments.getDevice(finstruments.fPOLAR, tokens.at(3)));

		orieROM.line = line;
        orieROM.setActive(activeLine);

		TInstrumentData::TPOLAR& instrument(orieROM.instrument);
		TOptionHelper opts(tokens.cbegin()+1, tokens.cend());

		instrument.defTarget  = opts.getParamS("TRGT", instrument.defTarget);

      instrument.sigmaInstrCentering = TLength(opts.getParamRmm2m("ICSE", instrument.sigmaInstrCentering)); //value given in mili-meters [mm], returned value in meters [m]

		if (opts.has("CST"))
			orieROM.fConstantAngle.setGonsValue(opts.getParamR("CST"));	// Value in the input file given in GONs, store it
		else
			orieROM.fConstantAngle.setRadiansValue(instrument.constAngle); // Value stored in radians, set the default one from the instrument definition

		proj.getCurrentNode().measurements.fORIE.emplace_back(orieROM); //add new round of measurement
	}
	else{
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("ORIE measurement must have at least 2 entries: targeted point ID and observed value");

		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin() + 1, tokens.cend());

		// look up the observed point, i.e. the target
		const auto& obspt(fpoints.getObject(tokens.at(0)));

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : proj.getCurrentNode().measurements.fORIE.back().measORIE)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("An ORIE measurement is duplicated");

		// get a station reference to update default values
		TInstrumentData::TPOLAR& instrument = proj.getCurrentNode().measurements.fORIE.back().instrument;

		// update the default target if specified
		instrument.defTarget  = opts.getParamS("TRGT", instrument.defTarget);

		// get a copy of  the specified target and update it
		TInstrumentData::TPOLAR::TTarget tgt(finstruments.getDevice(instrument.targets, instrument.defTarget));
		
		tgt.sigmaAngl = TAngle(opts.getParamRcc2rad("OBSE",  tgt.sigmaAngl));
		tgt.sigmaTargetCentering = TLength(opts.getParamRmm2m("TCSE",  tgt.sigmaTargetCentering));
		
		// set measurement value
		TORIE orie(obspt,tgt);

		orie.line = line;
        orie.setActive(proj.getCurrentNode().measurements.fORIE.back().isActive() && activeLine); // Active only if ROM active as well

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			orie.eolcomment = tokens.back();

		if (hasAllParams)
			orie.setAngle(TAngle(std::stor(tokens.at(1)), TAngle::kGons));

		proj.getCurrentNode().measurements.fORIE.back().measORIE.emplace_back(orie);
	}
}

void TKeyRADI::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");

	//On first line nothing appears so far: to be discussed
	if (firstline)
	{
		if (tokens.size() > 2)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);

        // Update the activation status of the RADI rom:
        proj.getCurrentNode().measurements.radiActive = activeLine;
	}
	else {
        bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("A RADI constraint must have at least 2 entries: "
			"One point and the angle.");

		TOptionHelper opts(tokens.cbegin() + 2, tokens.cend());

		// Store  the measured value
		proj.getCurrentNode().measurements.fRADI.emplace_back(
			TRADI(fpoints.getObject(tokens.at(0)), TAngle(!hasAllParams ? NO_VALf : std::stor(tokens.at(1)), TAngle::EUnits::kGons))
			);

		auto& radi(proj.getCurrentNode().measurements.fRADI.back());
		
		if (opts.has("SIGMA"))
			radi.setObservedStDev(TLength(opts.getParamRmm2m("SIGMA", proj.getCurrentNode().measurements.fRADI.back().getObservedStDev())));
		else
			radi.setObservedStDev(sigma);

		radi.line = line;
        radi.setActive(proj.getCurrentNode().measurements.radiActive && activeLine); // Active only if ROM active as well
		
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			radi.eolcomment = tokens.back();

	}
}


void TKeyOBSXYZ::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");

    if(firstline){

        // On the first line no additional data is given for now: to be discussed

        // Update the activation status of the OBSXYZ rom:
        proj.getCurrentNode().measurements.obsxyzActive = activeLine;
    }
    else {
		bool hasAllParams = (tokens.size() >= 7) && isNumber(tokens.at(1)) && isNumber(tokens.at(2)) && isNumber(tokens.at(3))
			&& isNumber(tokens.at(4)) && isNumber(tokens.at(5)) && isNumber(tokens.at(6));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("A OBSXYZ measurements must have at least 7 entries: "
			"One point, 3 observed coordinates, and 3 sigmas");

		// Store  the measured value
		proj.getCurrentNode().measurements.fOBSXYZ.emplace_back(
			TOBSXYZ(fpoints.getObject(tokens.at(0)),
			TPositionVector(std::stor(tokens.at(1)), std::stor(tokens.at(2)), std::stor(tokens.at(3)), TCoordSysFactory::ECoordSys::k3DCartesian),
			TLength((!hasAllParams ? NO_VALf : std::stor(tokens.at(4))), TLength::EUnits::kMillimetres), 
			TLength((!hasAllParams ? NO_VALf : std::stor(tokens.at(5))), TLength::EUnits::kMillimetres),
			TLength((!hasAllParams ? NO_VALf : std::stor(tokens.at(6))), TLength::EUnits::kMillimetres),
			proj.getCurrentPosition()
			));
		
		auto& obsxyz(proj.getCurrentNode().measurements.fOBSXYZ.back());
		obsxyz.line = line;
        obsxyz.setActive(proj.getCurrentNode().measurements.obsxyzActive && activeLine); // Active only if ROM active as well
		
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			obsxyz.eolcomment = tokens.back();
			
	}
}

void TKeyINCLY::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{

	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline) {
		if (tokens.size() < 3)
			throw std::runtime_error("INCLY measurement must have at least 1 entry, the INCL instrument ID");

		if (proj.getCurrentNode().ID.size() == 1) {
			throw std::runtime_error("INCLY keyword is only allowed in the root frame");
		}

		TINCLYROM inclyRom(finstruments.getDevice(finstruments.fINCL, tokens.at(2)), proj.getCurrentPosition());
		inclyRom.line = line;
		inclyRom.setActive(activeLine);

		proj.getCurrentNode().measurements.fINCLY.emplace_back(inclyRom); //add new round of measurement

		//The INCL instrument is only the default one used, it is not stored in TINCLYROM because it is specific for each observation
		currentTargetApplied = finstruments.getDevice(finstruments.fINCL, tokens.at(2)).ID;
	}
	else {
		bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("INCLY measurement must have at least 2 entries: stationed point ID and observed horizontal offset");
	
		// prepare the options analysis
		TOptionHelper opts(tokens.cbegin() + 1, tokens.cend());

		// look up the stationed point, i.e. the target
		const auto& stationPoint(fpoints.getObject(tokens.at(0)));

		currentTargetApplied = opts.getParamS("INSTR", currentTargetApplied); //If TINCL is used then change ID of CurrentTargetApplied for the following measurements.

		// get a station reference to update default values
		TInstrumentData::TINCL instrument = finstruments.getDevice(finstruments.fINCL, currentTargetApplied); //Throws exception if instrument not found, catched on the top level

		// get a station reference to update default values
		//TInstrumentData::TINCL instrument = proj.getCurrentNode().measurements.fINCLY.back().instrument;

		instrument.sigmaAngl = TAngle(opts.getParamRcc2rad("OBSE",instrument.sigmaAngl));
		instrument.angleCorrectionValue = TAngle(opts.getParamRgon2rad("AC", instrument.angleCorrectionValue));
		instrument.sigmaCorrectionValue = TAngle(opts.getParamRcc2rad("ACSE", instrument.sigmaCorrectionValue));
		instrument.refAngleCorrectionValue = TAngle(opts.getParamRgon2rad("RF", instrument.refAngleCorrectionValue));
		instrument.refSigmaCorrectionValue = TAngle(opts.getParamRcc2rad("RFSE", instrument.refSigmaCorrectionValue));
			
		// set measurement value
		TINCLY incly(stationPoint, instrument);

		TINCLYROM& inclyROMLatest = proj.getCurrentNode().measurements.fINCLY.back();
		
		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : inclyROMLatest.measINCLY)
				if (stationPoint.getName() == point.targetPos->getName())
					throw std::runtime_error("An INCLY measurement is duplicated");
		
		if (hasAllParams)
			incly.setAngle(TAngle(std::stor(tokens.at(1)), TAngle::EUnits::kGons));

		incly.line = line;
		incly.setActive(proj.getCurrentNode().measurements.fINCLY.back().isActive() && activeLine); // Active only if ROM active as well
		proj.getCurrentNode().measurements.fINCLY.back().measINCLY.emplace_back(incly);
	}
}


void TKeyECWS::parse(const std::vector<std::string>& tokens, bool activeLine, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline) {
		if (tokens.size() < 3)
			throw std::runtime_error("ECWS measurement must have at least 2 entries, the HLSR instrument ID and the water surface standard error");

		if (proj.getCurrentNode().ID.size() != 1) {
			throw std::runtime_error("ECWS keyword is only allowed in the root frame");
		}

		TECWSROM ecwsRom(finstruments.getDevice(finstruments.fHLSR, tokens.at(2)), TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres), nullptr);

		TOptionHelper opts(tokens.cbegin() + 1, tokens.cend());

		ecwsRom.line = line;
		ecwsRom.setActive(activeLine);

		//Read the WS sigma
		ecwsRom.sigmaWS = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
		
		if (opts.has("WSID"))
			ecwsRom.romName = opts.getParam("WSID");
		else
			ecwsRom.romName = "ECWS_line" + std::to_string(line);
	
		//Check if the name is unique 
		for (auto& itRomName : proj.getCurrentNode().measurements.fECWS) {
			if (itRomName.romName == ecwsRom.romName)
				throw std::runtime_error("Water Surface Names must be unique: " + ecwsRom.romName + " is duplicated");
		}
			
		//check if the name is unique, the ECWS can only be in the root.
		proj.getCurrentNode().measurements.fECWS.emplace_back(ecwsRom); //add new round of measurement

		//The HLSR instrument is only the default one used, it is not stored in TECWSROM because it is specific for each observation
		currentTargetApplied = finstruments.getDevice(finstruments.fHLSR, tokens.at(2)).ID;
	}
	else {
		bool hasAllParams = (tokens.size() > 1) && isNumber(tokens.at(1));
		if (!hasAllParams && !proj.getConfig().sim.isActive())
			throw std::runtime_error("ECWS measurement must have at least 2 entries: the HLSR instrument ID and the water surface standard error");

		/*This is a position of station point from which the plane is measured in the ECWS class it has a 'traget' name, since the abstract class is used. Bit confusing to be improved. */
		const auto& stationPoint(fpoints.getObject(tokens.at(0)));

		TOptionHelper opts(tokens.cbegin() + 1, tokens.cend());

		std::string currentTarget = currentTargetApplied; //Take the current target, which is used
		// Overwrite the target if specified and update the 'currentTargetApplied' to be used for upcoming measurements
		if (opts.has("INSTR")) {
			currentTarget = opts.getParam("INSTR");
			currentTargetApplied = currentTarget;
		}

		TInstrumentData::THLSR instr = finstruments.getDevice(finstruments.fHLSR, currentTargetApplied); //Throws exception if instrument not found, catched on the top level
		TECWSROM& ecwsROMLatest = proj.getCurrentNode().measurements.fECWS.back();


		instr.sigmaDist = TLength(opts.getParamRmm2m("OBSE", instr.sigmaDist));
		instr.sigmaInstrHeight = TLength(opts.getParamRmm2m("IHSE", instr.sigmaInstrHeight));
		instr.sigmaInstrCentering = TLength(opts.getParamRmm2m("ICSE", instr.sigmaInstrCentering));
		
		if (opts.has("WSSE"))
			instr.sigmaWS = TLength(opts.getParamRmm2m("WSSE", instr.sigmaWS));
		else
			instr.sigmaWS = TLength(ecwsROMLatest.sigmaWS.getMetresValue());
	
		// Store  the measured value
		TECWS ecws(stationPoint, instr, TLength(!hasAllParams ? NO_VALf : std::stor(tokens.at(1))));

		//NODUP used
		if (proj.getConfig().nodup.isActive()) {
			for (auto& point : ecwsROMLatest.measECWS) {
				if (stationPoint.getName() == point.targetPos->getName())
					throw std::runtime_error("An ECWS measurement is duplicated");
			}
		}

		ecws.line = line;
		ecws.setActive(ecwsROMLatest.isActive() && activeLine); // Active only if ROM active as well

		ecwsROMLatest.measECWS.emplace_back(ecws);
	}
}
