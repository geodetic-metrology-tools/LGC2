////////////////////////////////////////////////////////////////////
// TResultsFileWriter
/*!
Write an LGC output file
Creates a file from the calculation results and sends the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////////////////

#ifndef SU_TResultsFileWriter
#define SU_TResultsFileWriter

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
class	TLGCDataSet;
class	TAStreamFormatter;
class	LSCalcDataSet;
class	TLGCProject;
class   TLGCCalcRecord;
#include	"TAFileWriter.h"
// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGC

@{*/

//Class definition
class  TResultsFileWriter : public TAFileWriter  
{
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TResultsFileWriter(TAStreamFormatter* stream, const TLGCProject* project);

			//!Destructor
			virtual  ~TResultsFileWriter();
		//@}


	/*!@name Public menber functions*/
		//@{
			//! write the LGC results file corresponding to the given project
			virtual void	writeFile(TLGCProject &project);

			//!write the lgc file  when there is an error in the project
			virtual void	writeFile(TLGCDataSet*, const string error);


		//@}

protected:

	/*!default constructor*/
	TResultsFileWriter();

	/*!@name Protected menber functions*/
		//@{
			void initObsListNumber(TLGCDataSet* ds);

			/*! set LSCalcDataSet (used between 2 simulations)*/
			void setLSCalcDataSet(LSCalcDataSet*);

			//! write title
			void	writeTitle();

			//! Write a summary of the LGC input data
			void	writeDataSummary(TLGCProject &);

			//! Write the point data summary of the LGC input data
			void	writePointDataSummary(const string, const int) ;

			//! Write a given measurement data summary of the LGC input data
			void	writeMeasDataSummary(const string, const int) ;
			
			//! Write distance measurement data summary of the LGC input data
			void	writeDistMeasDataSummary(const string, const int) ;

			//!Write a summary of the LGC calculation data parameters
			void	writeCalcDataSummary();

			/*! write sigma0 a posteriori*/
			void	writeSigmaAPosteriori();

			/*! write sigma0 a posteriori from a calc record*/
			void	writeSigmaAPosteriori(const TLGCCalcRecord&);
			
			void	writeInstrumentsSummary(TLGCProject &project, InstrumentsCalcRecord* record);

			void	writeThodoliteObsSummary(stdext::hash_map<string, TLGCObsSummary*>* obsSummaries);
			void	writeEDMObsSummary(hash_map<string, TLGCObsSummary*>* obsSummaries);
			void	writeScaleObsSummary(hash_map<string, TLGCObsSummary*>* obsSummaries);
			void	writeLevelObsSummary(hash_map<string, TLGCObsSummary*>* obsSummaries);
									
			//! write calculated point results
			template <typename T>
			void	writePointsResults(const string &refSys, const TLGCDataSet& ds, const T& lsds)
			{
				TAStreamFormatter &stream = getStreamRef();

				int ptNumber = 0;
				bool changeStatus = true;

				PointConstIter pIter = ds.getWorkingPoints()->getPointsBeginIterator();
				LSPosVecConstIter iter = lsds.getPV(pIter->getPtName());
				TSpatialStatus::ESpatialStatus status = iter->getGlobalStatus();

				//write the point's list
				while(pIter!=ds.getWorkingPoints()->getPointsEndIterator())
				{
					//if the status changed, write header
					if(changeStatus == true)
					{
						changeStatus = false;

						if(status == TSpatialStatus::kCala)
							{ ptNumber = fNumFixedPoint;}
						if(status == TSpatialStatus::kVx)
							{ ptNumber = fNumVxPoint;}
						if(status == TSpatialStatus::kVy)
							{ ptNumber = fNumVyPoint;}
						if(status == TSpatialStatus::kVz)
							{ ptNumber = fNumVzPoint;}
						if(status == TSpatialStatus::kVxy)
							{ ptNumber = fNumVxyPoint;}
						if(status == TSpatialStatus::kVxz)
							{ ptNumber = fNumVxzPoint;}
						if(status == TSpatialStatus::kVyz)
							{ ptNumber = fNumVyzPoint;}
						if(status == TSpatialStatus::kVxyz)
							{ ptNumber = fNumVxyzPoint;}

						writeResultsPtsHeader(status, ptNumber, refSys);
					}

					//write point
					writeResultsPtsData(iter, status);
					stream<<endl;
					pIter++;

					//check if the status change
					if(pIter!=ds.getWorkingPoints()->getPointsEndIterator())
					{
						iter = lsds.getPV(pIter->getPtName());
						if(status != iter->getGlobalStatus())
						{
							changeStatus = true;
							status = iter->getGlobalStatus();
							stream<<endl<<endl;
						}
						else
						{
							changeStatus = false;
						}
					}
				}
				stream<<endl<<endl;
			}

