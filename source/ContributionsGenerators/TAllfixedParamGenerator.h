#ifndef TALLFIXED_PARAM_GENERATOR_H
#define TALLFIXED_PARAM_GENERATOR_H

#include "ContributionStructures.h"
#include "TPointTransformer.h"
#include "TTSTN.h"
struct TLEVEL;
struct TEDM;



/*!
\ingroup AllfixedParamGenerator

\brief Calculates the implicite unknown parameters by observation to analyse the network. (ALLFIXED option)

*/

class TAllfixedParamGenerator{

public:
	/// Constructor taking a reference to a TPointTransformer
	TAllfixedParamGenerator(TPointTransformer& fPointTransfoFunc);

	/*!@name Method for calculating specific parameter values (ALLFIXED) . */
	//@{

	/// Returns the V0 for an ANGL measurement
	TAngle getV0AllfixedANGL(const TTSTN& station, const TTSTN::TROM& rom, const TANGL& angl);

	/// Returns the V0 for an ECSP measurement
	TAngle* getV0AllfixedECSP(const TTSTN& station, const TTSTN::TROM& rom, const TECSP& ecsp);

	/// Returns the V0 for an ECTH measurement
	TAngle getV0AllfixedECTH(const TTSTN& station, const TECTH& ecth);

	/// Returns the V0 for a PLR measurement
	TAngle* getV0AllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);

	/// Returns RX for a PLR measurement
	TAngle* getRxAllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);

	/// Returns Ry for a PLR measurement
	TAngle* getRyAllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);

	/// Returns the collimation angle for a DLEV measurement
	TAngle getCollimationAllfixedDLEV(const TLEVEL& levelST, const TDLEV& dlev);

	/// Returns the instrument heigth for a PLR measurement
	TLength getHiAllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);

	/// Returns the instrument heigth for a ZEND measurement
	TLength getHiAllfixedZEND(const TTSTN& station, const TZEND& zend);

	/// Returns the instrument heigth for a DIST measurement
	TLength getHiAllfixedDIST(const TTSTN& station, const TLINE& dist);

	/// Returns the distance correction for a DHOR measurement
	TLength getCsAllfixedDHOR(const TTSTN& station, const TLINE& dist);

	/// Returns the distance correction for a DSPT measurement
	TLength getCsAllfixedDSPT(const TEDM& edmST, const TDSPT& dspt);

	/// Returns the distance correction for a DIST measurement
	TLength getCsAllfixedDIST(const TTSTN& station, const TLINE& dist);

	/// Returns the distance correction for a PLR measurement
	TLength getCsAllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);

	//@}

private:
	//Pointer to a TPointTransformationFunctions
	TPointTransformer& fPointTransfo;

	//solution of the equation A*cosX + B*sinX + C =0
	TAngle fSolutionTrigo[2];

	//solution of the equation a*x^2 + b*x + c =0
	TReal fSolution2ndD[2];

	// solve second degree equation
	void solve2ndDegree(TReal a, TReal b, TReal c);

	// solve trigo equation
	void solveTrigoEquation(TReal A, TReal B, TReal C);

	// reinitialize fSolutionTrigo & fSolution2ndD
	void initSolution();

};
#endif