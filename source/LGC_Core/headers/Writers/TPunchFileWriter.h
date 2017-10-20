/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_TPunchFileWriter
#define SU_TPunchFileWriter


//LGC
#include <TAFileWriter.h>
#include <TLGCConfig.h>
#include <TTSTNWriter.h>

class	TAStreamFormatter;

/*!
	\ingroup Writers
	\brief Write an additional LGC output file for point coordinates (PUNCH).

	Creates a file from the calculation results and sends the appropriate messages.
*/
class  TPunchFileWriter : public TAFileWriter  
{
public:

	/*!@name Constructors and Destructors*/
	//@{
		///constructor
		TPunchFileWriter(TAStreamFormatter* stream, const TLGCData* project);

		///Destructor
		virtual  ~TPunchFileWriter();
	//@}


	/*!@name Public member functions*/
	//@{
		/// write the point coordinate file corresponding to the given project
		virtual void writeFile();

		///write the lgc file  when there is an error in the project
		virtual void writeFile(const string error);
	//@} 

protected:

	/// default constructor
	TPunchFileWriter();

	/// write the file title
	void writeTitle();
	/// write the points data
	void writePoints();
	/// Write a point
	void writePoint(LGCAdjustablePoint const& point, TLGCConfig::TCoordOut::eMode);
	/// write the points XYZ data header
	void writeXYZHeader();
	/// write the point XYZ coordinates
	void writeXYZData(LGCAdjustablePoint const& point);
	/// write the points' XYH data header
	void writeXYHHeader();
	/// write the points' XYH coordinates
	void writeXYHData(LGCAdjustablePoint const& point);
	/// write the points' covariance and displacement header
	void writeXYZVarCovarDeltaHeader();
	/// write the points' covariance and displacement data
	void writeXYZVarCovarDeltaData(LGCAdjustablePoint const& point);
	/// write the points' coordinates, error ellipsoid parameters and displacement header
	void writeXYZErrorEllHeader();
	/// write the points' coordinates, error ellipsoid parameters and displacement data
	void writeXYZErrorEllData(LGCAdjustablePoint const& point);
	/// write the points' XYZ coordinates and covariance header
	void writeXYZSigmaHeader();
	/// write the points' XYZ coordinates and covariance data
	void writeXYZSigmaData(LGCAdjustablePoint const& point);
	/// write the points' XYZH coordinates header
	void writeXYZHHeader();
	/// write the points' XYZH coordinates data
	void writeXYZHData(LGCAdjustablePoint const& point);
	/// write the points' XYH coordinates and geoidal undulation header
	void writeXYHNHeader();
	/// write the points' XYH coordinates and geoidal undulation data
	void writeXYHNData(LGCAdjustablePoint const& point);
	/// write the points' XYZH coordinates and geoidal undulation header
	void writeXYZHNHeader();
	/// write the points' XYZH coordinates and geoidal undulation data
	void writeXYZHNData(LGCAdjustablePoint const& point);
	// write the points 'XYH, ID, displacement, DCUM, option header
	void writeCooHeader();
	// write the points 'XYH, ID, displacement, DCUM, option data
	void writeCooData(LGCAdjustablePoint const& point);


	///Calulate N value
	TReal getN(LGCAdjustablePoint const& point);
	//@}

	const TLGCData* fData;
};

/*@}*/

#endif // SU_TCSGEOFileWriter
