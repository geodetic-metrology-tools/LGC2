// TLGCFileReader.h
//
/* File reader for an LGC input data file

Patterns:
 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////



#ifndef SU_TLGCFileReader
#define SU_TLGCFileReader


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <**classname**>
//
//using namespace std;
//
class	TLGCProject;
class	TSeparatedFormatTStream;
class	TAStreamFormatter;

// typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
class  TLGCFileReader //: public TAConverter  
{
public:
	//constants

	/**@name Constructors and Destructors */
	//@{
		/**Constructor  
		@param project a pointer to the LGCProject where the read data will be stored*/
		explicit TLGCFileReader(TLGCProject* project);

		//! Destructor
		virtual  ~TLGCFileReader();
	//@}

	//!reads the LGC file
	virtual void	readFile();
	

private:
	/**@name private constructor and assignment operator */
	//@{
		/**Default Constructor (not implemented)*/ 
		TLGCFileReader();

		/*! Copy assignement operator (not implemented)*/
		TLGCFileReader& operator=(const TLGCFileReader&);
	//@}



	/**@name private member functions */
	//@{
		/**reacts to the reading of a keyword
		@param keyword the read keyword*/
		int	reactToKeyword(const string keywordLine, int lineNum);

		/*! method used for detecting a too large number of element on a line*/
		void endOfLineError(int i);

		/*! method used for detecting use of incompatible keyword for reference system definition*/
		void refSysError(int i);

		/*!reads a series of points (with different status)*/
		int	readPoints(TSpatialStatus::ESpatialStatus, int lineNum);

		/*!reads the title of the input file*/
		int	readTitle(int lineNum);

		/*!reads a orientation constraint (*PDOR)*/
		int	readOrieCnstr(int lineNum);

		/*! reads the precision to which the coordinates should be output*/ 
		int readCoordPrec(int lineNum);
		
		/*! reads the precision to which the observation values should be output*/
		int readObsPrec(int lineNum);

		/*! get the coordinate precision enumerator corresponding to an integer key*/
		TPointFormat::ECoordPrecision			getCoordPrecisionFromInt(const int i);

		/*! get the observation length precision enumerator corresponding to an integer key*/
		TObservationFormat::ELengthPrecision	getLengthPrecisionFromInt(const int i);

		/*! get the observation angle precision enumerator corresponding to an integer key*/
		TObservationFormat::EAnglePrecision		getAnglePrecisionFromInt(const int i);

		/*! read the type of punch file selected for the output of results */
		int	readPunchFileType(int lineNum);

		/*! read comment lines and increment the line number counter as necessary*/
		int	readCommentLines(int i);
	
	//@}


	TAStreamFormatter*				fSeparatedStream;
	TLGCProject*					fProject;

	bool							fRefFrameSet;

	int								fNumOfMeas;
	
};


#endif // SU_TLGCFileReader

