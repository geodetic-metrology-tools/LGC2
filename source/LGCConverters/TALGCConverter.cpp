#include	"TALGCConverter.h"
#include	"TAStreamFormatter.h"

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
//default constructor
TALGCConverter::TALGCConverter(): TAConverter()
{}

/*
TALGCConverter::TALGCConverter(TAStreamFormatter* stream):
TAConverter(stream)
{
}*/

TALGCConverter::TALGCConverter(TAStreamFormatter& stream):TAConverter(stream)
{
}


TALGCConverter::~TALGCConverter()
{//destructor
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
string	TALGCConverter::getKeyWord(TALGCConverter::ELGCObservations key)
{
	static const char* keywords[] = {
		"*PLR3D", "*UVEC", "*UVD", "*ANGL", "*ZEND", "*DIST", "*DHOR", 
		"*DLEV"
	};

	static_assert((sizeof(keywords)/sizeof(keywords[0])) == ALWAYS_LAST, "Not enough keyword strings for enums");

	return keywords[key];
}


void	TALGCConverter::writeKeyWord(const string &keyword)
{
	TAStreamFormatter& stream = this->getStreamRef();

	// Ouput the keyword to the stream
	//
	stream << keyword;
	stream << this->getSeparator();
	stream << endl;
}



