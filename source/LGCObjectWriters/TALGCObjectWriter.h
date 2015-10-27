#ifndef SU_TA_LGC_OBJECTWRITER
#define SU_TA_LGC_OBJECTWRITER

#include "TAConverter.h"
#include <map>
#include <string>

/*!
	\ingroup LGCObjectWriters
	\brief Base class for writing functionalities in LGC. 
@{*/
class  TALGCObjectWriter : public TAConverter
{
public:

	/*!@name Enum Observation Types Definitions*/
	//@{
		enum ELGCObservations {kPLR3D, kUVEC, kUVD, kANGL, kZEND, kDIST, kECTH,  kDHOR, kDSPT, kDLEV, kDVER, kECHO, kORIE, ALWAYS_LAST};
	//@}


	/*!@name Constructors and Destructors*/
	//@{
		/*!
		\brief Constructor

		\param[in] TAStreamFormatter& stream used to convert the data and write out into the output file. */
		TALGCObjectWriter(TAStreamFormatter& stream);

		/// Destructor 
		virtual  ~TALGCObjectWriter();

		/// Write the keyword
		void			writeKeyWord(const string &keyword);
	//@}

protected:
	/*!@name Protected member functions*/
	//@{
		/*! default Constructor*/
		TALGCObjectWriter();
	//@}
};

/*@}*/

#endif