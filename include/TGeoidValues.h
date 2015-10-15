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


// typedefs
typedef	TReal	GeoidValue;	


////////////////////////////////////////////////////////////////


// Class Definition
class	TGeoidValues //: public TObject
{

public:
	// constructors and destructors
	TGeoidValues();
	TGeoidValues( GeoidValue, GeoidValue, GeoidValue, GeoidValue);
	TGeoidValues(const TGeoidValues&);
	virtual	~TGeoidValues();
	TGeoidValues& operator=(const TGeoidValues&);
	

	// member functions

	void setN(GeoidValue);			//set geoidal height
	void setXi(GeoidValue);			//set N-S component of vertical deflection
	void setEta(GeoidValue);		//set E-O component of vertical deflection
	void setDAlpha(GeoidValue);
	
	GeoidValue	getN() const;
	GeoidValue	getXi() const;
	GeoidValue	getEta() const;
	GeoidValue	getDAlpha() const;

	GeoidValue	getXiAzimuth(TAngle);
	GeoidValue	getEtaAzimuth(TAngle);

private:
	GeoidValue			fNValue;		// default = LITERAL(0.0)
	GeoidValue			fXiValue;		// default = LITERAL(0.0)
	GeoidValue			fEtaValue;		// default = LITERAL(0.0)
	GeoidValue			fDAlphaValue;	// default = LITERAL(0.0)
	
	//ClassDef(TGeoidValues, 1)
};



//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////

inline void TGeoidValues::setN(const GeoidValue value)
{	// set as a length measure of the given metres value
	fNValue = value;
}


inline void TGeoidValues::setXi(const GeoidValue value)
{	// set as a length measure of the given metres value
	fXiValue = value;
}


inline void TGeoidValues::setEta(const GeoidValue value)
{	// set as a length measure of the given metres value
	fEtaValue = value;
}


inline void TGeoidValues::setDAlpha(const GeoidValue value)
{	// set as a length measure of the given metres value
	fDAlphaValue = value;
}


inline GeoidValue TGeoidValues::getN() const
{	// get the metres length value
	return fNValue;
}


inline GeoidValue TGeoidValues::getXi() const
{	// get the metres length value
	return fXiValue;
}


inline GeoidValue TGeoidValues::getEta() const
{	// get the metres length value
	return fEtaValue;
}


inline GeoidValue TGeoidValues::getDAlpha() const
{	// get the metres length value
	return fDAlphaValue;
}


#endif // SU_GEOIDAL_VALUES
