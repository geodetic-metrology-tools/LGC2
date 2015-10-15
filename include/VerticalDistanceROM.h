//
// VerticalDistanceROM.h : header file
/*! 

    Patterns:
  
    Copyright 2003 CERN EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////


#ifndef SU_VERTICAL_DISTANCE_ROM
#define SU_VERTICAL_DISTANCE_ROM

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

#include "ObservationROM.h"
#include "TVerticalDistMeasurement.h"
#include "Level.h"

class VerticalDistanceROM : public ObservationROM<TVerticalDistMeasurement>
{ 

public:

	VerticalDistanceROM(const Level* l)
	{
		level = l;
	}

	const Level* getLevel() const { return level; }

private:

	const Level* level;

};


#endif

