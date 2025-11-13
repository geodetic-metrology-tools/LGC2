/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
