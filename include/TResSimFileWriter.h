////////////////////////////////////////////////////////////////////
// TResSimFileWriter
/*!
Write an LGC output file for the results of a simulation calculation
Creates a file from the calculation results and sends the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////////////////

#ifndef SU_RESSIM_WRITER
#define SU_RESSIM_WRITER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
class	TLGCDataSet;
class	TAStreamFormatter;
class	LSCalcDataSet;
class	TLGCSimResults;
class	TLGCProject;
#include	"TResultsFileWriter.h"
#include	"TLGCSimPointSummary.h"



// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGC

@{*/

//Class definition
class  TResSimFileWriter : public TResultsFileWriter
{
public:

	using TAFileWriter::writeFile;

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TResSimFileWriter(TAStreamFormatter* stream, const TLGCProject* project);

			//!Destructor
			virtual  ~TResSimFileWriter();
		//@}


	/*!@name Public member functions*/
		//@{
			//! write the LGC results file corresponding to the given project
			virtual void	writeFile(TLGCProject &project);

		//@}



private:

	/*!default constructor*/
	TResSimFileWriter();

	/*!@name Private member functions*/
	//@{
		/*! set LSCalcDataSet (used between 2 simulations)*/
		//void setLSCalcDataSet(LSCalcDataSet*);
		/*!write the part of the lgc file corresponding all the simulations apart from the last*/
		virtual void	writeSimSummaries(TLGCProject&);
		
		/*!write the part of lgc file corresponding to the last simualtion
		used when there's no error in the project*/
		virtual void	writeLastSimResult(TLGCProject&);
		
		/*! write summary for the point's coordinates calculated in the simulation calculations
		@param projTitle title of the project calculation
		@param i number of simulations */
		void	writeSimPointsSummary(const string &projTitle, const LSCalcDataSet& lsds, int i);

		/*! write simulation calculation header
		@param i simulation number */
		void	writeSimHeader(TLGCCalcRecord &calcRec, int i);

		//!write point's data from simualtions
		void	writeSimPointData(SimPointConstIter iter, const int i);
								
	//@}

};

/*@}*/

#endif // SU_RESSIM_WRITER
