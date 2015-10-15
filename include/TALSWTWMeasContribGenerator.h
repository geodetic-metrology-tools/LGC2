/**Abstract Base Class. Defines the methods common to all LS contrib generator processing wire to wire distance measurements*/



#ifndef SU_VLS_WTW_DIST_MEASCONTGENERATOR
#define SU_VLS_WTW_DIST_MEASCONTGENERATOR


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
class TLSInputMatrices;
#include "TLSCalcWorkingStations.h"
#include "TLSCalcWTWStation.h"
#include "TAObsContributionsGenerator.h"

////////////////////
// Class declaration
////////////////////
class TALSWTWMeasContribGenerator : public TAObsContributionsGenerator{

public :

	/**Sets the current level station considered in the calculation of the contributions
	@param iterLevSt an iterator pointing to the relevant LSCalc level station*/
	virtual void	setCurrentWTWStation(TLSCalcWorkingStations::CalcWTWStIterator iterWTWSt);

	/**Calculates the contributions of a wire to wire distance and puts them in the matrices
	@param iterWTW an iterator pointing to the wire to wire distance to be processed*/
	virtual	void	processWTWDistMeas(TLSCalcWTWStation::CalcWTWDistMeasIterator iterWTW) = 0;

	///Destructor
	virtual ~TALSWTWMeasContribGenerator();

protected:

	///Default constructor
	TALSWTWMeasContribGenerator();
	/**Constructor
	@param im a pointer to the LS input matrices (for the generator to know where the put the contributions)*/
	explicit TALSWTWMeasContribGenerator(TLSInputMatrices* im);
	///Copy constructor
	TALSWTWMeasContribGenerator(const TALSWTWMeasContribGenerator& source);

	TLSInputMatrices *									fInputMatrices;
	TLSCalcWorkingStations::CalcWTWStIterator			fCurrentStation;

	double												fS0APrioriScaleFactor;
	
};
#endif