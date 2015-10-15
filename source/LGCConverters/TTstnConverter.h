////////////////////////////////////////////////////////////////////
// TTstnConverter
/*!
	Write TSTN  definition and its observations to an LGC output file.
*/
////////////////////////////////////////////////////////////////////////////////////
#ifndef SU_TSTN_CONVERTER
#define SU_TSTN_CONVERTER

////////////////////////////////////////////////////////////////
// Forward declarations
#include "Measurements/TTSTN.h"
#include "TObservationConverter.h"
////////////////////////////////////////////////////////////////


//Class definition
class  TTstnConverter : public TObservationConverter 
{
public:
	//! Constructor
	TTstnConverter(TAStreamFormatter& stream);
	//!Destructor
	virtual ~TTstnConverter();

	/*!
		Writes information about TSTN, its definition, calculated values and each ROM (round of measurements) as well as the observations included.
	*/
	void writeTSTNResults(const TTSTN& tstn);

	//! Writes TSTN header.
	void writeTSTNHeader(const TTSTN& tstn);
	//! Writes TSTN data, definition, provisional and calculated values.
	void writeTSTNData(const TTSTN& tstn);
	
	//! Writes V0 (ROM = round of measurement) header.
	void writeV0Header();
	//! Writes V0 (ROM = round of measurement) definition.
	void writeV0Data(const TTSTN::TROM& rom);

	//! Not yet implemented
	void	writeReliabilityData(const TTSTN& fTstn);
	//! Not yet implemented
	void	writeReliabilityHeader();
private:
	/*!@name Headers */
	//@{
		void writeANGLResultsHeader(int nOObs);
		void writeZENDResultsHeader(int nOObs);
		void writeDISTResultsHeader(int nOObs);
		void writePLRResultsHeader(int nOObs);
		//void writeDIRResultsHeader(int nOObs);
		void writeDHORResultsHeader(int nOObs);
		//! Not yet implemented
		void writeECTHResultsHeader(int nOObs);
	//@}

	/*!@name Results */
	//@{
		void writeANGLResults(const std::vector<TANGL>& measANGL);
		void writeZENDResults(const std::vector<TZEND>& measZEND);
		void writeDISTResults(const std::vector<TLINE>& measDIST);
		void writePLRResults(const std::vector<TPLR3D>& measPLR3D);
	//	void writeDIRResults(const std::vector<TDIR3D>& measDIR);
		void writeDHORResults(const std::vector<TLINE>& measDHOR);
		//! Not yet implemented
	//	void writeECTHResults(const TTSTN& fTstn);
	//@}
};



#endif //SU_TSTN_CONVERTER