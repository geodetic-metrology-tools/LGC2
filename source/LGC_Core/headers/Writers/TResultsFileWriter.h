/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_TResultsFileWriter
#define SU_TResultsFileWriter

//SURVEYLIB
#include <TRefSystemFactory.h>
#include <TSpatialStatus.h>
//LGC
#include <TAFileWriter.h>

class TPositionVector;

/*!
	\ingroup Writers
	\brief Write a standard LGC output file.

	Creates a file from the calculation results and sends the appropriate messages.
*/
class  TResultsFileWriter : public TAFileWriter  
{
public:

	/*!@name Constructors and Destructors*/
		//@{
			/// constructor
			TResultsFileWriter(TAStreamFormatter* stream, const TLGCData* project);

			/// Destructor
			virtual  ~TResultsFileWriter();
		//@}


	/*!@name Public member functions*/
		//@{
			///  write the LGC results file corresponding to the given project
			virtual void	writeFile();

			/// write the lgc file  when there is an error in the project
			virtual void	writeFile(const std::string error);
		//@}

protected:

	/*!default constructor*/
	TResultsFileWriter();

	/*!@name Protected menber functions*/
		//@{
			void initObsListNumber();

			///  write title
			void	writeTitle();

			///  Write a summary of the LGC input data
			void	writeDataSummary();

			///  Write the point data summary of the LGC input data
			void	writePointDataSummary(const std::string, const int) ;

			///  Write a given measurement data summary of the LGC input data
			void	writeMeasDataSummary(const std::string, const int);

			/// Write a summary of the LGC calculation data parameters
			void	writeCalcDataSummary();

			/// Write sigma0 a posteriori
			void	writeSigmaAPosteriori();

			/// Write sigma0 a posteriori from a calc record
			void	writeSigmaAPosteriori(const TLGCData&);

			///  Write the observation results
			void	writeFramesResults();

			///  Write the relative errors header
			void writeRelErrorHeader();
			/// Write the relative errors results
			void writeRelErrorResults(const TLGCData&);

			/// Write general information about the adjustable objects
			void writeAdjustableObjGeneralInfo(const std::string adjObjName, const int numAdj, const int numUnkn);
		//@}

private:

	/*!@name Private Attribute*/
	//@{

		/// number of each point's type and observation's type

	void transfUsingGeoid(TPositionVector& pv, const TRefSystemFactory::ERefFrame& rf);


		size_t							fNumberOfPoints;
		size_t							fNumberOfUnknOfPoints;
		bool withEllipses;

	//@}
};

/*@}*/

#endif // SU_TCSGEOFileWriter
