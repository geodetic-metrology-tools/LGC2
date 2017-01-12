#include "MeasurementReader_lgc1.h"
#include "TDist.h"
#include "TXYH2CCS.h"

TAMeasurementKey_lgc1::TAMeasurementKey_lgc1(TLGCData& project, const std::string& key) :
TAKeyWord(key, project),
fpoints(project.getPoints()),
finstruments(project.getInstruments()),
flengths(project.getLength()),
fangles(project.getAngles()),
fplanes(project.getPlanes()),
flines(project.getLines()),
fSIMUActive(project.getConfig().sim.isActiveRef())
{}

void TAMeasurementKey_lgc1::createPolarInstrument()
{
	//create a default polar instrument
	TInstrumentData::TPOLAR p;
	p.ID = "TSTNInstr";
	p.defTarget = "PolarTgt";
	p.instrHeight = TLength(0.0);
	p.sigmaInstrHeight = TLength(0.0, TLength::EUnits::kMillimetres);
	p.sigmaInstrCentering = TLength(0.0, TLength::EUnits::kMillimetres);
	p.constAngle = TAngle(0.0, TAngle::EUnits::kGons);
	
	// store the new station
	finstruments.fPOLAR.insert(std::make_pair("TSTNInstr", p));
	
	//create a default target
	// Add adjustable scalar into a global collection and store a pointer
	TAdjustableLength* distCorrectionAdjustable = &flengths.addObject(TAdjustableLength(TLength(0.0), 1, "POLAR_dcorr"));
	
	TInstrumentData::TPOLAR::TTarget t = {
		"PolarTgt",
		TAngle(1.0, TAngle::EUnits::kCCs),
		TAngle(1.0, TAngle::EUnits::kCCs),
		TLength(1.0, TLength::EUnits::kMillimetres),
		TLength(0.0, TLength::EUnits::kMillimetres),
		false,
		TLength(0.0, TLength::EUnits::kMetres),
		TLength(0.0, TLength::EUnits::kMillimetres),
		TLength(0.0, TLength::EUnits::kMillimetres),
		TLength(0.0, TLength::EUnits::kMetres), //htgt
		TLength(0.0, TLength::EUnits::kMillimetres),
		distCorrectionAdjustable // Adjustable object for "distCorrectionValue"
	};
	// store the new target
	finstruments.fPOLAR["TSTNInstr"].targets.insert(std::make_pair("PolarTgt", t));

}

void TAMeasurementKey_lgc1::createTSTN(string stn, int line)
{
	// Initialize the station
	shared_ptr<TTSTN> tstn = make_shared<TTSTN>(fpoints.getObject(stn), getPolarInstr());
	tstn->line = line;
	//emplace this station
	proj.getCurrentNode().measurements.fTSTN.emplace_back(tstn);
}

void TAMeasurementKey_lgc1::createROM(shared_ptr<TTSTN> tstn)
{
	//Initialize the rom
	TInstrumentData::TPOLAR::TTarget* tgt = &getPolarInstr().targets["PolarTgt"];
	//Prepare a name of an adjustable angle (V0) = Frame name + V0 + numberOfAngle
	string angleName = proj.getCurrentNode().frame.getName() + "V0" + std::to_string(proj.getAngles().numObjects());
	// Create a new ROM (round of measurements) for the current station with the given default target, v0 is set to be zero
	shared_ptr<TTSTN::TROM> rom = make_shared<TTSTN::TROM>(*tgt, &proj.getAngles().addObject(TAdjustableAngle(TAngle(0.0, TAngle::kGons), false, angleName)));
	// Add the ROM
	tstn->roms.emplace_back(rom);
}

void TKeyANGL_lgc1::parse(const std::vector<std::string>& tokens, int line)
{
	auto storeANGL = [&](shared_ptr<TTSTN::TROM> rom)   // FRK 17/11/2016; Suppressed reference "auto&"
	{
		if (tokens.size() < 3 && !fSIMUActive)
			throw std::runtime_error("An ANGL measurement must have at least 3 entries: "
			"The station, the observed point and the measured angle.");

		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(1)));

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : rom->measANGL)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("A ANGL measurement is duplicated");

		TInstrumentData::TPOLAR& instr = getPolarInstr();
		TInstrumentData::TPOLAR::TTarget& tgt = instr.targets["PolarTgt"];

		if ((tokens.size() == 4 && !tokens.at(3).compare(0, 1, "/")) || (tokens.size() == 5 && !tokens.at(3).compare(0, 1, "/") && (!tokens.at(4).compare(0, 1, "$") || !tokens.at(4).compare(0, 1, "%"))))
		{
			tgt.sigmaAngl = sigmaANGL;
			constanteANGL = TAngle(std::stor(tokens.at(3).substr(1)), TAngle::EUnits::kGons);
			instr.constAngle = constanteANGL;
		}
		else if ((tokens.size() == 4 && tokens.at(3).compare(0, 1, "/") && tokens.at(3).compare(0, 1, "$") && tokens.at(3).compare(0, 1, "%")) || (tokens.size() == 5 && tokens.at(3).compare(0, 1, "/") && (!tokens.at(4).compare(0, 1, "$") || !tokens.at(4).compare(0, 1, "%"))))
		{
			tgt.sigmaAngl = TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs);
			instr.constAngle = constanteANGL;
		}
		else if (tokens.size() >= 5)
		{
			tgt.sigmaAngl = TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs);
			constanteANGL = TAngle(std::stor(tokens.at(4).substr(1)), TAngle::EUnits::kGons);
			instr.constAngle = constanteANGL;
		}
		else
		{
			instr.constAngle = constanteANGL;
			tgt.sigmaAngl = sigmaANGL;
		}


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

		if (!fSIMUActive)
			angl.setAngle(TAngle(std::stor(tokens.at(2)), TAngle::kGons));

		rom->measANGL.emplace_back(angl);
	};

	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline)
	{
		// *ANGLE [sigma]
		if (tokens.size() == 3)
			sigmaANGL = TAngle(std::stor(tokens.at(2)), TAngle::EUnits::kCCs);
		else
			sigmaANGL = TAngle(1.0, TAngle::EUnits::kCCs);

		constanteANGL = TAngle(0.0, TAngle::EUnits::kGons);
		currentStation = "";
	}
	else
	{
		bool anglStored = false;

		if (currentStation != tokens.at(0))
		{
			currentStation = tokens.at(0);
			if (proj.getCurrentNode().measurements.fTSTN.size() != 0)
			{
				for (auto itTstn : proj.getCurrentNode().measurements.fTSTN)
				{
					//only 1 ROM per station oin lgc1
					if (itTstn->instrumentPos->getName() == currentStation)
					{
						//if TSNT already exist, see if ANGL meas is empty
						if (itTstn->roms.back()->measANGL.empty())
						{
							currentTSTN = itTstn;
							currentROM = currentTSTN->roms.back();
							storeANGL(currentROM);
							anglStored = true;
							break;
						}
						else
							continue;

					}
					else
						continue;
				}

				if (!anglStored)
				{
					createTSTN(currentStation, line);
					currentTSTN = proj.getCurrentNode().measurements.fTSTN.back();

					createROM(currentTSTN);
					currentROM = currentTSTN->roms.back();

					storeANGL(currentROM);
					anglStored = true;
				}
			}
			else
			{
				createTSTN(currentStation, line);
				currentTSTN = proj.getCurrentNode().measurements.fTSTN.back();

				createROM(currentTSTN);
				currentROM = currentTSTN->roms.back();

				storeANGL(currentROM);
			}
		}
		else
			storeANGL(currentROM);
	}
}

