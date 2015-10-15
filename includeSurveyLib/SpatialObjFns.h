#ifndef SPATIALOBJFNS_H
#define SPATIALOBSFNS_H
#include <string>

//using namespace std;

#include "TGlobalDefs.h"

class  SpatialObjFns  
{
public:
	SpatialObjFns();
	/// return the N value in meters for the position in CCS corresponding 
	/// to the double values
	static int  getCG2000N0(double x, double y, double* N);
	static int  getCG2000NMachine(double x, double y, double* N);
	
	static int getCG1985N0(double x, double y, double* N);
	static int getCG1985NMachine(double x, double y, double* N);


	/// return the (XYZ) position in MLA of a point from the origin of the MLA 
	/// and the point to be transformed both in CCS
	//???
	static int transformToMLA(double x0, double y0, double z0,
							  double* x, double* y, double* z, char* geoid);

	
	/// return the (XYZ) position in CCS of a point from the origin of the MLA system
	/// in CCS and the point to be transformed in MLA 
	//???@
	static int transformFromMLA(double x0, double y0, double z0,
								double* x, double* y, double* z, char* geoid);


	/// return the (XYZ) position in MLA of a point from the origin of the MLA 
	/// and the point to be transformed both in CCS (with slope and bearing equal to 0
	/// and false origin (0,0,0) )
	//???
	static int transformToMLA2(double x0, double y0, double z0,
							   double* x, double* y, double* z, char* geoid);


	/// return the (XYZ) position in CCS of a point from the origin of the MLA system
	/// in CCS and the point to be transformed in MLA (with slope and bearing equal to 0
	/// and false origin (0,0,0) )
	//???@
	static int transformFromMLA2(double x0, double y0, double z0,
								 double* x, double* y, double* z, char* geoid);


	/// return coordinates as X,Y,h of a point at H metres on the vertical of another point
	static int descenteVert(double x, double y, double h, double deltaH,
							double* xt, double* yt, double* ht);

};
#endif
