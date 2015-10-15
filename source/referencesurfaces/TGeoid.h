// TGeoid.h: 
//
// Purely Abstract Base Class (Interface) for a TGeoid.
// 
// 
// Copyright 1999,2000, Mark Jones, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////

#ifndef SU_GEOID
#define SU_GEOID

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//

class  TSpatialPoint;
#include "TGeoidValues.h"
#include  <string>
using namespace std;

//
// typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
class TGeoid //: public TObject  
{
public:
	//constants

	// constructors and destructors
	TGeoid();
	virtual	~TGeoid();

	//member functions
	virtual GeoidValue getN(TSpatialPoint&) const = 0;
	virtual GeoidValue getEta(TSpatialPoint&) const = 0;
	virtual GeoidValue getXi(TSpatialPoint&) const = 0;
	virtual GeoidValue getDAlpha(TSpatialPoint&) const = 0;
	virtual	GeoidValue getTGeoidValue(TSpatialPoint&) const =0;

	void	setName(string);
	string	getName() const;

protected:

	string  name;


private:
	// member attributes
	// copy constructor declaration
	TGeoid(const TGeoid&);
	// ClassDef(TGeoid, 1)
};




//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////

inline void TGeoid::setName(string n){	name = n;} // set the name of the geoid


inline string TGeoid::getName() const { return name;} // return the name of the geoid

#endif // SU_GEOID
