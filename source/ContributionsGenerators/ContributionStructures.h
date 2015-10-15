#include "TFreeVector.h"
#include "AdjustableObjects\TAdjustableHelmertTransformation.h"


//For transformation contrib might be better to use array rather that TFreeVector

/// Contributions for transformation parameters
struct TransformationContrib{
	TFreeVector fRotationContrib; // omega, phi and kappa contributions
	TFreeVector fTranslContrib; // t1, t2, t3 contributions
	TReal		fScaleContrib; // scale contribution
};

/// Spatial Distance measurement contribution (DIST)
struct DistMeasContrib{
	TReal		fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;
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
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib;
	TReal		fHIContrib;
	TReal		fV0Contrib;

	TReal		fObsVariance;
};

/// Horizontal distance (DHOR) measurement contributions made by TSTN 
struct HorDistContrib{
	TReal		fCalcMeas;
	TFreeVector fStCoordContrib;
	TFreeVector fTgCoordContrib;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib;
	TReal		fDistCorrection;

	TReal		fObsVariance;
};

/// Horizontal distance (DHOR) measurement contributions made in DLEV measurement, the Reference Point as well as Levelling Staff must be defined in a ROOT node, therefore no transformation's contributions.
struct HorDistContribLEVEL{
	TReal		fCalcMeas;
	TFreeVector fRpCoordContrib; 
	TFreeVector fLevStaffCoordContrib;
};

/// Levelling (DLEV) measurement contributions
struct DLEVContrib{
	TReal		fCalcMeas;
	TFreeVector	fStaffTargetContrib;
	TReal		fRefPtDistContrib;
	TReal		fCollAngleContrib;   // Value of the contribution of collimination angle is in radians (RADs)

	TReal		fObsVariance;
};

/// Contribution of a point in observation models consisting of 3 equations (PLR3D, DIR3D)
struct Point3DContrib{
	TFreeVector firstEqPtContrib; // x,y,z point's coordinate contributions  for a FIRST equation
	TFreeVector secondEqPtContrib; //x,y,z point's coordinate contributions  for a SECOND equation
	TFreeVector thirdEqPtContrib; // x,y,z point's coordinate contributions  for a THIRD equation
};

/// Contribution for rotations in observation models consisting of 3 equations (PLR3D, DIR3D)
struct Rotations3DContrib{
	TFreeVector firstEquationRotContrib; //omega, phi and kappa contributions for a first equation
	TFreeVector secondEquationRotContrib; //omega, phi and kappa contributions for a second equation
	TFreeVector thirdEquationRotContrib; //omega, phi and kappa contributions for a third equation
};

/// Contribution to a transformation in a observation model of 3 equations (PLR3D)
struct TransformationContrib3D{
	TransformationContrib	firstEquationTransContrib; 
	TransformationContrib	secondEquationTransContrib;
	TransformationContrib	thirdEquationTransContrib; 
};

/// Contribution to a transformation in observation models consisting of 2 equations (DIR3D)
struct TransformationContrib2D{
	TransformationContrib	firstEquationTransContrib; 
	TransformationContrib	secondEquationTransContrib;
};
 
/// Contribution for PLR3D measurement (TSTN, CAM)
struct PLR3DContrib{
	Point3DContrib fStCoordContrib;
	Point3DContrib fTgCoordContrib;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> fTgTransformContrib;		

	TReal       fInstrHeightContrib[3];	// For TSTN only, in CAM we have no instrument height
	TReal       fV0Contrib[3];//V0 contribution for a first, second and third equation.
	TReal		fThetaContrib[3]; //Theta contribution for a first, second and third equation.
	TReal		fPhiContrib[3]; //Phi contribution for a first, second and third equation.
	TReal		fDistAndCsContrib[3]; //Distance contribution and contribution of a distance correction (they are equal) for a first, second and third equation

	TReal		fRxContrib[3];
	TReal		fRyContrib[3];

	TReal	    fMisclosureVector[3];

	TReal		fObsVariance[3];
};


/// Contribution of DIR3D measurement (CAM, TSTN)
#if 0
//old struct
struct DIR3DContrib{
	TFreeVector fStCoordContribFirstEq;  // x,y,z station's coordinate contributions  for a first equation
	TFreeVector fStCoordContribSecondEq;

	TFreeVector fTgCoordContribFirstEq; // x,y,z target's coordinate contributions  for a first equation
	TFreeVector fTgCoordContribSecondEq;

	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>> fTgTransformContrib;

	TReal   fInstrHeightContrib[2];
	
	TReal   fV0Contrib[2];//V0 contribution for a first, second and third equation.
	
	TReal	fThetaContrib[2]; //Theta contribution for a first, second equation.
	TReal	fPhiContrib[2]; //Phi contribution for first and second equation

	TReal	fRxContrib[2];
	TReal	fRyContrib[2];

	TReal   fMisclosureVector[2];
	TReal	fObsVariance[2];
};
#endif

#if 1
//new struct
struct DIR3DContrib{
	Point3DContrib fStCoordContrib;
	Point3DContrib fTgCoordContrib;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>> fTgTransformContrib;

	TReal   fInstrHeightContrib[3]; //Instrument height for a first, second and third equation (ONLY FOR TSTN)
	
	TReal   fV0Contrib[3]; //V0 contribution for a first, second and third equation (ONLY FOR TSTN)
	
	TReal	fThetaContrib[3]; //Theta contribution for a first, second equation.
	TReal	fPhiContrib[3]; //Phi contribution for first and second equation

	TReal	fRxContrib[3];
	TReal	fRyContrib[3];

	TReal   fMisclosureVector[3];
	TReal	fObsVariance[3];
};
#endif