			//! Write the observation results
			void	writeObsResults(LSCalcDataSet &data, TLGCProject& project);

			//! write an observation table title
			void	writeObsTitle(const string&, const int);

			
			void	writeEDMStations(LSCalcDataSet &data, TLGCProject& project);
			void	writeLevelStations(LSCalcDataSet &data, TLGCProject& project);
			void	writeTheoStationHeader(LSCalcDataSet& lsobs, TheodoliteStation* station);
			void	writeEDMStationHeader(EDMStation* station);
			void	writeLevelStationHeader(LevelStation* station);
			void	writeTheoStationROMHeader(LSCalcDataSet& lsobs, TheodoliteStation* station, TheodoliteStationROM* rom);
			void	writeTheodoliteStations(LSCalcDataSet &data, TLGCProject& project);
			void	writeP3D(const PolarROM* rom, LSCalcDataSet &data);
			void	writeHorAng(const HorizontalAngleROM* rom, LSCalcDataSet &data);
			void	writeZD(const ZenithDistanceROM* rom, LSCalcDataSet &data);
			void	writeHD(const HorizontalDistanceROM* rom, LSCalcDataSet &data);
			void	writeSD(const SpatialDistanceROM* rom, LSCalcDataSet &data);
			void	writeEDMSD(const EDMSpatialDistanceROM* rom, LSCalcDataSet &data);
			void	writeOTP(const OffsetToTheodolitePlaneROM* rom, LSCalcDataSet &data);

			//! write horizontal angle observation results 
			void writeHorAng(const LSCalcDataSet &);

			//! write zenithal distance observation results (ZENH: Instrument Height known)
			void writeZENH(const LSCalcDataSet &);

			//! write zenithal distance observation results (ZENI: Instrument Height unknown)
			void writeZENI(const LSCalcDataSet &);

			//! write horizontal distance observations results
			void writeHorDist(const LSCalcDataSet &);

			//! write vertical distance observation results
			void writeVertDist(const LSCalcDataSet &, TLGCProject&);

			//! write observation results for SPATIAL DISTANCE: DMES (Instrument Height known)
			void writeDMES(const LSCalcDataSet &);

			//! write observation results for SPATIAL DISTANCE: DTHE (Instrument Height unknown)
			void writeDTHE(const LSCalcDataSet &);

			//! write observation results for distance to a vertical line (ECVE)
			void writeOffsetToVerLine(const LSCalcDataSet &, TLGCProject&);

			//! write observation results for OFFSET TO TWO POINTS (ESCP SPATIAL LINE) 
			void writeOffsetToSpaLine(const LSCalcDataSet &, TLGCProject&);

			//! write observation results for OFFSET TO TWO POINTS (ECHO VERTICAL PLANE)
			void writeOffsetToVerPlane(const LSCalcDataSet &, TLGCProject&);

			//! write observation results for OFFSET TO THEODOLITE PLANE (ECTH)
			void writeOffsetToTheoPlane(const LSCalcDataSet &, TLGCProject&);

			//! write observation results for ORIENTATION (GYROSCOPE ORIE)
			void writeGyroOrie(const LSCalcDataSet &);

			//! write observation results for RADIAL OFFSET CONSTRAINT (RADI)
			void writeRadOffCnstr(const LSCalcDataSet &);

			//! write the summary statistics for an angle type observation set
			void    writeAngleObsSummary(TLGCObsSummary&, const string&);

			//! write the summary statistics for an length type observation set
			void    writeLengthObsSummary(TLGCObsSummary&, const string&);

			//! write relative errors results (EREL)
			void	writeRelativeErrors();

		//@}

private:
			
		void	writeResultsPtsData(LSPosVecConstIter posVecIterator,
									const TSpatialStatus::ESpatialStatus status);

		void	writeResultsPtsHeader(  const TSpatialStatus::ESpatialStatus status,
										const int ptNumber, const string &refSys);

		void	writeResidualsHistogram(TLGCObsSummary&, string);


private:

	/*!@name Private Attribute*/
	//@{

		//!number of each point's type and observation's type
		int								fNumFixedPoint;
		int								fNumVxPoint;
		int								fNumVyPoint;
		int								fNumVzPoint;
		int								fNumVxyPoint;
		int								fNumVxzPoint;
		int								fNumVyzPoint;
		int								fNumVxyzPoint;
		int								fNumPolar;
		int								fNumHorAng;
		int								fNumZenDist;
		int								fNumVertDist;
		int								fNumLevelObs;
		int								fNumDistMeas;
		int								fNumEDMDistMeas;
		int								fNumHorDist;
		int								fNumECVE;
		int								fNumECSP;
		int								fNumECHO;
		int								fNumECTH;
		int								fNumOrie;
		int								fNumRADI;

	//@}
};

/*@}*/

#endif // SU_TCSGEOFileWriter
