// TFixedColFormatTStream.h
//
//
// Concrete class defining the old LGC file syntax,
//
// Patterns:
// A concrete decorator class of a QTextStream.
// 
// Copyright 1999,2000 CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_TFixedColFormatTStream
#define SU_TFixedColFormatTStream


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
//using namespace std;
//
class TWPSFilter;

class TAngle;
class TLength;
class TSpatialPointName;
class TSpatialPosition;
//class TSpatialLine;
class TCoordinateSystem;
class TLSCalcSpPoint;
///////class QTextStream;

class TDistMeasStation;
class TVerticalDistROM;
class TVerticalDistMeasurement;
class TZenithDistMeasurement;
class TOffsetToVerLineMeasurement;
class T2DOffsetMeasurement;
class TDistConstants;
class TAngleConstants;


#include "TAStreamFormatter.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
class  TFixedColFormatTStream : public TAStreamFormatter  
{
public:

	/**@name Constructors and Destructors */
	//@{
		//! Default Constructor
		TFixedColFormatTStream(TDataParameters&);
		TFixedColFormatTStream(const string&, TDataParameters&);
		TFixedColFormatTStream(TAStreamFormatter::EIOType, TADataSet&);

		TFixedColFormatTStream(TAStreamFormatter::EIOType, TADataSet&, TPointFormat&);

		TFixedColFormatTStream(const TAStreamFormatter::EIOType, TADataSet&, const TPointFormat&, const TObservationFormat&);

		//! Destructor
		virtual  ~TFixedColFormatTStream();
	//@}



	/**@name Overloaded stream extraction operators*/
	//@{
		using TAStreamFormatter::operator <<;
		using TAStreamFormatter::operator >>;

		//!Extraction of a TSpatialPoint object;
		virtual TAStreamFormatter &operator>>( TSpatialPoint & );

	//@}

	/**@name Overloaded stream insertion operator*/
	//@{
		//!Insertion of a TSpatialPosition object
		virtual TAStreamFormatter&		operator<<( const TSpatialPosition&);

		//!Insertion of a TSpatialPoint object
		virtual TAStreamFormatter&		operator<<( const TSpatialPoint&);

		//!Insertion of a TPositionVector object
		virtual TAStreamFormatter&		operator<<(const TPositionVector&);

		//!Insertion of a TFreeVector object
		virtual TAStreamFormatter&		operator<<(const TFreeVector&);

	//@}

	//!Skips the comment lines
    virtual  void  skipCommentLines();

	//!returns the charachter sequence used to spearate fields
	virtual string		getSeparator() const;
	virtual void		setSeparator(const string&);

	/*! for files which have no observations' values to read */
	virtual void  setNoObsToRead();
	virtual bool hasNoObsToRead() const {return fNoObsToRead;}
	//int width(int);
	
	int	 ajustAnglePrecTo100MicroGons(const TObservationFormat::EAnglePrecision angle);

private:


/*	TWPSFilter*		fWPSFilter;
	TWTWFilter*		fWTWFilter;*/

	string			fLineBuf;
	string			fSeparator;

	bool			fNoObsToRead;

	//ClassDef(TSeparatedFormatTStream, 1)
};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_TSeparatedFormatTStream
