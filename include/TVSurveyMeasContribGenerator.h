//TVSurveyMeasContribGenerator.h : header file
//Interface for the contrib generators processing survey measurements


#ifndef SU_VSURVEYMEASCONTGEN
#define SU_VSURVEYMEASCONTGEN


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
class TInstrumentStation;
class TRoundOfMeasurements;
class VSurveyMeasurement;

class TVInputMatrices;


class TVSurveyMeasContribGenerator{

public :

	virtual	void	processMeasurement(TInstrumentStation*, TRoundOfMeasurements*, VSurveyMeasurement*) = 0;
	virtual void	setInputMatrices(TVInputMatrices *) = 0;


};
#endif