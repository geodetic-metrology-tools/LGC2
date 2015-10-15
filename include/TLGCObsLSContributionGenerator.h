//TLGCObsLSContributionGenerator.h : header file
//
/*!Class for a LGC Observation LS contrib generator 
	Calculates the contributions for the least squares design matrices for all
	of the observations processed by LGC

Patterns:

 
Copyright 2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////



#ifndef SU_LGC_OBS_LS_CG
#define SU_LGC_OBS_LS_CG


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
class TGC2MLATransformation;
class TMLA2GCTransformation;
class TReferenceEllipsoid;
#include "TCoordSysFactory.h"
#include "TPositionVector.h"
#include "TLSCalcHorAngleObservation.h"
#include "TLSCalcZenithDistObservation.h"
#include "TLSCalcSpatialDistObservation.h"
#include "TLSCalcGyroOrientationObservation.h"
#include "TLSCalcHorDistObservation.h"
#include "TLSCalcVertDistObservation.h"
#include "TLSCalcOffsetToSpaLineObservation.h"
#include "TLSCalcOffsetToTheoPlaneObservation.h"
#include "TLSCalcOffsetToVerLineObservation.h"
#include "TLSCalcOffsetToVerPlaneObservation.h"
#include "TLSCalcRadialOffsetCnstrObservation.h"
#include "TLSCalcOrientationCnstrObservation.h"
#include	"TDataParameters.h"
#include <map>
using namespace std;


static const double kSmallValue = 1e-8;

typedef struct TheoStationContrib
{
	TAngle		fCalcMeas;
	TFreeVector fCoordContrib;
	TReal		fHIContrib;
	TReal		fV0Contrib;
}  TheoStnContrib;


typedef struct DistStationContrib
{
	TLength		fCalcMeas;
	TFreeVector fCoordContrib;
	TReal		fHIContrib;
	TReal		fConstContrib;
}  DistStnContrib;


typedef struct LevelStationContrib
{
	TLength		fCalcMeas;
	TFreeVector fRefCoordContrib;
	TFreeVector fTgtCoordContrib;
}  LevelStnContrib;


typedef struct OffsetStationContrib
{
	TLength		fCalcMeas;
	TFreeVector fStnCoordContrib;
	TFreeVector fTgt1CoordContrib;
	TFreeVector fTgt2CoordContrib;
}  OffsetStnContrib;


typedef struct TheoOffsetStationContrib
{
	TLength		fCalcMeas;
	TFreeVector fCoordContrib;
	TReal		fV0Contrib;
}  TheoOffsetStnContrib;


////////////////////
// Class declaration
////////////////////
//!Class for a LS contrib generator processing horizontal angle measurements defined in a local cartesian coordinate system
class TLGCObsLSContributionGenerator 
{

public :

	/*!@name Constructors / Destructor*/
	//@{
		//!Default constructor
		explicit TLGCObsLSContributionGenerator(TRefSystemFactory::ERefFrame refSur);

		//!Destructor
		virtual ~TLGCObsLSContributionGenerator();
	//@}


	/*! return the calculated observation value and design matrix contributions 
		for the given Horizontal Angle observation with the current parameters 
		!@param obsIt an iterator pointing to the obs to be processed*/
	TheoStnContrib  getHorAngDsgnMxContributions(const LSHorAngConstIter obsIt);

	/*! return the calculated observation value and design matrix contributions 
		for the given Zenith Distance observation with the current parameters 
		!@param obsIt an iterator pointing to the obs to be processed*/
	TheoStnContrib  getZenDistDsgnMxContributions(const LSZenDistConstIter obsIt);

	/*! return the calculated observation value and design matrix contributions 
		for the given Spatial Distrance observation with the current parameters 
		!@param obsIt an iterator pointing to the obs to be processed*/
	DistStnContrib  getSpaDistDsgnMxContributions(const LSSpaDistConstIter obsIt);

	/*! return the calculated observation value and design matrix contributions 
		for the given Horizontal Distance observation with the current parameters 
		!@param obsIt an iterator pointing to the obs to be processed*/
	pair<TLength, TFreeVector>	getHorDistDsgnMxContributions(const LSHorDistConstIter obsIt);
	
	/*! return the calculated observation value and design matrix contributions 
		for the given Vertical Distance observation with the current parameters 
		!@param obsIt an iterator pointing to the hor. angle obs to be processed*/
	LevelStnContrib	getVertDistDsgnMxContributions(const LSVertDistConstIter obsIt);

	/*! return the calculated observation value and design matrix contributions 
		for the given Vertical Distance observation with the current parameters 
		!@param obsIt an iterator pointing to the hor. angle obs to be processed*/
	LevelStnContrib	getDLEVDsgnMxContributions(const LSVertDistConstIter obsIt);

	/*! return the calculated observation value and design matrix contributions 
		for the given Offset to Vertical Line observation with the current parameters 
		!@param obsIt an iterator pointing to the hor. angle obs to be processed*/
	pair<TLength, TFreeVector>	getOffsetToVerLineDsgnMxContributions(const LSOffsetToVerLineConstIter obsIt);

	/*! return the calculated observation value and design matrix contributions 
		for the given Offset to Spatial Line observation with the current parameters 
		!@param obsIt an iterator pointing to the hor. angle obs to be processed*/
	OffsetStnContrib  getOffsetToSpaLineDsgnMxContributions(const LSOffsetToSpaLineConstIter obsIt);

	/*! return the calculated observation value and design matrix contributions 
		for the given Offset to Vertical Plane observation with the current parameters 
		!@param obsIt an iterator pointing to the hor. angle obs to be processed*/
	OffsetStnContrib  getOffsetToVerPlaneDsgnMxContributions(const LSOffsetToVerPlaneConstIter obsIt);

	/*! return the calculated observation value and design matrix contributions 
		for the given Offset to Theodolite Plane observation with the current parameters 
		!@param obsIt an iterator pointing to the hor. angle obs to be processed*/
	TheoOffsetStnContrib  getOffsetToTheoPlaneDsgnMxContributions(const LSOffsetToTheoPlaneConstIter obsIt);

	/*! return the calculated observation value and design matrix contributions 
		for the given Gyro Orientation observation with the current parameters 
		!@param obsIt an iterator pointing to the hor. angle obs to be processed*/
	pair<TAngle, TFreeVector>  getGyroOrieDsgnMxContributions(const LSGyroOrieConstIter obsIt);

	/*! return the calculated observation value and design matrix contributions 
		for the given Radial Offset Constraint observation with the current parameters 
		!@param obsIt an iterator pointing to the hor. angle obs to be processed*/
	pair<TLength, TFreeVector>  getRadOffCnstrDsgnMxContributions(const LSRadOffCnstrConstIter obsIt);

	/*! return the calculated constraint bearing value and design matrix contributions 
		for the given Orientation Constraint with the current parameters 
		!@param obsIt an iterator pointing to the hor. angle obs to be processed*/
	pair<TAngle, TFreeVector>  getOrientationCnstrDsgnMxContributions(const LSOrieCnstrConstIter obsIt);

	/*FreeCnstr*/
