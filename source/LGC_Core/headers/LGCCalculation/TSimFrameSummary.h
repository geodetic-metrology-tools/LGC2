/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SU_SIM_FRAME_SUMMARY
#define SU_SIM_FRAME_SUMMARY


//SURVEYLIB
#include <TAdjustableHelmertTransformation.h>
#include <TFreeVector.h>

/// Need to redifine a structure for the square residual because the angle are resize
struct TransformParametersSquare{
	TReal omega; //!< Rotation about the X axis
	TReal phi;  //!< Rotation about the Y axis
	TReal kappa; //!< Rotation about the Z axis 

	TLength tX; //!< Translation about the X axis in meters [m]
	TLength tY; //!< Translation about the Y axis in meters [m]
	TLength tZ; //!< Translation about the Z axis in meters [m]

	TReal scale; //!< The unitless scale factor

	TransformParametersSquare() : omega(TReal(0.0)), phi(TReal(0.0)), kappa(TReal(0.0)), tX(TLength(0.0)), tY(TLength(0.0)), tZ(TLength(0.0)), scale(TReal(0.0)) {};
};

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
		TSimFrameSummary(const TSimFrameSummary&);

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
		const TransformParametersSquare&		getSumRes2() const { return fSumRes2; }

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

	const TAdjustableHelmertTransformation* fFrame; /*!< pointer to the helmert transformation in the tree*/

	TransformParameters fSumRes; /*!< sum of the residus from all the simulations for this frame */
	TransformParameters fResMin; /*!< minimum of the residus from all the simulations for this frame */
	TransformParameters fResMax; /*!< maximum of the residus from all the simulations for this frame */
	TransformParametersSquare fSumRes2; /*!< sum of the square of the residus from all the simulations for this frame */  

	bool						fFirstSim;
};

#endif 
