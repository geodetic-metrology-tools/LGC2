#ifndef SU_SIM_FRAME_SUMMARY
#define SU_SIM_FRAME_SUMMARY

#include "TAdjustableHelmertTransformation.h"
#include "TFreeVector.h"

/*!
	\ingroup LGCCalculation

	\brief Class containing all information concerning an adjustable transformation which defines a FRAME. 
	Necessary for the final table of the simulation output file.
*/
class  TSimFrameSummary 

{
public:


	/*!@name Constructors and Destructors */
	//@{
		/// default constructor
		TSimFrameSummary();

		/// constructor taking a reference to the Adjustable Transofrmation
		TSimFrameSummary(const TAdjustableHelmertTransformation& point);

		/// copy constructor
		//TSimFrameSummary(const TSimFrameSummary&);

		/// destructor
		virtual  ~TSimFrameSummary();

	//@}


	/*!@name member functions */
	//@{
		/// copy assignment operator
		TSimFrameSummary& operator=(const TSimFrameSummary& );

		/// add a new value in the sum of the residus
		void	addNewResValue(const TransformParameters& res);

		/// get a pointer to the Adjustable Helmert transformation of this entity
		const TAdjustableHelmertTransformation*	getAdjustableTransformation() const { return fFrame; }

		/// get the sum of residuals
		const TransformParameters&		getSumRes() const { return fSumRes; }

		/// get the sum of squares of residuals
		const TransformParameters&		getSumRes2() const { return fSumRes2; }

		/// get the minimum of residuals
		const TransformParameters&		getMinRes() const { return fResMin; }

		/// get the minimum of residuals
		const TransformParameters&		getMaxRes() const { return fResMax; }

private:
		/// update, if necessary, the value of the minimum of residus
		void	ifNecessarySetMin(const TransformParameters& res);

		/// update, if necessary, the value of the minimum of residus
		void	ifNecessarySetMax(const TransformParameters& res);

			
	//@}


private:

	const TAdjustableHelmertTransformation*		fFrame; /*!< pointer to the helmert transformation in the tree*/

	TransformParameters					fSumRes; /*!< sum of the residus from all the simulations for this frame */
	TransformParameters					fResMin; /*!< minimum of the residus from all the simulations for this frame */
	TransformParameters					fResMax; /*!< maximum of the residus from all the simulations for this frame */
	TransformParameters					fSumRes2; /*!< sum of the square of the residus from all the simulations for this frame */

	bool						fFirstSim;
};

#endif 
