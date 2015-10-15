//TCoordSysFactory.cpp
//
//Class for use of coordinate system
//
// Patterns:
//
// 
// Copyright 2002 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include "T3DCartesianCoordSys.h"
#include "TGeodeticCoordSys.h"
#include "T2DCartesianCoordSys.h"
#include "T2DCartesianPlusHSys.h"

#include "TCoordSysFactory.h"

#include "TNotInGraphException.h"
////////////////////////////////////////////////////////////////


//ClassImp(TCoordSysFactory)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

TCoordSysFactory*   TCoordSysFactory::fCoordSysFactoryPtr = 0;

//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////
TCoordSysFactory::TCoordSysFactory()
{//default constructor
}


TCoordSysFactory::~TCoordSysFactory()
{//destructor

/*	int nbr = fCoordSysList.size();

	if(nbr != 0)
	{
		//fCoordSysList.clear();
		int i = 0;
		while( i != nbr)
		{
			delete fCoordSysList[0];
			i++;
		}
	}
	if(fCoordSysFactoryPtr != 0)
	{
		delete	fCoordSysFactoryPtr;
	}*/
}

void TCoordSysFactory::deleteCoordSysFactory()
{
	if(fCoordSysFactoryPtr != 0)
	{
		if(fCoordSysList.size() != 0)
		{
			std::vector<TVCoordinateSystem*>::iterator iter; 
			while(fCoordSysList.begin() != fCoordSysList.end())
			{
				iter = fCoordSysList.begin();
				delete (*iter);
				fCoordSysList.erase(fCoordSysList.begin());
			}
		}

		delete	fCoordSysFactoryPtr;
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////////////
//MEMBERS FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////
void	TCoordSysFactory::init()
{
	//insert the coordinate systems
	//3D cartesian
	T3DCartesianCoordSys* p3DCoordSys = new T3DCartesianCoordSys();
	p3DCoordSys->setCoordSysId(k3DCartesian);
	fCoordSysList.push_back(p3DCoordSys);

	//3D geodetic
	TGeodeticCoordSys* pGeodetic = new TGeodeticCoordSys();
	pGeodetic->setCoordSysId(kGeodetic);
	fCoordSysList.push_back(pGeodetic);

	//2D cartesian
	T2DCartesianCoordSys* p2DCoordSys = new T2DCartesianCoordSys();
	p2DCoordSys->setCoordSysId(k2DCartesian);
	fCoordSysList.push_back(p2DCoordSys);

	//2D + H
	T2DCartesianPlusHSys* p2DPlusH = new T2DCartesianPlusHSys();
	p2DPlusH->setCoordSysId(k2DPlusH);
	fCoordSysList.push_back(p2DPlusH);

	return;
}



TVCoordinateSystem* TCoordSysFactory::getCoordSys(const ECoordSys coordSysId)
{// return a pointer to the coordinate system askes for
	std::vector<TVCoordinateSystem*>::iterator iter = fCoordSysList.begin();
	std::vector<TVCoordinateSystem*>::iterator iterEnd = fCoordSysList.end();
	
	while (iter != iterEnd)
	{
		if((*iter)->getCoordSysId() == coordSysId)
			return *iter;
		iter++;
	}

	cerr << "Error : Id. not in CoordSysList" << endl;
	throw TNotInGraphException("TNotInGraphException");
}


	
TCoordSysFactory* TCoordSysFactory::getCoordSysFactoryPtr()
{//!return the unique object of TCoordSysFactory
	if(fCoordSysFactoryPtr==0)
	{
		fCoordSysFactoryPtr = new TCoordSysFactory();
		fCoordSysFactoryPtr->init();
	}
	return fCoordSysFactoryPtr;
}


///////////////////////////////////////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////////////////////////////////////