void TKeyZENI_lgc1::parse(const std::vector<std::string>& tokens, int line)
{
	auto storeZENI = [&](shared_ptr<TTSTN::TROM> rom) // FRK 17/11/2016; Suppressed reference "auto&"
	{
		if (tokens.size() < 3 && !fSIMUActive)
			throw std::runtime_error("An ZENI measurement must have at least 3 entries: "
			"The station, the observed point and the measured angle.");

		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(1)));

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : rom->measZEND)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("A ZEND measurement is duplicated");

		TInstrumentData::TPOLAR& instr = getPolarInstr();
		TInstrumentData::TPOLAR::TTarget& tgt = instr.targets["PolarTgt"];

		currentTSTN->instrumentHeightAdjustable = IH_adj;
		tgt.targetHt = TLength(0.0, TLength::EUnits::kMetres);
		tgt.sigmaZenD = sigmaZEND;

		if ((tokens.size() == 4 && !tokens.at(3).compare(0, 1, "/")) || (tokens.size() == 5 && !tokens.at(3).compare(0, 1, "/") && (!tokens.at(4).compare(0, 1, "$") || !tokens.at(4).compare(0, 1, "%"))))
		{
			tgt.sigmaZenD = sigmaZEND;
			tgt.targetHt = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);

		}
		else if ((tokens.size() == 4 && tokens.at(3).compare(0, 1, "/") && tokens.at(3).compare(0, 1, "$") && tokens.at(3).compare(0, 1, "%")) || (tokens.size() == 5 && tokens.at(3).compare(0, 1, "/") && (!tokens.at(4).compare(0, 1, "$") || !tokens.at(4).compare(0, 1, "%"))))
			tgt.sigmaZenD = TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs);
		else if (tokens.size() >= 5)
		{
			tgt.sigmaZenD = TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs);
			tgt.targetHt = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
		}


		// set measurement value
		TZEND zend(obspt, tgt);
		zend.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			zend.eolcomment = tokens.back();

		// set indices of LS matrices, ZENI introduces 1 equation and 1 observation, index of observation is not stored since it is not used, but need to be counted
		zend.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		zend.setFirstObservationIndex(proj.fUEOIndices.OIndex);

		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kZEND);

		if (!fSIMUActive)
			zend.setAngle(TAngle(std::stor(tokens.at(2)), TAngle::kGons));

		rom->measZEND.emplace_back(zend);
	};

	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline)
	{
		// *ZENI [sigma]
		if (tokens.size() == 3)
			sigmaZEND = TAngle(std::stor(tokens.at(2)), TAngle::EUnits::kCCs);
		else
			sigmaZEND = TAngle(1.0, TAngle::EUnits::kCCs);

		// Add adjustable scalar into a global collection and store a pointer
		IH_adj = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, currentStation + "_IH"));
		
		currentStation = "";
	}
	else
	{
		bool zendStored = false;

		if (currentStation != tokens.at(0))
		{
			currentStation = tokens.at(0);
			if (proj.getCurrentNode().measurements.fTSTN.size() != 0)
			{
				for (auto itTstn : proj.getCurrentNode().measurements.fTSTN)
				{
					//only 1 ROM per station oin lgc1
					if (itTstn->instrumentPos->getName() == currentStation)
					{
						//if TSNT already exist, see if ZEND meas is empty
						if (itTstn->roms.back()->measZEND.empty())
						{
							currentTSTN = itTstn;
							currentROM = currentTSTN->roms.back();
							storeZENI(currentROM);
							zendStored = true;
							break;
						}
						else
							continue;

					}
					else
						continue;
				}

				if (!zendStored)
				{
					createTSTN(currentStation, line);
					currentTSTN = proj.getCurrentNode().measurements.fTSTN.back();

					createROM(currentTSTN);
					currentROM = currentTSTN->roms.back();

					storeZENI(currentROM);
					zendStored = true;
				}
			}
			else
			{
				createTSTN(currentStation, line);
				currentTSTN = proj.getCurrentNode().measurements.fTSTN.back();

				createROM(currentTSTN);
				currentROM = currentTSTN->roms.back();

				storeZENI(currentROM);
			}
		}
		else
			storeZENI(currentROM);
	}
}

void TKeyZENH_lgc1::parse(const std::vector<std::string>& tokens, int line)
{
	auto storeZENH = [&](shared_ptr<TTSTN::TROM> rom) // FRK 17/11/2016; Suppressed reference "auto&"
	{
		if (tokens.size() < 3 && !fSIMUActive)
			throw std::runtime_error("An ZENH measurement must have at least 3 entries: "
			"The station, the observed point and the measured angle.");

		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(1)));

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : rom->measZEND)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("A ZEND measurement is duplicated");

		TInstrumentData::TPOLAR& instr = getPolarInstr();
		TInstrumentData::TPOLAR::TTarget& tgt = instr.targets["PolarTgt"];

		//default values
		currentTSTN->instrumentHeightAdjustable = IH_adj;
		tgt.targetHt = TLength(0.0, TLength::EUnits::kMetres);
		tgt.sigmaZenD = sigmaZEND;
		if (IH_adj!= nullptr && firstmeas)
			instr.instrHeight = IH_adj->getEstimatedValue();
		else
			instr.instrHeight = TLength(0.0, TLength::EUnits::kMetres);
		
		if (tokens.back().at(0) == '$' || tokens.back().at(0) == '%')
		{
			if (tokens.size() == 5)
			{
				if (!tokens.at(3).compare(0, 1, "\\"))
				{
					tgt.sigmaZenD = sigmaZEND;
					tgt.targetHt = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					if (firstmeas)
						// create a default TAdjustableLength at first measurement only if no instrument height is defined in the input file
						IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));


				}
				else if (!tokens.at(3).compare(0, 1, "/") && firstmeas)
				{

					tgt.sigmaZenD = sigmaZEND;
					// Add adjustable scalar into a global collection and store a pointer
					instr.instrHeight = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));
				}
				else
				{
					tgt.sigmaZenD = TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs);
					if (firstmeas)
						// create a default TAdjustableLength at first measurement only if no instrument height is defined in the input file
						IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));

				}
			}
			else if (tokens.size() == 6)
			{
				if (!tokens.at(3).compare(0, 1, "/") && firstmeas)
				{
					// Add adjustable scalar into a global collection and store a pointer
					instr.instrHeight = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));
					tgt.targetHt = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);

				}
				else if (!tokens.at(4).compare(0, 1, "/") && firstmeas)
				{
					tgt.sigmaZenD = TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs);
					// Add adjustable scalar into a global collection and store a pointer
					instr.instrHeight = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));
				}
				else
				{
					tgt.sigmaZenD = TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs);
					tgt.targetHt = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					if (firstmeas)
						// create a default TAdjustableLength at first measurement only if no instrument height is defined in the input file
						IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));

				}
			}
			else if (tokens.size() == 7 && firstmeas)
			{
				tgt.sigmaZenD = TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs);
				// Add adjustable scalar into a global collection and store a pointer
				instr.instrHeight = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));
				tgt.targetHt = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
			}
			else if (!IH_adj && firstmeas)
			{
				// create a default TAdjustableLength at first measurement only if no instrument height is defined in the input file
				IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));
			}
			else if (!firstmeas)
				if ((tokens.size() == 4 && !tokens.at(3).compare(0, 1, "/")) || (tokens.size() == 5 && !firstmeas && !tokens.at(4).compare(0, 1, "/")) || tokens.size() == 6)
					throw std::runtime_error("The instrument heigth can be defined only at the beginning of the ROM.");
		}
		else
		{//read options
			if (tokens.size() == 4)
			{
				if (!tokens.at(3).compare(0, 1, "\\"))
				{
					tgt.sigmaZenD = sigmaZEND;
					tgt.targetHt = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					if (firstmeas)
						// create a default TAdjustableLength at first measurement only if no instrument height is defined in the input file
						IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));

				}
				else if (!tokens.at(3).compare(0, 1, "/") && firstmeas)
				{

					tgt.sigmaZenD = sigmaZEND;
					// Add adjustable scalar into a global collection and store a pointer
					instr.instrHeight = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));
				}
				else 
				{
					tgt.sigmaZenD = TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs);
					if (firstmeas)
						// create a default TAdjustableLength at first measurement only if no instrument height is defined in the input file
						IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));
				}
			}
			else if (tokens.size() == 5)
			{
				if (!tokens.at(3).compare(0, 1, "/") && firstmeas)
				{
					// Add adjustable scalar into a global collection and store a pointer
					instr.instrHeight = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));
					tgt.targetHt = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);

				}
				else if (!tokens.at(4).compare(0, 1, "/") && firstmeas)
				{
					tgt.sigmaZenD = TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs);
					// Add adjustable scalar into a global collection and store a pointer
					instr.instrHeight = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));
				}
				else
				{
					tgt.sigmaZenD = TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs);
					tgt.targetHt = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					if (firstmeas)
						// create a default TAdjustableLength at first measurement only if no instrument height is defined in the input file
						IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));

				}
			}
			else if (tokens.size() == 6 && firstmeas)
			{
				tgt.sigmaZenD = TAngle(std::stor(tokens.at(3)), TAngle::EUnits::kCCs);
				// Add adjustable scalar into a global collection and store a pointer
				instr.instrHeight = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));
				tgt.targetHt = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
			}
			else if (!IH_adj && firstmeas)
			{
				// create a default TAdjustableLength at first measurement only if no instrument height is defined in the input file
				IH_adj = &flengths.addObject(TAdjustableLength(instr.instrHeight, 1, currentStation + std::to_string(line) + "_IH"));
			}
			else if (!firstmeas)
				if ((tokens.size() == 4 && !tokens.at(3).compare(0, 1, "/")) || (tokens.size() == 5 && !firstmeas && !tokens.at(4).compare(0, 1, "/")) || tokens.size() == 6)
					throw std::runtime_error("The instrument heigth can be defined only at the beginning of the ROM.");

		}

		firstmeas = false;


		// set measurement value
		TZEND zend(obspt, tgt);
		zend.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			zend.eolcomment = tokens.back();

		// set indices of LS matrices, ZENH introduces 1 equation and 1 observation, index of observation is not stored since it is not used, but need to be counted
		zend.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		zend.setFirstObservationIndex(proj.fUEOIndices.OIndex);

		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kZEND);

		if (!fSIMUActive)
			zend.setAngle(TAngle(std::stor(tokens.at(2)), TAngle::kGons));

		rom->measZEND.emplace_back(zend);
		
	};

	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline)
	{
		// *ZENH [sigma]
		if (tokens.size() == 3)
			sigmaZEND = TAngle(std::stor(tokens.at(2)), TAngle::EUnits::kCCs);
		else
			sigmaZEND = TAngle(1.0, TAngle::EUnits::kCCs);
		
		currentStation = "";
		
	}
	else
	{
		bool zendStored = false;

		if (currentStation != tokens.at(0))
		{
			currentStation = tokens.at(0);
			firstmeas = true;
			IH_adj = nullptr;

			if (proj.getCurrentNode().measurements.fTSTN.size() != 0)
			{
				for (auto itTstn : proj.getCurrentNode().measurements.fTSTN)
				{
					//only 1 ROM per station oin lgc1
					if (itTstn->instrumentPos->getName() == currentStation)
					{
						//if TSNT already exist, see if ZEND meas is empty
						if (itTstn->roms.back()->measZEND.empty())
						{
							currentTSTN = itTstn;
							currentROM = currentTSTN->roms.back();
							storeZENH(currentROM);
							zendStored = true;
							break;
						}
						else
							continue;
								
					}
					else
						continue;
				}

				if (!zendStored)
				{
					createTSTN(currentStation, line);
					currentTSTN = proj.getCurrentNode().measurements.fTSTN.back();

					createROM(currentTSTN);
					currentROM = currentTSTN->roms.back();

					storeZENH(currentROM);
					zendStored = true;
				}
			}
			else
			{
				createTSTN(currentStation, line);
				currentTSTN = proj.getCurrentNode().measurements.fTSTN.back();

				createROM(currentTSTN);
				currentROM = currentTSTN->roms.back();

				storeZENH(currentROM);
			}
		}
		else
			storeZENH(currentROM);
	}	
}

