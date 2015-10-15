////////////////////////////////////////////////////////////////////
// TSpatialDistConverter.cpp
/*!
Write Spatial Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TSpatialDistConverter.h"


void	TSpatialDistConverter::writeResultsHeader(bool hasOwnInstrumentHeight)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT"); //second point's Name
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured distance
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated distance
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (mm)
	(*stream).writeString(obsResWidth,	"RES/SIG"); //offset/sigma
	(*stream).writeString(obsResWidth,	"SENSI"); //sensitivity
	(*stream).writeString(obsWidth,	"HAUTEUR"); //prism's height 
	(*stream).writeString(obsResWidth,	"CONST"); //distance's constant 
	(*stream).writeString(obsResWidth,	"SCONST"); //sigma distance's constant 
	if (hasOwnInstrumentHeight)
	{
		(*stream).writeString(obsWidth,	"IH"); //instrument height
	}
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"(M)"); //mesured distance
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma
	(*stream).writeString(obsWidth,	"(M)"); //estimated distance
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	(*stream).writeString(obsResWidth,	""); //offset/sigma
	(*stream).writeString(obsResWidth,	"BILITE"); //sensitivity
	(*stream).writeString(obsWidth,	"DE PRISM"); //prism's height 
	(*stream).writeString(obsResWidth,	"(MM)"); //distance's constant 
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma distance's constant 
	if (hasOwnInstrumentHeight)
	{
		(*stream).writeString(obsWidth,	"(M)"); //instrument height
	}
	(*stream)<<endl;
	
	///////////////////////////////////////////////////////////////////////////////////
	//Third line
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	""); //mesured distance
	(*stream).writeString(obsResWidth,	""); //sigma
	(*stream).writeString(obsWidth,	""); //estimated distance
	(*stream).writeString(obsResWidth,	""); //offset (mm)
	(*stream).writeString(obsResWidth,	""); //offset/sigma
	(*stream).writeString(obsResWidth,	"(MM/CM)"); //sensitivity
	(*stream).writeString(obsWidth,	"(M)"); //prism's height 
	(*stream)<<endl<<endl;
	
	return;
}


/////////////////////////////////////////////////////////////////////////
//RELIABILITY
/////////////////////////////////////////////////////////////////////////


void	TSpatialDistConverter::writeReliabilityHeader()
{
	this->TObservationConverter::writeReliabilityHeader("TARGET",
														"OBSERVE",
														"M",
														"MM");
	return;
}

