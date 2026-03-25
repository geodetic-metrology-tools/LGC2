/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SU_FAUT_WRITER
#define SU_FAUT_WRITER

// STL
#include <string>
// LGC
#include <TAFileWriter.h>

class TLGCData;
class TAStreamFormatter;

/*!
	\ingroup Writers
	\brief Write an additional LGC output file for FAUT detection.
*/
class TFautFileWriter : TAFileWriter
{
public:
	/*@name Constructor / Destructor */
	//@{
	/// Default constructor
	TFautFileWriter(TAStreamFormatter *stream, const TLGCData *project);

	/// Destructor
	virtual ~TFautFileWriter();
	//@}

	/*!@name Public member functions*/
	//@{
	/// write the point coordinate file corresponding to the given project
	virtual void writeFile(TLGCData const *const);

	/// write the lgc file  when there is an error in the project
	virtual void writeFile(const std::string error);
	//@}

private:
	/// default constructor
	TFautFileWriter();

	/*!@name Private functions for header part of the file */
	//@{
	/// write title and date
	void writeTitle();
	/// write data summary (sigma zero a posteriori, alpha, beta, and others)
	void writeDataSummary();

	//@}

	/// Total reliability if possible
	void writeOverallReliability(TLGCData const *const project);
	/// Network Degrees of freedom (DOF)
	void writeNetworkDOF(TLGCData const *const project);

	/*!@name Private Attribute*/
	//@{
	TReal fAlpha; /*! level of significance, [0;1], default value : 1% */
	TReal fBeta; /*! risk of having false values, [0;1], default value : 10% */
	//@}
};

/*@}*/

#endif // SU_FAUT_WRITER