void TKeyDTHE_lgc1::parse(const std::vector<std::string>& tokens, int line)
{
	auto storeDIST = [&](shared_ptr<TTSTN::TROM> rom)  // FRK 17/11/2016; Suppressed reference "auto&"
	{
		if (tokens.size() < 3 && !fSIMUActive)
			throw std::runtime_error("A DTHE measurement must have at least 3 entries: "
			"The station, the observed point and the measured angle.");

		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(1)));

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : rom->measDIST)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("A DTHE measurement is duplicated");

		TInstrumentData::TPOLAR& instr = getPolarInstr();
		TInstrumentData::TPOLAR::TTarget& tgt = instr.targets["PolarTgt"];

		tgt.sigmaDist = sigmaDIST;
		tgt.ppmDist = ppm;
		tgt.distCorrectionValue = dcorr;
		tgt.targetHt = TLength(0.0);
		tgt.distCorrectionAdjustable = adjDCorr;

		if (tokens.back().at(0) == '$' || tokens.back().at(0) == '%')
		{
			if (tokens.size() == 5)
			{
				if (tokens.at(3) == "C")
				{
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, currentStation + "_adj"));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (!tokens.at(3).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else if (!tokens.at(3).compare(0, 1, "\\"))
				{
					tgt.targetHt = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
				}
				else
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
			}
			else if (tokens.size() == 6)
			{

				if (!tokens.at(3).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
					tgt.targetHt = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				}
				else if (tokens.at(4) == "C")
				{
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, currentStation + "_adj"));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (!tokens.at(4).compare(0, 1, "/"))
				{
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else if (!tokens.at(4).compare(0, 1, "\\"))
				{
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.targetHt = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				}
				else
				{
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				}
			}
			else if (tokens.size() == 7)
			{
				tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				if (tokens.at(4) == "C")
				{
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, currentStation + "_adj"));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (!tokens.at(4).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
					tgt.targetHt = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				}
				else if (!tokens.at(5).compare(0, 1, "/"))
				{
					tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
					dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else if (!tokens.at(5).compare(0, 1, "\\"))
				{
					tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
					tgt.targetHt = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				}

			}
			else if (tokens.size() == 8)
			{
				if (tokens.at(5) == "C")
				{
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, currentStation + "_adj"));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else
				{
					dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}

				tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				tgt.targetHt = TLength(std::stor(tokens.at(6).substr(1)), TLength::EUnits::kMetres);
			}
		}
		else
		{
			if (tokens.size() == 4)
			{
				if (tokens.at(3) == "C")
				{
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, currentStation + "_adj"));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (!tokens.at(3).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else if (!tokens.at(3).compare(0, 1, "\\"))
				{
					tgt.targetHt = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
				}
				else
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
			}
			else if (tokens.size() == 5)
			{

				if (!tokens.at(3).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
					tgt.targetHt = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				}
				else if (tokens.at(4) == "C")
				{
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, currentStation + "_adj"));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (!tokens.at(4).compare(0, 1, "/"))
				{
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else if (!tokens.at(4).compare(0, 1, "\\"))
				{
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.targetHt = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				}
				else
				{
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				}
			}
			else if (tokens.size() == 6)
			{
				tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				if (tokens.at(4) == "C")
				{
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, currentStation + "_adj"));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (!tokens.at(4).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
					tgt.targetHt = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				}
				else if (!tokens.at(5).compare(0, 1, "/"))
				{
					tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
					dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else if (!tokens.at(5).compare(0, 1, "\\"))
				{
					tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
					tgt.targetHt = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				}

			}
			else if (tokens.size() == 7)
			{
				if (tokens.at(5) == "C")
				{
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, currentStation + "_adj"));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else
				{
					dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}

				tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				tgt.targetHt = TLength(std::stor(tokens.at(6).substr(1)), TLength::EUnits::kMetres);
			}
		}


		// set measurement value
		TLINE dthe(obspt, tgt);
		dthe.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			dthe.eolcomment = tokens.back();

		// set indices of LS matrices, DTHE introduces 1 equation and 1 observation, index of observation is not stored since it is not used, but need to be counted
		dthe.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		dthe.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kDIST);

		if (!fSIMUActive)
			dthe.setDistance(TLength(std::stor(tokens.at(2)), TLength::kMetres));

		rom->measDIST.emplace_back(dthe);
	};

	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline)
	{
		// Add adjustable scalar into a global collection and store a pointer
		IH_adj = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, currentStation + "_IH"));
		// Add adjustable scalar into a global collection and store a pointer
		adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 1, currentStation + "_dcorr"));

		// *DTHE [sigma] [ppm]
		if (tokens.size() == 3)
		{
			sigmaDIST = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
			ppm = TLength(0.0, TLength::EUnits::kMillimetres);
		}
		else if (tokens.size() == 4)
		{
			sigmaDIST = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
			ppm = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
		}
		else
		{
			sigmaDIST = TLength(1.0, TLength::EUnits::kMillimetres);
			ppm = TLength(0.0, TLength::EUnits::kMillimetres);
		}

		dcorr = TLength(0.0, TLength::EUnits::kMillimetres);
		currentStation = "";
	}
	else
	{
		bool distStored = false;

		if (currentStation != tokens.at(0))
		{
			currentStation = tokens.at(0);
			if (proj.getCurrentNode().measurements.fTSTN.size() != 0)
			{
				for (auto itTstn : proj.getCurrentNode().measurements.fTSTN)
				{
					//only 1 ROM per station oin lgc1
					if (itTstn->instrumentPos->getName() == currentStation)
					{
						//if TSNT already exist, see if ANGL meas is empty
						if (itTstn->roms.back()->measDIST.empty())
						{
							currentTSTN = itTstn;
							currentROM = currentTSTN->roms.back();
							storeDIST(currentROM);
							distStored = true;
							break;
						}
						else
							continue;

					}
					else
						continue;
				}

				if (!distStored)
				{
					createTSTN(currentStation, line);
					currentTSTN = proj.getCurrentNode().measurements.fTSTN.back();

					createROM(currentTSTN);
					currentROM = currentTSTN->roms.back();

					storeDIST(currentROM);
					distStored = true;
				}
			}
			else
			{
				createTSTN(currentStation, line);
				currentTSTN = proj.getCurrentNode().measurements.fTSTN.back();

				createROM(currentTSTN);
				currentROM = currentTSTN->roms.back();

				storeDIST(currentROM);
			}
		}
		else
			storeDIST(currentROM);
	}
}

