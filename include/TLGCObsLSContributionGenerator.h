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
#include "TARefFrameTransformation.h"
#include "TLSCalcHorAngleObservation.h"
#include "TLSCalcPolarObservation.h"
#include "TLSCalcZenithDistObservation.h"
#include "TLSCalcSpatialDistObservation.h"
#include "TLSCalcGyroOrientationObservation.h"
#include "TLSCalcHorDistObservation.h"
#include "TLSCalcVertDistObservation.h"
#include "TLSCalcLevelObservation.h"
#include "TLSCalcOffsetToTheoPlaneObservation.h"
#include "TLSCalcOffsetToVerticalLineObservation.h"
#include "TLSCalcOffsetToSpatialLineObservation.h"
#include "TLSCalcOffsetToVerPlaneObservation.h"
#include "TLSCalcRadialOffsetCnstrObservation.h"
#include "TLSCalcOrientationCnstrObservation.h"
#include "TLSCalcOffsetToSpatialLineConstraint.h"
#include "TLSCalcOffsetToVerticalPlaneConstraint.h"
#include "TDataParameters.h"
#include "TRefSystemFactory.h"
#include "TDist.h"

#include <list>
using namespace std;


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
	TFreeVector fTgCoordContrib;
	TFreeVector fOriginCoordContrib;
	TFreeVector fUnitVectorCoordContrib;
	TLength		fDistanceContrib;
	TReal		miscVecContrib;
}  OffsetStnContrib;


typedef struct TheoOffsetStationContrib
{
	TLength		fCalcMeas;
	TFreeVector fCoordContrib;
	TReal		fV0Contrib;
}  TheoOffsetStnContrib;


typedef struct OffsetSpatialLineCstr
{
	TReal firstCstrMiscVec;
	TFreeVector firstCstrPtOnLineContrib;
	TFreeVector firstCstrUnitContrib;
	TReal secondCstrMiscVec;
	TFreeVector secondCstrUnitContrib;
}  OffsetSpatialLineConstraint;


typedef struct OffsetToVerticalLineContrib
{
	TLength		fCalcMeas;
	TFreeVector fCoordContrib;
	TFreeVector ptOnLineContrib;
	TReal		secondDesignMatrixContrib;
	TReal		miscVecContrib;
}  OffsetToVerLineContrib;

