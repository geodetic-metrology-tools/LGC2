/*! 
	Contatins structures holding contributions to all observations types

	Copyright 2015 - , CERN, SU. All rights reserved.
*/
#ifndef TCONTRIBUTION_STRUCTURES_H
#define TCONTRIBUTION_STRUCTURES_H

#include "TFreeVector.h"
#include "AdjustableObjects\TAdjustableHelmertTransformation.h"

//////////////////////////////////////Supporting structures/////////////////////////////////////////////////////////
/// Contributions for TRANSFORMATION
struct TransformationContrib{
	TFreeVector fRotationContrib; // Omega (Rx), Phi (Ry) and Kappa (Rz) contributions
	TFreeVector fTranslContrib; // Tx, Ty, Tz contributions
	TReal		fScaleContrib; // Scale contribution
};

/// Contributions of a point in observation models consisting out of 3 equations (PLR3D, UVD)
struct Point3DContrib{
	TFreeVector firstEqPtContrib; // x,y,z point's coordinate contributions  for a FIRST equation
	TFreeVector secondEqPtContrib; //x,y,z point's coordinate contributions  for a SECOND equation
	TFreeVector thirdEqPtContrib; // x,y,z point's coordinate contributions  for a THIRD equation
};

/// Contribution to a transformation in a observation model of 3 equations (PLR3D, UVD)
struct TransformationContrib3D{
	TransformationContrib	firstEquationTransContrib; 
	TransformationContrib	secondEquationTransContrib;
	TransformationContrib	thirdEquationTransContrib; 
};

/// Contribution to a transformation in observation models consisting of 2 equations (UVEC)
struct TransformationContrib2D{
	TransformationContrib	firstEquationTransContrib; 
	TransformationContrib	secondEquationTransContrib;
};

/// Spatial Distance measurement contribution (DIST)
struct DistMeasContrib{
	TReal		fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib;  //Vector of contributions for station's LOR transformations
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib;  //Vector of contributions for target's LOR transformations

	TReal		fHIContrib;
	TReal		fDistCorrection;

	TReal		fObsVariance;
};

/// Horizontal direction (ANGL) and Zenith Distance (ZEND) measurement contributions
struct AnglMeasContrib{
	LGC::TAngle	fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib;  //Vector of contributions for station's LOR transformations
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib;  //Vector of contributions for target's LOR transformations

	TReal		fHIContrib;
	TReal		fV0Contrib;

	TReal		fObsVariance;
};

/// Horizontal distance (DHOR) measurement contributions made by TSTN 
struct HorDistContrib{
	TReal		fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib;  
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib; 

	TReal		fDistCorrection;
	TReal		fObsVariance;
};

/// Horizontal distance (DHOR) measurement contributions introduced in DLEV measurement.
struct HorDistContribLEVEL{
	TReal		fCalcMeas;
	TFreeVector fStaffContrib; 
	TFreeVector fRefPtContrib;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStaffTransformContrib;  
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fRefPtTransformContrib;
};

/// Levelling (DLEV) measurement contributions
struct DLEVContrib{
	TReal		fCalcMeas;
	TFreeVector	fStaffContrib;
	TFreeVector	fRefPtContrib;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStaffTransformContrib;  
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fRefPtTransformContrib; 

	TReal		fRefPtDistContrib;
	TReal		fCollAngleContrib;   // Value of the contribution of collimination angle

	TReal		fObsVariance;
};


/// Levelling (DVER) measurement contributions
struct DVERContrib{
	TReal			fCalcMeas;
	TFreeVector	    fStCoordContrib;
	TFreeVector	    fTgCoordContrib;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fStTransformContrib;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib;

	TReal		fObsVariance;
};

/// Contribution for PLR3D measurement TSTN
struct PLR3DContrib{
	Point3DContrib fStCoordContrib;
	Point3DContrib fTgCoordContrib;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> fStTransformContrib;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> fTgTransformContrib;		

	TReal       fInstrHeightContrib[3];	
	TReal       fV0Contrib[3];//V0 contribution for a first, second and third equation.
	TReal		fThetaContrib[3]; //Theta contribution for a first, second and third equation.
	TReal		fPhiContrib[3]; //Phi contribution for a first, second and third equation.
	TReal		fDistAndCsContrib[3]; //Distance contribution and contribution of a distance correction (they are equal) for a first, second and third equation

	TReal		fRxContrib[3];
	TReal		fRyContrib[3];

	TReal	    fMisclosureVector[3];

	TReal		fObsVariance[3];
};

/// Contribution for UVD measurement (CAMERA)
struct UVDContrib{
	Point3DContrib fStCoordContrib;
	Point3DContrib fTgCoordContrib;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> fTgTransformContrib;		

	TReal		fXCompContrib[3]; // X vector component contribution for a first, second and third equation.
	TReal		fYCompContrib[3]; // Y vector component contribution for a first, second and third equation.
	TReal		fDistContrib[3]; // Distance contribution for a first, second and third equation

	TReal	    fMisclosureVector[3];
	TReal		fObsVariance[3];
};

/// Contribution for UVEC measurement (CAMERA)
struct UVECContrib{
	TFreeVector fStCoordContribFirstEq;  // x,y,z station's coordinate contributions for a first equation
	TFreeVector fStCoordContribSecondEq; //-//- second

	TFreeVector fTgCoordContribFirstEq; // x,y,z target's coordinate contributions for a first equation
	TFreeVector fTgCoordContribSecondEq;//-//-second

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>> fTgTransformContrib;		

	TReal		fXCompContrib[2]; // X vector component contribution for a first and second equation.
	TReal		fYCompContrib[2]; // Y vector component contribution for a first and second equation.

	TReal	    fMisclosureVector[2]; 
	TReal		fObsVariance[2];  
};


#endif


