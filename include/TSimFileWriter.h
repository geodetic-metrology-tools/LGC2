////////////////////////////////////////////////////////////////////
// TSimFileWriter
/*!
Write an LGC "input" file with simulated values for observations

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////////////////

#ifndef SU_SIM_OBS_WRITER
#define SU_SIM_OBS_WRITER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
class	TLGCDataSet;
class	LSCalcDataSet;
class	TAStreamFormatter;
class	TLGCProject;
#include "TAFileWriter.h"
#include <string>
using namespace std;
// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGC

@{*/

//Class definition
class  TSimFileWriter : public TAFileWriter  
{
public:

	/*!@name Constructors and Destructors*/
		//@{
			//!constructor
			TSimFileWriter(TAStreamFormatter* stream, const TLGCProject* project);

			//!Destructor
			virtual  ~TSimFileWriter();
		//@}


	/*!@name Public member functions*/
		//@{
			//!write the lgc file used when there's no error in the project
			virtual void	writeSimulatedObsFile(TLGCDataSet*, TFileParameters, LSCalcDataSet*);
		//@}

private:

	/*!default constructor*/
	TSimFileWriter();

	/*!@name Private menber functions*/
		//@{
			/*!write spatial distance simulated observation line
			@param inputFile stream for the input file of the application*/
			void	writeSimSpaDist(TAStreamFormatter* inputFile);
		
			//!write horizontal angle simulated observation line
			void	writeSimHorAng(TAStreamFormatter* inputFile);

			//!write horizontal distance simulated observation line
			void	writeSimHorDist(TAStreamFormatter* inputFile);

			//!write vertical distance simulated observation line
			void	writeSimVertDist(TAStreamFormatter* inputFile, bool isDLEV);
			
			//!write zenital distance simulated observation line
			void	writeSimZenDist(TAStreamFormatter* inputFile);
			
			//!write offset to a spatial line simulated observation line
			void	writeSimOffsetToSpaLine(TAStreamFormatter* inputFile);

			//!write offset to a vertical plane simulated observation line
			void	writeSimOffsetToVerPlane(TAStreamFormatter* inputFile);

			//!write offset to a vertical line simulated observation line
			void	writeSimOffsetToVerLine(TAStreamFormatter* inputFile);

			//!write offset to a theodolite vertical line simulated observation line
			void	writeSimOffsetToTheoPlane(TAStreamFormatter* inputFile);

			//! write orientation simulated observation line
			void	writeSimGyroOrie(TAStreamFormatter* inputFile);
		//@}
	

private:

};

/*@}*/

#endif // SU_SIM_OBS_WRITER