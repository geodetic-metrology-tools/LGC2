// 
// TEuclidianReferenceFrame.h : Header File
//
// Class for a frame of reference as defined for survey purposes
// 

#ifndef SU_EUCLIDIAN_REFERENCE_FRAME
#define SU_EUCLIDIAN_REFERENCE_FRAME

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include	<float.h>
#include <math.h>
//#include	<assert.h>
class    TSpatialPoint;
class    TReferenceEllipsoid;
class	 TCoordinateSystem;
#include	"TReferenceFrame.h"
#include    <string>
//using namespace std;
// typedefs

////////////////////////////////////////////////////////////////


// Class Definition
class	TEuclidianReferenceFrame : public TReferenceFrame // : public TObject
{

public:
	//constants
	
	// constructors and destructors
	TEuclidianReferenceFrame();
	TEuclidianReferenceFrame(const TEuclidianReferenceFrame&);
	virtual ~TEuclidianReferenceFrame();
	TEuclidianReferenceFrame& operator=(const TEuclidianReferenceFrame&);
	

	// member functions

	bool			*validCoordSys(TCoordinateSystem *);			//verify that a coordinate system is valid

	
private:
	
	//constants
	
private:

	
	//ClassDef(TEuclidianReferenceFrame, 1)
};



//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////
#endif // SU_EUCLIDIAN_REFERENCE_FRAME
