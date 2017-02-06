#ifndef _LGCGLOBAL_H_
#define _LGCGLOBAL_H_

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.


/*!
   \file
   \ingroup LGCProject
   Global definitions and includes shared among the classes.
*/

//STL
#include <string>
#include <cassert>
#include <stdexcept>
//SURVEYLIB
#include <TLength.h>
//LGC
#include <Defaults.h>
#include <tree.h>



#define assert3D(x) assert((x) >= 0 && (x) < 3)
#define assert4D(x) assert((x) >= 0 && (x) < 4)


/* Definition provided in TTreeEntry, only forward declaration */
struct TTreeEntry;
typedef std::shared_ptr<TTreeEntry> TDataSPtr;
// Using the tree implementatin from http://tree.phi-sci.com (stored in tree.h) 
// but manually added the namspace peka (had none before) derived from author's name.
typedef peka::tree<TDataSPtr> TDataTree;
typedef TDataTree::iterator TDataTreeIterator;
typedef peka::tree_node_<TDataSPtr>  Node;


inline void zerofy(TReal& v) {
		if (fabsq(v) < 2*EPSILON)
			v = 0.0;
}

inline bool isZero(TReal v) {
		return fabsq(v) < nullLimit;
}

namespace std
{
	/// string to real
	inline TReal stor(const std::string& s) {
	   return TReal(std::stod(s));
	}
};


#endif
