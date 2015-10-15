//////////////////////////////////////////////////////////////////////
// TAStreamFormatter.h: 
//
/*!	Abstract Base Class for a QTextStream Formatter.
	Enables the output of all the standard survey objects.

	Patterns:
	Maintains a reference to the C++ Standard Library 
	the decorator pattern.
	
    Copyright 2002-2008, M. Jones  CERN, TS/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_A_TEXT_STREAM_FORMATTER
#define SU_A_TEXT_STREAM_FORMATTER

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000



////////////////////////////////////////////////////////////////
// Forward declarations
//
#include	<iostream>
#include	<string>
#include	<fstream>
#include	<sstream>
//using namespace std;
//
class		TAngleFilter;
class		TLengthFilter;
class		TSpatialPointName;
class		TSpatialLineName;
class		TSpatialPoint;
class		TSpatialPositionFilter;
class		TAStreamFormatter;
class		TADataSet;
class		TDataParameters;
class		TFileParameters;


#include	"TAngle.h"
#include	"TLength.h"
#include	"TScalar.h"
#include	"TSpatialPosition.h"
#include	"TAReferenceFrame.h"
#include	"TPointFormat.h"
#include	"TObservationFormat.h"
// typedefs
typedef TAStreamFormatter & (*TSFFUNC)(TAStreamFormatter &);// manipulator function
//typedef size_t streamsize;
////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//Implementation of multiplication operator for STRING, can be used for "multiplying" TAB spaces if we want a structurized output file
///////////////////////////////////////////////////////////////
string operator*(const string& s, unsigned int n);
string operator*(unsigned int n, const string& s);
static const string TAB = "\t";

/*@{*/
//Class definition
///\ingroup StreamFormatters
class TAStreamFormatter//, public TObject  
{
	friend class TGeodeticFilter;
	friend class T3DCartesianFilter;
	friend class T2DPlusHFilter;

public:

	/**@name Constants */
	//@{
	enum	EIOType		{kWrite, kRead};
	enum	ETextFormat {kNull, kFreeFormat, kSeparatorFormat, kColumnFormat};
	//enum	Encoding	{C, /*lt_LN.bit8,*/ fr_FR, fr_CH};
	//@}


	/*!@name constructor / destuctor*/
	//@{
	/*!Constructors
	\param dp: data's parameters (unit, precision, refernce system, coord sys...)*/ 
	TAStreamFormatter(TDataParameters&);
	
	/*!Constructors
	\param str: string to read
	\param dp: data's parameters (unit, precision, refernce system, coord sys...)*/ 
	TAStreamFormatter(const string& str, TDataParameters& dp);
	
	/*!Constructors
	\param io: read or write
	\param fp: data file's parameters (name, path ...) 
	\param ds: data parameters (unit, precision, refernce system, coord sys...)*/ 
	TAStreamFormatter(EIOType io, TFileParameters& fp, TDataParameters& dp);

	/*!Constructors
	\param io: read or write
	\param ds: data and file's parameters (name, unit, precision, refernce system, coord sys...)*/ 
	TAStreamFormatter(EIOType io, TADataSet& ds);

	/*!Constructors
	\param io: read or write
	\param ds: data and file's parameters (name, unit, precision, refernce system, coord sys...) 
	\param pf: TpointFormat's object use for write output*/
	TAStreamFormatter(EIOType io, TADataSet& ds, TPointFormat& pf);

	/*!Constructors
	\param io: read or write
	\param ds: data and file's parameters (name, unit, precision, refernce system, coord sys...) 
	\param pf: TPointFormat's object use for write output
	\param pf: TObservationFormat's object use for write output*/
	TAStreamFormatter(const EIOType io, TADataSet& ds, const TPointFormat& pf, const TObservationFormat& obsFor);

	/*!Destructor*/
	virtual ~TAStreamFormatter();
	//@}

	/*Function which reset the stream with a new file name, all the options are kept, just a file is changed, so that we write or read from a different file*/
	void resetStreamName(std::string name);


	/*!@member functions: set*/
	//@{
	/*!set angle's units
	\return the old angle's units*/
    virtual	TAngle::EUnits  setAngleUnits(const TAngle::EUnits );
	void init();
	/*!set length's units
	\return the old length's units*/
    virtual	TLength::EUnits  setLengthUnits(const TLength::EUnits );

	/*!set coordinate system
	\return the old coordinate system*/
	virtual	TCoordSysFactory::ECoordSys  setCoordSys(const TCoordSysFactory::ECoordSys);
	
	/*!set reference Frame
	\return the old refernce frame*/
	virtual	TAReferenceFrame*  setReferenceFrame(TAReferenceFrame*);

	/*!set a spatial position filter
	\param TCoordSysFactory::ECoordSys : coordinate system
	\return the old spatial position filter*/
	virtual	TSpatialPositionFilter*	setPosFilter(const TCoordSysFactory::ECoordSys cs);

	
	/*!set a spatial status for TPointFormat*/
	virtual	TSpatialStatus::ESpatialStatus	setPointFormatStatus(const TSpatialStatus::ESpatialStatus status);

