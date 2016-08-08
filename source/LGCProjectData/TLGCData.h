#ifndef _LGC_DATA_H_
#define _LGC_DATA_H_

#include <TMeasurements.h>
#include "TLGCConfig.h"
#include "LGCAdjustableObjectCollection.h"
#include <Global.h>
#include "TFileLogger.h"
#include "UEOIndices.h"
#include "TLGCStatistic.h"
#include "TSpatialStatus.h"
#include "TTreeEntry.h"
#include "TLSCalcRelativeError.h"

/*! Counter of points based on the type */
struct TPointGlobal{
	int	fNumCala;
	int	fNumVx;
	int	fNumVy;
	int	fNumVz;
	int	fNumVxy;
	int	fNumVxz;
	int	fNumVyz;
	int	fNumVxyz;
};

/*! Counter of measurements based on the type */
struct TMeasurementsGlobal{
	enum EMeasurementType{kANGL, kZEND, kDIST, kPLR3D, kDLEV, kDHOR, kECTH, kECDIR, kDSPT, kDVER, kUVEC, kUVD, kECHO, kECSP, kECVE, kORIE, kPDOR, kRADI};

	int	fNumANGL;
	int	fNumZEND;
	int	fNumDIST;
	int	fNumPLR3D;
	int	fNumDLEV;
	int fNumDHOR;
	int fNumECTH;
	int fNumECDIR;
	int fNumDSPT;
	int fNumDVER;
	int	fNumUVEC;
	int	fNumUVD;
	int	fNumECHO;
	int	fNumECSP;
	int	fNumECVE;
	int	fNumORIE;
	int	fNumPDOR;
	int	fNumRADI;
};


/**
	\ingroup LGCProjectData

	\brief This composite can be seen as PROJECT DATA, containing everything that is needed.

	This is a compound for the classes that are pushed around during the execution of the
	program. It makes the handling of points, measurements, insruments and frames easier
	by grouping together everything that is needed so the interaction with the LGC modules 
	is reduced to ineracting with a single object.
*/ 
class TLGCData{
public:
	/*!
		The default constructor provides a tree which consists only of the root node 
		and it is in a valid state, i.e. the current node pointer. 	
	*/
	TLGCData();


	/*!@name Methods related to the TREE OF FRAMES.*/
	//@{
		/// Access the active level of the tree.
		inline TTreeEntry& getCurrentNode() { return *(pos->get());}

		/// Sets the position iterator to the parent of the current node.
		inline void moveUp() {pos = TDataTree::parent(pos);}

		/// Returns current iterator position in the tree.
		inline TDataTreeIterator getCurrentPosition(){return pos;}

		/// Adds a new level to the tree passing the local transformation that led to this level.
		TTreeEntry& addChild(TAdjustableHelmertTransformation* trafo);

		/// Returns the LGCData structure of the input files containing measurments
		TDataTree& getTree() {return tree;}
		TDataTree const& getTree() const {return tree;}

		/// Returns The number of frames used (if no frame used the result is 1 for a ROOT frame)
		size_t getNumberOfFrames() const{return tree.size();};

		///	Re-initialize for simulation, not the statistics, residuals because these are going to be rewritten. Only the adjustable objects.
		void reInitForSIMU();
	//@}



	/*!@name Accessing the data stored, adjustable collections, configuration, instruments, logger writer.*/
	//@{
		/// Returns the vector/ collection of all the adjustable points
		TAdjustablePointCollection& getPoints() {return points;}
		/// Returns a contant reference of the vector/ collection of all the adjustable points
		TAdjustablePointCollection const& getPoints() const {return points;}
		/// Returns the vector/ collection of all the adjustable lines
		LGCAdjustableLineCollection& getLines() {return lines;}
		/// Returns a contant reference of the vector/ collection of all the adjustable lines
		LGCAdjustableLineCollection const& getLines() const {return lines;}
		/// Returns the vector/ collection of all the adjustable planes
		LGCAdjustablePlaneCollection& getPlanes() {return planes;}
		/// Returns a contant reference of the vector/ collection of all the adjustable planes
		LGCAdjustablePlaneCollection const& getPlanes() const {return planes;}
		/// Returns the vector/ collection of all the adjustable angles
		TAdjustableAngleCollection& getAngles() {return angles;}
		/// Returns a contant reference of the vector/ collection of all the adjustable angles
		TAdjustableAngleCollection const& getAngles() const {return angles;}
		/// Returns the vector/ collection of all the adjustable lengths
		TAdjustableLengthCollection& getLength() {return lengths;}
		/// Returns a contant reference of the vector/ collection of all the adjustable lengths
		TAdjustableLengthCollection const& getLength() const {return lengths;}

	
		/// Returns the instruments defined
		TInstrumentData& getInstruments() {return instruments;}
		/// Returns constant reference on  instruments defined
		TInstrumentData const& getInstruments() const {return instruments;}


		/// Returns the project confifuration
		TLGCConfig& getConfig() {return config;}
		/// Returns a constant reference on  project confifuration
		TLGCConfig const& getConfig() const {return config;}

