/// TFileParameters.h
//
/*! A class defining the parameters of a data file:
        filename, 
		file type,
  		file format, 
  		data precision
*/
// Patterns:
//
// 
// Copyright 2000-2002 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_FILE_PARAMETERS
#define SU_FILE_PARAMETERS


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
//using namespace std;
//
//#include  "TAQtStreamFormatter.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
class  TFileParameters //: public TObject  
{
public:
	
	///@name Constants
	//@{
		/// copied from TAQtTestStreamFormatter ****Should NOT be here****
		enum  ETextFormat { kNullFormat, kFreeFormat };

		/// enumeration of the file types
		enum  EFileType{ kNull=1, kPassivePtFile, kCSGEOFile, kLGCFile };
		
		
	//@}


	/*!@name Constructors and Destructors */
	//@{
		/// default constructor
		TFileParameters();

		/// copy constructor
		TFileParameters(const  TFileParameters&);

		/// destructor
		virtual  ~TFileParameters();

	//@}


	/*!@name member functions */
	//@{
		
		/// copy assignment operator
		TFileParameters& operator=( const TFileParameters& );
		
		/// set the full filename, both path and name
        bool  setFileName( const std::string& name );
		bool  setFilePath( const std::string& path );
		/// set the file type
		bool  setTypeFormat( const std::string& );
		/// clear the file parameters and set the default values 
		void  defaultValues();

		/// get the full filename, both path and name
		std::string  getFileName()  const;  
		/// get the name of the file
		std::string  getName()  const  { return fName; }
		/// get the path for the file
		std::string  getPath()  const  { return fPath; }
		/// get the file type
		EFileType  getType()  const  { return fType; }
		/// get the text stream's format
		ETextFormat  getFormat() const  { return fFormat; }
		/// indicates if the file name, type and format have been set
		bool  defined()  const;

	//@}

private:
	/*!@name private member functions */
	//@{	
		/// convert a file type from string form to enumerator form 
		EFileType  toType( const std::string& format );

		/// convert a format from string form to enumerator form 
		ETextFormat  toFormat( const std::string& format );

	//@}


private:

    std::string  fName;
    std::string  fPath;
	EFileType  fType;
	ETextFormat  fFormat;

	//ClassDef(TFileParameters, 1)
};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_FILE_PARAMETERS
