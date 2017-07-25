#ifndef TData_Analyzer_H
#define TData_Analyzer_H

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

class TLGCData;
class TFileLogger;

/*! 
	\ingroup LGCProjectData

	Class responsible for:

		- consistency check of the TLGCData class, i.e. that all objects were initialized

		- initialization of some adjustable objects such as adjustable planes in DLEV or ECHO measurements

		- assigning unknowns indices to adjustable objects

		- identification of the calculation type and applying changes (add constraints, set all points to be fixed, simulations,...)

		- LS scenario identification (combined case, parametric case), with constraints/without constraints etc.
*/
class TDataAnalyzer{
public:
	/*!
		\brief Constructs a new TDataAnalyzer instance.

		\param[in] dat given TLGCData class instance to be analyzed
	*/
	TDataAnalyzer(TLGCData& dat);

	/*!
		\brief Returns TRUE if data is consistent, FALSE otherwise. 

		\note This method is also responsible for assigning indices to unknowns and initializing some uninitialized objects (e.g. planes in DLEV measurement).
	*/
	bool dataConsistent();

	/// Returns TRUE if at least one standard deviation assigned to a point or frame element.
	inline bool hasStandardDeviations() {return fStandDevUsed;};

private:

    //! Manage the deactivated points, measurements etc. from the data
    bool cleanDeactivated();

    //! Assign observation and equation indices to measurements.
    void assignEOIndices();

	void checkPDOR(TFileLogger& fileLog, bool dataConsistent);
	void predeterminePLR3DV0();

	bool fStandDevUsed; // If at least one standard deviation assigned to a point or frame element.

	// contains references: not assignable
	TDataAnalyzer& operator=(const TDataAnalyzer&);

	// LGC data set
	TLGCData& fData;
};

#endif