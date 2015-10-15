// TFileParameters.cpp
//
// A class defining the parameters of a data file.
//       filename, filetype, reference frame, coordinate system
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include	<cstdio>
#include	<sstream>
#include	"TFileParameters.h"
////////////////////////////////////////////////////////////////


//ClassImp(TFileParameters)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TFileParameters::TFileParameters()
        : fName(""), fPath(""), fType( kCSGEOFile ), fFormat( kFreeFormat )
{	// default constructor
}


TFileParameters::TFileParameters( const  TFileParameters& original )
{	// copy constructor

	*this = original;
}


TFileParameters::~TFileParameters()
{
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


TFileParameters&  TFileParameters::operator=( const TFileParameters& rhs )
{	// Copy Assignment operator

	if (this != &rhs)
	{
		fName = rhs.getName();
		fPath = rhs.getPath();
		fType = rhs.getType();
		fFormat = rhs.getFormat();
	}
	return *this;
}

/*
bool  TFileParameters::setFileName( const string& filename )
{	// Set the path and filename using the combined string
	// returns "true" if the parameters are set correctly
	int  i;
	bool retVal = true;

	// make sure the filename is not empty
	if( !filename.empty() )
	{
		//find the end of the path
		//i = filename.find_last_of( '/' );
		i = filename.find_last_of( '\\' );

		if( string::npos == i )
		{	
			i = filename.find_last_of( '/' );
		}


		if( string::npos == i )
		{	// filename has no path component

			// assign the filename
			fName = filename;
		}
		else
		{
			// assign the path and filename
			fPath = filename.substr(0, i);
			fName = filename.substr(i+1);
		}
	}
	else
	{
		fName = filename;
		retVal = false;
	}

	return retVal;
}*/




bool  TFileParameters::setFileName( const std::string& filename )
{	// Set the filename using the combined string
	// returns "true" if the parameters are set correctly
	fName = filename;
	return filename.empty() ? false : true;
#if 0
	bool retVal = true;

	// make sure the filename is not empty
	if( !filename.empty() )
	{
		fName = filename;
		retVal = true;
	}
	else
	{
		fName = filename;
		retVal = false;
	}

	return retVal;
#endif
}



bool  TFileParameters::setFilePath( const std::string& path )
{	// Set the path using the combined string
	// returns "true" if the parameters are set correctly
	bool retVal = true;

	// make sure the filename is not empty
	if( !path.empty() )
	{
		fPath = path;
		retVal = true;
	}
	else
	{
		fPath = path;
		retVal = false;
	}

	return retVal;
}














bool  TFileParameters::setTypeFormat( const std::string& str )
{	// Set the filetype from the file filter
	// returns "true" if the parameters are set correctly
	
	bool retVal = true;

	fType = toType( str );
	fFormat = toFormat( str );

	if( fType == kNull  ||  fFormat == kNullFormat )
	{
		retVal = false;
	}

	return retVal;
}


//! get the full filename, both path and name
std::string  TFileParameters::getFileName()  const
{ 
	std::string fn;
	
	if( fPath.empty() )
	{
		fn = fName;
	}
	else
	{
		std::stringstream path ;//(fPath, ios_base::out);
		path << fPath;
		char c = '/';
		bool qt = false;
		while(path.peek() != EOF)
		{
			char a = (char) path.get();
			if(a == c)
			{
				qt = true;
			}
		}

		if( qt )
		{
			fn = fPath + "/" + fName;
		}
		else
		{
			fn = fPath + "\\" + fName;
		}
	}
	return fn; 
}


bool  TFileParameters::defined()  const  
{ 
	// returns "true" if the parameters are defined

	bool retVal = true;

	// defined if the filename, file type and format
	// have all been set
	std::string  fn = getFileName();
	retVal = !(fn.empty())  
				&&  fType != kNull 
				&&  fFormat != kNullFormat;

	return retVal; 
}


//! clear the file parameters and set the default values 
void  TFileParameters::defaultValues()
{
    fName = ""; 
	fPath = "";
	fType = kCSGEOFile;
	fFormat = kFreeFormat;

	return;
}
//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////



//! convert a file type from string form to enumerator form
TFileParameters::EFileType  TFileParameters::toType( const std::string& type )
{
	EFileType  result;

	if ( type == "CSGEOFree" || type == "csgeoFree"  || type == "csgeofree" )
	{
		result = kCSGEOFile;
	}
	else
	{
		result = kNull;
	}

	return  result;
}


//! convert a format from string form to enumerator form
TFileParameters::ETextFormat  TFileParameters::toFormat( const std::string& format )
{
	ETextFormat  result;

	if ( format == "CSGEOFree" || format == "csgeoFree"  || format == "csgeofree" )
	{
		result = kFreeFormat;
	}
	else
	{
		result = kNullFormat;
	}

	return  result;
}

