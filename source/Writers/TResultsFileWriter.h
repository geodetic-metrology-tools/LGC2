#ifndef SU_TResultsFileWriter
#define SU_TResultsFileWriter

#include "TAFileWriter.h"
#include "TRefSystemFactory.h"
#include "TTSTNWriter.h"
#include "TSpatialStatus.h"

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
			virtual void	writeFile(const string error);
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
			void	writePointDataSummary(const string, const int) ;

			///  Write a given measurement data summary of the LGC input data
			void	writeMeasDataSummary(const string, const int);
			
			///  Write distance measurement data summary of the LGC input data
			void	writeDistMeasDataSummary(const string, const int, const bool) ;

			/// Write a summary of the LGC calculation data parameters
			void	writeCalcDataSummary();

			/// Write sigma0 a posteriori
			void	writeSigmaAPosteriori();

			/// Write sigma0 a posteriori from a calc record
			void	writeSigmaAPosteriori(const TLGCData&);
									
			///  Write calculated point results
			void	writePointsResults();

			///  Write the observation results
			void	writeFramesResults();

			///  Write an observation table title
			void	writeObsTitle(const string&, const int);

			///  Write horizontal angle observation results 
			void writeHorAng();



///////////////// MINE PROTECTED FUNCTIONS ////////////////////////////////
			/// Write general information about the adjustable objects
			void writeAdjustableObjGeneralInfo(const std::string adjObjName, const int numAdj, const int numUnkn);
		//@}
private:

		void	writeResultsPtsHeader(  const TSpatialStatus::ESpatialStatus status,
										const int ptNumber, const string &refSys);

		void	writeResultsPtsData(const TAdjustablePoint* pt);

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
