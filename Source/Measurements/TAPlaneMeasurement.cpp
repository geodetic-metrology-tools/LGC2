//TAPlaneMeasurement.cpp
/*	Abstract Base Class. Defines the methods common to all
	survey measurements having a target plane	
*/


#include "TAPlaneMeasurement.h"



//////////////////////////////////////////////////////////////////
//protected constructor /destructor
//////////////////////////////////////////////////////////////////
TAPlaneMeasurement::TAPlaneMeasurement()
{//default constructor
	TSpatialPlaneName defaultName;
	fTargetPlaneName = defaultName;
//	fIdentifier = 0;
//	fComment = "";
//	fHeaderComment = "";

	TLength null;//status kNull
	fObservedDist = null;
	fSigmaAPriori = null;

	TDistConstants dc;
	fMeasConstant = dc;
}


TAPlaneMeasurement::TAPlaneMeasurement(const TSpatialPlaneName tgName, const TLength obsDist, const TLength sigma)
{//constructor setting the target point's name
	fTargetPlaneName = tgName;
//	fIdentifier = 0;
//	fComment = "";
//	fHeaderComment = "";

	TLength null;//status kNull
	fObservedDist = obsDist;
	fSigmaAPriori = sigma;

	TDistConstants dc;
	fMeasConstant = dc;
}


TAPlaneMeasurement::TAPlaneMeasurement(const TAPlaneMeasurement& source)
{//copy constructor
	(*this) = source;
}


TAPlaneMeasurement::~TAPlaneMeasurement()
{//destructor
}


////////////////////////////////////////////////////////////////////////////////////
//operators
////////////////////////////////////////////////////////////////////////////////////
bool TAPlaneMeasurement::operator==(const TAPlaneMeasurement& right) const
{
	if (fTargetPlaneName == (right.fTargetPlaneName))
		return true;
	else
		return false;
}


void TAPlaneMeasurement::operator=(const TAPlaneMeasurement& right)
{
	this->TAMeasurement::operator =(right);
	fTargetPlaneName = right.fTargetPlaneName;
//	fIdentifier = right.fIdentifier;
//	fComment = right.fComment;
//	fHeaderComment = right.fHeaderComment;
	fObservedDist = right.fObservedDist;
	fSigmaAPriori = right.fSigmaAPriori;
	fMeasConstant = right.fMeasConstant;

	return;
}


///////////////////////////////////////////////////////////////////////////////////
//target's access and setting methods
///////////////////////////////////////////////////////////////////////////////////
TSpatialPlaneName TAPlaneMeasurement::getTargetPlaneName() const
{//returns the target's name
	return fTargetPlaneName;
}

/*
int TAPlaneMeasurement::getId() const
{//return the identifier of the measurement
	return fIdentifier;
}

void TAPlaneMeasurement::setId(int id)
{//sets the identifier of the measurement
	fIdentifier = id;
	return;
}


string	TAPlaneMeasurement::getComment() const
{//return the comment of the measurement
	return fComment;
}


void	TAPlaneMeasurement::setComment(string com)
{//sets the comment of the measurement
	fComment = com;
	return;
}*/


void	TAPlaneMeasurement::setPlaneParameters( const TSpatialPlaneName tgName,
												const TLength obsDist,
												const TLength sigma)
{
	fTargetPlaneName = tgName;
	fIdentifier = 0;
	fComment = "";
	fHeaderComment = "";

	TLength null;//status kNull
	fObservedDist = obsDist;
	fSigmaAPriori = sigma;

	TDistConstants dc;
	fMeasConstant = dc;
	return;
}



////////////////////////////
//Measurement function
////////////////////////////
TLength TAPlaneMeasurement::getLengthValue() const
{//returns the observed dist
	return fObservedDist;
}


TLength TAPlaneMeasurement::getSigma() const
{//returns the sigma a priori
	return fSigmaAPriori;
}


TDistConstants		TAPlaneMeasurement::getMeasConst() const
{//return the observed angle's constante
	return fMeasConstant;
}


void	TAPlaneMeasurement::ifNotDoneSetSigma(TLength sigma)
{//sets the sigma if it is still zero
	if (fSigmaAPriori.getMetresValue() == LITERAL(0.0))
	{
		fSigmaAPriori = sigma;
	}
	return ;
}


void	TAPlaneMeasurement::ifNotDoneSetMeasConst(const TDistConstants& constant)
{//Sets the measurement constant
	if (fMeasConstant.getValue().getMetresValue() == LITERAL(0.0))
	{
		fMeasConstant = constant;
	}
	return;
}