#ifndef MEASUREMENTS_H_
#define MEASUREMENTS_H_

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

//LGC
#include <TEDM.h>
#include <TTSTN.h>
#include <TLEVEL.h>
#include <TCAM.h>
#include <RoundOfMeasurements.h>

/*!
	\ingroup Measurements
	The measurement class bundles all possible measurements together. Every node of the tree of local frames has one class of this type.
*/
struct TMeasurements {
	/// All total station measurements in the order in which they appeared in the input file
	std::list<shared_ptr<TTSTN>>  fTSTN;

	/// All electronic distance measurements in the order in which they appeared in the input file
	std::list<TEDM>   fEDM;

	/// All leveling measurements in the order in which they appeared in the input file
	std::list<TLEVEL> fLEVEL;

	/// All measurements made by cameras in the order in which they appeared in the input file
	std::list<TCAM> fCAM;

	/// All DVER measurements in the order in which they appeared in the input file
	std::list<TDVER> fDVER;

	/// All ORIE (and PDOR if used) measurements in which they appeared in the input file
	std::list<TORIEROM> fORIE;

	/// All ECHO measurements in which they appeared in the input file
	std::list<TECHOROM> fECHO;

	/// All ECVE measurements in which they appeared in the input file
	std::list<TECVEROM> fECVE;

	/// All ECSP measurements in which they appeared in the input file
	std::list<TECSPROM> fECSP;

	///pdor measurement in which they appeared in the input file
	TPdorObs fPDOR;

	/// All RADI measurements in which they appeared in the input file
	std::list<TRADI> fRADI;

	/// All OBSXYZ measurements in which they appeared in the input file
	std::list<TOBSXYZ> fOBSXYZ;
};

#endif // MEASUREMENTS_H_
