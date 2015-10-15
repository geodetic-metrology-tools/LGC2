//////////////////////////////////////////////////////////////////////
// TLSInputMatricesFiller.h
/*! Class responsible for reading the least squares calculationmeasurements and constraints 
	and filling the least squares matrices
	Specific to a least squares calculation

  Pattern:

  Copyright 2000-2008 M.Jones CERN TS/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_LS_INPUT_MATRICES_FILLER
#define SU_LS_INPUT_MATRICES_FILLER


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
#include <string>
//#include  <nag.h>
//#include <nagg05.h>
using namespace std;

class LSCalcDataSet;
class TLSInputMatricesAdapter;
class TLSConstraintIdentifier;
class TGC2MLATransformation;
class TMLA2GCTransformation;
#include	"TLGCObsLSContributionGenerator.h"
#include	"TDataParameters.h"
#include    "TRefSystemFactory.h"

int pairCompare(const void* p1, const void* p2);

//! Class used to fill input matrices with data for least-square calculations
class TLSInputMatricesFiller
{

public :

	/*!@name Constructors / Destructor*/
	//@{
		/*! Contructor
		\param reference frame enumarator to indicate the calc option (OLOC, LEP...) */
		explicit TLSInputMatricesFiller(TRefSystemFactory::ERefFrame refSur, bool pdor);

		/*!Destructor */
		virtual ~TLSInputMatricesFiller();
	//@}
	
	/*!Fills the input matrices from the LSCalc DataSet */
	virtual bool 		fillMatrices(LSCalcDataSet&, TLSInputMatricesAdapter&);

	//! initialise the dimensions of the input matrices
	void		initMatriceDimension(const LSCalcDataSet&, TLSInputMatricesAdapter&);

	/*! Access to the error */
	string	getError() const { return fError; }


private:

	/*!Default constructor: not implemented */
	TLSInputMatricesFiller();
	/*! Copy constructor: not implemented */
	TLSInputMatricesFiller( const TLSInputMatricesFiller& source );

	/*! Equality operator: not implemented */
	TLSInputMatricesFiller& operator=( const TLSInputMatricesFiller& right );


	/*!@name Methods to add the design matrix contributions for each type of observation and constraint */
	//@{
		void		addPolarContributions(LSCalcDataSet&, TLSInputMatricesAdapter&);

		/*! Add the design matrices contributions for the hor. angle observations */
		void		addHorAngContributions(LSCalcDataSet&, TLSInputMatricesAdapter&);
		/*! Add the design matrices contributions for the zenithal angle observations */
		void		addZenDistContributions(LSCalcDataSet&, TLSInputMatricesAdapter&);
		/* Add the design matrices contributions for the spatial dist. observations */
		template <typename T>
		void		addSpaDistContributions(typename list<TLSCalcSpatialDistObservation<T> >::iterator obsIt, typename list<TLSCalcSpatialDistObservation<T> >::const_iterator endObsIt,TLSInputMatricesAdapter& matrices);
		/*! Add the design matrices contributions for the hor. dist. observations */
		template <typename T>
		void		addHorDistContributions(typename list<TLSCalcHorDistObservation<T> >::iterator obsIt, typename list<TLSCalcHorDistObservation<T> >::const_iterator endObsIt,TLSInputMatricesAdapter& matrices);
		/*! Add the design matrices contributions for the vertical dist. observations */
		void		addVertDistContributions(const LSCalcDataSet&, TLSInputMatricesAdapter&);
		/*! Add the design matrices contributions for the vertical dist. observations */
		void		addLevelObsContributions(const LSCalcDataSet&, TLSInputMatricesAdapter&);
		/*! Add the design matrices contributions for the offset to ver. line observations */
		void		addOffsetToVerLineContributions(const LSCalcDataSet&, TLSInputMatricesAdapter&);
		/*! Add the design matrices contributions for the offset to spa. line observations */
		void		addOffsetToSpaLineContributions(const LSCalcDataSet&, TLSInputMatricesAdapter&);
		/*! Add the design matrices contributions for the offset to ver. plane observations */
		void		addOffsetToVerPlaneContributions(const LSCalcDataSet&, TLSInputMatricesAdapter&);
		/*! Add the design matrices contributions for the offset to theo. plane observations */
		void		addOffsetToTheoPlaneContributions(const LSCalcDataSet&, TLSInputMatricesAdapter&);
		/*! Add the design matrices contributions for the gyro. orientation observations */
		void		addGyroOrieContributions(const LSCalcDataSet&, TLSInputMatricesAdapter&);
		
		/*! Add the design matrices contributions for the radial offset constraints */
		void		addRadOffCnstrContributions(const LSCalcDataSet&, TLSInputMatricesAdapter&);
		/*! Add the design matrices contributions for the offset to spa line constraints */
		void		addOffSpaLineCnstrContributions(const LSCalcDataSet&, TLSInputMatricesAdapter&);
		/*! Add the design matrices contributions for the offset to ver plane constraints */
		void		addOffVerPlaneCnstrContributions(const LSCalcDataSet&, TLSInputMatricesAdapter&);
		
		/*! Add the design matrices contributions for the orientation constraints */
		void		addOrieCnstrContributions(const LSCalcDataSet&, TLSInputMatricesAdapter&);
		/*! Add the design matrices contributions for the constraints of a free network calculation */
		void		addFreeCnstrContributions(const LSCalcDataSet&, TLSInputMatricesAdapter&, TLSConstraintIdentifier& cnstr);

	//@}


	/*!@name Related to the processing of the constraints for a free survey network*/
	//@{
		/*! Process the added constraints for a free network calculation */
		void		processFreeCnstr(const LSCalcDataSet&, TLSInputMatricesAdapter&, const TLSConstraintIdentifier& cnstr);
	//@}


	string							fError;	/*!< errors during filling */
	TLGCObsLSContributionGenerator  fGenerator;

	bool										fIsPdorBearingDefined;
};

#endif // SU_LS_INPUT_MATRICES_FILLER
