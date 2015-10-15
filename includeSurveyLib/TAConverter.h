#ifndef SU_TA_CONVERTER
#define SU_TA_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include <string>
#include <vector>
#include <Quad.h>
#include <TLength.h>
#include <TAngle.h>

////////////////////////////////////////////////////////////////
// Forward declarations
class	TAStreamFormatter;

// typedefs
typedef const std::vector<std::string>&& TStrTokens;
////////////////////////////////////////////////////////////////

/*!\ingroup Converters
	\brief Base class for converting and writing functionalities, specific classes to output points, observations or other application-specific objects should inherit from this class.
@{*/
class  TAConverter 
{

public:

	/*!@name Constructors and Destructors*/
	//@{
		/*! Constructor
		\param TAStreamFormatter* stream used to convert data */
		TAConverter(TAStreamFormatter* stream);
	
		TAConverter(TAStreamFormatter& stream);

		/*! Destructor */
		virtual  ~TAConverter();
	//@}

	// Tokenizes a std::string into a std::vector of strings based on the delimiters given to delim
	// e.g. " \t_" will split on every occurence of a blank, a tab or an underscore.
	//  Use the TStrTokens type to catch the return value avoiding copies
	static const std::vector<std::string> tokenizeString(const std::string& str, const char* delims);


protected:


	/*!@name Protected member functions*/
	//@{
		//! copy assignment operator
		TAConverter& operator=(const TAConverter& source);

		/*!write a std::string
		\param int : width used to write the std::string
		\param std::string : std::string to write*/
		void	writeString(const int width, const std::string data);
		void	writeStringLeft(const int width, const std::string data);

		/*!write a std::string and add a separator
		\param int : width used to write the std::string
		\param std::string : std::string to write*/
		void	writeStringSep(const int width, const std::string data);
		void	writeStringLeftSep(const int width, const std::string data);

		/*!write a TReal and
		\param int : width used to write the TReal
		\param int : precision used to write the TReal
		\param TReal : TReal to write*/
		void	writeDouble(const int width, const int pres, const TReal data);

		/*!write a TReal and add a separator
		\param int : width used to write the TReal
		\param int : precision used to write the TReal
		\param TReal : TReal to write*/
		void	writeDoubleSep(const int width, const int pres, const TReal data);

		void	writeLength(const int width, const int pres, const TLength::EUnits, const TLength data);
		void	writeAngle(const int width, const int pres, const TAngle::EUnits, const TAngle data);

		void	writeInteger(const int width, const int data);
		void	readP100Comment(std::string comments);
		std::string	readOptional(const std::string& keyword);
		bool	readOptionalExists(const std::string& keyword);

	//@}
	
	/*!@name Protected functions : access to attributs */
	//@{
		/*! get the stream*/
		TAStreamFormatter*	getStream() {return fStream;}

		/*! get the stream as a reference*/
		TAStreamFormatter&	getStreamRef() {return *fStream;}


		/*! get the stream*/
		void	setStream(TAStreamFormatter* stream);

		/*!get name's width*/
		int					getNameWidth() const {return fNameWidth;}

		int					getCoordWidth() const {return fCoordWidth;}
		int					getCoordResWidth() const {return fCoordResWidth;}

		int					getCoordPrecision() const {return fCoordPrecision;}
		int					getCoordErrorPrecision() const {return fCoordErrorPrecision;}

		/*!get observation's width*/
		int					getObsWidth() const {return fObsWidth;}

		/*!get observation residual's width*/
		int					getObsResWidth() const {return fObsResWidth;}

		/*!get angle precision*/
		int					getAnglePrecision() const {return fAnglePrecision;}

		/*!get angle residual precision*/
		int					getAngleResidualPrecision() const {return fAngleResidualPrecision;}

		/*!get length residual precision*/
		int					getLengthPrecision() const {return fLengthPrecision;}

		/*!get length residual precision*/
		int					getLengthResidualPrecision() const {return fLengthResidualPrecision;}

		/*!get separator*/
		std::string				getSeparator() const {return fSeparator;}
	//@}		






protected:

	/*!@name Private Function*/
	//@{
		/*! default Constructor*/
		TAConverter();
	//@}
		/*!pointer to a TAStreamFormatter object*/
		TAStreamFormatter*				fStream;

private:

	/*!@name Private Attribute*/
	//@{


		/*!Width used for observation I/O*/
		int								fNameWidth;
		int								fObsWidth;
		int								fObsResWidth;
		int								fCoordWidth;
		int								fCoordResWidth;

		/*!Precision used for observation I/O*/
		int								fCoordPrecision;
		int								fCoordErrorPrecision;
		int								fAnglePrecision;
		int								fAngleResidualPrecision;
		int								fLengthPrecision;
		int								fLengthResidualPrecision;

		/*!Separator between two elements*/
		std::string							fSeparator;


		/*std::string							fError;*/
	//@}

};

/*@}*/

#endif // SU_TA_CONVERTER
