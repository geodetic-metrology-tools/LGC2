// TSeparatedFormatTStream.h
//
//
/*! 
 Concrete class defining a separated stream text stream formatter

 Patterns:
 A concrete decorator class of a TAStreamFormatter.
 
 Copyright 1999-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TSeparatedFormatTStream
#define SU_TSeparatedFormatTStream


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
#include <iostream>
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


/*class TWPSMeasurement;
class TWireToWireDistMeas;*/

//class TRadialOffsetCnstr;

//#include "TWPSFilter.h"
//#include "TWTWFilter.h"

#include "TAStreamFormatter.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
///\ingroup StreamFormatters
class  TSeparatedFormatTStream : public TAStreamFormatter  
{
public:

	/**@name Constructors and Destructors */
	//@{
		//! Default Constructor
		TSeparatedFormatTStream(TDataParameters&);
		TSeparatedFormatTStream(const string&, TDataParameters&);

		/*!Constructors
		\param io: read or write
		\param fp: data file's parameters (name, path ...) 
		\param ds: data parameters (unit, precision, refernce system, coord sys...)*/ 
		TSeparatedFormatTStream(TAStreamFormatter::EIOType, TFileParameters& fp, TDataParameters& dp);

		TSeparatedFormatTStream(TAStreamFormatter::EIOType, TADataSet&);

		TSeparatedFormatTStream(TAStreamFormatter::EIOType, TADataSet&, TPointFormat&);

		TSeparatedFormatTStream(const TAStreamFormatter::EIOType, TADataSet&, const TPointFormat&, const TObservationFormat&);

		//! Destructor
		virtual  ~TSeparatedFormatTStream();
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

	
	int	 ajustAnglePrecTo100MicroGons(const TObservationFormat::EAnglePrecision angle);

	// overloaded functions to supress gaps
	//using TAStreamFormatter::width;
	virtual int			width() const;
	virtual int			width(int);
	virtual int			setWidthFormat(const int width);

private:


/*	TWPSFilter*		fWPSFilter;
	TWTWFilter*		fWTWFilter;*/

	string			fLineBuf;
	

	bool			fNoObsToRead;

	//ClassDef(TSeparatedFormatTStream, 1)
};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_TSeparatedFormatTStream
