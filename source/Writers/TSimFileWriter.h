#ifndef SU_SIM_OBS_WRITER
#define SU_SIM_OBS_WRITER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class	TLGCProject;
#include "TAFileWriter.h"
#include <string>
using namespace std;


/*!\ingroup LGC

@{*/

//Class definition
class  TSimFileWriter : public TAFileWriter  
{
public:

	/*!@name Constructors and Destructors*/
		//@{
			/// constructor
			TSimFileWriter(TAStreamFormatter* stream, const TLGCData* project);

			/// Destructor
			virtual  ~TSimFileWriter();
		//@}


	/*!@name Public member functions*/
		//@{
			/// write the lgc file used when there's no error in the project
			virtual void	writeFile();

			/// write the lgc file  when there is an error in the project
			virtual void	writeFile(const string error);
		//@}

private:

	/*!default constructor*/
	TSimFileWriter();

	void writeHeader();
	void writeInstrument();
	void writeData(TDataTreeIterator frameIt);

	void writeFrameHeader(TDataTreeIterator frameIt);
	void writePoint(TDataTreeIterator frameIt);
	void writeMeasurement(TDataTreeIterator frameIt);

	void writeCAMMeas(TCAM* meas);
	void writeDVERMeas(TDVER* meas);
	void writeECHOMeas(TECHOROM* meas);
	void writeECVEMeas(TECVEROM* meas);
	void writeEDMMeas(TEDM* meas);
	void writeLEVELMeas(TLEVEL* meas);
	void writeORIEMeas(TORIEROM* meas);
	void writeRADIMeas(TRADI* meas);
	void writeTSTNMeas(TTSTN* meas);


	/// Project data
	const TLGCData* data;
};

/*@}*/

#endif // SU_SIM_OBS_WRITER
