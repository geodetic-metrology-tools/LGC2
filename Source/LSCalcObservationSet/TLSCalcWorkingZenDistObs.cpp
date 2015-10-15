////////////////////////////////////////////////////////////////////
// TLSCalcWorkingZenDistObs.cpp :Implementation file
// Container for the least squares calculation zenith distance observations
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////



#include "TLSCalcWorkingZenDistObs.h"

//////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////

// Default constructor 
TLSCalcWorkingZenDistObs::TLSCalcWorkingZenDistObs()
{
	fNumZeni = 0;
	fZENIStatisticsOK = false; /*! flags if statistics are up to date or not */
	fZENIObsSummary.clear();
	fZENIObsSummary.defineAngleObservation();

	fNumZenh = 0;
	fZENHStatisticsOK = false; /*! flags if statistics are up to date or not */
	fZENHObsSummary.clear();
	fZENHObsSummary.defineAngleObservation();

}

//Destructor 
TLSCalcWorkingZenDistObs::~TLSCalcWorkingZenDistObs() {
}

////////////////////////////////////////////////////////////////
// PUBLIC METHODS		
////////////////////////////////////////////////////////////////

// Compiles the ZENI observations' summary statistics
void	TLSCalcWorkingZenDistObs::compileZENIObsSummary()
{
	if( !zeniStatisticsOK() )  	
	{
		LSZenDistIter	iter = begin();
		LSZenDistIter	iterE = end();

		// clear the observation summary statistics
		fZENIObsSummary.clear();

		// add the current observation results to the observation summary
		// iterate through all the observations
		while ( iter != iterE )
		{
			// add the ZENI observation residual to the observation summary
			if(iter->getHInstrStatus() == TALSCalcParameter::kVariable)
					fZENIObsSummary.addNewResidual(iter->getResidue().getSignedCCValue());

			iter++;
		}

		fZENIStatisticsOK = true;
	}

	return;
}


// Compiles the ZENH observations' summary statistics
void	TLSCalcWorkingZenDistObs::compileZENHObsSummary()
{
	if( !zenhStatisticsOK() )  	
	{
		LSZenDistIter	iter = begin();
		LSZenDistIter	iterE = end();

		// clear the observation summary statistics
		fZENHObsSummary.clear();

		// add the current observation results to the observation summary
		// iterate through all the observations
		while ( iter != iterE )
		{
			// add the ZENH observation residual to the observation summary
			if(iter->getHInstrStatus() == TALSCalcParameter::kFixed)
					fZENHObsSummary.addNewResidual(iter->getResidue().getSignedCCValue());

			iter++;
		}

		fZENHStatisticsOK = true;
	}

	return;
}


// Returns the ZENI observations' summary statistics
TLGCObsSummary	TLSCalcWorkingZenDistObs::getZENIObsSummary()
{
	compileZENIObsSummary();

	return fZENIObsSummary;
}
		

// Returns the ZENH observations' summary statistics
TLGCObsSummary	TLSCalcWorkingZenDistObs::getZENHObsSummary()
{
	compileZENHObsSummary();

	return fZENHObsSummary;
}
		

	
