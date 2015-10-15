////////////////////////////////////////////////////////////////////
// TALGCConverter
/*!
Base Class for converters writing to an LGC output file.

Copyright 2003-2015 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////
#ifndef SU_TA_LGC_CONVERTER
#define SU_TA_LGC_CONVERTER
////////////////////////////////////////////////////////////////
// Forward declarations
#include "TAConverter.h"
#include <map>
#include <string>
using namespace std;
////////////////////////////////////////////////////////////////

/*!\ingroup LGCpp

@{*/
//Class definition
class  TALGCConverter : public TAConverter
{

public:

	/*!@name Enum Type Definition*/
	//@{
	enum ELGCObservations {kPLR3D, kDIR3D, kANGL, kZEND, kDIST, kDHOR, kDLEV, ALWAYS_LAST};
	//@}


	/*!@name Constructors and Destructors*/
	//@{
		/*! Constructor
		\param TAStreamFormatter& stream used to convert data */
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


protected:


	/*!@name Protected member functions*/
	//@{
		/*! default Constructor*/
		TALGCConverter();
	//@}

};

/*@}*/

#endif // SU_TA_LGC_CONVERTER