////////////////////////////////////////////////////////////////////
//TLGCDataSet.h : header file
/*!
Container for the LGC project input data set

Patterns:
 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////

#ifndef SU_LGC_DATA_SET
#define SU_LGC_DATA_SET


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
class TVInputMatrices;
class VResultMatrices;


#include  "TFileParameters.h"
#include  "TDataParameters.h"

#include  "TADataSet.h"

#include "TWorkingPoints.h"
#include "TWorkingInstruments.h"
#include "TWorkingStations.h"
#include "TWorkingConstants.h"
#include "TWorkingConstraints.h"
#include "TWorkingROMs.h"
#include "lsalgo/UEOIndices.h"


//! Class holding all the input data of a LGC project
class TLGCDataSet : public TADataSet
{

public:
	/*!@name Constructors / Destructor*/
	//@{
	//!Default constructor
	TLGCDataSet();
	//!Destructor
	~TLGCDataSet();
	//@}
	
	/*!@name static member functions */
	//@{
		/*!return true if PDOR is used*/ 
	static bool usedOrieCnstr();
	//@}

	/*!return the number of unknowns, equations and observations of the project as a UEOIndices object*/
	virtual UEOIndices				getDimensions() const;

	/*!@name Access methods*/
	//@{
		/*!@return a pointer to the working stations*/
		TWorkingStations*		getWorkingStations() const	{return fWorkingStations;}
		
		/*!@return a pointer to the working stations*/
		TWorkingInstruments*	getWorkingInstruments() const	{return fWorkingInstruments;}

		/*!@return a pointer to the working points*/
		TWorkingPoints*			getWorkingPoints() const	{return fWorkingPoints;}

		/*!@return a pointer to the working constants*/
		TWorkingConstants*		getWorkingConstants() const	{return fWorkingConstants;}

		/*!@return a pointer to the working constraints*/
		TWorkingConstraints*	getWorkingConstraints() const	{return fWorkingConstraints;}

		/*!@return a pointer to the working constraints*/
		TWorkingROMs*			getWorkingROMs() const	{return fWorkingROMs;}

		
	//@}

	/*!@name Counting methods*/
	//@{

	//@}

		bool disallowsDuplicates() const { return noDuplicates; }
		void setNoDuplicates(bool disallow) { noDuplicates = disallow; }
	//@}

		int& getObservationsCount() { return observationsCount; }
		int& getUnknownsCount() { return unknownsCount; }

		/*!increment the number of read points for the corresponding status*/
		virtual void addToPointNum(TSpatialStatus::ESpatialStatus status);

		virtual void addToPolar(int count);
		/*!increment the number of read horizontal angles' measurements*/
		virtual void addToAnglNum(int count);
		/*!increment the number of read zenithal distances' measurements*/
		virtual void addToZenDistNum(int count);
		/*!increment the number of read horizontal distances' measurements*/
		virtual void addToHorDistNum(int count);
		/*!increment the number of read spatial distances' measurements*/
		virtual void addToSpaDistNum(int count);
		/*!increment the number of read spatial distances' measurements*/
		virtual void addToEDMSpaDistNum(int count);
		/*!increment the number of read vertical distances' measurements*/
		virtual void addToVertDistNum(int count);
		/*!increment the number of read vertical distances' measurements*/
		virtual void addToLevelObsNum(int count) { fNumDlev += count; }

		/*!increment the number of read Offset to vertical line measurements*/
		virtual void addToOffsetToVerLineNum(int count);
		/*!increment the number of read Offset to vertical line measurements*/
		virtual void addToOffsetToSpaLineNum(int count);
		/*!increment the number of read Offset to vertical line measurements*/
		virtual void addToOffsetToVerPlaneNum(int count);
		/*!increment the number of read Offset to vertical line measurements*/
		virtual void addToOffsetToTheoPlaneNum(int count);

		/*!increment the number of read orientation measurements*/
		virtual void addToGyroOrieNum(int count);

		/*!increment the number of read orientation measurements*/
		virtual void addToRadOffCnstrNum();
		/*!increment the number of read orientation measurements*/
		virtual void addToOrieCnstrNum();
	//@}

	/*!return the number of read points for the corresponding status*/
	virtual int getPointsDimension(TSpatialStatus::ESpatialStatus status) const;
	int getPolarDimension() const { return fNumPolar; }
	/*!return the number of read horizontal angles' measurements*/
	virtual int getHADimension() const;
	/*!return the number of read zenithal distances' measurements, classified by instrument height status*/
	virtual int getZDDimension() const;
	/*!return the number of read horizontal distances' measurements*/
	virtual int getHDDimension() const;
	/*!return the number of read spatial distances' measurements, classified by instrument height status*/
	virtual int getSDDimension() const;
	/*!return the number of read edm spatial distances' measurements, classified by instrument height status*/
	virtual int getEDMSDDimension() const;
	/*!return the number of read vertical distances' measurements*/
	virtual int getVDDimension() const;
	/*!return the number of read vertical distances' measurements*/
	virtual int getLevelObsDimension() const { return fNumDlev; }

	/*!return the number of read offset to vertical line's measurements*/
	virtual int getOffsetToVerLineDimension() const;
	/*!return the number of read offset to spatial line's measurements*/
	virtual int getOffsetToSpaLineDimension() const;
	/*!return the number of read offset to vertical plane's measurements*/
	virtual int getOffsetToVerPlaneDimension() const;
	/*!return the number of read offset to theodolite plane's measurements*/
	virtual int getOffsetToTheoPlaneDimension() const;

	/*!return the number of read gyro orientations' measurements*/
	virtual int getGyroOrieDimension() const;

	/*!return the number of read radial offset constraint*/
	virtual int getRadOffCnstrDimension() const;
	/*!return the number of read orientation constraint*/
	virtual int getOrieCnstrDimension() const;
	//@}

	void setDefaultRadOffCnstrSig(const bool b);

	
private:

	TWorkingPoints*				fWorkingPoints;
	TWorkingInstruments*		fWorkingInstruments;
	TWorkingROMs*				fWorkingROMs;
	TWorkingStations*			fWorkingStations;
	TWorkingConstants*			fWorkingConstants;
	TWorkingConstraints*		fWorkingConstraints;

	UEOIndices				fUEOIndices;

	int						fNumCala;
	int						fNumVx;
	int						fNumVy;
	int						fNumVz;
	int						fNumVxy;
	int						fNumVxz;
	int						fNumVyz;
	int						fNumVxyz;

	static bool					fUsedPDOR;

	bool					noDuplicates;

	int observationsCount;

	int unknownsCount;
	int						fNumRADI;
	int						fNumPDOR;

	int						fNumPolar;
	int						fNumAngl;
	int						fNumZend;
	int						fNumDhor;
	int						fNumDrdl;
	int						fNumDspt;
	int						fNumDver;
	int						fNumDlev;

	int						fNumECVE;
	int						fNumECSP;
	int						fNumECHO;
	int						fNumECTH;

	int						fNumOrie;

	int fDftSigRADI;



};
#endif // SU_LGC_DATA_SET
