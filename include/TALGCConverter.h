////////////////////////////////////////////////////////////////////
// TALGCConverter
/*!
Base Class for converters writing to an LGC output file

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_TA_LGC_CONVERTER
#define SU_TA_LGC_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
//class	TLGCProject;

#include "TAConverter.h"
//
#include <map>
#include <string>
using namespace std;

// typedefs
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/
//Class definition
class  TALGCConverter : public TAConverter
{

public:

	/*!@name Enum Type Definition*/
	//@{
	static enum ELGCObservations {kPLR, kANGL, kZEND, kDRDL, kDSPT, kORIE, kDVER, kDLEV, kDHOR, 
									kECHO, kECSP, kECTH, kECVE, kRADI};
	//@}


	/*!@name Constructors and Destructors*/
	//@{
		/*! Constructor
		\param TAStreamFormatter* stream used to convert data */
		TALGCConverter(TAStreamFormatter& stream);

		/*! Destructor */
		virtual  ~TALGCConverter();
	//@}


	/*!@name Public member functions*/
	//@{
		//! write the keyword corresponding to the observation type enumerator
		static string	getKeyWord(TALGCConverter::ELGCObservations);

		void			writeKeyWord(const string &keyword);
	//@}

		template <typename T>
		T* readOptionalValue(typename T::EUnits un, const string& keyword, const T* defaultValue)
		{
			T* result;
			if (readOptionalExists(keyword))
			{
				this->setUnits(un);
				result = new T();
				*fStream >> *result;
			}
			else
			{
				result = new T(*defaultValue);
			}

			return result;
		}

protected:


		void setUnits(TAngle::EUnits un);

		void setUnits(TLength::EUnits un);

		bool isComment();

	/*!@name Protected member functions*/
	//@{
		/*! default Constructor*/
		TALGCConverter();
	//@}

private:

	/*!@name Private Function*/
	//@{
	//@}
	
	/*!@name Private Attribute*/
	//@{
	//@}
};

/*@}*/

#endif // SU_TA_LGC_CONVERTER
