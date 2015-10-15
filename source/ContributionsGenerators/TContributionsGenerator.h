/*! 
	Calculates the contributions for the least squares design matrices for all
	of the observations processed by LGC 2.

	Copyright 2015, CERN, SU. All rights reserved.
*/
#ifndef TCONTRIBUTION_GENERATOR_H
#define TCONTRIBUTION_GENERATOR_H

#include <RefFrameTransformations/TCGRF2ILGTransformation.h>
#include <RefFrameTransformations/TILG2ILATransformation.h>
#include "Measurements\TTSTN.h"
#include "Measurements\TCAM.h"
#include "TLOR2LOR.h"
#include "ContributionStructures.h"

struct TLEVEL;

class TContributionsGenerator{

public:

	/*!@name Constructors */
	//@{
		/*!
			\param[in] tree A tree of local object reference frames.
			\param[in] refFrame A reference frame used in the project.
		*/
		TContributionsGenerator(TDataTree* tree, TLGCRefFrame::ERefs refFrame);
	//@}

	/*! 
		Updates the transformations between nodes (i.e. in a 'fLORTrafo' vector). It is needed, because the transformation parametres between LORs can change during adjustment.
		This method therefore should be called whenever the tree of LORs changed, typically in the beginning of Filling Input Matrices procedure.
	*/ 
	void updateTransformations();

	/*!@name Contributions to the observations. */
	//@{

	///Returns contribution of a Spatial Distance measurement (DIST)
	DistMeasContrib	getSpatialDistanceContrib(const TTSTN& station, const TLINE& dist);

	///Returns contribution of a Horizontal Angle measurement (ANGL)
	AnglMeasContrib	getHorAnglContrib(const TTSTN& station, const TTSTN::TROM& rom,const TANGL& angl);

	///Returns contribution of a Zenith Distance (Vertical Angle) measurement (ZEND)
	AnglMeasContrib	getZenDistContrib(const TTSTN& station, const TZEND& zend);

	///Returns contribution of a Horizontal Distance measurement (DHOR), made by a total station (TSTN)
	HorDistContrib	getHorDistContrib(const TTSTN& station, const TLINE& dhor);

	///Returns contribution of a PLR3D measurement, made by total station (TSTN)
	PLR3DContrib	getPolar3DContrib(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);

	///Returns contribution for a CAM's PLR3D measurement (PLR3D)
	PLR3DContrib	getPolar3DContrib(const TCAM& camera, const TPLR3D& plr3D);

	///Returns contribution of a DIR3D measurement, made by total station (TSTN)
	DIR3DContrib	getDIR3DContrib(const TTSTN& station, const TTSTN::TROM& rom, const TDIR3D& dir3D);

	///Returns contribution for a CAM's DIR3D measurement
	DIR3DContrib	getDIR3DContrib(const TCAM& camera, const TDIR3D& plr3D);

	///Returns contribution for a levelling measurement (DLEV) measurement
	DLEVContrib	getDLEVContrib(const TLEVEL& a, const TDLEV& dlev);

	///Returns Horizontal Distance measurement contribution (DHOR), made as a part of DLEV measurement
	HorDistContribLEVEL	getHorDistContrib(const TAdjustablePoint* referencePoint, const TDHOR& dhor);

	//@}

private:
	// Contains references: not assignable
	TContributionsGenerator& operator=(const TContributionsGenerator&);

	//Reference to a tree of LORs
	TDataTree* fTree;

	//Reference frame used (OLOC, SPHE, LEP, RS2K)
	TLGCRefFrame::ERefs fRefFrame;

	//Geoid model used, determined by the 'fRefFrame' in the constructor
	TRefSystemFactory::EGeoid fGeoidModel;

	//Name of the last processed stationed point
	std::string     fLastStationPtName;

	//Tells whether last processed contribution was  made in the Local Astronomical (LA) system or not 
	bool			fMLAused;

	//Returns point contributions of a given transformation between local object references (lorTrafo), with a given coefficients
	//The pattern is (e.g. for a X coordinate): a*pointXDerivative[0] + b*pointXDerivative[1] + c*pointXDerivative[2]
	TFreeVector getPointContributions(const TLOR2LOR& lorTrafo, TReal a, TReal b, TReal c);

	//Add contribution of a LOR transformation of every transformation in 'lorTrafo' into a 'transfContrib' vector, a,b,c are the coeficcients (see documentation in Mathematical Obsevation Models)
    void addTransformationsContributions(const TLOR2LOR& lorTrafo, const TPositionVector& pointPos, TReal a, TReal b, TReal c, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>>& transfContrib);
	
	//Add contribution of a every target's LOR transformation in 'lorTrafo' into a 'transfContrib' vector 
	void addPLR3DTgTransfContributionsCamera(const TLOR2LOR& lorTrafo, const TPositionVector& pointPos, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>>& transfContrib);
    //void addPLR3DTgTransfContributionsCamera(const TLOR2LOR& lorTrafo, const TPositionVector& stationPos, const TPositionVector& pointPos, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>>& transfContrib);


	//Add contribution of a every target's LOR transformation in 'lorTrafo' into a 'transfContrib' vector 
	void addDIR3DTgTransfContributions(const TLOR2LOR& lorTrafo, const TPositionVector& pointPos, TReal ANGLrad, TReal ZENDrad, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>>& transfContrib);
//NEW IMPELMENTATION OF EQUATIONS NOT USED
//	void addDIR3DTgROT3DTransfContributions(const TLOR2LOR& lorTrafo, const TPositionVector& targetPos, TReal thetaV0Rad, TReal phiRad, TReal rXRad, TReal rYRad, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib2D>>& transfContrib);

	///Returns a vector for partial derivatives with respect to target parameters to be multiplied by -A matrix for DIR3D
	TFreeVector getVectorForDIR3DTargetPartDer(TReal dx, TReal dy, TReal dz, TReal s, TFreeVector partDerwr2Unkn);
	///Returns a vector for partial derivatives with respect to station parameters to be multiplied by A matrix for DIR3D
	TFreeVector getVectorForDIR3DStationPartDer(TReal dX, TReal dY, TReal dZ, TReal s, TFreeVector partDerwr2Unkn);
////////////////////////////////////////////////
/////LOR Transformations stuff
////////////////////////////////////////////////
	// Vector containing all transformations between LORs of the tree, which were used
	std::vector<TLOR2LOR> fLORTrafo;

	/// Returns the index of a transformation named 'transfName' in the 'fLORTrafo' vector. Returns -1 if transformation was not set up.
	int getTransformationIndex(const std::string& transfName) const;

	//Returns an identity transformation
	TLOR2LOR getIdentityTransformation();

	//Returns a transformation from 'from' node into 'to' node
	// Be carefoul, in a case it is called for second time in a function and the transformation is not yet in the vector, it is reallocated and the reference to the first transformation is lost.
	const TLOR2LOR& getLORTransformation(TDataTreeIterator from, TDataTreeIterator to);
////////////////////////////////////////////////////////////////////////////////
//CCS2MLA related stuff
//////////////This part can be potentially moved into separate something like CCS2MLA class////

	//Transformation between reference frames
	TCCS2CGRFTransformation fccs2cgrf;
	TCGRF2ILGTransformation fcgrf2ilg;
	TILG2ILATransformation  filg2ila;

	//Sets new origin of the transformation to MLA system
	void set2MLATransformation (TPositionVector originInCCS);
	//Transform a position vector into a MLA system
	void transform2MLA(TPositionVector& pv);
	//Transform a free vector into a MLA system
	void transform2MLA(TFreeVector& pv);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
#endif 
