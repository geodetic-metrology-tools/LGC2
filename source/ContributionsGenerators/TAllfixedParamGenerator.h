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
	TAngle getV0AllfixedANGL(const TTSTN& station, const TTSTN::TROM& rom, const TANGL& angl);
	//TAngle getV0AllfixedECSP(const TTSTN& station, const TTSTN::TROM& rom, const TECSP& ecsp);
	TAngle getV0AllfixedECTH(const TTSTN& station, const TECTH& ecth);
	TAngle getV0AllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);
	TAngle getRxAllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);
	TAngle getRyAllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);
	TAngle getCollimationAllfixedDLEV(const TLEVEL& levelST, const TDLEV& dlev);

	TLength getHiAllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);
	TLength getHiAllfixedZEND(const TTSTN& station, const TZEND& zend);
	TLength getHiAllfixedDIST(const TTSTN& station, const TLINE& dist);
	TLength getCsAllfixedDHOR(const TTSTN& station, const TLINE& dist);
	TLength getCsAllfixedDSPT(const TEDM& edmST, const TDSPT& dspt);
	TLength getCsAllfixedDIST(const TTSTN& station, const TLINE& dist);
	TLength getCsAllfixedPLR(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);

	//@}

private:
	//Pointer to a TPointTransformationFunctions
	TPointTransformer& fPointTransfo;

	//solution of the equation A*sinX + B*cosX + C =0
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