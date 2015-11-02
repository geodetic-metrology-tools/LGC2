////////////////////////////////////////////////////////////////////
// TFautFileWriter.cpp :Implementation file
// Write an LGC fault detection file, from the use of the keyword *FAUT
////////////////////////////////////////////////////////////////////////////////////

#include "TFautFileWriter.h"

#include	<ctime>
#include	"TLGCData.h"
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
void	TFautFileWriter::writeFile(TLGCData* ds)
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
}

void TFautFileWriter::writeFile(const string error)
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

	//write time
	char tmpbuf[128];
	time_t ltime;
	time(&ltime);
	struct tm *today;
#ifdef __linux__
	tzset();
#else
	_tzset();
#endif
	today = localtime( &ltime );
	string essai = ctime( &ltime );
	strftime( tmpbuf, 128,"CALCULATED %d %B %Y %X", today );
	(*stream)<<tmpbuf<<endl;
	(*stream)<<"*********************************************************************************************************************************** "<<endl<<endl<<endl<<endl;
}

void	TFautFileWriter::writeDataSummary()
{
	TAStreamFormatter* stream = getStream();
	string separator = getSeparator();

	TReal S0Aposteriori = fProjectData->getS0APosteriori();
	TReal S0LowLimit = fProjectData->getChiS0LowLimit();
	TReal S0UpLimit = fProjectData->getChiS0UpLimit();


	//SIGMA ZERO A POSTERIORI
	stream->precision(5);
	stream->width( stream->getObsFormat()->getObsResidualWidth() );
	(*stream)<<"SIGMA ZERO A POSTERIORI ="<<S0Aposteriori;

	(*stream)<<", VALEUR CRITIQUE = (";
	stream->precision(5);
	stream->width( stream->getObsFormat()->getObsResidualWidth() );
	(*stream)<<S0LowLimit<<", ";

	stream->precision(5);
	stream->width( stream->getObsFormat()->getObsResidualWidth() );
	(*stream)<<S0UpLimit<<")";
	(*stream)<<endl<<endl;

	//SIGNIFICANCE LEVEL
	(*stream) << "SIGNIFICANCE LEVEL FOR TESTING WI, ALPHA =";
	stream->setf(ios::fixed,ios::floatfield);
	stream->width(5);
	stream->precision(1);
	(*stream) << right << fAlpha << " %" << separator;
	(*stream) << "OR EXPRESSED AS CONFIDENCE LEVEL, (1-ALPHA) = ";
	stream->width(5);
	stream->precision(1);
	(*stream) << right << (100-fAlpha) << " %";
	(*stream)<< endl << endl;
	
	// POWER OF TEST (1-beta)
	(*stream) << "POWER OF TEST TO DETERMINE NABLA AND DELTY, (1-BETA) = ";
	stream->width(5);
	stream->precision(1);
	(*stream) << right << (100-fBeta) << " %";
	(*stream)<< endl << endl << endl << endl;

}

void	TFautFileWriter::writeOverallReliability(TLGCData* project)
{
	TAStreamFormatter* stream =	getStream();
	string separator = getSeparator();

	TDouble F(project->getStatistics().getOVERALL());

	if (F.getStatus()!=TVNumericValue::kNull)
	{
		(*stream) << "* * * OVERALL NETWORK RELIABILITY FACTOR: " << separator;
		stream->width(stream->getObsFormat()->getObsResidualWidth());
		stream->precision(4);
		(*stream) << right << F.getValue() << " * * *";
	}
	else 
	{
		(*stream) << "* * * INDETERMINATE OVERALL NETWORK RELIABILITY FACTOR * * *";
	}
	(*stream) << endl << endl;
}

