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

#include "TLSCalcWorkingDistanceObservation.h"
#include "TLSCalcWorkingRelativeErrors.h"
#include "TLSCalcOffsetToVerticalLineObservation.h"

#include "lsalgo/TLSCalcWorkingUnknown.h"
#include "lsalgo/TLSCalcWorkingPosVec.h"
#include "TLSCalcWorkingRelativeErrors.h"

#include "TLGCSimResults.h"
#include "TLGCObsSummary.h"

#include "TLSConstraintIdentifier.h"
#include "TSparseMatrix.h"
#include "TheodoliteTarget.h"
#include "EDMTarget.h"

#include "TLSCalcOffsetToSpatialLineConstraint.h"
#include "TLSCalcOffsetToVerticalPlaneConstraint.h"
#include "TLSCalcOffsetToSpatialLineObservation.h"

#include "TLGCDataSet.h"


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
	UEOIndices		getConstraintDimensions() const;
	/*! updates the UEOIndices */
	void			updateIndices();
	/*! Controls the indices consistency */
	bool			indicesConsistency();

	/*! set the constraints for a free network calculation */
	void							setFreeConstraints(const TLSConstraintIdentifier&);
	/*! returns a copy of the free network constraints */
	TLSConstraintIdentifier			getFreeConstraints() const;
	/* returns the flag indicating if the project is for a free network calculation */
	bool							hasConstraints() const;

	/* gets the flag indicating if the calculation has converged */
	bool	isConvergenceOk() const {return fConvergenceIsOk;}
	/* sets the flag indicating if the calculation has converged */
	void	setConvergenceResult(const bool res) {fConvergenceIsOk = res; return;}

	
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
		LSHorAngIter getCalcHorAngByID(int id) const { return fLSWorkHorAngObs->getObservationByID(id); }
	//@}

		// PLR
		/*! Returns an iterator to the first item in the container */
		LSPolarIter		beginLSPolar();
		/*! Returns a const iterator to the first item in the container */
		LSPolarConstIter	beginLSPolar() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSPolarIter		endLSPolar();
		/*! Returns a const iterator to the position after the last item in the container */
		LSPolarConstIter	endLSPolar() const;
		/*! Adds a new item to the container */
		LSPolarIter		push_backLSPolar(const TLSCalcPolarObservation& ha);
		/*! Returns the number of the LSCalc working horizontal angle observations */
		int					numPolarObs() const				{return fLSWorkPolarObs->size();}
		LSPolarIter getCalcPolarByID(int id) const { return fLSWorkPolarObs->getObservationByID(id); }

	
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
		LSZenDistIter getCalcZenDistByID(int id) const { return fLSWorkZenDistObs->getObservationByID(id); }
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
		LSSpaDistIter	push_backLSSpaDist(const TLSCalcSpatialDistObservation<TheodoliteTarget>& sd);
		/*!Returns the number of the LSCalc working spatial dist observations */
		int		numSpaDistObs() const				{return fLSWorkSpaDistObs->size();}
		LSSpaDistIter getCalcSpaDistByID(int id) const { return fLSWorkSpaDistObs->getObservationByID(id); }
	//@}


	// EDM DMES / DTHE
	//
	/*!@name Specific methods for ls calc spatial dist. observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSEDMSpaDistIter	beginLSEDMSpaDist();
		/*! Returns a const iterator to the first item in the container */
		LSEDMSpaDistConstIter	beginLSEDMSpaDist() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSEDMSpaDistIter	endLSEDMSpaDist();
		/*! Returns a const iterator to the position after the last item in the container */
		LSEDMSpaDistConstIter	endLSEDMSpaDist() const;
		/*! Adds a new item to the container */
		LSEDMSpaDistIter	push_backLSEDMSpaDist(const TLSCalcSpatialDistObservation<EDMTarget>& sd);
		/*!Returns the number of the LSCalc working spatial dist observations */
		int		numEDMSpaDistObs() const				{return fLSWorkEDMSpaDistObs->size();}
		LSEDMSpaDistIter getCalcEDMSpaDistByID(int id) const { return fLSWorkEDMSpaDistObs->getObservationByID(id); }
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
		LSHorDistIter	push_backLSHorDist(const TLSCalcHorDistObservation<TheodoliteTarget>& hd);
		/*!Returns the number of the LSCalc working horizontal dist observations */
		int		numHorDistObs() const				{return fLSWorkHorDistObs->size();}
		LSHorDistIter getCalcHorDistByID(int id) const { return fLSWorkHorDistObs->getObservationByID(id); }
	//@}


	// DHOR for DLEV
	//
	/*!@name Specific methods for ls calc hor. dist. observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSLevelHorDistIter	beginLSLevelHorDist() { return fLSWorkLevelHorDistObs->begin(); }
		/*! Returns a const iterator to the first item in the container */
		LSLevelHorDistConstIter	beginLSLevelHorDist() const { return fLSWorkLevelHorDistObs->begin(); }
		/*! Returns an iterator to the position after the last item in the container */
		LSLevelHorDistIter	endLSLevelHorDist() { return fLSWorkLevelHorDistObs->end(); }
		/*! Returns a const iterator to the position after the last item in the container */
		LSLevelHorDistConstIter	endLSLevelHorDist() const { return fLSWorkLevelHorDistObs->end(); }
		/*! Adds a new item to the container */
		LSLevelHorDistIter	push_backLSLevelHorDist(const TLSCalcHorDistObservation<Staff>& hd) { return fLSWorkLevelHorDistObs->push_back(hd); }
		/*!Returns the number of the LSCalc working horizontal dist observations */
		int		numLevelHorDistObs() const				{return fLSWorkLevelHorDistObs->size();}
		LSLevelHorDistIter getCalcLevelHorDistByID(int id) const { return fLSWorkLevelHorDistObs->getObservationByID(id); }
	//@}


	// DVER
	//
	/*!@name Specific methods for ls calc vertical dist. observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSVertDistIter	beginLSVertDist();
		/*! Returns a const iterator to the first item in the container */
		LSVertDistConstIter	beginLSVertDist() const;
		/*! Returns an iterator to the position after the last item in the container */
		LSVertDistIter	endLSVertDist();
		/*! Returns a const iterator to the position after the last item in the container */
		LSVertDistConstIter	endLSVertDist() const;
		/*! Adds a new item to the container */
		LSVertDistIter	push_backLSVertDist(const TLSCalcVertDistObservation& vd);
		/*!Returns the number of the LSCalc working vertical dist observations */
		int		numVertDistObs() const {return fLSWorkVertDistObs->size();}
		LSVertDistIter getVertDistByID(int id) const { return fLSWorkVertDistObs->getObservationByID(id); }
	//@}


	// DLEV
	//
	/*!@name Specific methods for ls calc vertical dist. observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSLevelIter	beginLSLevelObs() { return fLSWorkLevelObs->begin(); }
		/*! Returns a const iterator to the first item in the container */
		LSLevelConstIter	beginLSLevelObs() const { return fLSWorkLevelObs->begin(); }
		/*! Returns an iterator to the position after the last item in the container */
		LSLevelIter	endLSLevelObs() { return fLSWorkLevelObs->end(); }
		/*! Returns a const iterator to the position after the last item in the container */
		LSLevelConstIter	endLSLevelObs() const { return fLSWorkLevelObs->end(); }
		/*! Adds a new item to the container */
		LSLevelIter	push_backLSLevelObs(const TLSCalcLevelObservation& vd) { return fLSWorkLevelObs->push_back(vd); }
		/*!Returns the number of the LSCalc working vertical dist observations */
		int		numLevelObs() const {return fLSWorkLevelObs->size();}
		LSLevelIter getLevelObsByID(int id) const { return fLSWorkLevelObs->getObservationByID(id); }
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
		LSOffsetToVerLineIter		push_backLSOffsetToVerLine(const TLSCalcOffsetToVerticalLineObservation& obs);
		/*!Returns the number of the LSCalc working offset to ver. line observations */
		int		numOffsetToVerLineObs() const		{return fLSWorkOffsetToVerLineObs->size();}
		LSOffsetToVerLineIter getOffVerLineByID(int id) const { return fLSWorkOffsetToVerLineObs->getObservationByID(id); }
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
		LSOffsetToSpaLineIter		push_backLSOffsetToSpaLine(const TLSCalcOffsetToSpatialLineObservation& obs);
		/*!Returns the number of the LSCalc working offset to spa. line observations */
		int		numOffsetToSpaLineObs() const		{return fLSWorkOffsetToSpaLineObs->size();}
		LSOffsetToSpaLineIter getOffSpaLineByID(int id) const { return fLSWorkOffsetToSpaLineObs->getObservationByID(id); }
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
		LSOffsetToVerPlaneIter getOffVerPlaneByID(int id) const { return fLSWorkOffsetToVerPlaneObs->getObservationByID(id); }
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
		LSOffsetToTheoPlaneIter getOffTheoPlaneByID(int id) const { return fLSWorkOffsetToTheoPlaneObs->getObservationByID(id); }
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
		LSGyroOrieIter getGyroOrieByID(int id) const { return fLSWorkGyroOrieObs->getObservationByID(id); }
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


	// Off Spa Line Contraint
	//
	/*!@name Specific methods for ls calc radial offset constraints observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSOffsetToSpaLineCstrIter			beginLSOffSpaLineCnstr() { return fLSWorkOffSpaLineCnstr.begin(); }
		/*! Returns a const iterator to the first item in the container */
		LSOffsetToSpaLineCstrConstIter		beginLSOffSpaLineCnstr() const { return fLSWorkOffSpaLineCnstr.begin(); }
		/*! Returns an iterator to the position after the last item in the container */
		LSOffsetToSpaLineCstrIter			endLSOffSpaLineCnstr() { return fLSWorkOffSpaLineCnstr.end(); }
		/*! Returns a const iterator to the position after the last item in the container */
		LSOffsetToSpaLineCstrConstIter		endLSOffSpaLineCnstr() const { return fLSWorkOffSpaLineCnstr.end(); }
		/*! Adds a new item to the container */
		LSOffsetToSpaLineCstrIter			push_backLSOffSpaLineCnstr(const TLSCalcOffsetToSpatialLineConstraint& obs)
			{ fHasConstraints = true; fLSWorkOffSpaLineCnstr.push_back(obs); return --fLSWorkOffSpaLineCnstr.end(); }
		/*!Returns the number of the LSCalc radial offset constraint observations */
		int		numOffSpaLineCnstr() const			{return fLSWorkOffSpaLineCnstr.size();}
	//@}


	// Off Ver Plane Contraint
	//
	/*!@name Specific methods for ls calc radial offset constraints observations containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSOffsetToVerPlaneCstrIter			beginLSOffVerPlaneCnstr() { return fLSWorkOffVerPlaneCnstr.begin(); }
		/*! Returns a const iterator to the first item in the container */
		LSOffsetToVerPlaneCstrConstIter		beginLSOffVerPlaneCnstr() const { return fLSWorkOffVerPlaneCnstr.begin(); }
		/*! Returns an iterator to the position after the last item in the container */
		LSOffsetToVerPlaneCstrIter			endLSOffVerPlaneCnstr() { return fLSWorkOffVerPlaneCnstr.end(); }
		/*! Returns a const iterator to the position after the last item in the container */
		LSOffsetToVerPlaneCstrConstIter		endLSOffVerPlaneCnstr() const { return fLSWorkOffVerPlaneCnstr.end(); }
		/*! Adds a new item to the container */
		LSOffsetToVerPlaneCstrIter			push_backLSOffVerPlaneCnstr(const TLSCalcOffsetToVerticalPlaneConstraint& obs)
			{ fHasConstraints = true; fLSWorkOffVerPlaneCnstr.push_back(obs); return --fLSWorkOffVerPlaneCnstr.end(); }
		/*!Returns the number of the LSCalc radial offset constraint observations */
		int		numOffVerPlaneCnstr() const			{return fLSWorkOffVerPlaneCnstr.size();}
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
		LSOrientIter	push_back_uniqueOrient(const TLSCalcOrientationParam& orient);
	
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
		LSPosVecIter		push_back_uniquePosVec(const TLSCalcPosVectorParam& pv) {return fLSWorkPosVec->push_back_unique(pv);}

		/*! Returns the number of position vector parameters*/
		int					numPosVectorParam() const {return fLSWorkPosVec->size();}

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


	// FREE VECTOR PARAMETER
	//
	/*!@ Specific methods for ls calc free vector parameters containers */
	//@{
		/*! Returns an iterator to the first item in the container */
		LSFreeVecIter	beginFV() { return fLSWorkFreeVec->begin(); }
		/*! Returns a const iterator to the first item in the container */
		LSFreeVecConstIter	beginFV() const { return fLSWorkFreeVec->begin(); }
		/*! Returns an iterator to the position after the last item in the container */
		LSFreeVecIter	endFV() { return fLSWorkFreeVec->end(); }
		/*! Returns a const iterator to the position after the last item in the container */
		LSFreeVecConstIter	endFV() const { return fLSWorkFreeVec->end(); }
		/*! Adds a new item to the container 
		\return an iterator to the new item */
		LSFreeVecIter	push_backFreeVec(const TLSCalcFreeVectorParam& fv) {return fLSWorkFreeVec->push_back(fv);}
		LSFreeVecIter	push_back_uniqueFreeVec(const TLSCalcFreeVectorParam& fv) {return fLSWorkFreeVec->push_back_unique(fv);}
		/*! Returns the number of length parameters*/
		int		numFreeVecParam() const {return fLSWorkFreeVec->size();}
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
		LSLengthIter	push_back_uniqueLength(const TLSCalcLengthParam& length);
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
		bool	updateSimResults(LSCalcDataSet& calcDS,  TLGCDataSet& data);

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

		void setUnknownsCovarianceMtrx(TSparseMatrix *matrix) { fUnknownsCovarianceMtrx = matrix; }
		TSparseMatrix* getUnknownsCovarianceMtrx() { return fUnknownsCovarianceMtrx; }
		void setForDeformationAnalysis() { fDeformationAnalysis = true; }
		bool isForDeformationAnalysis() { return fDeformationAnalysis; }


		void setIsCombinedCase() { fIsCombinedCase = true; }
		bool isCombinedCase() const { return fIsCombinedCase; }

		bool isLibrOptionUsed() const { return fUsedLibrOption; }

		int getIterationsCount() const { return iterationsCount; }
		void setIterationsCount(int i) { iterationsCount = i; }

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
		TLSCalcWorkingPolarObs*				fLSWorkPolarObs;   /*!< pointer to LSCalc polar 3D observations list */
		TLSCalcWorkingHorAngObs*				fLSWorkHorAngObs;   /*!< pointer to LSCalc polar 3D observations list */
		TLSCalcWorkingZenDistObs*				fLSWorkZenDistObs;  /*!< pointer to LSCalc zenithal angle observations list */
		TLSCalcWorkingSpaDistObs<TheodoliteTarget>*	fLSWorkSpaDistObs;  /*!< pointer to LSCalc spatial dist observations list */
		TLSCalcWorkingSpaDistObs<EDMTarget>*	fLSWorkEDMSpaDistObs;  /*!< pointer to LSCalc EDM spatial dist observations list */
		TLSCalcWorkingHorDistObs<TheodoliteTarget>*	fLSWorkHorDistObs;  /*!< pointer to LSCalc horizontal dist observations list */
		TLSCalcWorkingHorDistObs<Staff>*		fLSWorkLevelHorDistObs;  /*!< pointer to LSCalc horizontal dist observations list */
		TLSCalcWorkingVertDistObs*				fLSWorkVertDistObs; /*!< pointer to LSCalc vertical dist observations list */
		TLSCalcWorkingLevelObs*					fLSWorkLevelObs; /*!< pointer to LSCalc level observations list */
		TLSCalcWorkingOffsetToVerLineObs*		fLSWorkOffsetToVerLineObs; /*!< pointer to LSCalc offset ECVE observations list */
		TLSCalcWorkingOffsetToSpaLineObs*		fLSWorkOffsetToSpaLineObs; /*!< pointer to LSCalc offset ECSP observations list */
		TLSCalcWorkingOffsetToVerPlaneObs*		fLSWorkOffsetToVerPlaneObs; /*!< pointer to LSCalc offset ECHO observations list */
		TLSCalcWorkingOffsetToTheoPlaneObs*		fLSWorkOffsetToTheoPlaneObs; /*!< pointer to LSCalc offset ECTH observations list */
		TLSCalcWorkingGyroOrientationObs*		fLSWorkGyroOrieObs; /*!< pointer to LSCalc gyro. orientation observations list */
		TLSCalcWorkingRadialOffsetCnstrObs*		fLSWorkRadOffCnstrObs; /*!< pointer to LSCalc RADI observations list */
		TLSCalcWorkingOrientationCnstrObs*		fLSWorkOrieCnstrObs; /*!< pointer to LSCalc PDOR observations list */
		
		LSOffsetToSpaLineCstrContainer			fLSWorkOffSpaLineCnstr;
		LSOffsetToVerPlaneCstrContainer			fLSWorkOffVerPlaneCnstr;
	//@}

	/*!@name pointers to LSCalc parameters */
	//@{
		TLSCalcWorkingOrientation*		fLSWorkOrient; /*!< pointer to LSCalc orientation parameters list */
		TLSCalcWorkingPosVec*			fLSWorkPosVec; /*!< pointer to LSCalc position vector parameters list */
		TLSCalcWorkingFreeVec*			fLSWorkFreeVec; /*!< pointer to LSCalc free vector parameters list */
		TLSCalcWorkingLength*			fLSWorkLength; /*!< pointer to LSCals length parameters list */
	//@}

//		LSPosVecContainer				fCalaPtNotUsed;

	TLSConstraintIdentifier		fLSFreeCnstr;
	bool						fHasConstraints;
	bool						fUsedLibrOption;
	bool						fDeformationAnalysis;
	bool						fIsCombinedCase;
	int							iterationsCount;

	UEOIndices					fUEOIndices; /*!< nbrs of unkowns, equations, observations */
	UEOIndices					fUEOConstraintIndices; /*!< nbrs of unkowns, equations, observations for the constraints */
	TDouble						fS0APosteriori; /*!< standard deviation for observations post calculation */
	TReal						fChiUpLimit; /*!< upper chi-test limit for S0 a posteriori */
	TReal						fChiLoLimit; /*!< lower chi-test limit for S0 a posteriori */

	TLSCalcWorkingRelativeErrors*	fLSWorkRelErrors; /*!< pointer to the relative errors list */
	TLGCSimResults*				fSimResults; /*!< pointer to the simulation results */

	TSparseMatrix*				fUnknownsCovarianceMtrx; /*!< matrix (u x u) containing the variances and covariances for parameters */
};



#endif // SU_LS_CALC_DATA_SET
