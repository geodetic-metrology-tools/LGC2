////////////////////////////////////////////////////////////////////
// TRelativeErrorConverter
/*!
Write Relative Erros for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_EREL_CONVERTER
#define SU_EREL_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
class		TLGCProject;
#include "TAConverter.h"
#include "TLSCalcRelativeError.h"

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/


//Class definition
class  TRelativeErrorConverter : public TAConverter
{

	
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TRelativeErrorConverter(TAStreamFormatter& stream);

			//!Destructor
			virtual  ~TRelativeErrorConverter();
		//@}

			void	writeResults(LSRelErrorConstIter	obsIt);

			void	writeResultsHeader();

			int		readObservation(int i, TLGCProject* project);

private:
			void	formatMesLine(	const long int id,
									const string namept1,
									const string namept2,
									const string namept3, 
									const TAngle bear,
									const TLength res, 
									const TLength sig, 
									const string com);

			TRelativeErrorConverter();
};

/*@}*/

#endif // SU_EREL_CONVERTER