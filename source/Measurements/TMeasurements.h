#ifndef MEASUREMENTS_H_
#define MEASUREMENTS_H_
#include "TEDM.h"
#include "TTSTN.h"
#include "TLEVEL.h"
#include "TCAM.h"
#include "RoundOfMeasurements.h"

/*!
	\ingroup Measurements
	The measurement class bundles all possible measurements together. Every node of the tree of local frames has one class of this type.
*/
struct TMeasurements {
	/// All total station measurements in the order in which they appeared in the input file
	std::vector<shared_ptr<TTSTN>>  fTSTN;

	/// All electronic distance measurements in the order in which they appeared in the input file
	std::vector<TEDM>   fEDM;

	/// All leveling measurements in the order in which they appeared in the input file
	std::vector<TLEVEL> fLEVEL;

	/// All measurements made by cameras in the order in which they appeared in the input file
	std::vector<TCAM> fCAM;

	/// All DVER measurements in the order in which they appeared in the input file
	std::vector<TDVER> fDVER;

	/// All ORIE (and PDOR if used) measurements in which they appeared in the input file
	std::vector<TORIEROM> fORIE;

	/// All ECHO measurements in which they appeared in the input file
	std::vector<TECHOROM> fECHO;

	/// All ECVE measurements in which they appeared in the input file
	std::vector<TECVEROM> fECVE;

	///pdor measurement in which they appeared in the input file
	TPdorObs fPDOR;

	/// All RADI measurements in which they appeared in the input file
	std::vector<TRADI> fRADI;

};

#endif // MEASUREMENTS_H_
