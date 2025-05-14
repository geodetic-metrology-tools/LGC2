/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGCGLOBAL_H_
#define _LGCGLOBAL_H_

/*!
   \file
   \ingroup LGCProject
   Global definitions and includes shared among the classes.
*/

// STL
#include <cassert>
#include <stdexcept>
#include <string>
// External
#include <tree.hh>
// SURVEYLIB
#include <TLength.h>
// LGC
#include <Defaults.h>

#define assert3D(x) assert((x) >= 0 && (x) < 3)
#define assert4D(x) assert((x) >= 0 && (x) < 4)

/* Definition provided in TTreeEntry, only forward declaration */
struct TTreeEntry;
using TDataSPtr = std::shared_ptr<TTreeEntry>;

// Using the tree implementation
using TDataTree = tree<TDataSPtr>;
using TDataTreeIterator = TDataTree::iterator;
using Node = tree_node_<TDataSPtr>;

inline void zerofy(TReal &v)
{
	if (fabsq(v) < 2 * EPSILON)
		v = 0.0;
}

inline bool isZero(TReal v)
{
	return fabsq(v) < nullLimit;
}

namespace std
{
/// string to real
inline TReal stor(const std::string &s)
{
	return TReal(std::stod(s));
}
}; // namespace std

#endif
