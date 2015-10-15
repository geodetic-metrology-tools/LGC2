// TAReferenceFrame.cpp
//
/** An abstract base class for a reference frame. 
  
    Patterns:
    An Abstract STATE of a Spatial Position
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
//forward declarations
////////////////////////////////////////////////////////////////
#include	"TCompositeAffTransform.h"

#include	"TGeodeticRefFrame.h"
#include	"TModifiedLocalGeodeticRF.h"
#include	"TModifiedLocalAstronomicalRF.h"

#include    "TVRefFrameTransformation.h"
#include    <TARefFrameTransformation.h>
#include    <THelmertRefFrameTransform.h>

#include	"TAReferenceFrame.h"

#include	"TReferenceEllipsoid.h"

//////////////////////////////////////////////////////////////////////
// Initialisation
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


TAReferenceFrame::TAReferenceFrame( const string& name )
	: fName( name )/*, fRefFrameId(0)*/
{	// default constructor 
	fRefFrameId = TRefSystemFactory::kNotInGraph;
}


//TAReferenceFrame::TAReferenceFrame(const TAReferenceFrame& trf)
//{	// copy constructor 
//}


TAReferenceFrame::~TAReferenceFrame()
{
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

string  TAReferenceFrame::getName() const
{	//return the name of the reference frame
	
	return fName;
}

////////////////////////////////
// return the reference frame id
////////////////////////////////
TRefSystemFactory::ERefFrame  TAReferenceFrame::getRefFrameId() const
{
	return fRefFrameId;
}

////////////////////////////////////////////////
// return true if the reference frame is in graph
///////////////////////////////////////////////

bool TAReferenceFrame::isInRFFactory() const
{
	return TRefSystemFactory::getRefSystemFactory()->isInRFFactory(this);
}

/////////////////////////////////////////////////////////////
//Default code which return a "null" geodetic reference frame
/////////////////////////////////////////////////////////////

TGeodeticRefFrame* TAReferenceFrame::getGeodeticRF() const 
{ 
	return TGeodeticRefFrame::instance();
}

/////////////////////////
//Data extraction methods
/////////////////////////

//Access to the position vector of a spatial position 
//using private function of TSpatialPosition
TPositionVector TAReferenceFrame::getPositionVector(const TSpatialPosition* sp) const
{
	return sp->getPositionVector();
}



//Access to the free vector of a spatial vector
//using private function of TSpatialVector
TFreeVector TAReferenceFrame::getVector(const TSpatialVector* sv) const
{
	return sv->getVector();
}


//Access to the rotation matrix of a spatial orientation
//using private function of TSpatialOrientation
TRotationMatrix TAReferenceFrame::getOrientation(const TSpatialOrientation* so) const
{
	return so->getOrientation();
}


//////////////////
//Settings methods
//////////////////
bool TAReferenceFrame::setPositionVector(TSpatialPosition* sp, const TPositionVector& pv)
{//Setting of the position vector for a spatial position
//using private function of TSpatialPosition
	return sp->setPositionVector(pv);
	
}



bool TAReferenceFrame::setVector(TSpatialVector* sv, const TFreeVector& fv)
{//Setting of the free vector for a spatial vector
//using private function of TSpatialVector
	bool result;
	result = sv->setVector(fv);
	return result;
}



bool TAReferenceFrame::setOrientation(TSpatialOrientation* so, const TRotationMatrix& rm)
{//Setting of the rotation matrix for a spatial orientation
//using private function of TSpatialOrientation

	return so->setOrientation(rm);
}

bool TAReferenceFrame::transform(TSpatialPosition *sp, TAReferenceFrame* rf)
{
	TPositionVector position(sp->getCoordinates(sp->getCoordSys()));
	TRefFrameWrapper from(this);
	TRefFrameWrapper to(rf);
	TARefFrameTransformation *postGraphTrafo(0);

	if (! this->isInGraph()) {
		// transform to CGRF manually
		TARefFrameTransformation* toGRF = this->getRFTransfo2CGRF();
		toGRF->transform(position);

		sp->changeRefFrameTo(this);
		sp->setCoordinates(position);

		from.setFrame(this->getGeodeticRF());
	}

	if (! rf->isInGraph()) {
		// destination system is a local system
		// transform from CGRF manually
		postGraphTrafo = rf->getRFTransfo2CGRF()->inverse();

		to.setFrame(rf->getGeodeticRF());
	}

	if (! (from == to)) {
		vector<TARefFrameTransformation*> transfo = TGraph::getGraph()->getTransform(from, to);
		
		if (transfo[0] == 0)
		{return false;}

		// reverse the order of the vector to have transformations as applicated to the point
		reverse(transfo.begin(), transfo.end());
			

		// application of the successive transformations
		for (vector<TARefFrameTransformation*>::iterator iter = transfo.begin(); 
			 iter != transfo.end(); 
			 iter++)
			(*iter)->transform(position);
		
		sp->changeRefFrameTo(to.getFrame());
		sp->setCoordinates(position);
	}

	if (postGraphTrafo != 0) {
		// The destination system is not in the graph
		postGraphTrafo->transform(position);
		sp->changeRefFrameTo(rf);
		sp->setCoordinates(position);
	}
		
	return true;
}

// TODO: merge with the function above, same content, only signature differences
// between  TSpatialVector an TSpatialPosition
bool TAReferenceFrame::transform( TSpatialVector* sv, TAReferenceFrame* rf ) 
{
	// transform a vector from a reference frame to another
	TFreeVector freeVector(sv->getElements(sv->getCoordSys()));

	TRefFrameWrapper from(this);
	TRefFrameWrapper to(rf);
	TARefFrameTransformation *postGraphTrafo(0);

	if (! this->isInGraph()) {
		// transform to CGRF manually
		TARefFrameTransformation* toGRF = this->getRFTransfo2CGRF();
		toGRF->transform(freeVector);

		sv->changeRefFrameTo(this);
		sv->setElements(freeVector);
		
		from.setFrame(this->getGeodeticRF());
	}

	if (! rf->isInGraph()) {
		// destination system is a local system
		// transform from CGRF manually
		postGraphTrafo = rf->getRFTransfo2CGRF()->inverse();
		
		to.setFrame(rf->getGeodeticRF());
	}

	if (! (from == to)) {
		vector<TARefFrameTransformation*> transfo = TGraph::getGraph()->getTransform(from, to);
		
		if (transfo[0] == 0)
		{return false;}

		// reverse the order of the vector to have transformations as applicated to the point
		reverse(transfo.begin(), transfo.end());
			

		// application of the successive transformations
		for (vector<TARefFrameTransformation*>::iterator iter = transfo.begin(); 
			 iter != transfo.end(); 
			 iter++)
			(*iter)->transform(freeVector);
		
		sv->changeRefFrameTo(to.getFrame());
		sv->setElements(freeVector);
	}

	if (postGraphTrafo != 0) {
		// The destination system is not in the graph
		postGraphTrafo->transform(freeVector);
		sv->changeRefFrameTo(rf);
		sv->setElements(freeVector);
	}
		
	return true;
}

TARefFrameTransformation* TAReferenceFrame::getRFTransfo2CGRF()
{// transformation from this reference frame to CGRF (in case of a reference frame not included in TGraph)
	static THelmertRefFrameTransform* t = new THelmertRefFrameTransform();
	return t; 
}