void TKeyECTH_lgc1::parse(const std::vector<std::string>& tokens, int line)
{

	auto storeECTH = [&](shared_ptr<TTSTN::TROM> rom) // FRK 17/11/2016; Suppressed reference "auto&"
	{
		if (tokens.size() < 4 && !fSIMUActive)
			throw std::runtime_error("A ECTH measurement must have at least 4 entries: "
			" station point angle offset");

		// look up the observed point
		const LGCAdjustablePoint& obspt(fpoints.getObject(tokens.at(1)));

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : rom->measECTH)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("ECTH measurement is duplicated");


		TInstrumentData::TSCALE& ScaleInstr = proj.getInstruments().fSCALE["ECTHInstr"];
		ScaleInstr.sigmaD = sigma;
		ScaleInstr.distCorrectionValue = dcorr;

		if (tokens.back().at(0) == '$' || tokens.back().at(0) == '%')
		{
			if (tokens.size() == 6)
			{
				if (!tokens.at(4).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					ScaleInstr.distCorrectionValue = dcorr;
				}
				else
					ScaleInstr.sigmaD = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
			}
			else if (tokens.size() == 7)
			{
				ScaleInstr.sigmaD = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				ScaleInstr.distCorrectionValue = dcorr;
			}
		}
		else
		{
			if (tokens.size() == 5)
			{
				if (!tokens.at(4).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					ScaleInstr.distCorrectionValue = dcorr;
				}
				else
					ScaleInstr.sigmaD = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
			}
			else if (tokens.size() == 6)
			{
				ScaleInstr.sigmaD = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				ScaleInstr.distCorrectionValue = dcorr;
			}
		}


		// set measurement value
		TECTH ecth(obspt, ScaleInstr, TAngle(std::stor(tokens.at(2)), TAngle::EUnits::kGons));
		ecth.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			ecth.eolcomment = tokens.back();

		ecth.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		ecth.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kECTH);

		if (!fSIMUActive)
			ecth.setDistance(TLength(std::stor(tokens.at(3))));

		rom->measECTH.emplace_back(ecth);
	};

	//create a scale instrument for ecth measurements
	if (firstmeas)
	{//create a default scale instrument
		const TInstrumentData::TSCALE scl = {
			"ECTHInstr",
			TLength(1.0, TLength::EUnits::kMillimetres), //sigma
			TLength(0.0, TLength::EUnits::kMillimetres), //ppm
			TLength(0.0, TLength::EUnits::kMillimetres), //dcorrvalue
			TLength(0.0, TLength::EUnits::kMillimetres), //sigma dcorr
			TLength(0.0, TLength::EUnits::kMillimetres)  //centering
		};

		// store the new station
		finstruments.fSCALE.insert(std::make_pair("ECTHInstr", scl));

		if (tokens.size() == 3)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);

		dcorr = TLength(0.0, TLength::EUnits::kMetres);

		firstmeas = false;
	}

	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline)
	{
		// *ECTH [sigma]
		if (tokens.size() == 3)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);

		dcorr = TLength(0.0, TLength::EUnits::kMetres);
		currentStation = "";
	}
	else
	{
		bool ecthStored = false;

		if (currentStation != tokens.at(0))
		{
			currentStation = tokens.at(0);

			if (proj.getCurrentNode().measurements.fTSTN.size() != 0)
			{
				for (auto itTstn : proj.getCurrentNode().measurements.fTSTN)
				{
					//only 1 ROM per station on lgc1
					if (itTstn->instrumentPos->getName() == currentStation)
					{
						//if TSNT already exist, see if ECTH meas is empty
						if (itTstn->roms.back()->measECTH.empty())
						{
							currentTSTN = itTstn;
							currentROM = currentTSTN->roms.back();
							storeECTH(currentROM);
							ecthStored = true;
							break;
						}
						else
							continue;

					}
					else
						continue;
				}

				if (!ecthStored)
				{
					createTSTN(currentStation, line);
					currentTSTN = proj.getCurrentNode().measurements.fTSTN.back();

					createROM(currentTSTN);
					currentROM = currentTSTN->roms.back();

					storeECTH(currentROM);
					ecthStored = true;
				}
			}
			else
			{
				createTSTN(currentStation, line);
				currentTSTN = proj.getCurrentNode().measurements.fTSTN.back();

				createROM(currentTSTN);
				currentROM = currentTSTN->roms.back();

				storeECTH(currentROM);
			}
		}
		else
			storeECTH(currentROM);
	}	
}

