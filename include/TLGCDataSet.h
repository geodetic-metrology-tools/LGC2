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
#include "TWorkingStations.h"
#include "TWorkingConstants.h"
#include "TWorkingConstraints.h"
#include "UEOIndices.h"


//! Class holding all the input data of a LGC project
class TLGCDataSet : public TADataSet{

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
		static const bool usedOrieCnstr();
	//@}

	/*!return the number of unknowns, equations and observations of the project as a UEOIndices object*/
	virtual UEOIndices				getDimensions() const;

	/*!@name Access methods*/
	//@{
		/*!@return a pointer to the working stations*/
		virtual TWorkingStations*		getWorkingStations() const	{return fWorkingStations;}

		/*!@return a pointer to the working points*/
		virtual TWorkingPoints*			getWorkingPoints() const	{return fWorkingPoints;}

		/*!@return a pointer to the working constants*/
		virtual TWorkingConstants*		getWorkingConstants() const	{return fWorkingConstants;}

		/*!@return a pointer to the working constraints*/
		virtual TWorkingConstraints*	getWorkingConstraints() const	{return fWorkingConstraints;}

	//@}

	/*!@name Counting methods*/
	//@{
		/*!increment the number of read points for the corresponding status*/
		virtual void addToPointNum(TSpatialStatus::ESpatialStatus status);
		/*!increment the number of read horizontal angles' measurements*/
		virtual void addToAnglNum();
		/*!increment the number of read zenithal distances' measurements*/
		virtual void addToZenDistNum(TTheodoliteStation* thSt);
		/*!increment the number of read horizontal distances' measurements*/
		virtual void addToHorDistNum();
		/*!increment the number of read spatial distances' measurements*/
		virtual void addToSpaDistNum(TDistMeasStation* distSt);
		/*!increment the number of read vertical distances' measurements*/
		virtual void addToVertDistNum(bool isDLEV);

		/*!increment the number of read Offset to vertical line measurements*/
		virtual void addToOffsetToVerLineNum();
		/*!increment the number of read Offset to vertical line measurements*/
		virtual void addToOffsetToSpaLineNum();
		/*!increment the number of read Offset to vertical line measurements*/
		virtual void addToOffsetToVerPlaneNum();
		/*!increment the number of read Offset to vertical line measurements*/
		virtual void addToOffsetToTheoPlaneNum();

		/*!increment the number of read orientation measurements*/
		virtual void addToGyroOrieNum();

		/*!increment the number of read orientation measurements*/
		virtual void addToRadOffCnstrNum();
		/*!increment the number of read orientation measurements*/
		virtual void addToOrieCnstrNum();

		/*!return the number of read points for the corresponding status*/
		virtual int getPointsDimension(TSpatialStatus::ESpatialStatus status) const;
		/*!return the number of read horizontal angles' measurements*/
		virtual int getHADimension() const;
		/*!return the number of read zenithal distances' measurements, classified by instrument height status*/
		virtual int getZDDimension(TAMeasurement::ECalcStatus status) const;
		/*!return the number of read horizontal distances' measurements*/
		virtual int getHDDimension() const;
		/*!return the number of read spatial distances' measurements, classified by instrument height status*/
		virtual int getSDDimension(TAMeasurement::ECalcStatus status) const;
		/*!return the number of read vertical distances' measurements*/
		virtual int getVDDimension(bool isDLEV) const;

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