		/// Returns the reference on statistics of the project
		TLGCStatistic& getStatistics() {return stat;}
		/// Returns the constant reference on statistics of the project
		TLGCStatistic const& getStatistics() const {return stat;}

		/// Returns the file logger used to write on an output file
		LSRelErrorsContainer& getRelError() { return fRelError; }
		/// Returns the constant referrence on file logger used to write on an output file
		LSRelErrorsContainer const& getRelError() const { return fRelError; }

		/// Returns the file logger used to write on an output file
		TFileLogger& getFileLogger() {return fileLogger;}
		/// Returns the constant referrence on file logger used to write on an output file
		TFileLogger const& getFileLogger() const {return fileLogger;}
	//@}


	/*!@name Least squares/solution calculation related stuff.*/
	//@{

		/// Number of unkowns, equations and observations.
		UEOIndices	fUEOIndices; 


		/// Set the use of combined case to true
		void setCombinedCaseCalcUsed() {fCombinedCase = true;}
		/// Return if we used combined case
		bool isCombinedCaseUsed() {return fCombinedCase;}

		/// Sets the number of LS iterations.
		void setNumberOfLSIterations(int noi) {fLSRelatedInfo.fNumberOfLSIterations = noi;}
		///	Returns The number of LS iterations made to reach the solution.
		int getNumberOfLSIterations() const{return fLSRelatedInfo.fNumberOfLSIterations;}

		/// Sets the calculated sigma S0 a posteriori.
		void setS0APosteriori(TReal s0Apost) {fLSRelatedInfo.fS0APosteriori = s0Apost;}

		///	Returns The calculated sigma S0 a posteriori.
		TReal getS0APosteriori() const{return fLSRelatedInfo.fS0APosteriori;}

		/// Sets the range in which the sigma 0 a posteriori should be
		void setChiS0Limits(TReal chiLo, TReal chiUp);

		/// Returns the lower limit of the S0 range
		TReal getChiS0LowLimit() const{return fLSRelatedInfo.fChiLoLimit;}
		/// Returns the upper limit of the S0 range
		TReal getChiS0UpLimit() const{return fLSRelatedInfo.fChiUpLimit;}

		/// Sets the constraint number
		void setNumberOfConstraints(int numOfCnstr) {fNumberOfConstraints = numOfCnstr;}
		/// Returns the constraint number
		int getNumberOfConstraints() const {return fNumberOfConstraints;}

		/// Sets if the standard deviation are used to TRUE
		void setStandDevUsed() {fhasStandardDeviations = true;}
		/// Returns if the standard deviation are used
		bool hasStandDeviations() {return fhasStandardDeviations;}
	//@}


	/*!@name Counters of points and measurments related functions.*/
	//@{
		///	Increase the total number of points defined under specific keyword (TSpatialStatus::ESpatialStatus).
		void addToPointNum(TSpatialStatus::ESpatialStatus status);

		/// Returns Total number of points defined under specific keyword (TSpatialStatus::ESpatialStatus).
		int getPointsDimension(TSpatialStatus::ESpatialStatus status) const;

		///Increase the total number of measurement of some type defined.
		void addToMeasurementNum(TMeasurementsGlobal::EMeasurementType type);

		/// Returns Total number of measurement types defined.
		int getMeasurementDimension(TMeasurementsGlobal::EMeasurementType type) const;
	//@}


private:

	/*!@name Collections of adjustable objects*/
	//@{
		/// Adjustable points are collected globally 
		TAdjustablePointCollection points;
		/// Adjustable lines are collected globally
		LGCAdjustableLineCollection lines;
		/// Adjustable planes are collected globally
		LGCAdjustablePlaneCollection planes;
		/// Adjustable angles are collected globally
		TAdjustableAngleCollection angles;
		/// Adjustable lengths are collected globally
		TAdjustableLengthCollection lengths;
	//@}
	
	/// Represents the hierachical structure of the file and stores the measurements
	TDataTree tree;
	
	// current position in the tree
	TDataTreeIterator pos;

	/// The project configuration, one per project
	TLGCConfig config;

	/// Exactly one instrument section per project
	TInstrumentData instruments;

	/// Provides writing functionalities for writing errors, warnings, which occured in any stage.
	TFileLogger fileLogger;

	/// Statistic output
	TLGCStatistic stat;

	/// List of relative errors
	LSRelErrorsContainer fRelError;
	
	// contains references: not assignable
	TLGCData& operator=(const TLGCData&);

	// Combined case if at least one PLR3D measurement appeared
	bool fCombinedCase;
	// If standard deviations to points or frames assigned
	bool fhasStandardDeviations;
	/// Number of constraints introduced, for free network adjustment.
	int		fNumberOfConstraints;

	struct TLSRelatedInfo{
		int fNumberOfLSIterations;
		TReal fS0APosteriori;
		TReal fChiLoLimit;
		TReal fChiUpLimit;
	};

	TLSRelatedInfo fLSRelatedInfo;
	TPointGlobal   fPointInfo;
	TMeasurementsGlobal fMeasInfo;

	/// Sets default values for number of points, measuremets, etc.
	void setDefaultValues();
};


#endif
