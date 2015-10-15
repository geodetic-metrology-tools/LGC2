// TGeoidValues.h
//
// Class for geoidal values as defined for survey purposes
//
// Copyright 1999,2000, Mark Jones, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////


#ifndef SU_GEOIDAL_VALUES
#define SU_GEOIDAL_VALUES

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include	<TVNumericValue.h>
#include	<assert.h>
class	TAngle;

////////////////////////////////////////////////////////////////


// Class Definition
class	TGeoidValues //: public TObject
{

public:
	// constructors and destructors
	TGeoidValues();
	TGeoidValues( TReal, TReal, TReal, TReal);
	TGeoidValues(const TGeoidValues&);
	virtual	~TGeoidValues();
	TGeoidValues& operator=(const TGeoidValues&);
	

	// member functions

	void setN(TReal);			//set geoidal height
	void setXi(TReal);			//set N-S component of vertical deflection
	void setEta(TReal);		//set E-O component of vertical deflection
	void setDAlpha(TReal);
	
	TReal	getN() const;
	TReal	getXi() const;
	TReal	getEta() const;
	TReal	getDAlpha() const;

	TReal	getXiAzimuth(TAngle);
	TReal	getEtaAzimuth(TAngle);

private:
	TReal			fNValue;		// default = LITERAL(0.0)
	TReal			fXiValue;		// default = LITERAL(0.0)
	TReal			fEtaValue;		// default = LITERAL(0.0)
	TReal			fDAlphaValue;	// default = LITERAL(0.0)
	
};



//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////

inline void TGeoidValues::setN(const TReal value)
{	// set as a length measure of the given metres value
	fNValue = value;
}


inline void TGeoidValues::setXi(const TReal value)
{	// set as a length measure of the given metres value
	fXiValue = value;
}


inline void TGeoidValues::setEta(const TReal value)
{	// set as a length measure of the given metres value
	fEtaValue = value;
}


inline void TGeoidValues::setDAlpha(const TReal value)
{	// set as a length measure of the given metres value
	fDAlphaValue = value;
}


inline TReal TGeoidValues::getN() const
{	// get the metres length value
	return fNValue;
}


inline TReal TGeoidValues::getXi() const
{	// get the metres length value
	return fXiValue;
}


inline TReal TGeoidValues::getEta() const
{	// get the metres length value
	return fEtaValue;
}


inline TReal TGeoidValues::getDAlpha() const
{	// get the metres length value
	return fDAlphaValue;
}


#endif // SU_GEOIDAL_VALUES