	/*!@name used of a default sigma methods*/
	//@{
		/*!iniatialise the corresponding boolean attribut which define
		if the sigma used for calculation is a default sigma or not*/
		virtual void setDefaultAnglSig(const bool);
		/*!iniatialise the corresponding boolean attribut which define
		if the sigma used for calculation is a default sigma or not*/
		virtual void setDefaultZenDistSig(const bool, TAMeasurement::ECalcStatus);
		/*!iniatialise the corresponding boolean attribut which define
		if the sigma used for calculation is a default sigma or not*/
		virtual void setDefaultHorDistSig(const bool);
		/*!iniatialise the corresponding boolean attribut which define
		if the sigma used for calculation is a default sigma or not*/
		virtual void setDefaultSpaDistSig(const bool, TAMeasurement::ECalcStatus);
		/*!iniatialise the corresponding boolean attribut which define
		if the sigma used for calculation is a default sigma or not*/
		virtual void setDefaultVertDistSig(const bool isDLEV, const bool b);
		/*!iniatialise the corresponding boolean attribut which define
		if the sigma used for calculation is a default sigma or not*/
		virtual void setDefaultOffsetToVerLineSig(const bool);
		/*!iniatialise the corresponding boolean attribut which define
		if the sigma used for calculation is a default sigma or not*/
		virtual void setDefaultOffsetToSpaLineSig(const bool);
		/*!iniatialise the corresponding boolean attribut which define
		if the sigma used for calculation is a default sigma or not*/
		virtual void setDefaultOffsetToVerPlaneSig(const bool);
		/*!iniatialise the corresponding boolean attribut which define
		if the sigma used for calculation is a default sigma or not*/
		virtual void setDefaultOffsetToTheoPlaneSig(const bool);
		/*!iniatialise the corresponding boolean attribut which define
		if the sigma used for calculation is a default sigma or not*/
		virtual void setDefaultGyroOrieSig(const bool);
		/*!iniatialise the corresponding boolean attribut which define
		if the sigma used for calculation is a default sigma or not*/
		virtual void setDefaultRadOffCnstrSig(const bool);

		/*!return true if the default sigma is used for calculation*/
		virtual bool useDefaultSigHA() const;
		/*!return true if the default sigma is used for calculation*/
		virtual bool useDefaultSigZD(TAMeasurement::ECalcStatus status) const;
		/*!return true if the default sigma is used for calculation*/
		virtual bool useDefaultSigHD() const;
		/*!return true if the default sigma is used for calculation*/
		virtual bool useDefaultSigSD(TAMeasurement::ECalcStatus status) const;
		/*!return true if the default sigma is used for calculation*/
		virtual bool useDefaultSigVD(bool isDLEV) const;
		/*!return true if the default sigma is used for calculation*/
		virtual bool useDefaultSigECVE() const;
		/*!return true if the default sigma is used for calculation*/
		virtual bool useDefaultSigECSP() const;
		/*!return true if the default sigma is used for calculation*/
		virtual bool useDefaultSigECHO() const;
		/*!return true if the default sigma is used for calculation*/
		virtual bool useDefaultSigECTH() const;
		/*!return true if the default sigma is used for calculation*/
		virtual bool useDefaultSigORIE() const;
		/*!return true if the default sigma is used for calculation*/
		virtual bool useDefaultSigRADI() const;

		bool disallowsDuplicates() const { return noDuplicates; }
		void setNoDuplicates(bool disallow) { noDuplicates = disallow; }

		void enableErrorEllipses() { ferrorellipses = true; }
		bool hasErrorEllipses() const { return ferrorellipses; }
	//@}



	
private:

	TWorkingPoints*				fWorkingPoints;
	TWorkingStations*			fWorkingStations;
	TWorkingConstants*			fWorkingConstants;
	TWorkingConstraints*		fWorkingConstraints;

	bool ferrorellipses; ///< Enable the output of error ellipses
	
	UEOIndices				fUEOIndices;

	int						fNumCala;
	int						fNumVx;
	int						fNumVy;
	int						fNumVz;
	int						fNumVxy;
	int						fNumVxz;
	int						fNumVyz;
	int						fNumVxyz;

	int						fNumAngl;
	int						fNumZeni;
	int						fNumZenh;
	int						fNumDhor;
	int						fNumDmes;
	int						fNumDthe;
	int						fNumDver;
	int						fNumDlev;

	int						fNumECVE;
	int						fNumECSP;
	int						fNumECHO;
	int						fNumECTH;

	int						fNumOrie;

	int						fNumRADI;
	int						fNumPDOR;


	bool					fDftSigAngl;
	bool					fDftSigZeni;
	bool					fDftSigZenh;
	bool					fDftSigDhor;
	bool					fDftSigDmes;
	bool					fDftSigDthe;
	bool					fDftSigDver;
	bool					fDftSigDlev;
	bool					fDftSigECVE;
	bool					fDftSigECSP;
	bool					fDftSigECHO;
	bool					fDftSigECTH;
	bool					fDftSigOrie;
	bool					fDftSigRADI;

	static bool				fUsedPDOR;

	bool					noDuplicates;
};
#endif // SU_LGC_DATA_SET