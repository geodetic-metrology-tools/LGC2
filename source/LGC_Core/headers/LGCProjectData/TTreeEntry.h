/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef LGC2_TTREE_ENTRY_H
#define LGC2_TTREE_ENTRY_H


//SURVEYLIB
#include <TAdjustableHelmertTransformation.h>
//LGC
#include <TMeasurements.h>


/** 
\ingroup LGCProjectData

Each node of a tree is a collection of measurements, 
and addditional local frame. 

The frames, points, scalars, angles, lines and planes
are created and stored in a global collection to ensure their 
uniqueness and have a possibility to use objects that have not been declared yet.
Every point stored in the collection keeps an iterator to the node, where was defined.

Only the measurements are kept directly in the tree nodes since they are really unique
to each level of the tree.
*/
struct TTreeEntry {
	/// Unique ID of a tree node.
  	std::vector<int> ID; 
 
	/// Returns TRUE if the node is a ROOT node
	bool isROOTNode() const{return frame.getName() == "ROOT";}

	/// There is exactly one transformation that led to this node.
	TAdjustableHelmertTransformation frame; 

	/// Whole collection of measurements per node.
	TMeasurements    measurements; 

	explicit TTreeEntry() : 
 			measurements(TMeasurements()), 
			frame(std::bitset<3>(std::string("111")), std::bitset<3>(std::string("111")),std::bitset<1>(1),"ROOT") /*ROOT node*/
			{} 

	~TTreeEntry() {}

};

#endif // LGC2_TTREE_ENTRY_H