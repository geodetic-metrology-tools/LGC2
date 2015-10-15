/////////////////////////////////////////////////
// TGraph.cpp
// source file for Tgraph singleton class
////////////////////////////////////////////////

////////////////
// include files
////////////////
// #include <limits>
#include <iomanip>

#include "TReferenceEllipsoid.h"
#include "TGeodeticRefFrame.h"
#include "TAModifiedLocalastronomicalRF.h"
#include "TGraphMLARF.h"
#include "TXYHeProjection.h"
#include "TX0Y0HeProjection.h"
#include "TXYHgProjection.h"
#include "TCernGridGeoid.h"
#include "TCernParabolicGeoid.h"

#include "TMLA2GCTransformation.h"
#include "TLA2LGTransformation.h"
#include "TLG2GCTransformation.h"
#include "THelmertTransformation.h"
#include "THelmertRefFrameTransform.h"
#include "TXYHe2MLATransformation.h"
#include "TX0Y0He2XYHeTransformation.h"
#include "TXYHg2XYHeTransformation.h"


#include "TGraph.h"


/////////////////////////////
// fGraph initialisation to 0
/////////////////////////////
TGraph*  TGraph::fGraph = 0;

////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////
TGraph::TGraph()
{// default constructor
}


TGraph::~TGraph()
{//destructor

}


void	TGraph::deleteGraph()
{
	if(fGraph != 0)
	{
		delete fGraph;
	}
	return;
}


////////////////////////////////////////////////////////////////////////////////////////
//INIT FUNCTION
////////////////////////////////////////////////////////////////////////////////////////
void	TGraph::init()
{
	// initialise the elements of the last transformation
	fLastFrom = "NULL";
	fLastTo = "NULL";

	// insertion of transformations taking from the reference system factory
	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCCS2CGRF),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCGRF2CCS));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kLAp02LGp0),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kLGp02LAp0));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kLGp02CGRF),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCGRF2LGp0));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kLAp02CCS),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCCS2LAp0));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kROMA2WGS),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kWGS2ROMA));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kWGS2CGRF),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCGRF2WGS));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kITRF972CGRF),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCGRF2ITRF97));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kITRF972ETRF93),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kETRF932ITRF97));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kETRF932kCHTRF95),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCHTRF952kETRF93));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kETRF932kRGF93),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kRGF932kETRF93));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kETRF932CH1903plus),
            TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCH1903plus2ETRF93));
#ifdef USE_SWISSTOPO
    insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCH1903plus2SwissLV95),
            TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kSwissLV952CH1903plus));

    insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kSwissLV952SwissLV03),
            TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kSwissLV032SwissLV95));
#endif

    insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kETRF932FrenchRGF93),
            TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kFrenchRGF932ETRF93));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kETRF932kLambert93),
            TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kLambert932ETRF93));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHe2CCS),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCCS2XYHe));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kX0Y0He2XYHe),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHe2X0Y0He));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHg2XYHe),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHe2XYHg));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHg2XYHe00Topo),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHe00Topo2XYHg));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHg2XYHe00Machine),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHe00Machine2XYHg));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHg2XYHe85),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHe852XYHg));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHg2XYHe85Machine),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHe85Machine2XYHg));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kXYHsSphereSPS2CCS),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCCS2XYHsSphereSPS));

	insert( TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCCS2CGRFSphere),
			TRefSystemFactory::getRefSystemFactory()->getTransformation(TRefSystemFactory::kCGRFSphere2CCS));

	return;
}





////////////////////////////////////////////////////////////////////////////////////////////////////
//MEMBER FUNCTION
////////////////////////////////////////////////////////////////////////////////////////////////////
TGraph* TGraph::getGraph()
{//returns the unique instance : clients access trought this member function
	if (fGraph == 0)
	{
		fGraph = new TGraph();
		fGraph->init();
	}

	return fGraph;
}

	

