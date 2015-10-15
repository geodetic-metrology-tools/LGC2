#ifndef _LGC_DATA_H_
#define _LGC_DATA_H_


#include <Measurements/TMeasurements.h>
#include "TLGCConfig.h"
#include <AdjustableObjects\TAdjustableObjectCollection.h>
#include "TTransformation.h"
#include <Global.h>
#include "TFileLogger.h"

#include "TADataSet.h"


/**
	This is a compound for the classes that are pushed around during the execution of the
	program. It makes the handling of points, measurements, insruments and frames easier
	by grouping together everything that is needed so the interaction with the LGC modules 
	is reduced to ineracting with a single object.
*/
/** 
Each node of a tree is a collection of measurements, 
and addditional local frame. 

Points which belong to a node are not kept directly here, 
but every point stores a iterator to a node where was defined.

The frames, points, scalars, angles, lines and planes
are created and stored in a global collection to ensure their 
uniqueness and have a possibility to use objects that have not been declared yet.
They will be resolved at a later step.

Only the measurements are kept directly in the tree nodes since they are really unique
to each level of the tree where points.
*/
struct TTreeEntry {
	/// Unique ID of a tree node
  	std::vector<int> ID; 
 
	///Returns TRUE if the node is a ROOT node
	bool isROOTNode() const{return frame.getName() == "ROOT";}

	/// There is exactly one transformation that led to this node
	TAdjustableHelmertTransformation frame; 

	/// Whole collection of measurements per node
	TMeasurements    measurements; 

	/// The ignoreMeasurements entry usually comes from the config (sim) object
	explicit TTreeEntry(const bool& ignoreMeasValues) : 
 	
	measurements(TMeasurements(ignoreMeasValues)), frame("", std::bitset<3>(std::string("111")), std::bitset<3>(std::string("111")),std::bitset<1>(1)){frame.getLine();} 
	~TTreeEntry() {}
};

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

struct TMeasurementsGlobal{
	enum EMeasurementType{kANGL, kZEND, kDIST, kPLR3D, kDLEV, kDHOR, kECTH, kDSPT, kDVER, kUVEC, kUVD};

	int	fNumANGL;
	int	fNumZEND;
	int	fNumDIST;
	int	fNumPLR3D;
	int	fNumDLEV;
	int fNumDHOR;
	int fNumECTH;
	int fNumDSPT;
	int fNumDVER;
	int	fNumUVEC;
	int	fNumUVD;
};

// TDataTree, and iterators defined in "Global.h" file

/// This composite can be seen as PROJECT DATA, containing everything that is needed.
class TLGCData : public TADataSet{
public:

	/**
		The default constructor provides a tree which consists only of the root node 
		and it is in a valid state, i.e. the current node pointer. 
	*/
	TLGCData();

	/// access the active level of the tree
	inline TTreeEntry& getCurrentNode() { return *(pos->get());}

	/// Sets the position iterator to the parent of the current node
	inline void moveUp() {pos = TDataTree::parent(pos);}

	/// Returns current iterator position in the tree
	inline TDataTreeIterator getCurrentPosition(){return pos;}

	/// Adds a new level to the tree passing the local transformation that led to this level.
	TTreeEntry& addChild(TAdjustableHelmertTransformation* trafo);

	/// nbrs of unkowns, equations, observations 
	UEOIndices	fUEOIndices; 

	/*!
		\returns collctions of points, lines, planes, angles & scalars
	*/
	TAdjustablePointCollection& getPoints() {return points;}
	TAdjustablePointCollection const& getPoints() const {return points;}
	TAdjustableLineCollection& getLines() {return lines;}
	TAdjustableLineCollection const& getLines() const {return lines;}
	TAdjustablePlaneCollection& getPlanes() {return planes;}
	TAdjustablePlaneCollection const& getPlanes() const {return planes;}
	TAdjustableAngleCollection& getAngles() {return angles;}
	TAdjustableAngleCollection const& getAngles() const {return angles;}
	TAdjustableScalarCollection& getScalars() {return scalars;}
	TAdjustableScalarCollection const& getScalars() const {return scalars;}

	/*!
		\returns the project confifuration
	*/
	TLGCConfig& getConfig() {return config;}
	TLGCConfig const& getConfig() const {return config;}