typedef struct PContrib
{
	TReal haCalcMeas;
	TReal zdCalcMeas;
	TReal sdCalcMeas;
	TReal haContrib[3];
	TReal zdContrib[3];
	TReal sdContrib[3];
	TReal miscVecElements[3];
	TFreeVector fCoordContrib[3];
	Angles rotAnglesContrib[3];
} PolarContrib;


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

	PolarContrib	getPolarDsgnMxContributions(const LSPolarConstIter obsIt);//, bool rot3D);

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
		!@param obsIt an iterator pointing to the obs to be processed*/// return the calculated observation value and design matrix contributions 
	template <typename T>
	DistStnContrib  getSpaDistDsgnMxContributions(typename list<TLSCalcSpatialDistObservation<T> >::const_iterator obsIt)
	{
		TReal a,b,c;
		TLength calcMeas;
		TFreeVector coordContrib(TCoordSysFactory::k3DCartesian);
		TReal hiContrib;
		TReal constContrib;
		TPositionVector station = obsIt->getStEstCoordinate();
		TPositionVector target = obsIt->getTgEstCoordinate();
		vector<TARefFrameTransformation*>::iterator currentIter;
		string mlaName = obsIt->getStPointName();

		// If a new station is being processed initialise the transformation to the MLA system
		// corresponding to the station position
		if(fMLAName != mlaName)
		{
			setToMLATransformation(obsIt->getStEstCoordinate(), mlaName);
		}

		//TRANSFORM COORDINATES INTO LOCAL INSTRUMENT SYSTEM IF NECESSARY
		currentIter = this->fCCStoMLABegin;
		while (currentIter != this->fCCStoMLAEnd)
		{
			(*currentIter)->transform(station);
			(*currentIter)->transform(target);
			currentIter++;
		}

		//get calculate distance in the local system
		TReal xSt = station.getX().getMetresValue();
		TReal ySt = station.getY().getMetresValue();
		TReal zSt = station.getZ().getMetresValue();

		TReal xTg = target.getX().getMetresValue();
		TReal yTg = target.getY().getMetresValue();
		TReal zTg = target.getZ().getMetresValue();

		TReal hi = obsIt->getEstHInst().getMetresValue();
		TReal hp = obsIt->getHPrism().getMetresValue();

		TReal cst = obsIt->getEstDistConst().getMetresValue();

		//CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
		calcMeas = TLength( dist3D(xSt, ySt, (zSt + hi), xTg, yTg, (zTg + hp)) - cst );
				
		// calculation of the constribution in the local system
		a = -LITERAL(1.0) * (xTg - xSt)/calcMeas.getMetresValue();// xSt coefficient
		b = -LITERAL(1.0) * (yTg - ySt)/calcMeas.getMetresValue();//ySt coefficient
		c = -LITERAL(1.0) * (zTg - zSt + hp - hi)/calcMeas.getMetresValue();//zSt coefficient
		coordContrib = TFreeVector(a,b,c, TCoordSysFactory::k3DCartesian);

		//hiContrib =  c;//hi coefficient
		constContrib =  -1;//c coefficient


		//TRANSFORM CONTRIBUTIONS FROM LOCAL INSTRUMENT SYSTEM TO CCS, IF NECESSARY
		currentIter = this->fMLAtoCCSBegin;
		while (currentIter != this->fMLAtoCCSEnd )
		{
			(*currentIter)->transform(coordContrib);
			currentIter++;
		}
		hiContrib =  coordContrib.getZ().getMetresValue();//hi coefficient

		DistStnContrib  contrib = {calcMeas, coordContrib, hiContrib, constContrib};
		return contrib;
	}


	/*! return the calculated observation value and design matrix contributions 
		for the given Horizontal Distance observation with the current parameters 
		!@param obsIt an iterator pointing to the obs to be processed*/
	template <typename T>
	pair<TLength, TFreeVector>	getHorDistDsgnMxContributions(typename list<TLSCalcHorDistObservation<T> >::const_iterator obsIt)
	{
		TReal a,b;
		TLength calcMeas;
		TFreeVector coordContrib(TCoordSysFactory::k3DCartesian);
		TPositionVector station = obsIt->getStEstCoordinate();
		TPositionVector target = obsIt->getTgEstCoordinate();

		//
		// this observation is curently only valid in a local reference frame (OLOC)
		//

		//PARAMETERS IN LOCAL INSTRUMENT SYTEM
		TReal xSt = station.getX().getMetresValue();
		TReal ySt =  station.getY().getMetresValue();
		TReal xTg = target.getX().getMetresValue();
		TReal yTg = target.getY().getMetresValue();
		TReal cte = obsIt->getEstDistConst().getMetresValue();

		//CALCULATE VALUE IN LOCAL INSTRUMENT SYSTEM
		calcMeas = TLength( dist(xSt, ySt, xTg, yTg) - cte );
		
		//CALCULATION OF THE CONTRIBUTION IN INSTRUMENT LOCAL SYSTEM
		a = -1 * (xTg - xSt)/calcMeas.getMetresValue();// xSt coefficient
		b = -1 * (yTg - ySt)/calcMeas.getMetresValue();//ySt coefficient
		coordContrib = TFreeVector(a, b, LITERAL(0.0), TCoordSysFactory::k3DCartesian);

		return make_pair(calcMeas, coordContrib);
	}

	/*! return the calculated observation value and design matrix contributions 
		for the given Vertical Distance observation with the current parameters 
		!@param obsIt an iterator pointing to the hor. angle obs to be processed*/
	LevelStnContrib	getVertDistDsgnMxContributions(const LSVertDistConstIter obsIt);

	/*! return the calculated observation value and design matrix contributions 
		for the given Level Vertical Distance observation with the current parameters 
		!@param obsIt an iterator pointing to the hor. angle obs to be processed*/
	LevelStnContrib	getLevelObsDsgnMxContributions(const LSLevelConstIter obsIt);


	/*! return the calculated observation value and design matrix contributions 
		for the given Offset to Vertical Line observation with the current parameters 
		!@param obsIt an iterator pointing to the hor. angle obs to be processed*/
	OffsetToVerLineContrib	getOffsetToVerLineDsgnMxContributions(const LSOffsetToVerLineConstIter obsIt);

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
	
	OffsetSpatialLineConstraint getOffSpaLineCnstrDsgnMxContributions(const LSOffsetToSpaLineCstrConstIter obsIt);

	pair<pair<TReal, TFreeVector>, pair<TReal, TFreeVector> >  getOffVerPlaneCnstrDsgnMxContributions(const LSOffsetToVerPlaneCstrConstIter obsIt);

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

	// transformation from the CCS to the CGRF
	vector<TARefFrameTransformation*>			fTransfoCCStoCGRF;
	vector<TARefFrameTransformation*>::iterator	fCCStoCGRFBegin;
	vector<TARefFrameTransformation*>::iterator	fCCStoCGRFEnd;

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
	TRefSystemFactory::ERefFrame				fGeoRefFrame;
	TReferenceEllipsoid*						fEllipsoid;
	TRefSystemFactory::EGeoid					fGeoid;
	TModifiedLocalAstronomicalRF*				fLocalRF;


	//TModifiedLocalAstronomicalRF*				fLocalRFsaveForGyro;
	//vector<TARefFrameTransformation*>			fTransfoCGRFtoCCS;

	//TRefSystemFactory::ERefFrame				fRefSystem;

	//bool										fIsPdorBearingIsDefined;

};
#endif //SU_LGC_OBS_LS_CG

