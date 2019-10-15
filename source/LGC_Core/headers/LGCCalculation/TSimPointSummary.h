/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef SU_SIM_PT_SUMMARY
#define SU_SIM_PT_SUMMARY


//SURVEYLIB
#include <TFreeVector.h>
//LGC
#include <LGCAdjustablePoint.h>


/*! 
	\ingroup LGCCalculation

	\brief Class containing all information concerning an adjustable point.
	Necessary for the final table of the simulation output file.
*/
class  TSimPointSummary 

{
public:


	/*!@name Constructors and Destructors */
	//@{
		/// default constructor
		TSimPointSummary();

		/// constructor taking a reference to th Adjustable point
		TSimPointSummary(const LGCAdjustablePoint& point);

		/// copy constructor
		TSimPointSummary(const TSimPointSummary&);

		/// destructor
		virtual  ~TSimPointSummary();

	//@}


	/*!@name member functions */
	//@{
		/// copy assignment operator
		TSimPointSummary& operator=(const TSimPointSummary& );

		/// equivalence operator
		bool	operator==(const TSimPointSummary& ) const;

		/// add a new value in the sum of the residus
		void	addNewResValue(const TFreeVector& res);

		/// get the pointer to the Adjustable Point of this entity
		const LGCAdjustablePoint*	getAdjustablePoint() const { return fPoint; }

		/// get the sum of residuals
		TFreeVector		getSumRes() const { return fSumRes; }

		/// get the sum of squares of residuals
		TFreeVector		getSumRes2() const { return fSumRes2; }

		/// get the minimum of residuals
		TFreeVector		getMinRes() const { return fResMin; }

		/// get the minimum of residuals
		TFreeVector		getMaxRes() const { return fResMax; }

private:
		/// update, if necessary, the value of the minimum of residus
		void	ifNecessarySetMin(const TFreeVector& res);

		/// update, if necessary, the value of the minimum of residus
		void	ifNecessarySetMax(const TFreeVector& res);

			
	//@}


private:

	const LGCAdjustablePoint*		fPoint; /*!< pointer to a global collection of points in TLGCData*/

	TFreeVector					fSumRes; /*!< sum of the residus from all the simulations for this point */
	TFreeVector					fResMin; /*!< minimum of the residus from all the simulations for this point */
	TFreeVector					fResMax; /*!< maximum of the residus from all the simulations for this point */
	TFreeVector					fSumRes2; /*!< sum of the square of the residus from all the simulations for this point */

	bool						fFirstSim;
	
	//ClassDef(TSimPointSummary, 1)
};

#endif // SU_LGC_SIM_PT_SUMMARY