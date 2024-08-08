// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

////////////////////////////////////////////////////////////////////
// TFautFileWriter.cpp :Implementation file
// Write an LGC fault detection file, from the use of the keyword *FAUT
////////////////////////////////////////////////////////////////////////////////////

#include "TFautFileWriter.h"

#include	<ctime>
#include	<TLGCData.h>
#include	"TScalar.h"
#include	"TSeparatedFormatTStream.h"
#include "TLGCApp.h"
#include "TFRAMEWriter.h"


/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TFautFileWriter::TFautFileWriter() : TAFileWriter()
{// default constructor
	fAlpha = LITERAL(0.01);
	fBeta = LITERAL(0.1);
}

TFautFileWriter::TFautFileWriter(TAStreamFormatter* stream, const TLGCData* project) :
TAFileWriter(stream, project)
{//constructor
	fAlpha = project->getConfig().faut.alpha;
	fBeta = project->getConfig().faut.beta;
}

TFautFileWriter::~TFautFileWriter()
{// Destructor
}


//////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC: WRITING RESULTS
//////////////////////////////////////////////////////////////////////////////////////////
void	TFautFileWriter::writeFile(TLGCData const * const ds)
{

	TAStreamFormatter*	stream = getStream();
	TFRAMEWriter frameWriter(*stream, fProjectData);

	// write headers
	this->writeTitle();
	this->writeDataSummary();

	//Tteration through the tree nodes
	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++){	
		frameWriter.writeFRAMEAllReliability(itTree);	//Writes 
	}

	this->writeOverallReliability(ds);
	this->writeNetworkDOF(ds);
}

void TFautFileWriter::writeFile(const std::string error)
{//write error messages from project
	writeTitle();
	writeError(error);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE: TITLE
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TFautFileWriter::writeTitle()
{
	TAStreamFormatter* stream =	getStream();

	//write main title
	//write software id.
	(*stream)<<(TLGCApp::getProgId())<<endl;

	//write software copyright
	(*stream)<<(TLGCApp::getCopyright())<<endl;

	//last compilation
	(*stream)<<"Last compilation : "<<__DATE__<<endl<<endl<<endl;

	//write title
	(*stream)<<"*********************************************************************************************************************************** "<<endl;
	(*stream)<<(fProjectData->getConfig().title)<<endl;
	(*stream)<<"RELIABILITY PARAMETERS"<<endl<<endl;
	(*stream)<<endl;

	// write date and time
	(*stream) << "CALCULATION OF " << TLGCApp::getStartProcessingTimestamp() << ". PROCESSING ELAPSED SECONDS " << TLGCApp::getProcessingElapsedSeconds() << endl;
	(*stream)<<"*********************************************************************************************************************************** "<<endl<<endl<<endl<<endl;
}

void	TFautFileWriter::writeDataSummary()
{
	TAStreamFormatter* stream = getStream();
	std::string separator = getSeparator();

	TReal S0Aposteriori = fProjectData->getS0APosteriori();
	TReal S0LowLimit = fProjectData->getChiS0LowLimit();
	TReal S0UpLimit = fProjectData->getChiS0UpLimit();


	//A POSTERIORI SIGMA ZERO
	stream->precision(5);
	stream->width( stream->getObsFormat()->getObsResidualWidth() );
	(*stream)<<"A POSTERIORI SIGMA ZERO ="<<S0Aposteriori;

	(*stream)<<", CRITICAL VALUE = (";
	stream->precision(5);
	stream->width( stream->getObsFormat()->getObsResidualWidth() );
	(*stream)<<S0LowLimit<<", ";

	stream->precision(5);
	stream->width( stream->getObsFormat()->getObsResidualWidth() );
	(*stream)<<S0UpLimit<<")";
	(*stream)<<endl<<endl;

	//SIGNIFICANCE LEVEL
	(*stream) << "SIGNIFICANCE LEVEL FOR TESTING WI, ALPHA =";
	stream->setf(std::ios::fixed, std::ios::floatfield);
	stream->width(5);
	stream->precision(1);
	(*stream) << right << fAlpha * 100 << " %" << separator;
	(*stream) << "OR EXPRESSED AS CONFIDENCE LEVEL, (1-ALPHA) = ";
	stream->width(5);
	stream->precision(1);
	(*stream) << right << (100 - (fAlpha * 100)) << " %";
	(*stream)<< endl << endl;
	
	// POWER OF TEST (1-beta)
	(*stream) << "POWER OF TEST TO DETERMINE NABLA AND DELTY, (1-BETA) = ";
	stream->width(5);
	stream->precision(1);
	(*stream) << right << (100 - (fBeta * 100)) << " %";
	(*stream)<< endl << endl << endl << endl;

}


void	TFautFileWriter::writeOverallReliability(TLGCData const* const project)
{
	TAStreamFormatter* stream = getStream();
	std::string separator = getSeparator();

	TDouble F(project->getStatistics().getOVERALL());

	if (!isnan(F.getValue()))
	{
		(*stream) << "\n\n" << "* * * OVERALL NETWORK RELIABILITY FACTOR: " << separator;
		stream->width(stream->getObsFormat()->getObsResidualWidth());
		stream->precision(4);
		(*stream) << right << F.getValue() << " * * *";
	}
	else
	{
		(*stream) << "\n\n" << "* * * INDETERMINATE OVERALL NETWORK RELIABILITY FACTOR * * *";
	}
}


void	TFautFileWriter::writeNetworkDOF(TLGCData const* const project)
{
	TAStreamFormatter* stream = getStream();
	std::string separator = getSeparator();

	TDouble F(project->getStatistics().getDOF());

	(*stream) << "\n\n" << "* * * NETWORK DEGREES OF FREEDOM: " << separator;
	stream->width(stream->getObsFormat()->getObsResidualWidth());
	stream->precision(0);
	(*stream) << right << F.getValue() << " * * *";

	(*stream) << endl << endl;
}
