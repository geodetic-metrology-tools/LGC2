////////////////////////////////////////////////////////////////////
// TCamConverter
/*!
Write the TSTN instrument for an LGC output file

*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_CAM_CONVERTER
#define SU_CAM_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "Measurements/TCAM.h"
#include "TObservationConverter.h"
////////////////////////////////////////////////////////////////

//Class definition
class  TCamConverter : public TObservationConverter 
{
public:
	//! Constructor
	TCamConverter(TAStreamFormatter& stream);
	//!Destructor
	virtual ~TCamConverter();
	/*!
		Writes information about CAM, its definition, calculated values and all the observations included.
	*/
	void writeCAMResults(const TCAM& camera);

	//! Writes CAM header.
	void writeCAMHeader(const TCAM& camera);

	//! Writes CAM data, definition.
	void writeCAMData(const TCAM& camera);

private:
	/*!@name Headers */
	//@{
		void writePLRResultsHeader(int nOObs);
		void writeDIRResultsHeader(int nOObs);
	//@}

	/*!@name Results */
	//@{
		void writePLRResults(const std::vector<TPLR3D>& measPLR3D);
		void writeDIRResults(const std::vector<TDIR3D>& measDIR);
	//@}
	
	
	void	writeReliabilityData(const TCAM& fCam);
	void	writeReliabilityHeader();
	
};

#endif //SU_CAM_CONVERTER