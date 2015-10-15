#ifndef _MEASUREMENTS_H_
#define _MEASUREMENTS_H_

#include <array>

#include <Global.h>
#include <TInstrumentData.h>
#include "TEDM.h"
#include "TTSTN.h"
#include "TLEVEL.h"
#include "TCAM.h"


/*!
	The measurement class bundles all possible measurements together. 
*/
struct TMeasurements {
	/// All total station measurements in the order in which they appeared in the input file
	std::vector<TTSTN>  fTSTN;
	/// All electronic distance measurements in the order in which they appeared in the input file
	std::vector<TEDM>   fEDM;
	/// All leveling measurements in the order in which they appeared in the input file
	std::vector<TLEVEL> fLEVEL;
	/// All measurements made by cameras in the order in which they appeared in the input file
	std::vector<TCAM> fCAM;

	/// All DVER measurements are stored globally
	std::vector<TDVER> fDVER;

	/// Tells if the measurement values should be ignored, e.g. during a simulation
	const bool& fignoreValues;

	/*!
		Default constructor for the measurements collection. The object needs to know if the measured values should be
		ignored, for example when a simulation is run.

		\param ignoreValues Reference to the boolean information if the measured values should be ignored.
		A suitable member would be  TLGCConfig::Simulation::ignoreMeasurements.
	*/
	TMeasurements(const bool& ignoreValues) :
		fignoreValues(ignoreValues) {}

	private:
		TMeasurements& operator=(const TMeasurements&);
};

#endif