	/*!set a width for stream*/
	virtual	int	setWidthFormat(const int width);

	/*!set a precision for stream*/
	virtual	int	setPrecisionFormat(const int precision);

	virtual void	setPointFormat(const TPointFormat ptFormat);
	
	virtual void	setObsFormat(const TObservationFormat ptFormat);
	
	virtual void	setFileName(string const& Name);
	//@}



	/*!@member functions: get*/
	//@{
	/*!get angle's units*/
    virtual	TAngle::EUnits  getAngleUnits() const;

	/*!get length's units*/
    virtual	TLength::EUnits  getLengthUnits() const;

	/*!get coordinate system*/
	virtual	TCoordSysFactory::ECoordSys  getCoordSys() const;

	/*!get reference frame*/
	virtual	TAReferenceFrame*  getReferenceFrame() const;

	/*!get the TPointFormat's spatial status for */
	virtual	TSpatialStatus::ESpatialStatus	getPointFormatStatus() const;

	/*!get the stream's width*/
	virtual	int	getWidthFormat() const;

	/*!get the stream's precision*/
	virtual	int	getPrecisionFormat() const;

	virtual TPointFormat *  getPointFormat();
	virtual TObservationFormat *  getObsFormat();
	//@}



	/*!@Input spatial object operator*/
	//@{
		/*!Input TAngle*/
		virtual	TAStreamFormatter	&operator>>( TAngle & );

		/*!Input TLength*/
		virtual	TAStreamFormatter	&operator>>( TLength & );

		/*!Input TSpatialPointName*/
		virtual	TAStreamFormatter	&operator>>( TSpatialPointName & );

		/*!Input TSpatialPosition*/
		virtual	TAStreamFormatter	&operator>>( TSpatialPosition & );

		/*!Input TSpatialPoint*/
		virtual	TAStreamFormatter	&operator>>( TSpatialPoint & );
	//@}

	
	/*!@Output spatial object operator*/
	//@{
		/*!Output TAngle*/
		virtual	TAStreamFormatter	&operator<<( const TAngle & );

		/*!Output TLength*/
		virtual	TAStreamFormatter	&operator<<( const TLength & );

		/*!Output TSpatialPointName*/
		virtual	TAStreamFormatter	&operator<<( const TSpatialPointName & );

		/*!Output TSpatialPosition*/
		virtual	TAStreamFormatter	&operator<<( const TSpatialPosition & );

		/*!Output TSpatialPoint*/
		virtual	TAStreamFormatter	&operator<<( const TSpatialPoint & );

		/*!Output TPointFormat*/
		virtual	 TAStreamFormatter& operator<<(const TPointFormat& ptFormat);

		/*!Output TPositionVector object*/
		virtual TAStreamFormatter&	operator<<(const TPositionVector&);

		/*!Output TFreeVector object*/
		virtual TAStreamFormatter&	operator<<(const TFreeVector&);

		/*!Output TDouble object*/
		virtual TAStreamFormatter&	operator<<(const TDouble&);
		
		/*!Output TScalar object*/
		virtual TAStreamFormatter&	operator<<(const TScalar&);
		
		/*!Output TObservationFormat*/
		virtual	 TAStreamFormatter& operator<<(const TObservationFormat& obsFormat);
	//@}



	/*!@Overloaded Input Operator*/
	//@{
		virtual	TAStreamFormatter	&operator>>(signed short&);
		virtual	TAStreamFormatter	&operator>>(unsigned short&);
		virtual	TAStreamFormatter	&operator>>(signed int&);
		virtual	TAStreamFormatter	&operator>>(unsigned int&);
		virtual	TAStreamFormatter	&operator>>(signed long&);
		virtual	TAStreamFormatter	&operator>>(unsigned long&);
		virtual	TAStreamFormatter	&operator>>(float&);
		virtual	TAStreamFormatter	&operator>>(TReal&);
		virtual	TAStreamFormatter	&operator>>(char*);
		virtual	TAStreamFormatter	&operator>>(string&);
		virtual	TAStreamFormatter	&operator>>(TSFFUNC);
	//@}



	/*!@overloaded Output Operator*/
	//@{
		virtual	TAStreamFormatter	&operator<<( const char );
		virtual	TAStreamFormatter	&operator<<(signed short);
		virtual	TAStreamFormatter	&operator<<(unsigned short);
		virtual	TAStreamFormatter	&operator<<(signed int);
		virtual	TAStreamFormatter	&operator<<(unsigned int);
		virtual	TAStreamFormatter	&operator<<(signed long);
		virtual	TAStreamFormatter	&operator<<(unsigned long);
		virtual	TAStreamFormatter	&operator<<(float);
		virtual	TAStreamFormatter	&operator<<(TReal);
		virtual	TAStreamFormatter	&operator<<(const char*);
		virtual	TAStreamFormatter	&operator<<(const string&);
		virtual	TAStreamFormatter	&operator<<(void*); 
		virtual	TAStreamFormatter	&operator<<(TSFFUNC);
	//@}
 		/*!write a string
		\param int : width used to write the string
		\param string : string to write*/
		void	writeString(const int width, const string data);
		void	writeStringLeft(const int width, const string data);

