/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SU_TA_LGC_OBJECTWRITER
#define SU_TA_LGC_OBJECTWRITER

// STL
#include <map>
#include <string>
// SURVEYLIB
#include "TAConverter.h"

/*!
	\ingroup LGCObjectWriters
	\brief Base class for writing functionalities in LGC.
@{*/
class TALGCObjectWriter : public TAConverter
{
public:
	/*!@name Enum Observation Types Definitions*/
	//@{
	enum ELGCObservations
	{
		kPLR3D,
		kUVEC,
		kUVD,
		kANGL,
		kZEND,
		kDIST,
		kECTH,
		kECDIR,
		kDHOR,
		kDSPT,
		kDLEV,
		kDLEVDHOR,
		kDVER,
		kECHO,
		kECSP,
		kECVE,
		kORIE,
		kPDOR,
		kRADI,
		kOBSXYZ,
		kINCLY,
		kROLLY,
		kECWS,
		kECWI,
		ALWAYS_LAST
	};
	//@}

	/*!@name Constructors and Destructors*/
	//@{
	/*!
	\brief Constructor

	\param[in] TAStreamFormatter& stream used to convert the data and write out into the output file. */
	TALGCObjectWriter(TAStreamFormatter &stream);

	/// Destructor
	virtual ~TALGCObjectWriter();

	/// Write the keyword
	void writeKeyWord(const std::string &keyword);
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
