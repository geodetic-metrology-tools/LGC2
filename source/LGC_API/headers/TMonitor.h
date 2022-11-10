// STL
#include <Eigen/Dense>

#include <TLGCData.h>

#include "TVAbstractAlgorithm.h"

class TMonitor
{
public:
	// constructor
	TMonitor();
	// access to measurements, for updating observations
	void updateMeas(std::string, double);
	// get Meas IDs
	std::vector<std::string> getMeasIds();
	// triggering the adjustment claculation
	void adjust();
	// containing maps to measurement object references
	// should probably be private. is public to make the previous measurements available in the mockup
	struct
	{
		// Polar type
		std::unordered_map<std::string, TANGL &> ANGL;
		std::unordered_map<std::string, TZEND &> ZEND;
		std::unordered_map<std::string, TLINE &> DIST;
		std::unordered_map<std::string, TECTH &> ECTH;
		std::unordered_map<std::string, TECDIR &> ECDIR;
		std::unordered_map<std::string, TLINE &> DHOR;
		std::unordered_map<std::string, TPLR3D &> PLR3D;
		std::unordered_map<std::string, TORIE &> ORIE;
		// CAMD type
		std::unordered_map<std::string, TUVEC &> UVEC;
		std::unordered_map<std::string, TUVD &> UVD;
		// EDM type
		std::unordered_map<std::string, TDSPT &> DSPT;
		// LEVEL type, what about RefPt and RefPt+DHOR config?
		std::unordered_map<std::string, TDLEV &> DLEV;
		// SCALE Type
		std::unordered_map<std::string, TECHO &> ECHO;
		std::unordered_map<std::string, TECSP &> ECSP;
		std::unordered_map<std::string, TECVE &> ECVE;
		// INCL Type
		std::unordered_map<std::string, TINCLY &> INCLY;
		// HLSR Type
		std::unordered_map<std::string, TECWS &> ECWS;
		// "No" type
		std::unordered_map<std::string, TDVER &> DVER;
		std::unordered_map<std::string, TRADI &> RADI;
		std::unordered_map<std::string, TOBSXYZ &> OBSXYZ;
	} measRefs;

private:
	// LGC adjustment algorithm used by adjust method
	std::unique_ptr<TVAbstractAlgorithm> algorithm;
	// containing measurement configuration, observations, estimates
	std::shared_ptr<TLGCData> project;
	void initialize();
	// create a map for easy access to references to measurement objects without the need to go thrugh the frame tree
	void createMeasurementReferences();
};