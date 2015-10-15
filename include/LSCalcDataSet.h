////////////////////////////////////////////////////////////////////
// LSCalcDataSet.h
/*!
Container for the LGC least squares calculation data set

Patterns:
 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////


#ifndef SU_LS_CALC_DATA_SET
#define SU_LS_CALC_DATA_SET

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#pragma warning(disable:4786)

#include "TWorkingPoints.h"

#include "TLSCalcWorkingAngleObservation.h"
#include "TLSCalcWorkingDistanceObservation.h"
#include "TLSCalcWorkingRelativeErrors.h"
#include "TLSCalcWorkingSpaDistObs.h"
#include "TLSCalcWorkingZenDistObs.h"

#include "TLSCalcWorkingUnknown.h"
#include "TLSCalcWorkingPosVec.h"
#include "TLSCalcWorkingRelativeErrors.h"

#include "TLGCSimResults.h"
#include "TLGCObsSummary.h"

#include "TLSConstraintIdentifier.h"
#include "TSparseMatrix.h"



class LSCalcDataSet {

public:


	/*!@name Constructor / destructor */
	//@{
	/*! Default constructor */
	LSCalcDataSet();
	/*! Destructor */
	~LSCalcDataSet();
	//@}

	//! Copy Assignment Operator 
	LSCalcDataSet& operator=( const LSCalcDataSet& );


	
	/*! returns the number of unknowns, equations and observations of the project as a UEOIndices object */
	UEOIndices		getDimensions() const;
	/*! updates the UEOIndices */
	void			updateIndices();
	/*! Controls the indices consistency */
	bool			indicesConsistency();

	/*! set the constraints for a free network calculation */
	void							setFreeConstraints(const TLSConstraintIdentifier&);
	/*! returns a copy of the free network constraints */
	TLSConstraintIdentifier			getFreeConstraints() const;
	/* returns the flag indicating if the project is for a free network calculation */
	bool							isLibrOptionUsed() const;

	/* gets the flag indicating if the calculation has converged */
	bool	isConvergenceOk() const {return fConvergenceIsOk;}
	/* sets the flag indicating if the calculation has converged */
	void	setConvergenceResult(const bool res) {fConvergenceIsOk = res; return;}
	

	int getIterationsCount() const { return iterationsCount; }
	void setIterationsCount(int i) { iterationsCount = i; }

	
	// ANGL
	//
	/*!@name Specific methods for ls calc hor. ang. observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSHorAngIter	beginLSHorAng();
		/*! Returns a const iterator to the first item in the container */
		LSHorAngConstIter	beginLSHorAng() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSHorAngIter	endLSHorAng();
		/*! Returns a const iterator to the position after the last item in the container */
		LSHorAngConstIter	endLSHorAng() const;
		/*! Adds a new item to the container */
		LSHorAngIter	push_backLSHorAng(const TLSCalcHorAngleObservation& ha);
		/*! Returns the number of the LSCalc working horizontal angle observations */
		int		numHorAngObs() const				{return fLSWorkHorAngObs->size();}
		/*! Get a copy of the Horizontal Angle summary statistics */
		TLGCObsSummary	getHorAngSummary() const;
	//@}

	
	// ZENH / ZENI
	//
	/*!@name Specific methods for ls calc zen. dist. observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSZenDistIter	beginLSZenDist();
		/*! Returns a const iterator to the first item in the container */
		LSZenDistConstIter	beginLSZenDist() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSZenDistIter	endLSZenDist();
		/*! Returns a const iterator to the position after the last item in the container */
		LSZenDistConstIter	endLSZenDist() const;
		/*! Adds a new item to the container */
		LSZenDistIter	push_backLSZenDist(const TLSCalcZenithDistObservation& zd);
		/*!Returns the number of the LSCalc working zenithal distance observations */
		int		numZenDistObs() const				{return fLSWorkZenDistObs->size();}
		/*!Returns the number of the LSCalc working ZENI observations */
		int		numZENIObs() const				{return fLSWorkZenDistObs->numZENI();}
		/*!Returns the number of the LSCalc working ZENH observations */
		int		numZENHObs() const				{return fLSWorkZenDistObs->numZENH();}
		/*! Get a copy of the ZENH summary statistics */
		TLGCObsSummary	getZENHSummary() const;
		/*! Get a copy of the ZENI summary statistics */
		TLGCObsSummary	getZENISummary() const;
	//@}


	// DMES / DTHE
	//
	/*!@name Specific methods for ls calc spatial dist. observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSSpaDistIter	beginLSSpaDist();
		/*! Returns a const iterator to the first item in the container */
		LSSpaDistConstIter	beginLSSpaDist() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSSpaDistIter	endLSSpaDist();
		/*! Returns a const iterator to the position after the last item in the container */
		LSSpaDistConstIter	endLSSpaDist() const;
		/*! Adds a new item to the container */
		LSSpaDistIter	push_backLSSpaDist(const TLSCalcSpatialDistObservation& sd);
		/*!Returns the number of the LSCalc working spatial dist observations */
		int		numSpaDistObs() const				{return fLSWorkSpaDistObs->size();}
		/*!Returns the number of the LSCalc working DMES observations */
		int		numDMESObs() const				{return fLSWorkSpaDistObs->numDMES();}
		/*!Returns the number of the LSCalc working DTHE observations */
		int		numDTHEObs() const				{return fLSWorkSpaDistObs->numDTHE();}
		/*! Get a copy of the DMES summary statistics */
		TLGCObsSummary	getDMESSummary() const;
		/*! Get a copy of the DTHE summary statistics */
		TLGCObsSummary	getDTHESummary() const;
	//@}


	// DHOR
	//
	/*!@name Specific methods for ls calc hor. dist. observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSHorDistIter	beginLSHorDist();
		/*! Returns a const iterator to the first item in the container */
		LSHorDistConstIter	beginLSHorDist() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSHorDistIter	endLSHorDist();
		/*! Returns a const iterator to the position after the last item in the container */
		LSHorDistConstIter	endLSHorDist() const;
		/*! Adds a new item to the container */
		LSHorDistIter	push_backLSHorDist(const TLSCalcHorDistObservation& hd);
		/*!Returns the number of the LSCalc working horizontal dist observations */
		int		numHorDistObs() const				{return fLSWorkHorDistObs->size();}
		/*! Get a copy of the Horizontal DIstance summary statistics */
		TLGCObsSummary	getHorDistSummary() const;
	//@}


	// DVER / DLEV
	//
	/*!@name Specific methods for ls calc vertical dist. observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSVertDistIter	beginLSVertDist(bool isDLEV);
		/*! Returns a const iterator to the first item in the container */
		LSVertDistConstIter	beginLSVertDist(bool isDLEV) const;
		/*! Returns an iterator to the position after the last item in the container */
		LSVertDistIter	endLSVertDist(bool isDLEV);
		/*! Returns a const iterator to the position after the last item in the container */
		LSVertDistConstIter	endLSVertDist(bool isDLEV) const;
		/*! Adds a new item to the container */
		LSVertDistIter	push_backLSVertDist(const TLSCalcVertDistObservation& vd, bool isDLEV);
		/*!Returns the number of the LSCalc working vertical dist observations */
		int		numVertDistObs(bool isDLEV) const {return  isDLEV? fLSWorkDLEVDistObs->size() : fLSWorkVertDistObs->size();}
		/*! Get a copy of the Vertical Distance summary statistics */
		TLGCObsSummary	getVertDistSummary(bool isDLEV) const;
	//@}


	// ECVE
	//
	/*!@name Specific methods for ls calc offset to ver. line observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSOffsetToVerLineIter		beginLSOffsetToVerLine();
		/*! Returns a const iterator to the first item in the container */
		LSOffsetToVerLineConstIter	beginLSOffsetToVerLine() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSOffsetToVerLineIter		endLSOffsetToVerLine();
		/*! Returns a const iterator to the position after the last item in the container */
		LSOffsetToVerLineConstIter	endLSOffsetToVerLine() const;
		/*! Adds a new item to the container */
		LSOffsetToVerLineIter		push_backLSOffsetToVerLine(const TLSCalcOffsetToVerLineObservation& obs);
		/*!Returns the number of the LSCalc working offset to ver. line observations */
		int		numOffsetToVerLineObs() const		{return fLSWorkOffsetToVerLineObs->size();}
		/*! Get a copy of the Offset to a vertical line summary statistics */
		TLGCObsSummary	getOffsetToVerLineSummary() const;
	//@}


	// ECSP
	//
	/*!@name Specific methods for ls calc offset to spa. line observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSOffsetToSpaLineIter		beginLSOffsetToSpaLine();
		/*! Returns a const iterator to the first item in the container */
		LSOffsetToSpaLineConstIter	beginLSOffsetToSpaLine() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSOffsetToSpaLineIter		endLSOffsetToSpaLine();
		/*! Returns a const iterator to the position after the last item in the container */
		LSOffsetToSpaLineConstIter	endLSOffsetToSpaLine() const;
		/*! Adds a new item to the container */
		LSOffsetToSpaLineIter		push_backLSOffsetToSpaLine(const TLSCalcOffsetToSpaLineObservation& obs);
		/*!Returns the number of the LSCalc working offset to spa. line observations */
		int		numOffsetToSpaLineObs() const		{return fLSWorkOffsetToSpaLineObs->size();}
		/*! Get a copy of the Offset to a spatial line summary statistics */
		TLGCObsSummary	getOffsetToSpaLineSummary() const;
	//@}


	// ECHO
	//
	/*!@name Specific methods for ls calc offset to ver. plane observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSOffsetToVerPlaneIter		beginLSOffsetToVerPlane();
		/*! Returns a const iterator to the first item in the container */
		LSOffsetToVerPlaneConstIter	beginLSOffsetToVerPlane() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSOffsetToVerPlaneIter		endLSOffsetToVerPlane();
		/*! Returns a const iterator to the position after the last item in the container */
		LSOffsetToVerPlaneConstIter	endLSOffsetToVerPlane() const;
		/*! Adds a new item to the container */
		LSOffsetToVerPlaneIter		push_backLSOffsetToVerPlane(const TLSCalcOffsetToVerPlaneObservation& obs);
		/*!Returns the number of the LSCalc working offset to ver. plane observations */
		int		numOffsetToVerPlaneObs() const	{return fLSWorkOffsetToVerPlaneObs->size();}
		/*! Get a copy of the Offset to a vertical plane summary statistics */
		TLGCObsSummary	getOffsetToVerPlaneSummary() const;
	//@}


	// ECTH
	//
	/*!@name Specific methods for ls calc offset to theodolite ver. plane observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSOffsetToTheoPlaneIter			beginLSOffsetToTheoPlane();
		/*! Returns a const iterator to the first item in the container */
		LSOffsetToTheoPlaneConstIter	beginLSOffsetToTheoPlane() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSOffsetToTheoPlaneIter			endLSOffsetToTheoPlane();
		/*! Returns a const iterator to the position after the last item in the container */
		LSOffsetToTheoPlaneConstIter	endLSOffsetToTheoPlane() const;
		/*! Adds a new item to the container */
		LSOffsetToTheoPlaneIter			push_backLSOffsetToTheoPlane(const TLSCalcOffsetToTheoPlaneObservation& obs);
		/*!Returns the number of the LSCalc working offset to theo. plane observations */
		int		numOffsetToTheoPlaneObs() const	{return fLSWorkOffsetToTheoPlaneObs->size();}
		/*! Get a copy of the Offset to a vertical Theodolite plane summary statistics */
		TLGCObsSummary	getOffsetToTheoPlaneSummary() const;
	//@}


	// ORIE
	//
	/*!@name Specific methods for ls calc offset to ver. line observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSGyroOrieIter			beginLSGyroOrie();
		/*! Returns a const iterator to the first item in the container */
		LSGyroOrieConstIter		beginLSGyroOrie() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSGyroOrieIter			endLSGyroOrie();
		/*! Returns a const iterator to the position after the last item in the container */
		LSGyroOrieConstIter		endLSGyroOrie() const;
		/*! Adds a new item to the container */
		LSGyroOrieIter			push_backLSGyroOrie(const TLSCalcGyroOrientationObservation& obs);
		/*!Returns the number of the LSCalc gyro. orientation observations */
		int		numGyroOrieObs() const			{return fLSWorkGyroOrieObs->size();}
		/*! Get a copy of the Gyro Orientation summary statistics */
		TLGCObsSummary	getGyroOrieSummary() const;
	//@}


	// RADI
	//
	/*!@name Specific methods for ls calc radial offset constraints observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSRadOffCnstrIter			beginLSRadOffCnstr();
		/*! Returns a const iterator to the first item in the container */
		LSRadOffCnstrConstIter		beginLSRadOffCnstr() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSRadOffCnstrIter			endLSRadOffCnstr();
		/*! Returns a const iterator to the position after the last item in the container */
		LSRadOffCnstrConstIter		endLSRadOffCnstr() const;
		/*! Adds a new item to the container */
		LSRadOffCnstrIter			push_backLSRadOffCnstr(const TLSCalcRadialOffsetCnstrObservation& obs);
		/*!Returns the number of the LSCalc radial offset constraint observations */
		int		numRadOffCnstrObs() const			{return fLSWorkRadOffCnstrObs->size();}
		/*! Get a copy of the Radial Offset Constraint summary statistics */
		TLGCObsSummary	getRadOffCnstrSummary() const;
	//@}


	// PDOR
	//
	/*!@name Specific methods for ls calc orientation constraints observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSOrieCnstrIter			beginLSOrieCnstr();
		/*! Returns a const iterator to the first item in the container */
		LSOrieCnstrConstIter	beginLSOrieCnstr() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSOrieCnstrIter			endLSOrieCnstr();
		/*! Returns a const iterator to the position after the last item in the container */
		LSOrieCnstrConstIter	endLSOrieCnstr() const;
		/*! Adds a new item to the container */
		LSOrieCnstrIter			push_backLSOrieCnstr(const TLSCalcOrientationCnstrObservation& obs);
		/*!Returns the number of the LSCalc orientation constraints */
		int		numOrieCnstrObs() const			{return fLSWorkOrieCnstrObs->size();}
	//@}


	// ORIENTATION PARAMETER
	//
	/*!@ Specific methods for ls calc orientation parameters containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSOrientIter	beginOrient();
		/*! Returns a const iterator to the first item in the container */
		LSOrientConstIter	beginOrient() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSOrientIter	endOrient();
		/*! Returns a const iterator to the position after the last item in the container */
		LSOrientConstIter	endOrient() const;
		/*! Adds a new item to the container 
		\return an iterator to the new item */
		LSOrientIter	push_backOrient(const TLSCalcOrientationParam& orient);
	
		/*! Returns the number of orientation parameters*/
		int		numOrientationParam() const {return fLSWorkOrient->size();}
	//@}


	// POSITION VECTOR PARAMETER
	//
	/*!@ Specific methods for ls calc position vector parameters containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSPosVecIter		beginPV() {return fLSWorkPosVec->begin();}

		/*! Returns a const iterator to the first item in the container */
		LSPosVecConstIter	beginPV() const {return fLSWorkPosVec->begin();}

		/*! Returns an iterator to the position after the last item in the container */
		LSPosVecIter		endPV() {return fLSWorkPosVec->end();}

		/*! Returns a const iterator to the position after the last item in the container */
		LSPosVecConstIter	endPV() const {return fLSWorkPosVec->end();}

		/*! Adds a new item to the container 
		\return an iterator to the new item */
		LSPosVecIter		push_backPosVec(const TLSCalcPosVectorParam& pv) {return fLSWorkPosVec->push_back(pv);}

		/*! Returns the number of position vector parameters*/
		int		numPosVectorParam() const {return fLSWorkPosVec->size();}

		/*!\return an iterator pointing to a position vector specified through its name*/
		LSPosVecIter		getPV(const string name) {return fLSWorkPosVec->getPoint(name);}
		/*!\return a const iterator pointing to a position vector specified through its name*/
		LSPosVecConstIter	getPV(const string name) const {return fLSWorkPosVec->getPoint(name);}
	
		/*!\class the container like the input point'container
		\return error */
		string				orderPVList(const TWorkingPoints* ptList);

		/*! Verification that all the input data points are used in the calculation */
		string				checkAllPointsAreUsed(const TWorkingPoints* ptList);
	//@}


	// LENGTH PARAMETER
	//
	/*!@ Specific methods for ls calc length parameters containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSLengthIter	beginLength();
		/*! Returns a const iterator to the first item in the container */
		LSLengthConstIter	beginLength() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSLengthIter	endLength();
		/*! Returns a const iterator to the position after the last item in the container */
		LSLengthConstIter	endLength() const;
		/*! Adds a new item to the container 
		\return an iterator to the new item */
		LSLengthIter	push_backLength(const TLSCalcLengthParam& length);
		/*! Returns the number of length parameters*/
		int		numLengthParam() const {return fLSWorkLength->size();}
	//@}
	

	// EREL
	//
	/*!@ Specific methods for relative error statistics */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSRelErrorIter	beginRelError();
		/*! Returns a const iterator to the first item in the container */
		LSRelErrorConstIter	beginRelError() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSRelErrorIter	endRelError();
		/*! Returns a const iterator to the position after the last item in the container */
		LSRelErrorConstIter	endRelError() const;
		/*! Adds a new item to the container 
		\return an iterator to the new item */
		LSRelErrorIter	push_backRelError(const TLSCalcRelativeError& erel);
		/*! get any warning related to the relative errors */
		string	getRelErrorWarning() const;
		/*! set a warning related to the relative errors */
		void	setRelErrorWarning(const string& warning);
	//@}


	// CALCULATION STATISTICS
	//
	/*!@ Specific methods for calculation statistics */
	//@{
		/*! Returns the standard deviation a posteriori of ls calc observations */
		TDouble			getS0APosteriori() const { return fS0APosteriori; }
		/*! Sets the standard deviation a posteriori of ls calc observations */
		void			setS0APosteriori(TDouble s0Post) { fS0APosteriori = s0Post; }

		/*! Computes the overall network reliability factor, if needed */
		TDouble		computeOverallNetworkReliability();

		/*! returns the upper chi-test limit for s0 a posteriori */
		TReal		getChiUpLimit() const { return fChiUpLimit; }
		/*! returns the lower chi-test limit for s0 a posteriori */
		TReal		getChiLoLimit() const { return fChiLoLimit; }

		/*! sets the upper chi-test limit for s0 a posteriori */
		void		setChiUpLimit(TReal up) { fChiUpLimit = up; }
		/*! sets the lower chi-test limit for s0 a posteriori */
		void		setChiLoLimit(TReal low) { fChiLoLimit = low; }
	//@}
	

	/*!@ Specific methods for simulation result statistics */
	//@{
		/*! initialise the summaries for simulation calculation results */
		bool	initSimResults();

		/*! update the summaries for simulation calculation results */
		bool	updateSimResults();

		// Reinitialse the values set during and after a calculation
		void	reInitialiseForSim();

		/*! Returns an iterator to the first Simulation Calculation Record in the container */
		CalcRecordIter	beginSimCalcRec();

		/*! Returns a const iterator to the first Simulation Calculation Record in the container */
		CalcRecordConstIter	beginSimCalcRec() const;

		/*! Returns an iterator to the position after the last Simulation Calculation Record 
		in the container */
		CalcRecordIter	endSimCalcRec();

		/*! Returns a const iterator to the position after the last Simulation Calculation Record 
		in the container */
		CalcRecordConstIter	endSimCalcRec() const;

		/*! Returns an iterator to the first Simulation Point Statistic in the container */
		SimPointIterator	beginSimResPoint();

		/*! Returns a const iterator to the first Simulation Point Statistic in the container */
		SimPointConstIter	beginSimResPoint() const;

		/*! Returns an iterator to the position after the last Simulation Point Statistic 
		in the container */
		SimPointIterator	endSimResPoint();

		/*! Returns a const iterator to the position after the last Simulation Point Statistic 
		in the container */
		SimPointConstIter	endSimResPoint() const;
	//@}

		void setUnknownsCovarianceMtrx(TSparseMatrix *matrix) { fUnknownsCovarianceMtrx = *matrix; }
		TSparseMatrix* getUnknownsCovarianceMtrx() { return &fUnknownsCovarianceMtrx; }
		void setForDeformationAnalysis() { fDeformationAnalysis = true; }
		bool isForDeformationAnalysis() { return fDeformationAnalysis; }

