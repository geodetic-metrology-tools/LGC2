#ifndef TCONTRIBUTION_GENERATOR_H
#define TCONTRIBUTION_GENERATOR_H

#include "TCCS2CGRFTransformation.h"
#include "TCGRF2LGTransformation.h"
#include "TILG2ILATransformation.h"
#include "ContributionStructures.h"
#include "TRefSystemFactory.h"
#include "TTSTN.h"
#include "TLOR2LOR.h"

struct TLEVEL;
struct TEDM;
struct TECHOROM;
struct TORIEROM;
class TUVEC;
class TUVD;
struct TCAM;

/*!
	\ingroup ContributionsGenerators

	\brief Calculates the contributions for the least squares design matrices for all of the observations processed by LGC2.
	
	In addition, calculation measurement values can be returned separately (used e.g. in the SIMULATION).
*/
class TContributionsGenerator{

public:

	/*!@name Constructors*/
	//@{
		/*!
			\param[in] tree A tree of local object reference frames.
			\param[in] refFrame A reference frame used in the project.
		*/
		TContributionsGenerator(const TDataTree* tree, const TRefSystemFactory::ERefFrame& refFrame);
	//@}

	/*! 
		\brief Updates the transformations between nodes, stored in the 'fLORTrafo' vector. 

		The transformation parametres stored in the tree can change during the adjustment, therefore update of the transformations is REQUIRED.
		This method therefore HAVS TO BE CALLED whenever the TREE of local frames CHANGED, typically in the beginning of Filling Input Matrices procedure.
	*/ 
	void updateTransformations();

	/*!@name Contributions for the total station (TTSTN) observations. 
		
			All the TSTN contributions are calculated either in ROOT node of the TREE of local frames or in the modified local astronomical
			system of the instrument (station).

			In all cases the STATION and TARGET points can be defined anywhere in the TREE of local frames.
	*/
	//@{

		/// Returns contribution for the Spatial Distance measurement (TDIST)
		DistMeasContrib	getSpatialDistanceContrib(const TTSTN& station, const TLINE& dist);

		/// Returns contribution for the Horizontal Angle measurement (TANGL)
		AnglMeasContrib	getHorAnglContrib(const TTSTN& station, const TTSTN::TROM& rom, const TANGL& angl);

		/// Returns contribution for the Zenith Distance (Vertical Angle) measurement (TZEND)
		AnglMeasContrib	getZenDistContrib(const TTSTN& station, const TZEND& zend);

		/// Returns contribution for the Horizontal Distance measurement (DHOR), made by a total station (TSTN).
		HorDistContrib	getHorDistContrib(const TTSTN& station, const TLINE& dhor);

		/// Returns contribution for the TPLR3D measurement.
		PLR3DContrib	getPolar3DContrib(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);

		/// Returns contribution for the offset to a theodolite plane (TECTH) measurement.
		ECTHContrib	 getECTHContrib(const TTSTN& station, const TTSTN::TROM& rom, const TECTH& ecth);

	//@}


	/*!@name Contributions to the CAMERA (TCAM) observations. */
	//@{

		/// Returns the contribution for the TUVEC measurement.
		UVECContrib	getUVECContrib(const TCAM& camera, const TUVEC& uvec);

		/// Returns the contribution for the TUVD measurement.
		UVDContrib	getUVDContrib(const TCAM& camera, const TUVD& uvd);

	//@}

	/*!@name Contributions to the other measurement types (no TSTN nor CAM).*/
	//@{

		///  Returns the contribution for a TDVER measurement
		DVERContrib	getDVERContrib(const TDVER& dver);

		///  Returns the contribution for a levelling measurement (TDLEV) measurement
		DLEVContrib	getDLEVContrib(const TLEVEL& levelST, const TDLEV& dlev);

		///  Returns the contribution for offset to a vertical plane (TECHO)
		ECHOContrib	getECHOContrib(const TECHOROM& echoROM, const TECHO& echo);

