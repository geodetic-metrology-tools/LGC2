#ifndef TLIBR_CNSTR_GENERATOR_H
#define TLIBR_CNSTR_GENERATOR_H


#include "TPointTransformer.h"
#include "TLGCData.h"
#include "TLSInputMatrices.h"


/*!
\ingroup LibrCnstrGenerators

\brief Calculates the LIBR constraints for the least squares design matrices processed by LGC2.

*/


struct isFreeCnstr { bool dx; bool dy; bool dz; bool rx; bool ry; bool rz; bool k; };
struct freeCnstrRank { int dx; int dy; int dz; int rx; int ry; int rz; int k; };


class TLibrCnstrGenerator{

public:
	/// Constructor taking a reference to a TPointTransformer and a TLGCData
	TLibrCnstrGenerator(TPointTransformer& fPointTransfoFunc, const TLGCData& fDataSet);

	//!Calculates the contributions of a offset (LIBR) constraints and puts them in the matrices
	/*!\param ptIter an iterator pointing to the point which has a constraint to be processed*/
	bool	processFreeCnstr(TLSInputMatrices& matrices);

	/*!initialise translation, rotation and scale constraints using point's status*/
	void	initCnstrIdentifier(const TLGCData&);

	//! set the coordinates for the centre of gravity of the point data
	void	setEstimatedGravityCenterCoord(TLength x, TLength y, TLength z);

	//! get the number of free network constraints for the LGC project
	int						getNumberOfConstraint() const;

	isFreeCnstr getCnstIdentifier() const{ return fCnstrVector; }

private:
	/*!@name Private Member functions*/
	//@{
	//! Contains references: not assignable
	TLibrCnstrGenerator& operator=(const TLibrCnstrGenerator&);

	//! initialise the free network constraints identifier	
	void	setCnstrIdentifier(const bool dx, const bool dy, const bool dz,
		const bool rx, const bool ry, const bool rz,
		const bool k);

	//! add a free network constraint
	void	addCnstr(const struct isFreeCnstr);

	//! Calculate the Rx value
	TReal getRxCalcValue(const TAdjustablePoint& ptIt);
	//! Calculate the Ry value	  
	TReal getRyCalcValue(const TAdjustablePoint& ptIt);
	//! Calculate the Rz value	  
	TReal getRzCalcValue(const TAdjustablePoint& ptIt);
	//! Calculate the scale value  
	TReal getScaleCalcValue(const TAdjustablePoint& ptIt);
	//@}


	///Reference to a TPointTransformationFunctions
	TPointTransformer& fPointTransfo;

	///Reference to a TLGCData
	const TLGCData& data;

	struct isFreeCnstr		fCnstrVector; /*!<constraint vector*/
	struct freeCnstrRank	fCnstrNumber;
	TLength					fXcg;/*!<x coordinate of the gravity center*/
	TLength					fYcg;/*!<y coordinate of the gravity center*/
	TLength					fZcg;/*!<z coordinate of the gravity center*/
	TLength					fXcgEst;/*!<x estimated coordinate of the gravity center*/
	TLength					fYcgEst;/*!<y estimated coordinate of the gravity center*/
	TLength					fZcgEst;/*!<z estimated coordinate of the gravity center*/
	
};
#endif