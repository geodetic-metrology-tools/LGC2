////////////////////////////////////////////////////////////////////
// TLGCCalcParams.cpp : implementation file
// The parameters for a LGC computation:
//  	way to compute standard deviations
//		convergence criteria
//		computation systems and reference surfaces
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include  "TLGCCalcParams.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TLGCCalcParams::TLGCCalcParams()
{//Default Constructor
	fUsingSAPriori = false;
	fConvCriteria = LITERAL(0.000005);
	
	fAlpha = LITERAL(0.01);
	fBeta = LITERAL(0.1);
	fSaveFaultDetect = false;

	fPdorBearDefined = true;

	fRefSurface = TRefSystemFactory::kNotInGraph;

	/*fIsSim = false;
	fNumOfSim = 0;

	fFreeCalc = false;

	allPointsFixed = false;*/
	
	//fPtNamePairsList = new TWorkingPointNamePairs();
}


TLGCCalcParams::TLGCCalcParams(const TLGCCalcParams& source)
{//copy constructor
	//fPtNamePairsList = 0;//initialisation
	*this = source;

}


TLGCCalcParams::~TLGCCalcParams()
{// destructor
	/*if(fPtNamePairsList != 0)
	{
		delete fPtNamePairsList;
	}*/
}



////////////////////////////////////////////////////////////////////////
//Member function
////////////////////////////////////////////////////////////////////////
TLGCCalcParams& TLGCCalcParams::operator=(const TLGCCalcParams& right)
{//copy assignment operator
	if (this != &right)
	{
		fUsingSAPriori = right.fUsingSAPriori;
		fConvCriteria = right.fConvCriteria;
		fRefSurface = right.fRefSurface;
		//fIsSim = right.fIsSim;
		//fNumOfSim = right.fNumOfSim;
		fSaveFaultDetect = right.fSaveFaultDetect;
		fAlpha = right.fAlpha;
		fBeta = right.fBeta;
		//fFreeCalc = right.fFreeCalc;
		/*if(fPtNamePairsList != 0)
		{
			delete fPtNamePairsList;
			fPtNamePairsList=0;
		}*/
		//fPtNamePairsList = new TWorkingPointNamePairs(*(right.fPtNamePairsList));
		fPdorBearDefined = right.fPdorBearDefined;
		//allPointsFixed = right.allPointsFixed;
	}
	return *this;
}



bool	TLGCCalcParams::operator==(const TLGCCalcParams& right) const
{//equivalence operator
	bool equal =  fUsingSAPriori == right.fUsingSAPriori &&
					fConvCriteria == right.fConvCriteria &&
					fRefSurface == right.fRefSurface &&
					//fIsSim == right.fIsSim &&
					//fNumOfSim == right.fNumOfSim &&
					fSaveFaultDetect == right.fSaveFaultDetect &&
					//fFreeCalc == right.fFreeCalc &&
					fPdorBearDefined == right.fPdorBearDefined; // &&
					//fPtNamePairsList == right.fPtNamePairsList;

	return equal;
}


// return the name of the reference surface
string  TLGCCalcParams::getRefSurfaceName() const
{
	if(fRefSurface == TRefSystemFactory::kNotInGraph)   {return "LOCAL";}
	else if(fRefSurface == TRefSystemFactory::kCERNXYHsSphereSPS)  {return "SPHE";}
	else if(fRefSurface == TRefSystemFactory::kCernXYHg85Machine)  {return "LEP";}
	else if(fRefSurface == TRefSystemFactory::kCernXYHg00Machine)  {return "RS2K";}
	return "Unknown";
}


////////////////////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////////////////////