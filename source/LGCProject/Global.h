#ifndef _LGCGLOBAL_H_
#define _LGCGLOBAL_H_

/*!
   \file
   \ingroup LGCProject
   Global definitions and includes shared among the classes.
*/

#include <string>
#include <cassert>
#include <stdexcept>
#include "Defaults.h"
#include "tree.h"
#include "Quad.h"
#include "TLength.h"


#define assert3D(x) assert((x) >= 0 && (x) < 3)
#define assert4D(x) assert((x) >= 0 && (x) < 4)

#ifndef powq
#define powq(x,y) pow((x),(y)) 
#endif
#ifndef pow2q
#define pow2q(x) pow2(x) 
#endif
#ifndef fabsq
#define fabsq(x) fabs(x) 
#endif
#ifndef sqrtq
#define sqrtq(x) sqrt(x)
#endif


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

namespace LGC {

	static const TReal VECCONV = 0.001;
	static const TReal VECCONVINV = 1000;
	
	/// Access coordinate axes or rotations around axes by their index
	enum {X, Y, Z, H=2};
}

#endif