vector<TARefFrameTransformation*> TGraph::getTransform(TRefFrameWrapper& from,
													   TRefFrameWrapper& to)
{// return the transformation between 2 RF from the graph
	if (fLastFrom == from.getName() && fLastTo == to.getName())
		return fLastTransformation;
	else 
	{
		vector<int> tr, pr;
		vector<TTransformWrapper> transform;
		vector<TARefFrameTransformation*> transfo;
		int start = -1, end = -1;

		string cgrfTransformation = "no";
		TTransformWrapper transfoWithCGRF;
		

	/*	
		vector<RefFrame>::iterator refFrameIterator;

		{
			int i = 0;
			for(refFrameIterator = C.begin(); 
				refFrameIterator != C.end();
				refFrameIterator++)
			{
				if (from == (*refFrameIterator).first)
				{
					start = i;
				}
				++i;
			}
		}
		//Should output 1 4 8
	*/	

		for (unsigned int i = 0; i < size(); i++)
		{
			if (from == C[i].first)
				start = i;
			
		}

		if (start == -1)
		{
			cgrfTransformation = "before";

			transfoWithCGRF.setTransformation(from.getFrame()->getRFTransfo2CGRF());
			TRefFrameWrapper rffrom(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCGRF));

			for (unsigned int i = 0; i < size(); i++)
			{
				if (rffrom == C[i].first)
					start = i;
			}
		}


		path(tr, pr,transform, start);

		unsigned int  i;
		for (i = 0; i < size(); i++)
		{
			if (to == C[i].first)
				end = i;
		}
	
		if (end == -1)
		{
			cgrfTransformation = "after";

			transfoWithCGRF.setTransformation(to.getFrame()->getRFTransfo2CGRF()->inverse());
			
			TRefFrameWrapper rfto(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCGRF));

			for (i = 0; i < size(); i++)
			{
				if (rfto == C[i].first)
					end = i;
			}

		}


		transfo.resize(tr[end]);
	
		transfo[0] = transform[end].getTransformation();
		if (transfo[0] != 0)
		{
			vector<int> precedent(transfo.size(), -1);
			precedent[0] = pr[end];
	
			for (i = 1; i < transfo.size(); i++)
			{
				precedent[i] = pr[precedent[i-1]];
				transfo[i] = transform[precedent[i-1]].getTransformation();
			}
			
		}


		if (cgrfTransformation == "before")
			transfo.push_back(transfoWithCGRF.getTransformation());
		else if (cgrfTransformation == "after")
		{
			transfo.insert(transfo.begin(), transfoWithCGRF.getTransformation());
		}

		fillLastTransformation(from.getName(), to.getName(), transfo);

		
		return transfo;
	}

}




////////////////////////////////////////////////////////////////////////////////////////////////////
//PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////
int TGraph::insert(const TRefFrameWrapper& RF)
{// insert a RF throught a reference frame wrapper
	for (unsigned int i = 0; i < size(); i++)
	{
		if (RF == C[i].first)
			return i;
	}

	RefFrame rf(RF, Successor());

	C.push_back(rf);
	C[size()-1].first.setIdentifier(size()-1);

	//returns the indice of the introduced reference frame
	return size()-1;
}


int TGraph::insert(TAReferenceFrame* RF)
{// insert a RF throught a reference frame pointer
	TRefFrameWrapper wrapper(RF);
	return insert(wrapper);
}



void TGraph::insert(TARefFrameTransformation* refFrameTrans, TARefFrameTransformation* inv)
{// insert a transformation
	TTransformWrapper trans(refFrameTrans);
	int RF1 = insert(refFrameTrans->getSourceFrame());
	int RF2 = insert(refFrameTrans->getDestinationFrame());

	(C[RF1].second)[RF2] = trans;

	//transformation inverse
	(C[RF2].second)[RF1] = inv;

	return;	
}



void TGraph::path(vector<int>& trans, vector<int>& pred,vector<TTransformWrapper>& transform, int start)
{// find the shortest a RF and all the otehers in the graph

	trans = vector<int>(C.size(), 5000);
	trans[start] = 0;
	pred = vector<int>(C.size(),-1);

	transform = vector<TTransformWrapper>(C.size(),(TTransformWrapper)0);

	
	DynamicPriorityQueue<int> Q(trans);
	int i;

	while (!Q.empty())
	{
		i=Q.topIndex();
		Q.pop();

		Successor::iterator startN = C[i].second.begin(), 
			endN = C[i].second.end();

		while (startN != endN)
		{
			int j = (*startN).first;
			if (trans[j]>trans[i]+1)
			{
				Q.changeKeyAt(j,trans[i]+1);
				pred[j]=i;
				transform[j] = (C[i].second)[j];
			}
			++startN;
		}
	}
}


void TGraph::fillLastTransformation(const string from, const string to, const vector<TARefFrameTransformation*> lastTransform)
{// fill the last transformation objects
	fLastFrom = from;
	fLastTo = to;
	fLastTransformation = lastTransform;

	return;
}



