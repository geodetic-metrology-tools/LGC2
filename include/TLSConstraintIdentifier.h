//////////////////////////////////////////////////////////////////////
// TLSConstraintIdentifier.h
/*! Class to create the constraint vector for a free network adjustment

  Pattern:

  Copyright 2003-2008 M.Jones CERN TS/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////


#ifndef SU_LS_CONSTRAINT_IDENTIFIER
#define SU_LS_CONSTRAINT_IDENTIFIER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
//using namespace std;
class LSCalcDataSet;
class TLGCDataSet;
#include "TLength.h"


struct freeCnstr {bool dx; bool dy; bool dz; bool rx; bool ry; bool rz; bool k; };
struct freeCnstrRank {int dx; int dy; int dz; int rx; int ry; int rz; int k; };

//! class for input matrices used in the ls-calculation
class TLSConstraintIdentifier{

public:

	/*!@name Constructors / Destructor*/
	//@{
		//!Constructor
		TLSConstraintIdentifier();
		//!Destructor
		~TLSConstraintIdentifier();
	//@}

	/*!initialise translation, rotation and scale constraints using point's status*/
	void	initCnstrIdentifier(const LSCalcDataSet&, TLGCDataSet&);

	//! set the coordinates for the centre of gravity of the point data
	void	setEstimatedGravityCenterCoord(TLength x, TLength y, TLength z);

	/*!@name Access methods*/
	//@{
		//! get the free constraint vector
		struct freeCnstr		getFreeConstraints() const { return fCnstrVector;} 
		//! get the X-coordinate of the centre of gravity of the point data
		TLength					getXGravityCenter() const { return fXcg;}
		//! get the Y-coordinate of the centre of gravity of the point data
		TLength					getYGravityCenter() const { return fYcg;}
		//! get the Z-coordinate of the centre of gravity of the point data
		TLength					getZGravityCenter() const { return fZcg;}
		//! get the X-coordinate of the centre of gravity of the estimated points
		TLength					getXEstimatedGravityCenter() const { return fXcgEst;}
		//! get the Y-coordinate of the centre of gravity of the estimated points
		TLength					getYEstimatedGravityCenter() const { return fYcgEst;}
		//! get the Z-coordinate of the centre of gravity of the estimated points
		TLength					getZEstimatedGravityCenter() const { return fZcgEst;}
		//! get the least squares matrices indices of the free network constraints
		struct freeCnstrRank	getIndiceOfConstraint() const { return fCnstrNumber;}
		//! get the number of free network constraints for the LGC project
		int						getNumberOfConstraint() const;
	//@}


private:

	/*!@name Private Member functions*/
	//@{
		//! initialise the free network constraints identifier	
		void	setCnstrIdentifier(const bool dx, const bool dy, const bool dz, 
									const bool rx, const bool ry, const bool rz, 
									const bool k);

		//! add a free network constraint
		void	addCnstr(const struct freeCnstr);

	//@}


	struct freeCnstr		fCnstrVector; /*!<constraint vector*/
	TLength					fXcg;/*!<x coordinate of the gravity center*/
	TLength					fYcg;/*!<y coordinate of the gravity center*/
	TLength					fZcg;/*!<z coordinate of the gravity center*/
	TLength					fXcgEst;/*!<x estimated coordinate of the gravity center*/
	TLength					fYcgEst;/*!<y estimated coordinate of the gravity center*/
	TLength					fZcgEst;/*!<z estimated coordinate of the gravity center*/
	struct freeCnstrRank	fCnstrNumber;
};


#endif // SU_LS_CONSTRAINT_IDENTIFIER
