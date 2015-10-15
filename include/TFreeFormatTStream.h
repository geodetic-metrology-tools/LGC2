// TFreeFormatTStream.h
//
//
// Concrete class defining the free format file syntax,
// for example the file as used in CSGeo, and Geotran.
//
// Patterns:
// A concrete decorator class of a QTextStream.
// 
// Copyright 1999,2000 CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////

#ifndef SU_FREE_FORMAT_TSTREAM 
#define SU_FREE_FORMAT_TSTREAM

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include <string>
//using namespace std;
//
class  TAngle;
class  TLength;
class  TSpatialPointName;
class  TSpatialPosition;
class  TCoordinateSystem;
//class  QTextStream;
#include "TAStreamFormatter.h"
//
// typedefs
////////////////////////////////////////////////////////////////



//Class definition
class TFreeFormatTStream :public TAStreamFormatter //: public TObject  
{
public:
	// constructors and destructors
	TFreeFormatTStream(TDataParameters&);
	TFreeFormatTStream(const string&, TDataParameters&);
	TFreeFormatTStream(EIOType, TADataSet&);
	virtual ~TFreeFormatTStream();

	// member functions
	using TAStreamFormatter::operator <<;
	using TAStreamFormatter::operator >>;

	virtual  TAStreamFormatter  &operator>>( TSpatialPoint & );

	virtual  TAStreamFormatter  &operator<<( const TSpatialPosition & );
	virtual  TAStreamFormatter  &operator<<( const TSpatialPoint & );

    virtual  void  skipCommentLines();

	///returns the charachter sequence used to separate fields
	virtual string		separator() const;

protected:

private:
	// member attributes
	string			fLineBuf;

	//ClassDef(TFreeFormatTStream, 1)
};




//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////



#endif // SU_FREE_FORMAT_TSTREAM
