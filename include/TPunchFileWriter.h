////////////////////////////////////////////////////////////////////
// TPunchFileWriter
/*!
Write an LGC output punch (summary) file
Creates a file from the calculation results and sends the appropriate messages

Patterns:

Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////////////////

#ifndef SU_TPUNCH_FILE_WRITER
#define SU_TPUNCH_FILE_WRITER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
class	TLGCDataSet;
class	TAStreamFormatter;
class	LSCalcDataSet;
class	TLGCProject;
#include "TAFileWriter.h"
#include "TPointConverter.h"
// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGC

@{*/

//Class definition
class  TPunchFileWriter : public TAFileWriter  
{
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TPunchFileWriter(TAStreamFormatter* stream, const TLGCProject* project);

			//!Destructor
			virtual  ~TPunchFileWriter();
		//@}


	/*!@name Public menber functions*/
		//@{
			//!write the lgc file used when there's no error in the project
			virtual void	writeFile(TLGCDataSet*, LSCalcDataSet*);

		//@}

private:

	/*!default constructor*/
	TPunchFileWriter();


	/*!@name Private member functions*/
	//@{
		//!write the file title
		void	writeTitle();

		//!write the points data
		void	writePoints();

		//!write the points XYZ data header
		void	writeXYZHeader();
		//! write the point XYZ coordinates
		void	writeXYZData(LSPosVecConstIter posVecIterator);

		//!write the points' XYH data header
		void	writeXYHHeader();
		//! write the points' XYH coordinates
		void	writeXYHData(LSPosVecConstIter posVecIterator);

		//! write the points' covariance and displacement header
		void	writeXYZVarCovarDeltaHeader();
		//! write the points' covariance and displacement data
		void	writeXYZVarCovarDeltaData(LSPosVecConstIter posVecIterator);

		//! write the points' coordinates, error ellipsoid parameters and displacement header
		void	writeXYZErrorEllSigZDeltaHeader();
		//! write the points' coordinates, error ellipsoid parameters and displacement data
		void	writeXYZErrorEllSigZDeltaData(LSPosVecConstIter posVecIterator);

		//! write the points' XYZ coordinates and covariance header
		void	writeXYZSigmaHeader();
		//! write the points' XYZ coordinates and covariance data
		void	writeXYZSigmaData(LSPosVecConstIter posVecIterator);

		//! write the points' XYZH coordinates header
		void	writeXYZHHeader();
		//! write the points' XYZH coordinates data
		void	writeXYZHData(LSPosVecConstIter posVecIterator);

		//! write the points' XYH coordinates and geoidal undulation header
		void	writeXYHNHeader();
		//! write the points' XYH coordinates and geoidal undulation data
		void	writeXYHNData(LSPosVecConstIter posVecIterator);

		//! write the points' XYZH coordinates and geoidal undulation header
		void	writeXYZHNHeader();
		//! write the points' XYZH coordinates and geoidal undulation data
		void	writeXYZHNData(LSPosVecConstIter posVecIterator);

	//@}


};

/*@}*/

#endif // SU_TPUNCH_FILE_WRITER