		/*!write a TReal and
		\param int : width used to write the TReal
		\param int : precision used to write the TReal
		\param TReal : TReal to write*/
		void	writeDouble(const int width, const int pres, const TReal data);

		void	writeInteger(const int width, const int data);
		//void	writeLength(const int width, const int pres, const TLength::EUnits, const TLength data);
		//void	writeAngle(const int width, const int pres, const TAngle::EUnits, const TAngle data);

	//!member functions
    fstream*	device() const;


	virtual	bool		atEnd();
	virtual	bool		atEndCom();
    string				readLine();
	char				readChar();
    //non utilisee? string	read();
    void				skipWhiteSpace();
    void				skipBOM(); // removes the byte order marks that is introduced by recent versions of notepad
	TAStreamFormatter&	get(char&);
	char				peek();
	TAStreamFormatter&	readRawBytes(char*, streamsize);
	bool				fail() const;
	void				clear() const;
	int					flags() const;
    int					flags( int f );
    int					setf( int bits );
    int					setf( int bits, int mask );
    void				unsetf( int bits );
	void				reset();
	TAStreamFormatter&	unget() { fIOStream->unget(); return (*this); }

    virtual int			width()	const;
    virtual int			width(int);
    int					fill()	const;
    int					fill(int);
    int					precision()	const;
    int					precision(int);
    TAStreamFormatter&	writeRawBytes(const char*, streamsize);

	const string&		getError() const;
	void				setError(const string);

	string				getWarning() const;
	void				initWarning();
	void				addWarning(const string);

	/*! for files which have no observations' values to read */
	virtual void  setNoObsToRead();
	virtual bool hasNoObsToRead() const;



	
	
	//!default function
	virtual string	separator() const;
	virtual	void	skipCommentLines();



	//!open & close function
	void openIn();
	void openOutApp();
	bool isOpen();
	void close();

	virtual string		getSeparator() const;
	virtual void		setSeparator(const string&);


	//test
	string getString();



	iostream&				getIOStream() {return	*fIOStream;}


	/*!@ set the format for the spacing between the data*/
	void setDataSpacing();
		
	/*!@ set no space between the data*/
	void setNoGapBetweenData();

	/*!@ set a space between the data*/
	void setGapBetweenData();

	std::streampos tellg();
	TAStreamFormatter& seekg(streamoff off, ios_base::seekdir dir);

	//! Returns the current depth in a tree, reflecting e.g. the hierarchical structure.
	inline void setTreeDepth(unsigned int dpth){fNofSpaces = dpth;};

	//! Returns the current depth in a tree, reflecting e.g. the hierarchical structure.
	inline int getTreeDepth() const{return fNofSpaces;};

	//! Returns the current space, one TAB for each depth.
	inline std::string getCurrSpace() const{return fNofSpaces*TAB;};

	//! Returns the current space extended for any number of extra TABs without affecting the current depth level.
	inline std::string getCurrSpaceExtended(int ext) const{return (fNofSpaces + ext)*TAB;};

protected:
	static TAngleFilter *getAngleFilter( TAngle::EUnits );
	static TLengthFilter *getLengthFilter( TLength::EUnits );
	static TSpatialPositionFilter *getPositionFilter( TCoordSysFactory::ECoordSys );

	// member attributes
	stringstream*							fSStream;
	fstream*								fFStream;
	iostream*								fIOStream;
	string   								fName;
	EIOType									fIOType;
	string									fError;
	string									fWarning;

	TAngle::EUnits							fAngles;
	TAngleFilter*							fAngFilter;
	TLength::EUnits							fLengths;
	TLengthFilter*							fLenFilter;
	TCoordSysFactory::ECoordSys				fCoordSys;
	TSpatialPositionFilter*					fPosFilter;
	TAReferenceFrame*						fRefFrame;
	TPointFormat							fPointFormat;
	TObservationFormat						fObservationFormat;
	int										fWidth;
	int										fPrecision;

	string									fNonSpaceSeparator;
	string									fSeparator;

private:

	bool									fLineGap;
	//Used for formating, number of 'TAB' before start of the line.
	unsigned int							fNofSpaces;

	//ClassDef(TAStreamFormatter, 1)
};

/*@}*/


extern TAStreamFormatter &bin( TAStreamFormatter &s );	// set bin notation
extern TAStreamFormatter &oct( TAStreamFormatter &s );	// set oct notation
extern TAStreamFormatter &dec( TAStreamFormatter &s );	// set dec notation
extern TAStreamFormatter &hex( TAStreamFormatter &s );	// set hex notation
extern TAStreamFormatter &endl( TAStreamFormatter &s );	// insert EOL ('\n')
extern TAStreamFormatter &flush( TAStreamFormatter &s );	// flush output
extern TAStreamFormatter &ws( TAStreamFormatter &s );	// eat whitespace on input
extern TAStreamFormatter &reset( TAStreamFormatter &s );	// set default flags
extern TAStreamFormatter &left( TAStreamFormatter &s );
extern TAStreamFormatter &right( TAStreamFormatter &s );


#endif // !defined(SU_A_TEXT_STREAM_FORMATTER)