	/*!
		\returns the file logger used to write on an output file
	*/
	TFileLogger& getFileLogger() {return fileLogger;}
	TFileLogger const& getFileLogger() const {return fileLogger;}
	
	/*!
		\returns the LGCData structure of the input files containing measurments
	*/
	TDataTree& getTree() {return tree;}
	TDataTree const& getTree() const {return tree;}

	/*!
		\returns the instruments section
	*/
	TInstrumentData& getInstruments() {return instruments;}
	TInstrumentData const& getInstruments() const {return instruments;}

	void setCombinedCaseCalcUsed() {fCombinedCase = true;}

	bool isCombinedCaseUsed() {return fCombinedCase;}

	/*!
		\returns The number of frames used (if no frame used the result is 1 for a ROOT frame)
	*/
	size_t getNumberOfFrames() const{return tree.size();};

	/*!
		Sets the number of LS iterations.
	*/
	void setNumberOfLSIterations(int noi) {fLSRelatedInfo.fNumberOfLSIterations = noi;}


	/*!
		\returns The number of LS iterations made to reach the solution.
	*/
	int getNumberOfLSIterations() const{return fLSRelatedInfo.fNumberOfLSIterations;}



	void setS0APosteriori(TReal s0Apost) {fLSRelatedInfo.fS0APosteriori = s0Apost;}

	/*!
		\returns The calculated sigma S0 a posteriori.
	*/
	TReal getS0APosteriori() const{return fLSRelatedInfo.fS0APosteriori;}

	/*!
		Increase the total number of points defined under specific keyword (TSpatialStatus::ESpatialStatus).
	*/
	void addToPointNum(TSpatialStatus::ESpatialStatus status);

	/*!
		\returns Total number of points defined under specific keyword (TSpatialStatus::ESpatialStatus).
	*/
	int getPointsDimension(TSpatialStatus::ESpatialStatus status) const;

	/*!
		Increase the total number of measurement of some type defined.
	*/
	void addToMeasurementNum(TMeasurementsGlobal::EMeasurementType type);

	/*!
		\returns Total number of measurement types defined.
	*/
	int getMeasurementDimension(TMeasurementsGlobal::EMeasurementType type) const;

	void setChiS0Limits(TReal chiLo, TReal chiUp) {
		fLSRelatedInfo.fChiLoLimit = chiLo;
		fLSRelatedInfo.fChiUpLimit = chiUp;
	}

	TReal getChiS0LowLimit() const{return fLSRelatedInfo.fChiLoLimit;}
	TReal getChiS0UpLimit() const{return fLSRelatedInfo.fChiUpLimit;}


private:

	/*!@name Collections of adjustable objects*/
	//@{
		/// Adjustable points are collected globally 
		TAdjustablePointCollection points;
		/// Adjustable lines are collected globally
		TAdjustableLineCollection lines;
		/// Adjustable planes are collected globally
		TAdjustablePlaneCollection planes;
		/// Adjustable angles are collected globally
		TAdjustableAngleCollection angles;
		/// Adjustable scalars are collected globally
		TAdjustableScalarCollection scalars;
	//@}

	/// Needed for the instantiation of  measurement 
	/// objects to tell if a measurement is really missing or 
	/// omitted intentionally, e.g. in a simulation
	bool& ignmeas;
	
	/// Represents the hierachical structure of the file and stores the measurements
	TDataTree tree;
	
	/// There is one configuration per project
	TLGCConfig config;

	/// For writing to an output file
	TFileLogger fileLogger;
	
	/// Exactly one instrument section per project
	TInstrumentData instruments;
	
	// contains references: not assignable
	TLGCData& operator=(const TLGCData&);
	// current position in the tree
	TDataTreeIterator pos;
	//Combined case if at least one PLR3D or DIR3D measurements appeared
	bool fCombinedCase;

	struct TLSRelatedInfo{
		int fNumberOfLSIterations;
		TReal fS0APosteriori;
		TReal fChiLoLimit;
		TReal fChiUpLimit;
	};

	TLSRelatedInfo fLSRelatedInfo;
	TPointGlobal   fPointInfo;
	TMeasurementsGlobal fMeasInfo;

	void setDefaultValues();
};

#endif