protected:

	/*! Returns the total number of observations, PDOR excluded */
	int			totalSize();

private:
	/*!@name Re-initialisation methods */
	//@{
		/*! Re-initialise the LSCalc parameters */
		void	reInitialiseParams();

		/*! Re-initialise the LSCalc observations */
		void	reInitObsForSim();

	//@}

		bool	fConvergenceIsOk;

	/*!@name Settings of indices for parameters and observations */
	//@{
		/*! Updates the parameters unknown index */
		void	setUIndex();
		/*! Updates the observations equation and observation indices */
		void	setEOIndices();
	//@}

	/*!@name pointers to LSCalc observations */
	//@{
		TLSCalcWorkingHorAngObs*			fLSWorkHorAngObs;   /*!< pointer to LSCalc horizontal angle observations list */
		TLSCalcWorkingZenDistObs*			fLSWorkZenDistObs;  /*!< pointer to LSCalc zenithal angle observations list */
		TLSCalcWorkingSpaDistObs*			fLSWorkSpaDistObs;  /*!< pointer to LSCalc spatial dist observations list */
		TLSCalcWorkingHorDistObs*			fLSWorkHorDistObs;  /*!< pointer to LSCalc horizontal dist observations list */
		TLSCalcWorkingVertDistObs*			fLSWorkVertDistObs; /*!< pointer to LSCalc vertical dist observations list */
		TLSCalcWorkingVertDistObs*			fLSWorkDLEVDistObs; /*!< pointer to LSCalc vertical dist observations list */
		TLSCalcWorkingOffsetToVerLineObs*	fLSWorkOffsetToVerLineObs; /*!< pointer to LSCalc offset ECVE observations list */
		TLSCalcWorkingOffsetToSpaLineObs*	fLSWorkOffsetToSpaLineObs; /*!< pointer to LSCalc offset ECSP observations list */
		TLSCalcWorkingOffsetToVerPlaneObs*	fLSWorkOffsetToVerPlaneObs; /*!< pointer to LSCalc offset ECHO observations list */
		TLSCalcWorkingOffsetToTheoPlaneObs*	fLSWorkOffsetToTheoPlaneObs; /*!< pointer to LSCalc offset ECTH observations list */
		TLSCalcWorkingGyroOrientationObs*	fLSWorkGyroOrieObs; /*!< pointer to LSCalc gyro. orientation observations list */
		TLSCalcWorkingRadialOffsetCnstrObs*	fLSWorkRadOffCnstrObs; /*!< pointer to LSCalc RADI observations list */
		TLSCalcWorkingOrientationCnstrObs*	fLSWorkOrieCnstrObs; /*!< pointer to LSCalc PDOR observations list */
	//@}

	/*!@name pointers to LSCalc parameters */
	//@{
		TLSCalcWorkingOrientation*		fLSWorkOrient; /*!< pointer to LSCalc orientation parameters list */
		TLSCalcWorkingPosVec*			fLSWorkPosVec; /*!< pointer to LSCalc position vector parameters list */
		TLSCalcWorkingLength*			fLSWorkLength; /*!< pointer to LSCals length parameters list */
	//@}

//		LSPosVecContainer				fCalaPtNotUsed;

	TLSConstraintIdentifier		fLSFreeCnstr;
	bool						fUsedLibrOption;
	bool						fDeformationAnalysis;

	int							iterationsCount;

	UEOIndices					fUEOIndices; /*!< nbrs of unkowns, equations, observations */
	TDouble						fS0APosteriori; /*!< standard deviation for observations post calculation */
	TReal						fChiUpLimit; /*!< upper chi-test limit for S0 a posteriori */
	TReal						fChiLoLimit; /*!< lower chi-test limit for S0 a posteriori */

	TLSCalcWorkingRelativeErrors*	fLSWorkRelErrors; /*!< pointer to the relative errors list */
	TLGCSimResults*				fSimResults; /*!< pointer to the simulation results */

	TSparseMatrix				fUnknownsCovarianceMtrx; /*!< matrix (u x u) containing the variances and covariances for parameters */
};



#endif // SU_LS_CALC_DATA_SET