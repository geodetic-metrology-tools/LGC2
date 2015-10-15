#include "TLevelConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"

TLevelConverter::TLevelConverter(TAStreamFormatter& stream):TObservationConverter(stream)
{}

TLevelConverter::~TLevelConverter(){}

#if 0
void TLevelConverter::writeReliabilityData(const TLEVEL& fLevel)
{
TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResidualPrecision = getAngleResidualPrecision();
	string				separator = getSeparator();

	/*
	for(auto const& rom : fStn.roms)
		for(auto const& a : rom.measANGL)
			cout << a.

	for(int i=0 ; i<fStn.roms.size() ; i++)
		for(int j=0 ; j<fStn.roms[i].measANGL.size() ; j++)
			fStn.roms[i].measANGL[j].
	*/


}
#endif