void TKeyDHOR_lgc1::parse(const std::vector<std::string>& tokens, int line)
{
	auto storeDHOR = [&](shared_ptr<TTSTN::TROM> rom)  // FRK 17/11/2016; Suppressed reference "auto&"
	{
		if (tokens.size() < 3 && !fSIMUActive)
			throw std::runtime_error("A DHOR measurement must have at least 3 entries: "
			"The station, the observed point and the measured angle.");

		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(1)));

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : rom->measDHOR)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("A DHOR measurement is duplicated");

		TInstrumentData::TPOLAR& instr = getPolarInstr();
		TInstrumentData::TPOLAR::TTarget& tgt = instr.targets["PolarTgt"];

		tgt.sigmaDist = sigmaDIST;
		tgt.ppmDist = ppm;
		tgt.distCorrectionValue = dcorr;
		
		if (tokens.back().at(0) == '$' || tokens.back().at(0) == '%')
		{
			if (tokens.size() == 5)
			{
				if (!tokens.at(3).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
			}
			else if (tokens.size() == 6)
			{
				if (!tokens.at(4).compare(0, 1, "/"))
				{
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else if (!tokens.at(3).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
					tgt.targetHt = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				}
				else
				{
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				}
			}
			else if (tokens.size() == 7)
			{
				tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				if (!tokens.at(4).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
					tgt.targetHt = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				}
				else if (!tokens.at(5).compare(0, 1, "/"))
				{
					tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
					dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
			}
			else if (tokens.size() == 8)
			{
				tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				tgt.distCorrectionValue = dcorr;
			}
		}
		else
		{
			if (tokens.size() == 4)
			{
				if (!tokens.at(3).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
			}
			else if (tokens.size() == 5)
			{
				if (!tokens.at(4).compare(0, 1, "/"))
				{
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else if (!tokens.at(3).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
					tgt.targetHt = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				}
				else
				{
					tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				}
			}
			else if (tokens.size() == 6)
			{
				tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				if (!tokens.at(4).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
					tgt.targetHt = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				}
				else if (!tokens.at(5).compare(0, 1, "/"))
				{
					tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
					dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}

			}
			else if (tokens.size() == 7)
			{
				tgt.sigmaDist = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				tgt.ppmDist = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				tgt.distCorrectionValue = dcorr;
			}
		}


		// set measurement value
		TLINE dhor(obspt, tgt);
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
		proj.addToMeasurementNum(TMeasurementsGlobal::kDIST);

		if (!fSIMUActive)
			dhor.setDistance(TLength(std::stor(tokens.at(2)), TLength::kMetres));

		rom->measDHOR.emplace_back(dhor);
	};

	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline)
	{
		// *DHOR [sigma] [ppm]
		if (tokens.size() == 3)
		{
			sigmaDIST = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
			ppm = TLength(0.0, TLength::EUnits::kMillimetres);
		}
		else if (tokens.size() == 4)
		{
			sigmaDIST = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
			ppm = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
		}
		else
		{
			sigmaDIST = TLength(1.0, TLength::EUnits::kMillimetres);
			ppm = TLength(0.0, TLength::EUnits::kMillimetres);
		}

		dcorr = TLength(0.0, TLength::EUnits::kMillimetres);
		currentStation = "";
	}
	else
	{
		bool distStored = false;

		if (currentStation != tokens.at(0))
		{
			currentStation = tokens.at(0);
			if (proj.getCurrentNode().measurements.fTSTN.size() != 0)
			{
				for (auto itTstn : proj.getCurrentNode().measurements.fTSTN)
				{
					//only 1 ROM per station oin lgc1
					if (itTstn->instrumentPos->getName() == currentStation)
					{
						//if TSNT already exist, see if ANGL meas is empty
						if (itTstn->roms.back()->measDHOR.empty())
						{
							currentTSTN = itTstn;
							currentROM = currentTSTN->roms.back();
							storeDHOR(currentROM);
							distStored = true;
							break;
						}
						else
							continue;

					}
					else
						continue;
				}

				if (!distStored)
				{
					createTSTN(currentStation, line);
					currentTSTN = proj.getCurrentNode().measurements.fTSTN.back();

					createROM(currentTSTN);
					currentROM = currentTSTN->roms.back();

					storeDHOR(currentROM);
					distStored = true;
				}
			}
			else
			{
				createTSTN(currentStation, line);
				currentTSTN = proj.getCurrentNode().measurements.fTSTN.back();

				createROM(currentTSTN);
				currentROM = currentTSTN->roms.back();

				storeDHOR(currentROM);
			}
		}
		else
			storeDHOR(currentROM);
	}
}

///////////////////////
//   NON TSTN MEAS   //
///////////////////////
void TKeyDMES_lgc1::parse(const std::vector<std::string>& tokens, int line)
{
	if (fistrDMES)
	{
		//create a default EDM instrument
		const TInstrumentData::TEDM e = {
			"EDMInstr",
			"EDMTgt",
			TLength(0.0, TLength::EUnits::kMetres), //hi
			TLength(0.0, TLength::EUnits::kMillimetres), //sigma hi
			TLength(0.0, TLength::EUnits::kMillimetres) //sigma instr centering
		};

		// store the new station
		finstruments.fEDM.insert(std::make_pair("EDMInstr", e));
		
		
		//create an EDM target
		// Add adjustable scalar into a global collection and store a pointer
		adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 1, "EDM_dcorr"));

		TInstrumentData::TEDM::TTarget t = {
			"EDMTgt",
			TLength(1.0, TLength::EUnits::kMillimetres), //sigma
			TLength(0.0, TLength::EUnits::kMillimetres), //ppm
			false, //dcorr adjustable
			TLength(0.0, TLength::EUnits::kMetres), //dcorr value
			TLength(0.0, TLength::EUnits::kMillimetres), // sigma dcorr
			TLength(0.0, TLength::EUnits::kMillimetres), //sigma tgt centering
			TLength(0.0, TLength::EUnits::kMetres), //htgt
			TLength(0.0, TLength::EUnits::kMillimetres), //sigma htgt
			adjDCorr // adjustableLength*
		};

		if (tokens.size() == 3)
		{
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
			ppm = TLength(0.0, TLength::EUnits::kMillimetres);
		}
		else if (tokens.size() == 4)
		{
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
			ppm = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
		}
		else
		{
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);
			ppm = TLength(0.0, TLength::EUnits::kMillimetres);
		}
		dcorr = TLength(0.0, TLength::EUnits::kMetres);

		// store the new target
		finstruments.fEDM.begin()->second.targets.insert(std::make_pair("EDMTgt", t)); // we have only one EDM instrument

		fistrDMES = false;
	}


	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline) 
	{
		// *DMES [sigma] [ppm]
		if (tokens.size() == 3)
		{
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
			ppm = TLength(0.0, TLength::EUnits::kMillimetres);
		}
		else if (tokens.size() == 4)
		{
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
			ppm = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
		}
		else
		{
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);
			ppm = TLength(0.0, TLength::EUnits::kMillimetres);
		}
		dcorr = TLength(0.0, TLength::EUnits::kMetres);
		
		currentStation = "";
		// Add adjustable scalar into a global collection and store a pointer
		adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 1, "EDM_dcorr"+line));
	}
	else 
	{
		// stn tgt meas [sigma][ppm] [/const | C ][ \hI HRefl] [$comments]
		if (tokens.size() < 3 && !fSIMUActive)
			throw std::runtime_error("A DMES measurement must have at least 3 entries: "
			"The station, the observed point and the measured distance.");

		// Initialize the station
		if (currentStation != tokens.at(0))
		{
			currentStation = tokens.at(0);
			TEDM edm(fpoints.getObject(currentStation), finstruments.getDevice(finstruments.fEDM, "EDMInstr"));

			// get a reference to modify the default values for this station
			auto& instrument(edm.instrument);

			proj.getCurrentNode().measurements.fEDM.emplace_back(edm);
			proj.getCurrentNode().measurements.fEDM.back().line = line;
		}

		// look up the observed point
		const auto& obspt(fpoints.getObject(tokens.at(1)));

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : proj.getCurrentNode().measurements.fEDM.back().measDSPT)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("A DMES measurement is duplicated");

		// get a station reference to update default values
		TInstrumentData::TEDM& instrument = proj.getCurrentNode().measurements.fEDM.back().instrument;

		// get a copy of  the specified target and update it
		TInstrumentData::TEDM::TTarget tgt(finstruments.getDevice(instrument.targets, instrument.defTarget));

		//default value
		tgt.sigmaDSpt = sigma;
		tgt.ppmDSpt = ppm;
		tgt.distCorrectionValue = dcorr;
		tgt.distCorrectionAdjustable = adjDCorr;

		if (tokens.back().at(0) == '$' || tokens.back().at(0) == '%')
		{
			if (tokens.size() == 5)
			{
				if (tokens.at(3) == "C")
				{
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, "EDM_dcorr" + line));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (!tokens.at(3).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
			}

			else if (tokens.size() == 6)
			{
				if (!tokens.at(3).compare(0, 1, "\\"))
				{
					instrument.instrHeight = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.targetHt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMetres);
				}
				else if (tokens.at(4) == "C")
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, "EDM_dcorr" + line));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (!tokens.at(4).compare(0, 1, "/"))
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.ppmDSpt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				}
			}

			else if (tokens.size() == 7)
			{
				if (!tokens.at(3).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
					instrument.instrHeight = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.targetHt = TLength(std::stor(tokens.at(5)), TLength::EUnits::kMetres);
				}
				else if (!tokens.at(4).compare(0, 1, "\\"))
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					instrument.instrHeight = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.targetHt = TLength(std::stor(tokens.at(5)), TLength::EUnits::kMetres);
				}
				else if (tokens.at(5) == "C")
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.ppmDSpt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, "EDM_dcorr" + line));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (!tokens.at(5).compare(0, 1, "/"))
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.ppmDSpt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
					dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else  //comments at the end
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.ppmDSpt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				}
			}

			else if (tokens.size() == 9)
			{
				tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				tgt.ppmDSpt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				instrument.instrHeight = TLength(std::stor(tokens.at(6).substr(1)), TLength::EUnits::kMetres);
				tgt.targetHt = TLength(std::stor(tokens.at(7)), TLength::EUnits::kMetres);

				if (tokens.at(5) == "C")
				{
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, "EDM_dcorr" + line));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else
				{
					dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
			}
			else if (tokens.size() == 8)
			{
				tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				instrument.instrHeight = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				tgt.targetHt = TLength(std::stor(tokens.at(6)), TLength::EUnits::kMetres);

				if (tokens.at(4) == "C")
				{
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, "EDM_dcorr" + line));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (tokens.at(4) == "/")
				{
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else
					tgt.ppmDSpt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);

			}
		}
		else
		{
			if (tokens.size() == 4)
			{
				if (tokens.at(3) == "C")
				{
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, "EDM_dcorr" + line));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (!tokens.at(3).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
			}

			else if (tokens.size() == 5)
			{
				if (!tokens.at(3).compare(0, 1, "\\"))
				{
					instrument.instrHeight = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.targetHt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMetres);
				}
				else if (tokens.at(4) == "C")
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, "EDM_dcorr" + line));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (!tokens.at(4).compare(0, 1, "/"))
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.ppmDSpt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				}
			}

			else if (tokens.size() == 6)
			{
				if (!tokens.at(3).compare(0, 1, "/"))
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
					instrument.instrHeight = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.targetHt = TLength(std::stor(tokens.at(5)), TLength::EUnits::kMetres);
				}
				else if (!tokens.at(4).compare(0, 1, "\\"))
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					instrument.instrHeight = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
					tgt.targetHt = TLength(std::stor(tokens.at(5)), TLength::EUnits::kMetres);
				}
				else if (tokens.at(5) == "C")
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.ppmDSpt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, "EDM_dcorr" + line));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else if (!tokens.at(5).compare(0, 1, "/"))
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.ppmDSpt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
					dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
				else  //comments at the end
				{
					tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.ppmDSpt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				}
			}

			else if (tokens.size() == 8)
			{
				tgt.sigmaDSpt = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				tgt.ppmDSpt = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				instrument.instrHeight = TLength(std::stor(tokens.at(6).substr(1)), TLength::EUnits::kMetres);
				tgt.targetHt = TLength(std::stor(tokens.at(7)), TLength::EUnits::kMetres);

				if (tokens.at(5) == "C")
				{
					// Add adjustable scalar into a global collection and store a pointer
					adjDCorr = &flengths.addObject(TAdjustableLength(TLength(0.0), 0, "EDM_dcorr" + line));
					tgt.distCorrectionAdjustable = adjDCorr;
				}
				else
				{
					dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
					tgt.distCorrectionValue = dcorr;
				}
			}
		}

		// Store  the measured value
		proj.getCurrentNode().measurements.fEDM.back().measDSPT.emplace_back(
			TDSPT(obspt, tgt, TLength(fSIMUActive ? NO_VALf : std::stor(tokens.at(2))))
			);

		//get a reference to the inserted measurement
		auto& dpst(proj.getCurrentNode().measurements.fEDM.back().measDSPT.back());
		dpst.line = line;
		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			dpst.eolcomment = tokens.back();

		// set indices of LS matrices, DMES introduces 1 equation and 1 observation, index of observation is not stored since it is not used, but need to be counted
		dpst.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		dpst.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kDSPT);
	}
}

void TKeyDVER_lgc1::parse(const std::vector<std::string>& tokens, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");

	//On first line nothing appears so far: to be discussed
	if (firstline) {
		if (tokens.size() > 2)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);
		dcorr = TLength(0.0, TLength::EUnits::kMetres);
	}
	else {

		if (tokens.size() < 3 && !fSIMUActive)
			throw std::runtime_error("A DVER measurement must have at least 3 entries: "
			"Two points and the measured distance.");

		TOptionHelper opts(tokens.cbegin() + 2, tokens.cend());

		// Store  the measured value
		proj.getCurrentNode().measurements.fDVER.emplace_back(
			TDVER(fpoints.getObject(tokens.at(0)), fpoints.getObject(tokens.at(1)), TLength(fSIMUActive ? NO_VALf : std::stor(tokens.at(2))))
			);

		auto& dver(proj.getCurrentNode().measurements.fDVER.back());

		dver.setObservedStDev(sigma);
		dver.setDistanceCorrection(dcorr);

		if (tokens.back().at(0) == '$' || tokens.back().at(0) == '%')
		{
			if (tokens.size() == 5)
			{
				if (tokens.at(3).compare(0, 1, "/"))
				{
					dver.setObservedStDev(TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres));
					dver.setDistanceCorrection(dcorr);
				}
				else
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					dver.setDistanceCorrection(dcorr);
					dver.setObservedStDev(sigma);
				}
			}
			else if (tokens.size() == 6)
			{
				dver.setObservedStDev(TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres));
				dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				dver.setDistanceCorrection(dcorr);
			}
		}
		else
		{
			if (tokens.size() == 4)
			{
				if (tokens.at(3).compare(0, 1, "/"))
				{
					dver.setObservedStDev(TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres));
					dver.setDistanceCorrection(dcorr);
				}
				else
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					dver.setDistanceCorrection(dcorr);
					dver.setObservedStDev(sigma);
				}
			}
			else if (tokens.size() == 5)
			{
				dver.setObservedStDev(TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres));
				dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				dver.setDistanceCorrection(dcorr);
			}
		}

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