		///  Returns the contribution for a free spatial distance measurement (TDSPT) made by EDM instrument
		DistMeasContrib	getDSPTContrib(const TEDM& edmST, const TDSPT& dspt);

		///  Returns the horizontal distance measurement contribution (RDHOR), made as a part of DLEV measurement
		HorDistContribLEVEL	getHorDistContrib(const TAdjustablePoint* referencePoint, const TDLEV::TDHOR& dhor);

		/// Returns contribution for the Gyro-Theodolithe measurement (TORIE)
		AnglMeasContrib	getOrieContrib(const TORIEROM& orieROM, const TORIE& orie);

	//@}


	/*!@name Method for accessing the calculation measurement values only . */
	//@{
		/// Returns the the ANGL measurement calculated meas in RADIANS [rad]
		TReal getANGLCalcMeas(const TTSTN& station, const TTSTN::TROM& rom, const TAdjustablePoint* targetAdjPoint);

		/// Returns the ZEND measurement calculated meas in RADIANS [rad]
		TReal getZENDCalcMeas(const TTSTN& station, const TAdjustablePoint* targetAdjPoint, TReal targetHt);

		/// Returns the DIST measurement calculated meas in meters [m]
		TReal getDISTCalcMeas(const TTSTN& station, const TAdjustablePoint* targetAdjPoint, TReal targetHt, TReal distanceCorr);

		/// Returns the DHOR measurement made by a TSTN, calculated meas in meters [m]
		TReal getDHORCalcMeas(const TTSTN& station, const TLINE& dhor);

		///  Returns the TPLR3D calculated measurement values
		PLR3DCalcMeas getPLR3DCalcMeas(const TTSTN& station, const TTSTN::TROM& rom, const TPLR3D& plr3D);

		/// Returns contribution for the offset to a theodolite plane (TECTH) measurement.
		TReal	 getECTHCalcMeas(const TTSTN& station, const TTSTN::TROM& rom, const TECTH& ecth);

		/// Returns the the TUVEC measurement calculaded measurement value
		TFreeVector getUVECCalcMeas(const TCAM& camera, const TUVEC& uvec);

		/// Returns the TUVD measurement calculaded measurement value
		UVDCalcMeas getUVDCalcMeas(const TCAM& camera, const TUVD& uvec);

		/// Returns the TDLEV measurement calculated meas in meters [m]
		TReal getDLEVCalcMeas(const TLEVEL& levelST, const TDLEV& dlev);

		/// Returns the TDSPT measurement calculated meas in meters [m]
		TReal	getDSPTCalcMeas(const TEDM& edmST, const TDSPT& dspt);

		/// Returns the TECHO measurement calculated meas in meters [m]
		TReal getECHOCalcMeas(const TECHOROM& echoROM, const TECHO& echo);

		/// Returns the TDHOR measurement calculated meas in meters [m]
		TReal	getHorDistCalcMeas(const TAdjustablePoint* referencePoint, const TDLEV::TDHOR& dhor);

		/// Returns the the ORIE measurement calculated meas in RADIANS [rad]
		TReal getORIECalcMeas(const TORIEROM& orieROM, const TORIE& orie);

	//@}
private:
	// Contains references: not assignable
	TContributionsGenerator& operator=(const TContributionsGenerator&);

	//Pointer to a tree of LORs
	const TDataTree* fTree;

	//Reference to the reference frame used (Stored in the TLGCData configuration)
	const TRefSystemFactory::ERefFrame fRefFrame;

	//Geoid model used, determined by the fRefFrame
	TRefSystemFactory::EGeoid fGeoidModel;

	/*!@name Supporting functions used in contribution calculation methods */
	//@{
		// Returns the point contributions of a given transformation between local object references (lorTrafo), with a given coefficients
		//The pattern is (e.g. for a X coordinate): a*pointXDerivative[0] + b*pointXDerivative[1] + c*pointXDerivative[2]
		TFreeVector getPointContributions(const TLOR2LOR& lorTrafo, TReal a, TReal b, TReal c);

