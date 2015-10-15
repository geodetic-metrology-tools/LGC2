// TFautFileWriter.h

////////////////////////////////////////////////////////////////////
/*!
Write an LGC fault detection file, from the use of the keyword *FAUT
Creates a file from the data read and sends the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////////////////


#ifndef SU_FAUT_WRITER
#define SU_FAUT_WRITER


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Forward declarations
#include "TAFileWriter.h"
#include "TALSCalcLengthObservation.h"
#include "TLSCalcRadialOffsetCnstrObservation.h"
#include "TLGCProject.h"

#include <string>
using namespace std;

/*!\ingroup LGCppVCppProjects

@{*/

//! Class for a fault detection file writer
class TFautFileWriter : public TAFileWriter
{

public:
	/*@name Constructor / Destructor */
	//@{
		/*! Default constructor */
		TFautFileWriter(TAStreamFormatter* stream, const TLGCProject* project);


		/*! Destructor */
		virtual ~TFautFileWriter();
	//@}

	/*!@name Methods for writing reliability results */
	//@{
		/*! Write results or errors if some occured during reliability computation */
		void	writeResults(LSCalcDataSet &data, TLGCProject& project);
	//@}

private:
	
	/*!default constructor */
	TFautFileWriter();

	/*!@name Private functions for header part of the file */
	//@{
		/*! write title and date */
		void	writeTitle();
		/*! write data summary (sigma zero a posteriori, alpha, beta, and others) */
		void	writeDataSummary();
	//@}
		

		void	writeTheoStationHeader(TheodoliteStation* station);
		void	writeEDMStationHeader(EDMStation* station);
		void	writeTheoStationROMHeader(TheodoliteStationROM* rom);
		void	writeTheodoliteStations(LSCalcDataSet &data, TLGCProject& project);
		void	writeEDMStations(LSCalcDataSet &data, TLGCProject& project);

	/*!@name Concrete observations processing */
	//@{
		void	writePolarData(const PolarROM* rom, LSCalcDataSet &data);
		/*! Horizontal Angle Observations */
		void	writeHorAngData(const HorizontalAngleROM* rom, LSCalcDataSet &data);
		/*! Horizontal Distance Observations */
		void	writeHorDistObs(const HorizontalDistanceROM* rom, LSCalcDataSet &data);
		/*! Vertical distance observations */
		void	writeVertDistObs();
		/*! Zenithal distance observations */
		void	writeZenDistData(const ZenithDistanceROM* rom, LSCalcDataSet &data);
		/*! Spatial distance observations */
		void	writeSpaDistObs(const SpatialDistanceROM* rom, LSCalcDataSet &data);
		/*! EDM Spatial distance observations */
		void	writeEDMSpaDistObs(const EDMSpatialDistanceROM* rom, LSCalcDataSet &data);
		/*! ECVE observations */
		void	writeOffToVertLineObs();
		/*! ECSP observations */
		void	writeOffToSpaLineObs();
		/*! ECHO observations */
		void	writeOffToVertPlaneObs();
		/*! ECTH observations */
		void	writeOffToTheoPlaneObs(const OffsetToTheodolitePlaneROM* rom, LSCalcDataSet &data);
		/*! ORIE observations */
		void	writeGyroOrieData();
		/*! RADI observations */
		void	writeAllRadOffCnstrObs();
	//@}

	/*! Total reliability if possible */
	void	writeOverallReliability();

	/*!@name Private Attribute*/
	//@{
		TReal	fAlpha;	/*! level of significance, [0;1], default value : 1% */
		TReal	fBeta; /*! risk of having false values, [0;1], default value : 10% */
	//@}

};

/*@}*/

#endif //SU_FAUT_WRITER