void TKeyDLEV_lgc1::parse(const std::vector<std::string>& tokens, int line)
{
	if (fistrDLEV)
	{
		//create a default LEVEL instrument
		// Add adjustable scalar into a global collection and store a pointer
		TAdjustableAngle* collANGLEAdjustable = &fangles.addObject(TAdjustableAngle(TAngle(0.0), 1, "coll_angle_adjustable"));
		const TInstrumentData::TLEVEL l = {
			"LEVELInstr",
			"Staff1",
			false,
			TAngle(0.0), //collimation angle value
			collANGLEAdjustable
		};

		// store the new station
		finstruments.fLEVEL.insert(std::make_pair("LEVELInstr", l));

		//create an staff 
		if (tokens.size() == 3)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);
		dcorr = TLength(0.0, TLength::EUnits::kMetres);

		TInstrumentData::TLEVEL::TTarget s = {
			"Staff1",
			sigma,
			TLength(0.0, TLength::EUnits::kMillimetres), //ppm
			TLength(0.0, TLength::EUnits::kMetres), // distcorr
			TLength(0.0, TLength::EUnits::kMillimetres), //sigma distcorr
			TLength(0.0, TLength::EUnits::kMetres), //hstaff
			TLength(0.0, TLength::EUnits::kMillimetres) //sigma hstaff
		};

		// store the new staff
		finstruments.fLEVEL.begin()->second.targets.insert(std::make_pair("Staff1", s)); // we have only one LEVEL instrument

		fistrDLEV = false;
	}


	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline)
	{
		// *DLEV [sigma]
		if (tokens.size() == 3)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);
		dcorr = TLength(0.0, TLength::EUnits::kMetres);
	}
	else
	{
		// p1 p2 meas [sigma][/const]
		if (tokens.size() < 3 && !fSIMUActive)
			throw std::runtime_error("A DLEV measurement must have at least 3 entries: "
			"The station, the observed point and the measured distance.");

		// Initialize the station
		if (currentStation != tokens.at(0))
		{
			currentStation = tokens.at(0);

			const std::string& name = "DLEVPLANE" + std::to_string(proj.getCurrentNode().measurements.fLEVEL.size()); //name of the measured adjustable plane
			//Both angle are 0, which is a (0 0 1) direction vector, both angles are fixed
			fplanes.addObject(LGCAdjustablePlane(&fpoints.getObject(currentStation), TLength(0.0), TAngle(0.0, TAngle::kRadians),
				TAngle(0.0, TAngle::kRadians), true, true, name));

			TLEVEL level(fplanes.back(), finstruments.getDevice(finstruments.fLEVEL, "LEVELInstr"));
			level.line = line;

			proj.getCurrentNode().measurements.fLEVEL.emplace_back(level); //add new measurement
			proj.getCurrentNode().measurements.fLEVEL.back().line = line;
		}

		const auto& tgtfPoint(fpoints.getObject(tokens.at(1)));

		if (tokens.size() < 3 && !fSIMUActive)
			throw std::runtime_error("A DLEV measurement must have at least 3 entries: the instrument,  the stationed point ID and observed vertical distance");


		TLEVEL& levelGrOfMeas = proj.getCurrentNode().measurements.fLEVEL.back();

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : levelGrOfMeas.measDLEV)
				if (tgtfPoint.getName() == point.targetPos->getName())
					throw std::runtime_error("A DLEV measurement is duplicated");

		TInstrumentData::TLEVEL::TTarget tgt = finstruments.getDevice(levelGrOfMeas.instrument.targets, levelGrOfMeas.instrument.defStaffID);
		tgt.sigmaD = sigma;
		tgt.distCorrectionValue = dcorr;

		if (tokens.back().at(0) == '$' || tokens.back().at(0) == '%')
		{
			if (tokens.size() == 5)
			{
				if (tokens.at(3).compare(0, 1, "/"))
				{
					tgt.sigmaD = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.distCorrectionValue = dcorr;
				}
				else
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.sigmaD = sigma;
				}
			}
			else if (tokens.size() == 6)
			{
				tgt.sigmaD = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				tgt.distCorrectionValue = TLength(-1.0*std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
			}
		}
		else
		{
			if (tokens.size() == 4)
			{
				if (tokens.at(3).compare(0, 1, "/"))
				{
					tgt.sigmaD = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
					tgt.distCorrectionValue = dcorr;
				}
				else
				{
					dcorr = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
					tgt.sigmaD = sigma;
				}
			}
			else if (tokens.size() == 5)
			{
				tgt.sigmaD = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				tgt.distCorrectionValue = TLength(-1.0*std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
			}
		}

		// Store  the dlev measured value
		TDLEV dlev(tgtfPoint, tgt, TLength(fSIMUActive ? NO_VALf : -1.0*std::stor(tokens.at(2))));

		dlev.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		dlev.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;

		dlev.line = line;
		proj.addToMeasurementNum(TMeasurementsGlobal::kDLEV);
		levelGrOfMeas.measDLEV.emplace_back(dlev);
	}
}

void TKeyECHO_lgc1::parse(const std::vector<std::string>& tokens, int line)
{
	if (firstECHO)
	{
		//create a default scale instrument
		const TInstrumentData::TSCALE scl = {
			"ECHOInstr",
			TLength(1.0, TLength::EUnits::kMillimetres), //sigma
			TLength(0.0, TLength::EUnits::kMillimetres), //ppm
			TLength(0.0, TLength::EUnits::kMillimetres), //dcorrvalue
			TLength(0.0, TLength::EUnits::kMillimetres), //sigma dcorr
			TLength(0.0, TLength::EUnits::kMillimetres)  //centering
		};

		// store the new station
		finstruments.fSCALE.insert(std::make_pair("ECHOInstr", scl));

		if (tokens.size() == 3)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);
		
		constante = TLength(0.0, TLength::EUnits::kMetres);
		firstECHO = false;
	}


	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline)
	{
		// *ECHO [sigma]
		if (tokens.size() == 3)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);
		constante = TLength(0.0, TLength::EUnits::kMetres);
		encrage1 = "";
		encrage2 = "";
	}
	else
	{
		// p1 stn p2 meas [sigma][/const]
		if (tokens.size() < 4 && !fSIMUActive)
			throw std::runtime_error("A ECHO measurement must have at least 3 entries: "
			"The 2 encrage, the stationned point and the measured distance.");

		// Initialize the station
		if (encrage1 != tokens.at(0) || encrage2 != tokens.at(2))
		{
			encrage1 = tokens.at(0);
			encrage2 = tokens.at(2);

			const std::string& name = "ECHOPLANE" + std::to_string(proj.getCurrentNode().measurements.fECHO.size()); //name of the measured adjustable plane

			fplanes.addObject(LGCAdjustablePlane::createUninitialized(name));
			TECHOROM echoRom(fplanes.back());

			echoRom.line = line;
			proj.getCurrentNode().measurements.fECHO.emplace_back(echoRom); //add new round of measurement
			proj.getCurrentNode().measurements.fECHO.back().line = line;

			//initialise the plane instead of in TDataAnalyser
			const LGCAdjustablePoint& p1 = fpoints.getObject(encrage1);
			const LGCAdjustablePoint& p2 = fpoints.getObject(encrage2);
			TReal referencePoint[3] = { p1.getEstimatedValue().getX().getMetresValue(), 
										p1.getEstimatedValue().getY().getMetresValue(),
										p1.getEstimatedValue().getZ().getMetresValue() };
			TReal initialRefPtDistance = 0.0;

			/*Fixed reference point for the ECHO measurement*/
			LGCAdjustablePoint& rp =
				proj.getPoints().addObject(LGCAdjustablePoint(TPositionVector(referencePoint[0], referencePoint[1], referencePoint[2], TCoordSysFactory::ECoordSys::k3DCartesian),
				true, true, true, "ECHO_line" + std::to_string(line), proj.getConfig().referential, proj.getTree().begin()));

			TReal thetaLineVectorAngle = atan2q(p2.getEstimatedValue().getX().getMetresValue() - p1.getEstimatedValue().getX().getMetresValue(), p2.getEstimatedValue().getY().getMetresValue() - p1.getEstimatedValue().getY().getMetresValue());

			proj.getCurrentNode().measurements.fECHO.back().fMeasuredPlane->initialize(&rp, TLength(initialRefPtDistance), TAngle(thetaLineVectorAngle, TAngle::EUnits::kRadians),
				TAngle(M_PI_2, TAngle::EUnits::kRadians), false, true);

			// Create measurements for the encrage points
			TInstrumentData::TSCALE& instr = proj.getInstruments().fSCALE["ECHOInstr"];
			instr.sigmaD = sigma*0.01;

			TECHO echo1(p1, instr, TLength(fSIMUActive ? NO_VALf : 0.0));
			echo1.setFirstEquationIndex(proj.fUEOIndices.EIndex);
			echo1.setFirstObservationIndex(proj.fUEOIndices.OIndex);
			proj.fUEOIndices.EIndex++;
			proj.fUEOIndices.OIndex++;
			echo1.line = line;
			proj.addToMeasurementNum(TMeasurementsGlobal::kECHO);
			TECHOROM& echoROMLatest = proj.getCurrentNode().measurements.fECHO.back();
			echoROMLatest.measECHO.emplace_back(echo1);

			TECHO echo2(p2, instr, TLength(fSIMUActive ? NO_VALf : 0.0));
			echo2.setFirstEquationIndex(proj.fUEOIndices.EIndex);
			echo2.setFirstObservationIndex(proj.fUEOIndices.OIndex);
			proj.fUEOIndices.EIndex++;
			proj.fUEOIndices.OIndex++;
			echo2.line = line;
			proj.addToMeasurementNum(TMeasurementsGlobal::kECHO);
			echoROMLatest.measECHO.emplace_back(echo2);

		}

		//This is a position of station point from which the plane is measured in the ECHO class it has a 'traget' name, since the abstract class is used. Bit confusing to be improved.
		const auto& stationPoint(fpoints.getObject(tokens.at(1)));
		//The SCALE instrument is only the default one used, it is not stored in TECHOROM because it is specific for each observation
		TInstrumentData::TSCALE& instr = proj.getInstruments().fSCALE["ECHOInstr"];
		instr.distCorrectionValue = constante;
		instr.sigmaD = sigma;

		if (tokens.back().at(0) == '$' || tokens.back().at(0) == '%')
		{
			if (tokens.size() == 6 && !tokens.at(4).compare(0, 1, "/"))
			{
				instr.sigmaD = sigma;
				constante = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				instr.distCorrectionValue = constante;
			}
			else if (tokens.size() == 6 && tokens.at(4).compare(0, 1, "/"))
			{
				instr.sigmaD = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				instr.distCorrectionValue = constante;
			}
			else if (tokens.size() == 7)
			{
				instr.sigmaD = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				constante = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				instr.distCorrectionValue = constante;
			}
		}
		else
		{
			if (tokens.size() == 5 && !tokens.at(4).compare(0, 1, "/"))
			{
				instr.sigmaD = sigma;
				constante = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				instr.distCorrectionValue = constante;
			}
			else if (tokens.size() == 5 && tokens.at(4).compare(0, 1, "/"))
			{
				instr.sigmaD = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				instr.distCorrectionValue = constante;
			}
			else if (tokens.size() == 6)
			{
				instr.sigmaD = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				constante = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				instr.distCorrectionValue = constante;
			}
		}

		// Store  the measured value
		TECHO echo(stationPoint, instr, TLength(fSIMUActive ? NO_VALf : std::stor(tokens.at(3))));
		echo.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		echo.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		echo.line = line;
		proj.addToMeasurementNum(TMeasurementsGlobal::kECHO);
		TECHOROM& echoROMLatest = proj.getCurrentNode().measurements.fECHO.back();
		echoROMLatest.measECHO.emplace_back(echo);


		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : echoROMLatest.measECHO)
				if (stationPoint.getName() == point.targetPos->getName())
					throw std::runtime_error("An ECHO measurement is duplicated");
	}
}

