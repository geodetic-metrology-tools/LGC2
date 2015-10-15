////////////////////////////////////////////////////////////////////
// TResultsFileWriter
/*!
Write an LGC output file.
Creates a file from the calculation results and sends the appropriate messages.
*/
/////////////////////////////////////////////////////////////////////

#ifndef SU_TResultsFileWriter
#define SU_TResultsFileWriter

////////////////////////////////////////////////////////////////
// Forward
#include	"TAFileWriter.h"
#include "Legacy.h"

#include "LGCConverters\TTstnConverter.h"

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
			TResultsFileWriter(TAStreamFormatter* stream, const TLGCData* project);

			//!Destructor
			virtual  ~TResultsFileWriter();
		//@}


	/*!@name Public member functions*/
		//@{
			//! write the LGC results file corresponding to the given project
			virtual void	writeFile();

			//!write the lgc file  when there is an error in the project
			virtual void	writeFile(const string error);
		//@}

protected:

	/*!default constructor*/
	TResultsFileWriter();

	/*!@name Protected menber functions*/
		//@{
			void initObsListNumber();

			/*! set LSCalcDataSet (used between 2 simulations)*/
			void setLSCalcDataSet(TLGCData*);

			//! write title
			void	writeTitle();

			//! Write a summary of the LGC input data
			void	writeDataSummary();

			//! Write the point data summary of the LGC input data
			void	writePointDataSummary(const string, const int) ;

			//! Write a given measurement data summary of the LGC input data
			void	writeMeasDataSummary(const string, const int);
			
			//! Write distance measurement data summary of the LGC input data
			void	writeDistMeasDataSummary(const string, const int, const bool) ;

			//!Write a summary of the LGC calculation data parameters
			void	writeCalcDataSummary();

			/*! write sigma0 a posteriori*/
			void	writeSigmaAPosteriori();

			/*! write sigma0 a posteriori from a calc record*/
			void	writeSigmaAPosteriori(const TLGCData&);
									
			//! write calculated point results
			void	writePointsResults();

			//! Write the observation results
			void	writeFramesResults();

			//! write an observation table title
			void	writeObsTitle(const string&, const int);

			//! write horizontal angle observation results 
			void writeHorAng();



///////////////// MINE PROTECTED FUNCTIONS ////////////////////////////////
			void writeAdjustableObjGeneralInfo(const std::string adjObjName, const int numAdj, const int numUnkn);
#if 0
			//! write zenithal distance observation results (ZENH: Instrument Height known)
			void writeZENH(const LSCalcDataSet &);

			//! write zenithal distance observation results (ZENI: Instrument Height unknown)
			void writeZENI(const LSCalcDataSet &);

			//! write horizontal distance observations results
			void writeHorDist(const LSCalcDataSet &);

			//! write vertical distance observation results (DVER/DLEV)
			void writeVertDist(const LSCalcDataSet &, bool isDLEV);

			//! write observation results for SPATIAL DISTANCE: DMES (Instrument Height known)
			void writeDMES(const LSCalcDataSet &);

			//! write observation results for SPATIAL DISTANCE: DTHE (Instrument Height unknown)
			void writeDTHE(const LSCalcDataSet &);

			//! write observation results for distance to a vertical line (ECVE)
			void writeOffsetToVerLine(const LSCalcDataSet &);

			//! write observation results for OFFSET TO TWO POINTS (ESCP SPATIAL LINE) 
			void writeOffsetToSpaLine(const LSCalcDataSet &);

			//! write observation results for OFFSET TO TWO POINTS (ECHO VERTICAL PLANE)
			void writeOffsetToVerPlane(const LSCalcDataSet &);

			//! write observation results for OFFSET TO THEODOLITE PLANE (ECTH)
			void writeOffsetToTheoPlane(const LSCalcDataSet &);

			//! write observation results for ORIENTATION (GYROSCOPE ORIE)
			void writeGyroOrie(const LSCalcDataSet &);

			//! write observation results for RADIAL OFFSET CONSTRAINT (RADI)
			void writeRadOffCnstr(const LSCalcDataSet &);

			//! write the summary statistics for an angle type observation set
			void    writeAngleObsSummary(const TLGCObsSummary&, const string&);

			//! write the summary statistics for an length type observation set
			void    writeLengthObsSummary(const TLGCObsSummary&, const string&);

			//! write relative errors results (EREL)
			void	writeRelativeErrors();
#endif

#if 0
			//! get the number of Fixed points
			int			getNumFixedPoint() const {return fNumFixedPoint;}
			//! get the number of variable X-coordinate points
			int			getNumVxPoint() const {return fNumVxPoint;}
			//! get the number of variable Y-coordinate points
			int			getNumVyPoint() const {return fNumVyPoint;}
			//! get the number of variable Z-coordinate points
			int			getNumVzPoint() const {return fNumVzPoint;}
			//! get the number of variable XY-coordinate points
			int			getNumVxyPoint() const {return fNumVxyPoint;}
			//! get the number of variable XZ-coordinate points
			int			getNumVxzPoint() const {return fNumVxzPoint;}
			//! get the number of variable YZ-coordinate points
			int			getNumVyzPoint() const {return fNumVyzPoint;}
			//! get the number of variable XYZ-coordinate points
			int			getNumVxyzPoint() const {return fNumVxyzPoint;}
			//! get the number of ABLG observations
			int			getNumHorAng() const {return fNumHorAng;}
			//! get the number of ZENH observations
			int			getNumZenDistFixed() const {return fNumZenDistFixed;}
			//! get the number of ZENI observations
			int			getNumZenDistVar() const {return fNumZenDistVar;}
			//! get the number of DVER/DLEV observations
			int			getNumVertDist(bool isDLEV) const {return isDLEV?fNumDlevDist:fNumVertDist;}
			//! get the number of DMES observations
			int			getNumDistMeas() const {return fNumDistMeas;}
			//! get the number of DTHE observations
			int			getNumDistTheo() const {return fNumDistTheo;}
			//! get the number of DHOR observations
			int			getNumHorDist() const {return fNumHorDist;}
			//! get the number of ECVE observations
			int			getNumECVE() const {return fNumECVE;}
			//! get the number of ECSP observations
			int			getNumECSP() const {return fNumECSP;}
			//! get the number of ECHO observations
			int			getNumECHO() const {return fNumECHO;}
			//! get the number of ECTH observations
			int			getNumECTH() const {return fNumECTH;}
			//! get the number of ORIE observations
			int			getNumOrie() const {return fNumOrie;}
			//! get the number of RADI observations
			int			getNumRADI() const {return fNumRADI;}

		//@}
#endif
		//@}
private:
#if 0			
		void	writeResultsPtsData(LSPosVecConstIter posVecIterator,
									const TSpatialStatus::ESpatialStatus status);

		void	writeResultsPtsHeader(  const TSpatialStatus::ESpatialStatus status,
										const int ptNumber, const string &refSys);

		void	writeResidualsHistogram(TLGCObsSummary&, string);
#endif

		void	writeResultsPtsHeader(  const TSpatialStatus::ESpatialStatus status,
										const int ptNumber, const string &refSys);

		void	writeResultsPtsData(const TAdjustablePoint* pt);

private:

	/*!@name Private Attribute*/
	//@{

		//!number of each point's type and observation's type

	void transfUsingGeoid(TPositionVector& pv, const TLGCRefFrame::ERefs& rf);


		size_t							fNumberOfPoints;
		size_t							fNumberOfUnknOfPoints;
		bool withEllipses;

	//@}
};

/*@}*/

#endif // SU_TCSGEOFileWriter
