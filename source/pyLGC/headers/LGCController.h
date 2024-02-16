
#ifndef _LGCController_H_
#define _LGCController_H_

#include "LGCController.h"
#include "TAStreamFormatter.h"
#include <iostream>

// STL
#include <Eigen/Dense>

#include <Behavior.h>
#include <TLGCData.h>
#include <TLSAlgorithm.h>
#include <TReader.h>

#include "FileUtils.h"
#include "TDataAnalyzer.h"
#include "TLGCCalculation.h"
#include "TLSResultsMatrices.h"
#include "TLSSimulation.h"
#include "TVAbstractAlgorithm.h"
#include "TInputFileWriter.h"
#include "TSimFileWriter.h"
#include "TStreamFormatterFactory.h"
#include <iomanip> // put_time
#include <chrono>
#include "TLSEvaluator.h"



class LGCController
{
public:
	LGCController(const std::string path);
	// get the currently set parameter values
	Eigen::VectorXd getParameters();
	// set the parameter values
	void setParameters(Eigen::VectorXd par);
	// compute and get the misclosure for a given parameter
	Eigen::VectorXd getMisclosure(Eigen::VectorXd par);
	// compute and get the constraint misclosure
	Eigen::VectorXd getConstraintMisclosure(Eigen::VectorXd par);
	// compute and get the A matrix (d misclosure /d parameter)
	Eigen::SparseMatrix<double> getFirstDesignMatrix(Eigen::VectorXd par);
	// compute and get the A2 matrix (d constraint misclosure /d parameter)
	Eigen::SparseMatrix<double> getFirstConstraintDesignMatrix(Eigen::VectorXd par);
	// compute and get the B matrix (d misclosure /d observation)
	Eigen::SparseMatrix<double> getSecondDesignMatrix(Eigen::VectorXd par);
	// given a row-index of a element of the misclosure vector or A matrix get the linenumber in the inputfile where the measurement is defined
	int getLineNumber(int obsIdx) { return fProject->fObsIdx2LineIdx[obsIdx]; }
	// diagonal of weight matrix
	Eigen::VectorXd getPv(Eigen::VectorXd par);

	// for controlling the masking:
	// parameter masking will split the parameters in active and inactive (masked). in this state the evaluator expects a vector of length (active) and evaluates at a combination of the actively set parameters and at the incative parameters, which have the values they had at the moment of setting the mask
	// observation masking essentially is projecting the model function onto the unmasked indices.
	void setObservationMask(std::vector<int> maskedObservationIndices) { fEvaluator->currentMask.equationIndices = maskedObservationIndices; };
	void setParameterMask(std::vector<int> maskedParameterIndices) { fEvaluator->currentMask.parameterIndices = maskedParameterIndices; };
	std::vector<int> getObservationMask() { return fEvaluator->currentMask.equationIndices; };
	std::vector<int> getParameterMask() { return fEvaluator->currentMask.parameterIndices; };
	void unmask() { fEvaluator->unmask(); };

private:
	void initialize();
	std::string inputFilePath;
	// containing measurement configuration, observations, estimates
	std::shared_ptr<TLGCData> fProject;
	std::shared_ptr<TLSEvaluator> fEvaluator;
};


#endif
