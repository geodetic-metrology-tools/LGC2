///TALineMeasurement.cpp
/*	Abstract Base Class. Defines the methods common to all
	survey measurements having a target Line	
*/


#include "TALineMeasurement.h"



//////////////////////////////////////////////////////////////////
//protected constructor /destructor
//////////////////////////////////////////////////////////////////
TALineMeasurement::TALineMeasurement()
{//default constructor
	TSpatialLineName defaultName;
	fTargetLineName = defaultName;
	
	TLength null;//status kNull
	fObservedDist = null;
	fSigmaAPriori = null;

	TDistConstants dc;
	fMeasConstant = dc;
}


TALineMeasurement::TALineMeasurement(const TSpatialLineName tgName, const TLength obsDist, const TLength sigma)
{//constructor setting the target point's name
	fTargetLineName = tgName;
	
	TLength null;//status kNull
	fObservedDist = obsDist;
	fSigmaAPriori = sigma;

	TDistConstants dc;
	fMeasConstant = dc;
}


TALineMeasurement::~TALineMeasurement()
{//destructor
}


////////////////////////////////////////////////////////////////////////////////////
//operators
////////////////////////////////////////////////////////////////////////////////////
bool TALineMeasurement::operator==(const TALineMeasurement& right) const
{
	if (fTargetLineName == (right.fTargetLineName))
		return true;
	else
		return false;
}


void TALineMeasurement::operator=(const TALineMeasurement& right)
{
	this->TAMeasurement::operator =(right);
	fTargetLineName = right.fTargetLineName;
	fObservedDist = right.fObservedDist;
	fSigmaAPriori = right.fSigmaAPriori;
	fMeasConstant = right.fMeasConstant;

	return;
}


///////////////////////////////////////////////////////////////////////////////////
//target's access and setting methods
///////////////////////////////////////////////////////////////////////////////////
TSpatialLineName TALineMeasurement::getTargetLineName() const
{//returns the target's name
	return fTargetLineName;
}




////////////////////////////
//Measurement function
////////////////////////////
TLength TALineMeasurement::getLengthValue() const
{//returns the observed dist
	return fObservedDist;
}


TLength TALineMeasurement::getSigma() const
{//returns the sigma a priori
	return fSigmaAPriori;
}


TDistConstants		TALineMeasurement::getMeasConst() const
{//return the observed angle's constante
	return fMeasConstant;
}


void	TALineMeasurement::ifNotDoneSetSigma(TLength sigma)
{//sets the sigma if it is still zero
	if (fSigmaAPriori.getMetresValue() == LITERAL(0.0))
	{
		fSigmaAPriori = sigma;
	}
	return;
}


void	TALineMeasurement::ifNotDoneSetMeasConst(const TDistConstants& constant)
{//Sets the measurement constant
	if (fMeasConstant.getValue().getMetresValue() == LITERAL(0.0))
	{
		fMeasConstant = constant;
	}
	return;
}


void	TALineMeasurement::setLineParameters(const TSpatialLineName tgName,
											 const TLength obsDist,
											 const TLength sigma)
{
	fTargetLineName = tgName;
	
	TLength null;//status kNull
	fObservedDist = obsDist;
	fSigmaAPriori = sigma;

	TDistConstants dc;
	fMeasConstant = dc;

	return;
}
////////////////////////////
//END
////////////////////////////
