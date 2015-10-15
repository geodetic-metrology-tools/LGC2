////////////////////////////////////////////////////////////////////
// TLSCalcWorkingSpaDistObs.cpp :Implementation file
// Container for the least squares calculation spatial distance observations
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////



#include "TLSCalcWorkingSpaDistObs.h"

//////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////
TLSCalcWorkingSpaDistObs::TLSCalcWorkingSpaDistObs()
{// Default constructor 
	fNumDMES = 0;
	fDMESStatisticsOK = false; /*! flags if statistics are up to date or not */
	fDMESObsSummary.clear();
	fDMESObsSummary.defineLengthObservation();

	fNumDTHE = 0;
	fDTHEStatisticsOK = false; /*! flags if statistics are up to date or not */
	fDTHEObsSummary.clear();
	fDTHEObsSummary.defineLengthObservation();
}

 
TLSCalcWorkingSpaDistObs::~TLSCalcWorkingSpaDistObs()
{//Destructor
}


////////////////////////////////////////////////////////////////
// PUBLIC METHODS		
////////////////////////////////////////////////////////////////
/* Compiles the DMES observations' summary statistics */
void	TLSCalcWorkingSpaDistObs::compileDMESObsSummary()
{
	if( !dmesStatisticsOK() )  	
	{
		LSSpaDistIter	iter = begin();
		LSSpaDistIter	iterE = end();

		// clear the observation summary statistics
		fDMESObsSummary.clear();

		// add the current observation results to the observation summary
		// iterate through all the observations
		while ( iter != iterE )
		{
			// add the DMES observation residual to the observation summary
			if(iter->getHInstStatus() == TALSCalcParameter::kFixed)
					fDMESObsSummary.addNewResidual(iter->getResidue().getMMetresValue());
			iter++;
		}

		fDMESStatisticsOK = true;
	}

	return;
}

/* Returns the DMES observations' summary statistics*/
TLGCObsSummary	TLSCalcWorkingSpaDistObs::getDMESObsSummary()
{
	compileDMESObsSummary();

	return fDMESObsSummary;
}


/* Compiles the DTHE observations' summary statistics */
void	TLSCalcWorkingSpaDistObs::compileDTHEObsSummary()
{
	if( !dtheStatisticsOK() )  	
	{
		LSSpaDistIter	iter = begin();
		LSSpaDistIter	iterE = end();

		// clear the observation summary statistics
		fDTHEObsSummary.clear();

		// add the current observation results to the observation summary
		// iterate through all the observations
		while ( iter != iterE )
		{
			// add the DTHE observation residual to the observation summary
			if(iter->getHInstStatus() == TALSCalcParameter::kVariable)
					fDTHEObsSummary.addNewResidual(iter->getResidue().getMMetresValue());
			iter++;
		}

		fDTHEStatisticsOK = true;
	}

	return;
}

/* Returns the DTHE observations' summary statistics*/
TLGCObsSummary	TLSCalcWorkingSpaDistObs::getDTHEObsSummary()
{
	compileDTHEObsSummary();

	return fDTHEObsSummary;
}
		

////////////////////////////////////////////////////////////////
//END	
////////////////////////////////////////////////////////////////