void TKeyECSP_lgc1::parse(const std::vector<std::string>& tokens, int line)
{

	//create a scale instrument for ecth measurements
	if (firstmeas)
	{//create a default scale instrument
		const TInstrumentData::TSCALE scl = {
			"ECSPInstr",
			TLength(1.0, TLength::EUnits::kMillimetres), //sigma
			TLength(0.0, TLength::EUnits::kMillimetres), //ppm
			TLength(0.0, TLength::EUnits::kMillimetres), //dcorrvalue
			TLength(0.0, TLength::EUnits::kMillimetres), //sigma dcorr
			TLength(0.0, TLength::EUnits::kMillimetres)  //centering
		};

		// store the new station
		finstruments.fSCALE.insert(std::make_pair("ECSPInstr", scl));

		if (tokens.size() == 3)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);

		dcorr = TLength(0.0, TLength::EUnits::kMetres);
		firstmeas = false;

	}

	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline)
	{
		// *ECSP [sigma]
		if (tokens.size() == 3)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);

		dcorr = TLength(0.0, TLength::EUnits::kMetres);
		point1 = "";
		point2 = "";
	}
	else
	{
		// p1 stn p2 meas [sigma][/const]
		if (tokens.size() < 4 && !fSIMUActive)
			throw std::runtime_error("A ECSP measurement must have at least 3 entries: "
			"The instrument position, the measured point and the offset measurement.");

		// Initialize the station
		if (point1 != tokens.at(0) || point2 != tokens.at(2))
		{
			point1 = tokens.at(0);
			point2 = tokens.at(2);
			const std::string& name = "ECSPLINE" + std::to_string(proj.getCurrentNode().measurements.fECVE.size()); //name of the measured adjustable line

			//The line will be initialized in TDataAnalyzer class, when checked for consistency
			TECSPROM ecspRom(name, fpoints.getObject(point1), fpoints.getObject(point2));
			ecspRom.line = line;
			proj.getCurrentNode().measurements.fECSP.emplace_back(ecspRom); //add new round of measurement
		}


		/*This is a position of station point from which the plane is measured in the ECVE class it has a 'traget' name, since the abstract class is used. Bit confusing to be improved. */
		const auto& stationPoint(fpoints.getObject(tokens.at(1)));
		//The SCALE instrument is only the default one used, it is not stored in TECHOROM because it is specific for each observation
		TInstrumentData::TSCALE& instr = proj.getInstruments().fSCALE["ECSPInstr"];
		instr.distCorrectionValue = dcorr;
		instr.sigmaD = sigma;

		if (tokens.back().at(0) == '$' || tokens.back().at(0) == '%')
		{
			if (tokens.size() == 6 && !tokens.at(4).compare(0, 1, "/"))
			{
				instr.sigmaD = sigma;
				dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				instr.distCorrectionValue = dcorr;
			}
			else if (tokens.size() == 6 && tokens.at(4).compare(0, 1, "/"))
			{
				instr.sigmaD = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				instr.distCorrectionValue = dcorr;
			}
			else if (tokens.size() == 7)
			{
				instr.sigmaD = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				instr.distCorrectionValue = dcorr;
			}
		}
		else
		{
			if (tokens.size() == 5 && !tokens.at(4).compare(0, 1, "/"))
			{
				instr.sigmaD = sigma;
				dcorr = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				instr.distCorrectionValue = dcorr;
			}
			else if (tokens.size() == 5 && tokens.at(4).compare(0, 1, "/"))
			{
				instr.sigmaD = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				instr.distCorrectionValue = dcorr;
			}
			else if (tokens.size() == 6)
			{
				instr.sigmaD = TLength(std::stor(tokens.at(4)), TLength::EUnits::kMillimetres);
				dcorr = TLength(std::stor(tokens.at(5).substr(1)), TLength::EUnits::kMetres);
				instr.distCorrectionValue = dcorr;
			}
		}


		// Store  the measured value
		TECSP ecsp(stationPoint, instr, TLength(fSIMUActive ? NO_VALf : std::stor(tokens.at(3))));
		ecsp.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		ecsp.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		ecsp.line = line;
		proj.addToMeasurementNum(TMeasurementsGlobal::kECSP);
		TECSPROM& ecspROMLatest = proj.getCurrentNode().measurements.fECSP.back();
		ecspROMLatest.measECSP.emplace_back(ecsp);

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : ecspROMLatest.measECSP)
				if (stationPoint.getName() == point.targetPos->getName())
					throw std::runtime_error("An ECSP measurement is duplicated");

	}
}