private:


	/*!@name Related to the reference frame transformations of station positions */
	//@{
		/*! set the local reference system for a calculation in a geodetic system
			to be the MLA system located at the origin provided */
		void		setToMLATransformation(TPositionVector mlaOrigin, string mlaName);

		/*! set the local reference system for a calculation in a geodetic system
			to be the LA system located at the origin provided */
		void		setToLATransformation(TPositionVector laOrigin, string laName);
	//@}

	// calculate a height difference between SPatial positions that are in an 2D+H system
	inline TReal calcDh(const TSpatialPosition& ref, const TSpatialPosition& tgt) {
		return tgt.getCoordinates(TCoordSysFactory::k2DPlusH).getH().getMetresValue()- 
			   ref.getCoordinates(TCoordSysFactory::k2DPlusH).getH().getMetresValue();
	} 

	// transformation from the CCS to the CGRF
	vector<TARefFrameTransformation*>			fTransfoCCStoCGRF;
	vector<TARefFrameTransformation*>::iterator	fCCStoCGRFBegin;
	vector<TARefFrameTransformation*>::iterator	fCCStoCGRFEnd;

	// transformation from the CGRF to the CCS
	vector<TARefFrameTransformation*>			fTransfoCGRFtoCCS;
	vector<TARefFrameTransformation*>::iterator	fCGRFtoCCSBegin;
	vector<TARefFrameTransformation*>::iterator	fCGRFtoCCSEnd;

	// transformations between the CGRF and the local astronomical reference frame
	TGC2MLATransformation*						fTransfoCGRFtoMLA;
	TMLA2GCTransformation*						fTransfoMLAtoCGRF;

	// transformation from the CCS to the local astronomical reference frame
	vector<TARefFrameTransformation*>			fTransfoCCStoMLA;
	vector<TARefFrameTransformation*>::iterator	fCCStoMLABegin;
	vector<TARefFrameTransformation*>::iterator	fCCStoMLAEnd;

	// transformation from the local astronomical reference frame to the CCS
	vector<TARefFrameTransformation*>			fTransfoMLAtoCCS;
	vector<TARefFrameTransformation*>::iterator	fMLAtoCCSBegin;
	vector<TARefFrameTransformation*>::iterator	fMLAtoCCSEnd;

	// transformation from the CCS to the XYH projection system
	vector<TARefFrameTransformation*>			fTransfoCCStoXYH;
	vector<TARefFrameTransformation*>::iterator	fCCStoXYHBegin;
	vector<TARefFrameTransformation*>::iterator	fCCStoXYHEnd;

	string										fLAName;
	string										fMLAName;
	TRefSystemFactory::ERefFrame				fRefSystem;
	TRefSystemFactory::ERefFrame				fGeoRefFrame;
	TReferenceEllipsoid*						fEllipsoid;
	TRefSystemFactory::EGeoid					fGeoid;
	TModifiedLocalAstronomicalRF*				fLocalRF;


	//TModifiedLocalAstronomicalRF*				fLocalRFsaveForGyro;
	//vector<TARefFrameTransformation*>			fTransfoCGRFtoCCS;

	//TDataParameters::ERefFrame				fRefSystem;

	//bool										fIsPdorBearingIsDefined;

};
#endif //SU_LGC_OBS_LS_CG

