// 
// THorizontalDistMeas.cpp : implementation file
//
// Class for a EDM dist measurement as defined for survey purposes
//
//#include "TWorkingStations.h"
#include "THorizontalDistROM.h"
#include "THorizontalDistMeas.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////
// default constructor
//////////////////////
THorizontalDistMeas::THorizontalDistMeas(): TAPointMeasurement(),
fObservedDist(LITERAL(0.0)),fSigmaAPriori(LITERAL(0.0)), fSigmaPpm(LITERAL(0.0)), fMeasConstant()
{

}

///////////////////////////////////////////////////////////////////////////////////
// constructor storing the station's setup point's name and the target point's name
///////////////////////////////////////////////////////////////////////////////////
THorizontalDistMeas::THorizontalDistMeas( TSpatialPointName targetName, TLength obsDist, TLength sigma):
TAPointMeasurement(targetName),fObservedDist(obsDist), fSigmaAPriori(sigma), fSigmaPpm(0), fMeasConstant(){


}



THorizontalDistMeas::~THorizontalDistMeas()
{
	

}


///////////////////
// copy constructor
///////////////////
THorizontalDistMeas::THorizontalDistMeas(const THorizontalDistMeas& source)
{
	*this = source;
}

////////////////////////////
// copy assignement operator
////////////////////////////
THorizontalDistMeas& THorizontalDistMeas::operator=(const THorizontalDistMeas& source){

	fTargetPtName = source.fTargetPtName;
	fIdentifier = source.fIdentifier;
	fComment = source.fComment;
	fHeaderComment = source.fHeaderComment;

	fObservedDist = source.getDistValue();
	fSigmaAPriori = source.getSigma();
	fSigmaPpm = source.fSigmaPpm;
	fMeasConstant = source.fMeasConstant;

//	fStationPtName = source.fStationPtName;

	return (*this);
}

///////////////////////////////////////////////////////
// returns the string representing the measurement kind
///////////////////////////////////////////////////////
string THorizontalDistMeas::getMeasKind() const{

	string s("HorizontalDist");
	return s;
}


////////////////////////////
// returns the observed dist
////////////////////////////
TLength THorizontalDistMeas::getDistValue() const{

	return fObservedDist;
}

/////////////////////////////
// returns the sigma a priori
/////////////////////////////
TLength THorizontalDistMeas::getSigma() const{

	return fSigmaAPriori;
}

////////////////////////////////////
// returns the sigma in ppm a priori
////////////////////////////////////
TLength THorizontalDistMeas::getSigmaPpm() const{

	return fSigmaPpm;
}

/////////////////////////////////////
// sets the sigma if it is still zero
/////////////////////////////////////
void THorizontalDistMeas::ifNotDoneSetSigma(TLength sigma){

	if (fSigmaAPriori.getMetresValue() == LITERAL(0.0))
		fSigmaAPriori = sigma;
}

/////////////////////////////////////////
// sets the sigma ppm if it is still zero
/////////////////////////////////////////
void THorizontalDistMeas::ifNotDoneSetSigmaPpm(TLength ppm){

	if (fSigmaPpm.getMetresValue() == LITERAL(0.0))
		fSigmaPpm = ppm;
}

////////////////////////////////////////////////////
// sets the measurement constant if it is still zero
////////////////////////////////////////////////////
void THorizontalDistMeas::ifNotDoneSetMeasConst(const TDistConstants& cte){

	if (fMeasConstant.getValue().getMetresValue() == LITERAL(0.0))
		fMeasConstant = cte;
}