void TKeyECVE_lgc1::parse(const std::vector<std::string>& tokens, int line)
{
	if (fistrECVE)
	{
		//create a default scale instrument
		const TInstrumentData::TSCALE scl = {
			"ECVEInstr",
			TLength(1.0, TLength::EUnits::kMillimetres), //sigma
			TLength(0.0, TLength::EUnits::kMillimetres), //ppm
			TLength(0.0, TLength::EUnits::kMillimetres), //dcorrvalue
			TLength(0.0, TLength::EUnits::kMillimetres), //sigma dcorr
			TLength(0.0, TLength::EUnits::kMillimetres)  //centering
		};

		// store the new station
		finstruments.fSCALE.insert(std::make_pair("ECVEInstr", scl));

		if (tokens.size() == 3)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);

		constante = TLength(0.0, TLength::EUnits::kMetres);
		fistrECVE = false;
	}


	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline)
	{
		// *ECVE [sigma]
		if (tokens.size() == 3)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);
		constante = TLength(0.0, TLength::EUnits::kMetres);
		ptLine = "";
	}
	else
	{
		// p1 stn p2 meas [sigma][/const]
		if (tokens.size() < 3 && !fSIMUActive)
			throw std::runtime_error("A ECVE measurement must have at least 3 entries: "
			"The instrument position, the measured point and the offset measurement.");

		// Initialize the station
		if (ptLine != tokens.at(0))
		{
			ptLine = tokens.at(0);

			const std::string& name = "ECVELINE" + std::to_string(proj.getCurrentNode().measurements.fECVE.size()); //name of the measured adjustable line

			/*The pointLine is known (ref point = point on the line)*/
			flines.addObject(LGCAdjustableLine(&fpoints.getObject(ptLine), TFreeVector(0.0, 0.0, 1.0, TCoordSysFactory::ECoordSys::k3DCartesian), std::bitset<3>(111), name));

			//The line will be initialized in TDataAnalyzer class, when checked for consistency
			TECVEROM ecveRom(flines.back());
			ecveRom.line = line;
			proj.getCurrentNode().measurements.fECVE.emplace_back(ecveRom); //add new round of measurement
		}


		/*This is a position of station point from which the plane is measured in the ECVE class it has a 'traget' name, since the abstract class is used. Bit confusing to be improved. */
		const auto& stationPoint(fpoints.getObject(tokens.at(1)));
		//The SCALE instrument is only the default one used, it is not stored in TECHOROM because it is specific for each observation
		TInstrumentData::TSCALE& instr = proj.getInstruments().fSCALE["ECVEInstr"];
		instr.distCorrectionValue = constante;
		instr.sigmaD = sigma;

		if (tokens.back().at(0) == '$' || tokens.back().at(0) == '%')
		{
			if (tokens.size() == 5 && !tokens.at(3).compare(0, 1, "/"))
			{
				instr.sigmaD = sigma;
				constante = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
				instr.distCorrectionValue = constante;
			}
			else if (tokens.size() == 5 && tokens.at(3).compare(0, 1, "/"))
			{
				instr.sigmaD = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				instr.distCorrectionValue = constante;
			}
			else if (tokens.size() == 6)
			{
				instr.sigmaD = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				constante = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				instr.distCorrectionValue = constante;
			}
		}
		else
		{
			if (tokens.size() == 4 && !tokens.at(3).compare(0, 1, "/"))
			{
				instr.sigmaD = sigma;
				constante = TLength(std::stor(tokens.at(3).substr(1)), TLength::EUnits::kMetres);
				instr.distCorrectionValue = constante;
			}
			else if (tokens.size() == 4 && tokens.at(3).compare(0, 1, "/"))
			{
				instr.sigmaD = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				instr.distCorrectionValue = constante;
			}
			else if (tokens.size() == 5)
			{
				instr.sigmaD = TLength(std::stor(tokens.at(3)), TLength::EUnits::kMillimetres);
				constante = TLength(std::stor(tokens.at(4).substr(1)), TLength::EUnits::kMetres);
				instr.distCorrectionValue = constante;
			}
		}


		// Store  the measured value
		TECVE ecve(stationPoint, instr, TLength(fSIMUActive ? NO_VALf : std::stor(tokens.at(2))));
		ecve.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		ecve.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		ecve.line = line;
		proj.addToMeasurementNum(TMeasurementsGlobal::kECVE);
		TECVEROM& ecveROMLatest = proj.getCurrentNode().measurements.fECVE.back();
		ecveROMLatest.measECVE.emplace_back(ecve);

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : ecveROMLatest.measECVE)
				if (stationPoint.getName() == point.targetPos->getName())
					throw std::runtime_error("An ECVE measurement is duplicated");

	}
}

void TKeyORIE_lgc1::parse(const std::vector<std::string>& tokens, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline){
		if (tokens.size() == 3)
			sigma = TAngle(std::stor(tokens.at(2)), TAngle::EUnits::kCCs);
		else
			sigma = TAngle(1.0, TAngle::EUnits::kCCs);
		constante = TAngle(0.0, TAngle::EUnits::kGons);

		currentStation = "";
	}
	else
	{
		if (tokens.size() < 3 && !fSIMUActive)
			throw std::runtime_error("ORIE measurement must have at least 3 entries: station, targeted point and observed value");

		if (tokens.at(0) != currentStation)
		{
			currentStation = tokens.at(0);
			TORIEROM orieROM(fpoints.getObject(tokens.at(0)), getPolarInstr());
			orieROM.line = line;

			proj.getCurrentNode().measurements.fORIE.emplace_back(orieROM); //add new round of measurement
		}


		// look up the observed point, i.e. the target
		const auto& obspt(fpoints.getObject(tokens.at(1)));
		TORIEROM& rom = proj.getCurrentNode().measurements.fORIE.back();

		//NODUP used
		if (proj.getConfig().nodup.isActive())
			for (auto& point : proj.getCurrentNode().measurements.fORIE.back().measORIE)
				if (obspt.getName() == point.targetPos->getName())
					throw std::runtime_error("An ORIE measurement is duplicated");

		// get a station reference to update default values
		TInstrumentData::TPOLAR& instrument = proj.getCurrentNode().measurements.fORIE.back().instrument;
		// get a copy of  the specified target and update it
		TInstrumentData::TPOLAR::TTarget tgt(finstruments.getDevice(instrument.targets, instrument.defTarget));

		tgt.sigmaAngl = sigma;
		rom.fConstantAngle = constante;

		if (tokens.back().at(0) == '$' || tokens.back().at(0) == '%')
		{
			if (tokens.size() == 5 && tokens.at(3).compare(0, 1, "/"))
				tgt.sigmaAngl = TAngle(stor(tokens.at(3)), TAngle::EUnits::kCCs);
			else if (tokens.size() == 5 && !tokens.at(3).compare(0, 1, "/"))
			{
				constante = TAngle(stor(tokens.at(3).substr(1)), TAngle::EUnits::kGons);
				rom.fConstantAngle = constante;
			}
			else if (tokens.size() == 6)
			{
				tgt.sigmaAngl = TAngle(stor(tokens.at(3)), TAngle::EUnits::kCCs);
				constante = TAngle(stor(tokens.at(4).substr(1)), TAngle::EUnits::kGons);
				rom.fConstantAngle = constante;
			}
		}
		else
		{
			if (tokens.size() == 4 && tokens.at(3).compare(0, 1, "/"))
				tgt.sigmaAngl = TAngle(stor(tokens.at(3)), TAngle::EUnits::kCCs);
			else if (tokens.size() == 4 && !tokens.at(3).compare(0, 1, "/"))
			{
				constante = TAngle(stor(tokens.at(3).substr(1)), TAngle::EUnits::kGons);
				rom.fConstantAngle = constante;
			}
			else if (tokens.size() == 5)
			{
				tgt.sigmaAngl = TAngle(stor(tokens.at(3)), TAngle::EUnits::kCCs);
				constante = TAngle(stor(tokens.at(4).substr(1)), TAngle::EUnits::kGons);
				rom.fConstantAngle = constante;
			}
		}

		// set measurement value
		TORIE orie(obspt, tgt);
		orie.line = line;

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			orie.eolcomment = tokens.back();

		// set indices of LS matrices, ORIE introduces 1 equation and 1 observation, index of observation is not stored since it is not used, but need to be counted
		orie.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		orie.setFirstObservationIndex(proj.fUEOIndices.OIndex);

		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kORIE);

		if (!fSIMUActive)
			orie.setAngle(TAngle(std::stor(tokens.at(2)), TAngle::kGons));

		proj.getCurrentNode().measurements.fORIE.back().measORIE.emplace_back(orie);
	}
}

void TKeyRADI_lgc1::parse(const std::vector<std::string>& tokens, int line)
{
	bool firstline(tokens.size() > 0 && tokens.at(0) == "*");
	if (firstline)
	{
		if (tokens.size() > 2)
			sigma = TLength(std::stor(tokens.at(2)), TLength::EUnits::kMillimetres);
		else
			sigma = TLength(1.0, TLength::EUnits::kMillimetres);
	}
	else {
		if (tokens.size() < 2 && !fSIMUActive)
			throw std::runtime_error("A RADI constraint must have at least 2 entries: "
			"One point and the angle.");

		// Store  the measured value
		proj.getCurrentNode().measurements.fRADI.emplace_back(
			TRADI(fpoints.getObject(tokens.at(0)), TAngle(fSIMUActive ? NO_VALf : std::stor(tokens.at(1)), TAngle::EUnits::kGons))
			);

		auto& radi(proj.getCurrentNode().measurements.fRADI.back());

		if (tokens.size() >= 3 && tokens.at(2).compare(0, 1, "$") && tokens.at(2).compare(0, 1, "%"))
			radi.setObservedStDev(TLength(std::stor(tokens.at(2)),TLength::EUnits::kMillimetres));
		else
			radi.setObservedStDev(sigma);

		radi.line = line;

		//If last token starts with a comment character, store it as a end of line comment
		const char fOfLastToken = tokens.back().at(0);
		if (fOfLastToken == '$' || fOfLastToken == '%')
			radi.eolcomment = tokens.back();


		radi.setFirstEquationIndex(proj.fUEOIndices.EIndex);
		radi.setFirstObservationIndex(proj.fUEOIndices.OIndex);
		proj.fUEOIndices.EIndex++;
		proj.fUEOIndices.OIndex++;
		proj.addToMeasurementNum(TMeasurementsGlobal::kRADI);
	}

	auto& debug = proj.getCurrentNode().measurements;
}
