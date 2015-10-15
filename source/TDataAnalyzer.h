#ifndef TData_Analyzer_H
#define TData_Analyzer_H

#include "TLGCData.h"
#include "TLSConstraintIdentifier.h"

/*!
	This struct captures all the specifics of particular calculation, tells if it is a free network adjustment, stores eventual constraints to be added.
	Includes variables (fCombinedCase, fStandDevUsed) which determines a LS model needed.
*/
struct TCalcType{
		TLSConstraintIdentifier freeNetwCnstr;
		bool fPDORused;
		bool fLIBR;
		bool fOrieUsed;
		TLGCRefFrame::ERefs fReferential;

		bool fCombinedCase;
		bool fStandDevUsed;
};

/*! 
	Class responsible for:
		- consistency check of the TLGCData class, i.e. that all objects were initialized
		- initialization of some adjustable objects such as adjustable planes in DLEV measurements (calculation of reference point might be required) 
		- assigning unknowns indices to adjustable objects
		- identification of the calculation type and applying changes (add constraints, set all points to be fixed, simulations,...)
		- LS scenario identification (combined case, parametric case), with constraints/without constraints etc.
*/
class TDataAnalyzer{
public:
	/*!
		Constructs a new TDataAnalyzer instance.

		\param[in] dat given TLGCData class instance to be analyzed
	*/
	TDataAnalyzer(TLGCData& dat);

	/*!
		Returns TRUE if data is consistent, FALSE otherwise. 
		\note This method is also responsible for assigning indices to unknowns and initializing some uninitialized objects (e.g. planes in DLEV measurement).
	*/
	bool dataConsistent();

	/*! Identify calculation type and propagate changes.*/
	TCalcType getCalculationType();

private:
	// Identifies and adds network constraints
	void addNetworkConstraints();

	// Simulation option used
	void setSimulation();

	bool fStandDevUsed; // If at least one standard deviation assigned to a point or frame element.

	// contains references: not assignable
	TDataAnalyzer& operator=(const TDataAnalyzer&);

	// LGC data set
	TLGCData& fData;


};

#endif