		// Adds contribution of a LOR transformations for 1D measurements (ANGL,ZEND,DIST,DHOR) into a 'transfContrib' vector, a,b,c are the coeficcients (see documentation in Mathematical Obsevation Models)
		void addTransformationsContributions(const TLOR2LOR& lorTrafo, const TPositionVector& pointPos, TReal a, TReal b, TReal c, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>>& transfContrib);

		// Adds contribution of a LOR transformations for PLR3D measurements into a 'transfContrib' vector
		void addTransformationsContributions3D(const TLOR2LOR& lorTrafo, const TPositionVector& pointPos, const TFreeVector& line1AMat,  const TFreeVector& line2AMat,  const TFreeVector& line3AMat, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>>& transfContrib);

		// Adds Point contributions for the PLR3D measurement
		void addPointContributionsPLR3D(const TLOR2LOR& lorTrafo, const TFreeVector& line1AMat,  const TFreeVector& line2AMat,  const TFreeVector& line3AMat, Point3DContrib& pointContrib, bool station);
	
		//Adds contribution of a every target's LOR transformation in 'lorTrafo' into a 'transfContrib' vector for UVD measurement
		void addUVDTgTransfContributionsCamera(const TLOR2LOR& lorTrafo, const TPositionVector& pointPos, std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib3D>>& transfContrib);
	//@}


	/*!@name LOR Transformations stuff */
	//@{
		/// Vector containing all transformations between LORs of the tree, which were used
		std::list<TLOR2LOR> fLORTrafo;

		///  Returns the the index of a transformation named 'transfName' in the 'fLORTrafo' vector.  Returns the -1 if transformation was not set up.
		int getTransformationIndex(const std::string& transfName) const;

		/// Returns the an identity transformation
		TLOR2LOR getIdentityTransformation();

		// Returns the a transformation from 'from' node into 'to' node
		// Be carefoul, in a case it is called for second time in a function and the transformation is not yet in the vector, it is reallocated and the reference to the first transformation is lost.
		const TLOR2LOR& getLORTransformation(TDataTreeIterator from, TDataTreeIterator to);

		/// Transform points into an MLA system, where the first point is the origin of the system.
		void transformPointsToMLASystem(std::string originName, TPositionVector& originOfMLAPos, TPositionVector& additPointPos);
	//@}


	/*!@name CCS2MLA related stuff */
	//@{
		//Transformation between reference frames
		TCCS2CGRFTransformation fccs2cgrf;
		TCGRF2LGTransformation fcgrf2ilg;
		TILG2ILATransformation  filg2ila;

		///Sets new origin of the transformation to MLA system
		void set2MLATransformation (TPositionVector originInCCS);
		
		///Transform a free vector fromo a MLA system to CGRF - used only for the dver measurements
		void transformMLA2CGRF(TFreeVector& fv);

		///Transform a position vector into a MLA system
		void transform2MLA(TPositionVector& pv);
		///Transform a free vector into a MLA system
		void transform2MLA(TFreeVector& pv);

		///Transform a position vector from a MLA into a LA system - use for ORIE measurements
		void transformMLA2LA(TPositionVector& pv);

		///Transform a position vector from a LA into a MLA system - use for ORIE measurements
		void transformMLA2LAInverse(TFreeVector& pv);

		/// Name of the last processed point which was used as the origin of the MLA.
		// If a transformation to MLA is needed, we firstly compare the name of the new origin point with this name.
		// If they are the same it means that the MLA transformation which is currently initialized does not need to be re-initialized.
		std::string     fLastStationPtName;

		///Tells whether last processed contribution was  made in the Modified Local Astronomical (MLA) system 
		bool			fMLAused;
		///Tells whether last processed contribution was  made in the CGRF system 
		bool			fCGRFused; /*for DVER*/

		bool fIsSphere;
	//@}
};
#endif 
