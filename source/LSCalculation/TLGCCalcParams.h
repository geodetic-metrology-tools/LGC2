// TLGCCalcParams.h
//
/*! The parameters for a LGC computation:

  		way to compute standard deviations
		convergence criteria
		computation systems and reference surfaces
*/
// Patterns:
//
// 
// Copyright 2000-2008 M.Jones CERN TS/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef LGC_CALC_PARAMS
#define LGC_CALC_PARAMS


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////
#include  "TDataParameters.h"
class  TWorkingPointNamePairs;

#include "TRefSystemFactory.h"

//Class definition
class  TLGCCalcParams //: public TObject 

{
public:
	
	/*!@name Constructors and Destructors */
	//@{
		//! default constructor
		TLGCCalcParams();

		//! copy constructor
		TLGCCalcParams(const TLGCCalcParams&);

		//! destructor
		virtual  ~TLGCCalcParams();
	//@}


	/*!@name member functions */
	//@{
		//! copy assignment operator
		TLGCCalcParams& operator=(const TLGCCalcParams& );

		//! equivalence operator
		bool	operator==(const TLGCCalcParams& ) const;
	//@}

	/*!@name get functions */
	//@{
		/*! indicate if observations' standard deviations are calculated with the a priori sigma*/
		bool	usingSigmaAPriori() const {return fUsingSAPriori;}

		/*! get the convergence criteria as a TReal*/
		TReal	getConvCriteria() const {return fConvCriteria;}

		/*! get the enumerator corresponding to the reference surface*/
		TRefSystemFactory::ERefFrame	getRefSurface() const {return fRefSurface;}

		/*! get the name of the reference surface */
		string  getRefSurfaceName() const;

		/*!check if the computation is made with simulated values or not*/
		//bool isSimulation() const { return fIsSim; }

		/*! get the number of simulation to do*/
		//int	getNumOfSim() const {return fNumOfSim;}
	//@}
		/*! get the flag indicating if the calculation is for a Free Network Adjustment */
		//bool	isLibrDetect() const {return fFreeCalc;} 

		/*! set the flag indicating that the calculation is for a Free Network Adjustment */
		//void	librIsDetected() {fFreeCalc = true;	return;}

		/*! get the flag indicating if an orientation point is defined */
		bool	isPdorBearDefined() const {return fPdorBearDefined;}

		/*! set the flag indicating that an orientation point is defined */
		void	pdorBearIsNotDefined() {fPdorBearDefined = false; return;}
		
		//bool	areAllPointsFixed() { return allPointsFixed; }
		//void	setAllPointsFixed() { allPointsFixed = true; }

	/*!@name Reliability related functions */
	//@{
		/*! Returns the alpha value */
		TReal	getAlpha() const { return fAlpha; }

		/*! Returns the beta value */
		TReal  getBeta() const { return fBeta; }

		/*! sets alpha value */
		void	setAlpha(const TReal a) {fAlpha = a;return;}

		/*! sets beta values */
		void	setBeta(const TReal b) {fBeta  = b;return;}

			
		/*! indicates if fault detection file has to be created or not */
		bool	isFaultDetectToBeSaved() const {return fSaveFaultDetect;}	
		
		/*! turn on the creation of fault detection file */
		void    faultDetectToCreate()  {fSaveFaultDetect = true; return;}
	//@}




	/*!@name set functions */
	//@{
		/*! set the convergence criteria*/
		void	setConvCriteria(const TReal cc) {fConvCriteria = cc; return;}

		/*! turn on the using sigma a priori option*/
		void	setUsingSAPriori() {fUsingSAPriori = true;	return;}

		/*! set the enumerator corresponding to the reference surface*/
		void	setRefSurface(const TRefSystemFactory::ERefFrame ell) {fRefSurface = ell;	return;}

		/*! turn on the simulation option */
		//void setSimulation() {fIsSim = true; return;}

		/*! set the number of simulations to do */
		//void	setNumOfSim(const int num) {fNumOfSim = num; fIsSim = true; return;}
			
	//@}

#if 0
	/*!@name Specific methods for point name pairs container */
	//@{
		/*! Returns an iterator to the first item in the list */
		PtNamePairIter	beginPtNamePairs() {return fPtNamePairsList->begin();}

		/*! Returns a const iterator to the first item in the list */
		PtNamePairConstIter	beginPtNamePairs() const {return fPtNamePairsList->begin();}

		/*! Returns an iterator to the position after the last item of the list */
		PtNamePairIter	endPtNamePairs() {return fPtNamePairsList->end();}

		/*! Returns a const iterator to the position after the last item of the list */
		PtNamePairConstIter	endPtNamePairs() const {return fPtNamePairsList->end();}

		/*! Adds a new item to the list */
		PtNamePairIter	push_backPtNamePairs(const TPointNamePair& pn) {	return fPtNamePairsList->push_back(pn);}

		/*! Sets the warning message */
		void	setWarning(const string msg) {fPtNamePairsList->setWarning(msg);}

		/*! Returns the warning message */
		string	getWarning() const {return fPtNamePairsList->getWarning();}

		int		getPtNamePairsSize() const {return fPtNamePairsList->size();}

		bool isPunchEOrEE() const { return fIsPunchEOrEE; }
		void setIsPunchEOrEE() { fIsPunchEOrEE = true; }
	//@}
#endif


private:

	bool									fUsingSAPriori; /*!< boolean indicating if observations' sigma are calculated with sigma a priori*/
	TReal									fConvCriteria; /*!< convergence criteria (default : LITERAL(0.00001))*/

	/*!@name reliability parameters */
	//@{
	bool									fSaveFaultDetect; /*! boolean indicating if the reliability parameters have to be saved in a file or not */
	TReal									fAlpha; /*!< level of significance, [0;1], default value : 1% */
	TReal									fBeta; /*!< risk of having false values, [0;1], default value : 10% */
	//@}

	TRefSystemFactory::ERefFrame				fRefSurface; /*!< reference surface for the project*/
	
	//bool									fIsSim; /*!< boolean indicating if a simulation calculation is requested */
	//int										fNumOfSim; /*!<number of simulation to do */

	//bool									fFreeCalc;
	bool									fIsPunchEOrEE;
	bool									fPdorBearDefined;

	//TWorkingPointNamePairs*					fPtNamePairsList; /*!< pointer to the list of point name pairs */

	//bool									allPointsFixed;
	//ClassDef(TLGCCalcParams, 1)
};


#endif // SU_LGC_CALC_PARAMS
